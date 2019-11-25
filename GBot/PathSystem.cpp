#include "PathSystem.h"
#include <math.h>
#include <stdio.h>
#include <qline.h>
#include <qfile.h>
#include <qstringlist.h>
#include <time.h>
#include <QGraphicsEllipseItem>
#include "setupbotdlg.h"

PathSystem::PathSystem(HWND *hwnd,ProcessManager *ProcMan)
{
	moving = false;
	iSim = new inputSim(hwnd);
	currentWaypoint = 0;
	m_ProcMan = ProcMan;
	channeledAbilityOn = false;

	leftDown = false;
	rightDown = false;
	m_turning = false;
	m_SellEnabled = m_reviveEnabled = false;
	m_Companion = 0;
	m_Vent = false;

	oldDistance = 0;
	movementDifference = 0;
	pathInverted = false;

	timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(timerFinished()));

	tabSpamTimer = new QTimer(this);
	connect(tabSpamTimer,SIGNAL(timeout()),this,SLOT(tabTimerSlot()));

	//attackTimer = new QTimer(this);
	//connect(attackTimer,SIGNAL(timeout()),this,SLOT(attackMoveSlot()));

	enemiesAttackingTimer = new QTimer(this);
	connect(enemiesAttackingTimer,SIGNAL(timeout()),this,SLOT(checkLife()));

	selfBuffTimer = new QTimer(this);
	connect(selfBuffTimer,SIGNAL(timeout()),this,SLOT(selfBuff()));

	sellItemsTimer = new QTimer(this);
	connect(sellItemsTimer,SIGNAL(timeout()),this,SLOT(sellItemsSlot()));

	targetTimeOut = new QTimer(this);
	connect(targetTimeOut,SIGNAL(timeout()),this,SLOT(targetTimeOutSlot()));

	nextTargetTimer = new QTimer(this);
	connect(nextTargetTimer,SIGNAL(timeout()),this,SLOT(nextTargetTimeOutSlot()));

	m_hwnd = hwnd;
	attackBot = false;
	currentPhase = None;
	m_recoverKeyPressed = false;
	beginOfAttack = true;

	currentAbility = 0;
	numofJumps = 0;
	m_totalretries = 0;
	m_OldTargetID = 0;

	m_Type = -1;
	m_pveBot = false;
	m_pvpBot = false;
	m_isPetPresent = false;
	m_LootEnable = false;

	m_rotTimers = new rotationTimers(this);
	connect(m_rotTimers,SIGNAL(abilityTriggered(int,const abilityData *)),this,SLOT(abilityTriggeredSlot(int,const abilityData *)));

	reloadConfigIni();
}

PathSystem::~PathSystem(void)
{
}

void PathSystem::reloadConfigIni()
{
	PveBotSetup setup;
	setup.loadInidata();
	m_PveBotConfig = setup.getPveBotConfig();
}

void PathSystem::startBot()
{
	m_recoverKeyPressed = false;
	m_stopBot = false;
	if(m_pveBot)
		attackBot = true;
	currentPhase = PathFollowing;
	if(attackBot)
	{
		tabSpamTimer->start(1000);
		if(m_SellEnabled && !sellItemsTimer->isActive()) 
			sellItemsTimer->start(1500000);
		//sellItemsTimer->start(3000);
	}
}

void PathSystem::stopBot()
{
	m_stopBot = true; 
	attackBot = false;
	channeledAbilityOn = false;
	stopMove();
	timer->stop();
	tabSpamTimer->stop();
	m_rotTimers->stop();
	selfBuffTimer->stop();
	sellItemsTimer->stop();
	currentPhase = PathFollowing;
	m_recoverKeyPressed = false;
}

void PathSystem::reset()
{
	mv_waypoints.clear();
	currentWaypoint = 0;

	stopMove();
	stopBot();
}

void PathSystem::addWayPoint( PointCoords &w )
{
	PointCoords coord;
	coord.x = w.x;
	coord.y = w.y;
	coord.z = w.z;
	
	mv_waypoints.push_back(coord);
	currentWaypoint = mv_waypoints.size()-1;
}

float getPlayerAngle(float angle)
{
	float ang = angle;
	 if(ang < 0) 
			ang = ang += 2*PI;
	     ang = (ang * 180) / PI;
        ang = ang + 90;
        if(ang > 360) 
			ang = ang - 360;
        if(ang > 180) 
			ang = 180 - (ang - 180);
		else 
			ang = -1 * ang;
        return ang;
}

int PathSystem::getCurrentFollowPathPoint()
{
	if(pathInverted)
		return mv_waypoints.size()-currentWaypoint;

	return currentWaypoint;
}

void PathSystem::followPath(QString pathName,PointCoords *coords, float angle,float jumpDelay)
{
	if(m_stopBot || mv_waypoints.size() < 2)
		return;

	if(GetForegroundWindow() != *m_hwnd)
		SetForegroundWindow(*m_hwnd);


	float xPlayer,yPlayer;
	if(jumpDelay < 500) 
		jumpDelay = 500;
	
	xPlayer = coords->x;
	yPlayer = coords->y;

	QLineF line1(xPlayer, yPlayer,mv_waypoints[currentWaypoint].x, mv_waypoints[currentWaypoint].y);
	float angleDiff = line1.angle();
	float distance = line1.length();

	currentPhase = PathFollowing;

	//inverto il percorso
	if(currentWaypoint >= mv_waypoints.size()-1)
	{
		std::reverse(mv_waypoints.begin(), mv_waypoints.end());
		currentWaypoint = 0;
		pathInverted = !pathInverted;
	}

	float absAngDif = abs(angleDiff - angle);

	if (distance  <= 2.0)
	{
		m_hasFinished = true;
		stopMove();
		timer->stop();
		currentWaypoint++;
		numofJumps = 0;
		m_totalretries = 0;
	}
	
		//tolerance to jump
	if(moving)
	{
		
		movementDifference = abs((int)distance - oldDistance);
		if(movementDifference < 1)
		{
			
			if(!timer->isActive())
			{
				timer->start(jumpDelay);
			}
		
		}
		else
		{
			timer->stop();
		}

		oldDistance = (int)distance;
	}

	if ( absAngDif >= 120)
	{
		// Facing too different. Stop and turn.
		m_turning = false;
		turnFine( angle, angleDiff );
		if(distance > 2.0) move();
		return;
	}
	else if ( absAngDif >= 10)
	{
		// Facing different but not too much. Turn while running.
		setFacing( angle, angleDiff, 10);
		if(distance > 2.0) move();
		return;
	}
	else
	{
		if ( leftDown )
		{
			iSim->pressButtonUp(VK_LEFT);
			leftDown = false;
		}
		if ( rightDown )
		{
			iSim->pressButtonUp(VK_RIGHT);
			rightDown = false;
		}

		if(distance > 2.0) move();
		m_turning = false;
	}

	m_turning = false;
}

bool PathSystem::followTargetPosition(PointCoords *coords, float angle, PointCoords *targetCoords,float dist, float jumpDelay, bool looting, bool rotateToFace)
{
	if(channeledAbilityOn)
		return true;

	float xPlayer,yPlayer;
	if(jumpDelay < 500) 
		jumpDelay = 500;
	
	xPlayer = coords->x;
	yPlayer = coords->y;

	QLineF line1(xPlayer, yPlayer,targetCoords->x, targetCoords->y);
	float angleDiff = line1.angle();
	float distance = line1.length();

	float absAngDif = abs(angleDiff - angle);
	
	if(!looting)
		dist = m_botDistance;
	if (distance  <= dist && (!rotateToFace || m_botDistance <= 3))
	{
		m_hasFinished = true;
		stopMove();
		timer->stop();
		return true;
	}
	
		//tolerance to jump
	if(moving)
	{
		
		movementDifference = abs((int)distance - oldDistance);
		if(movementDifference < 1)
		{
			 
 			if(!timer->isActive())
			{
				timer->start(jumpDelay);
			}
		
		}
		else
		{
			timer->stop();
		}

		oldDistance = (int)distance;
	}

	if ( absAngDif >= 120)
	{
		// Facing too different. Stop and turn.
		m_turning = false;
		turnFine( angle, angleDiff );
		if(distance > dist) move();
		return false;
	}
	else if ( absAngDif >= 10)
	{
		// Facing different but not too much. Turn while running.
		setFacing( angle, angleDiff, 10);
		if(distance > dist) move();
		return false;
	}
	else
	{
		if ( leftDown )
		{
			iSim->pressButtonUp(VK_LEFT);
			leftDown = false;
		}
		if ( rightDown )
		{
			iSim->pressButtonUp(VK_RIGHT);
			rightDown = false;
		}

		if(distance > dist) move();
		m_turning = false;
		if(rotateToFace && m_botDistance > 3)
		{
			m_hasFinished = true;
			stopMove();
			timer->stop();
			return true;
		}
		
	}

	m_turning = false;
	return false;
}

int PathSystem::getNumWaypoints()
{
	return mv_waypoints.size();
}

bool PathSystem::hasFinished()
{
	return m_hasFinished;
}

void PathSystem::move()
{
	//if (moving)
	//	return;

	iSim->pressButtonDown(VK_UP);
	
	moving = true;
}

void PathSystem::stopMove()
{
	iSim->pressButtonUp(VK_UP);

	if ( leftDown )
	{
		iSim->pressButtonUp(VK_LEFT);
		leftDown = false;
	}
	if ( rightDown )
	{
		iSim->pressButtonUp(VK_RIGHT);
		rightDown = false;
	}
	
	moving = false;
}


void PathSystem::turnFine(float playerAngle, float targetR)
{
	stopMove();

	setFacing( playerAngle, targetR, 10);

}


bool PathSystem::setFacing( float playerAngle, float requiredFacing, float tolerance)
{
	ulong timer;
	float targetRDif;
	float initFacing;
	ulong timerChange;
	
	float toMove = 0.0;
	if(playerAngle != requiredFacing)
		toMove = playerAngle-requiredFacing;
	else
		return true;

	float direction = 0.0;
	if(requiredFacing < playerAngle)
		direction = 360-fabs(toMove);
	else
		direction = fabs(playerAngle-requiredFacing);

	if (direction <= tolerance )
	{
		if ( leftDown )
		{
			iSim->pressButtonUp(VK_LEFT);
			leftDown = false;
		}
		if ( rightDown )
		{
			iSim->pressButtonUp(VK_RIGHT);
			rightDown = false;
		}

		m_turning = false;
		return true;
	}

	if(m_turning)
		return false;

	if  (direction >= 180)
	{
		if ( leftDown )
		{
			iSim->pressButtonUp(VK_LEFT);
			leftDown = false;
	    }
		if ( !rightDown )
		{
			iSim->pressButtonDown(VK_RIGHT);
			rightDown = true;
			m_turning = true;
		}
			
	
	}
	else
	{
		if ( rightDown )
		{
			iSim->pressButtonUp(VK_RIGHT);
			rightDown = false;
		}
		if ( !leftDown )
		{
			iSim->pressButtonDown(VK_LEFT);
			leftDown = true;
			m_turning = true;
		}
		
	}

	return false;
}

void PathSystem::cancelTarget()
{
	iSim->pressButtonUp(VK_UP);

	if ( leftDown )
	{
		iSim->pressButtonUp(VK_LEFT);
		leftDown = false;
	}
	if ( rightDown )
	{
		iSim->pressButtonUp(VK_RIGHT);
		rightDown = false;
	}
	
	moving = false;
	m_turning = false;
	timer->stop();
}

void PathSystem::timerFinished( )
{
	if(m_totalretries >= 8)
	{
		m_totalretries = 0;
	}
	if(numofJumps >= 2 && (currentPhase == Attacking || currentPhase == targetCheck))
	{
		if(m_target.isValid)
		{
			iSim->pressButtonDown(VK_ESCAPE);
			iSim->pressButtonUp(VK_ESCAPE);
		}
		moveRandom();
		numofJumps = 0;
		return;
	}

	if(numofJumps >= 2 && currentPhase == PathFollowing)
	{
		//iSim->pressButtonDown(VK_ESCAPE);
		//iSim->pressButtonUp(VK_ESCAPE);
		//pathInverted = !pathInverted;
		moveRandom();
		numofJumps = 0;
		return;
	}

	stopMove();
	iSim->pressButtonDown(VK_DOWN);
	Sleep(300);
	iSim->pressButtonUp(VK_DOWN);
	move();
	Sleep(100);
	iSim->pressButtonUp(VK_SPACE);
	iSim->pressButtonDown(VK_SPACE);	
	timer->stop();
	numofJumps++;
	m_totalretries++;
}

bool PathSystem::loadPath(QString fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
		return false;

	reset();

	QString mapName = file.readLine(512).simplified();

	while(!file.atEnd())
	{
		PointCoords wP;
		QString line = file.readLine(512);
		QStringList list = line.split(';');

		if(list.size() != 4)
		{
			file.close();
			return false;
		}
		wP.x = list[2].toFloat();
		wP.y = list[3].toFloat();
		mv_waypoints.push_back(wP);

	}

	file.close();

	PointCoords localPlayerCoord;
	localPlayerCoord.x = m_player.baseData.mob_x;
	localPlayerCoord.y = m_player.baseData.mob_y;
	localPlayerCoord.z = m_player.baseData.mob_z;

	currentWaypoint = searchNearestWayPoint(&localPlayerCoord);

	return true;
}

bool  PathSystem::savePath(QString fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))
		return false;

	QString mapline = QString("%1\n").arg("GenericPath");
	file.write(mapline.toAscii(),mapline.length());

	for(int i = 0; i < mv_waypoints.size(); i++)
	{
		QString line = QString("%1;%2;%3;%4\n").arg(i)
			.arg("0")
			.arg(mv_waypoints[i].x)
			.arg(mv_waypoints[i].y);

		file.write(line.toAscii(),line.length());
	}

	file.close();

	return true;
}

int PathSystem::searchNearestWayPoint(PointCoords *playerCoords)
 {
	if(mv_waypoints.size() == 0)
	{
		return -1;
	}
	float distance = 99999999.0f;
	int nearestPoint = 0;
	for(int i = 0; i < mv_waypoints.size(); i++)
	{
		QLineF line(playerCoords->x,playerCoords->y,mv_waypoints[i].x,mv_waypoints[i].y);
		if(line.length() < distance)
		{
			nearestPoint = i;
			distance = line.length();
		}
	}

	return nearestPoint;
}

void PathSystem::tabTimerSlot()
{

	iSim->pressButtonDown(VK_TAB);
	iSim->pressButtonUp(VK_TAB);	
}

void PathSystem::setCurrentTargetData(LocalPlayer *localPlayer, MobData *playerdata, MobData *target, float angle, float dist, float jumpDelay)
{
	m_target = *target;
	m_localPlayer = *localPlayer;
	m_player = *playerdata;

	if(currentPhase != Attacking)
		channeledAbilityOn = false;

	if(!attackBot)
	{
		stopTimers();
		return;
	}

	if(m_Vent)
	{
		m_player.heroData.energy = 100-m_player.heroData.energy;
	}

	if(m_Companion && m_Companion->baseData.id == m_target.baseData.id && m_target.isValid)
	{
		iSim->pressButtonDown(VK_ESCAPE);
		iSim->pressButtonUp(VK_ESCAPE);	
		Sleep(1000);
		return;
	}

		float distanceFromLastWaypoint = 0.0;

	//controllo se troppa differenza in z
	float zDifference = 0;
	if(m_target.isValid)
	{
		zDifference = abs(m_player.baseData.mob_z- m_target.baseData.mob_z);
		m_rotTimers->setMobsData(&m_player,&m_target);
	}
	else
		m_rotTimers->setMobsData(0,0);

	if(mv_waypoints.size() > 0)
	{
		QLineF line(m_target.baseData.mob_x,m_target.baseData.mob_y, mv_waypoints[currentWaypoint].x, mv_waypoints[currentWaypoint].y);
		distanceFromLastWaypoint = line.length();
	}
	else
	{
		QLineF line(m_target.baseData.mob_x,m_target.baseData.mob_y, m_player.baseData.mob_x, m_player.baseData.mob_y);
		distanceFromLastWaypoint = line.length();
	}


	if(m_player.heroData.hp <= 0 || currentPhase == playerDead)
	{
		ressWithProbe(angle,3,1500);
		return;
	}


	if(currentPhase == sellingItems)
	{
		return;
	}

	
	if(m_localPlayer.playerData.fPartner_Health <= 0 && m_isPetPresent && (currentPhase == PathFollowing ||
		currentPhase == CheckingLife))
	{
		if(m_reviveEnabled)
		{
			revivePet();
			Sleep(1000);
			return;
		}
		else
			m_isPetPresent = false;
	}
	
	
	if(currentPhase == nextGroupTargeting && !m_target.isValid)
	{
		tabSpamTimer->stop();
		PointCoords localPlayerCoord;
		localPlayerCoord.x = playerdata->baseData.mob_x;
		localPlayerCoord.y = playerdata->baseData.mob_y;
		localPlayerCoord.z = playerdata->baseData.mob_z;

		PointCoords coord;
		coord.x = target->baseData.mob_x;
		coord.y = target->baseData.mob_y;
		coord.z = target->baseData.mob_z;

		MobData mob = m_ProcMan->getMobWithID(currentPullMobs[0].baseData.id);

		if(mob.isValid)
		{
			iSim->pressButtonDown(VK_UP);
			Sleep(100);
			iSim->pressButtonUp(VK_UP);
			bool finished = followTargetPosition(&localPlayerCoord, angle,&coord,dist,jumpDelay, false,true);
			if(finished && !m_target.isValid)
			{
				iSim->pressButtonDown(VK_TAB);	
				iSim->pressButtonUp(VK_TAB);	
				Sleep(400);
				iSim->pressButtonDown(m_PveBotConfig.openingAttack);
				iSim->pressButtonUp(m_PveBotConfig.openingAttack);
				return;
			}
		}
		else
		{
			nextTargetTimer->stop();
			currentPhase = PathFollowing;
		}
	}
	

	if(!m_target.isValid)
	{
		targetTimeOut->stop();
		m_rotTimers->stop();
	}


	if(currentPhase == looting)
	{
		PointCoords localPlayerCoord;
		localPlayerCoord.x = m_player.baseData.mob_x;
		localPlayerCoord.y = m_player.baseData.mob_y;
		localPlayerCoord.z = m_player.baseData.mob_z;

		Loot(&localPlayerCoord,angle,&m_lastTarget,3,2000);
	}
	
	if(m_target.isValid && distanceFromLastWaypoint < m_PveBotConfig.BotMaxDistance && zDifference <= m_PveBotConfig.BotMaxZValue)
	{
		tabSpamTimer->stop();
		enemiesAttackingTimer->stop();
		if(currentPhase == PathFollowing)
		{
			m_stopBot = true;
		}			

		if(m_OldTargetID != m_target.baseData.id)
		{
			m_OldTargetID = m_target.baseData.id;
			targetTimeOut->stop();
			
		}

		if(m_OldTargetID == m_target.baseData.id && !targetTimeOut->isActive())
		{
			targetTimeOut->start(20000);
		}

		PointCoords coord;
		coord.x = target->baseData.mob_x;
		coord.y = target->baseData.mob_y;
		coord.z = target->baseData.mob_z;

		m_lastTarget.x = coord.x;
		m_lastTarget.y = coord.y;
		m_lastTarget.z = coord.z;

		PointCoords localPlayerCoord;
		localPlayerCoord.x = playerdata->baseData.mob_x;
		localPlayerCoord.y = playerdata->baseData.mob_y;
		localPlayerCoord.z = playerdata->baseData.mob_z;

		bool finished = followTargetPosition(&localPlayerCoord, angle,&coord,dist,jumpDelay);

		
		//if(!finished)
		//	beginOfAttack = true;
		/*
		//devo controllare quanti nemici ci sono in un dato raggio per sapere se è un attacco multiplo se più di 3 cambia zona
		if(beginOfAttack && currentPullMobs.size() == 0)
		{
			//zona di controllo
			QGraphicsEllipseItem item(coord.x-14,coord.y-14,28,28);
			QVector<MobData> mobs = m_ProcMan->getMobs();
			for(int i = 0; i < mobs.size(); i++)
			{
				if(item.contains(QPointF(mobs[i].baseData.mob_x, mobs[i].baseData.mob_y)))
					currentPullMobs.push_back(mobs[i]);
			}
			
		}
		*/

		currentPhase = Attacking;
		//if( m_Type == 0 && localPlayer->playerData.fDist_2_Target <= 2.5 && beginOfAttack)
		//{
		//	attackMoveSlot();
		//}
		if(!m_rotTimers->isActive())
		{
			m_rotTimers->start();
			//attackMoveSlot();
			//attackTimer->start(m_PveBotConfig.attackDelay);
		}
	}
	else if(m_target.isValid && (distanceFromLastWaypoint >= m_PveBotConfig.BotMaxDistance || zDifference >= m_PveBotConfig.BotMaxZValue) && (currentPhase == targetCheck || currentPhase == Attacking))
	{
		iSim->pressButtonDown(VK_ESCAPE);
		iSim->pressButtonUp(VK_ESCAPE);	

		m_rotTimers->stop();
		PointCoords localPlayerCoord;
		localPlayerCoord.x =  m_player.baseData.mob_x;
		localPlayerCoord.y =  m_player.baseData.mob_y;
		localPlayerCoord.z =  m_player.baseData.mob_z;

		
		int nPoint = searchNearestWayPoint(&localPlayerCoord);
		if(!tabSpamTimer->isActive())
			tabSpamTimer->start(1000);
		enemiesAttackingTimer->stop();
		if(nPoint >= 0)
		{
			currentWaypoint = nPoint;
			startBot();
			beginOfAttack = true;
		}
		
	}
	else if(m_player.heroData.combat == 0 && currentPhase != looting && currentPhase != PathFollowing && currentPhase != targetCheck)
	{
		checkLife();
		channeledAbilityOn = false;
		
		/*else
		{
			iSim->pressButtonDown(VK_TAB);
			iSim->pressButtonUp(VK_TAB);
			channeledAbilityOn = false;
			Sleep(100);
		}
		
		*/
		//controllo se il pet è sotto attacco
		//if(m_player.heroData.combat)
		//	currentPhase = Attacking;
		
			/*
		QVector<MobData> mobs = m_ProcMan->getMobs();
		for(int i = 1; i < mobs.size(); i++)
		{
			if(mobs[i].baseData.targetId == m_Companion->baseData.id)
			{
				currentPhase = nextGroupTargeting;
				break;
			}
			else
				currentPhase = Attacking;

		}
		*/
		 
		//stopMove();
	}
	else if(!m_target.isValid && m_player.heroData.combat == 1)
	{
		iSim->pressButtonDown(VK_TAB);
		iSim->pressButtonUp(VK_TAB);
	}
	else if(!m_target.isValid && currentPhase == targetCheck)
	{
		m_rotTimers->stop();
		enemiesAttackingTimer->stop();
		PointCoords localPlayerCoord;
		localPlayerCoord.x = playerdata->baseData.mob_x;
		localPlayerCoord.y = playerdata->baseData.mob_y;
		localPlayerCoord.z = playerdata->baseData.mob_z;

		
		int nPoint = searchNearestWayPoint(&localPlayerCoord);
		//if(!tabSpamTimer->isActive())
		//	tabSpamTimer->start(1000);
		tabSpamTimer->stop();

		
		if(nPoint >= 0)
		{
			currentWaypoint = nPoint;
			startBot();
			beginOfAttack = true;
		}
		
	}
	else if(currentPhase == targetCheck)
	{
		m_rotTimers->stop();
		tabSpamTimer->stop();
		enemiesAttackingTimer->stop();
	} 
}

void PathSystem::checkLife()
{
	beginOfAttack = true;
	m_rotTimers->stop();
	tabSpamTimer->stop();

	currentPhase = CheckingLife;

	float partnerHealth = m_localPlayer.playerData.fPartner_Health;
	if(!m_isPetPresent)
		partnerHealth = 100;

	Sleep(100);

	if(m_localPlayer.playerData.fHealth < m_PveBotConfig.minimumPlayerHealh || partnerHealth < m_PveBotConfig.minimumCopmanionHealth || m_player.heroData.energy < m_PveBotConfig.minimumPlayerEnergy)
	{
		if(!m_recoverKeyPressed)
		{
			iSim->pressButtonDown(VK_UP);
			Sleep(50);
			iSim->pressButtonUp(VK_UP);
			Sleep(500);
			stopMove();
			m_recoverKeyPressed = true;
			abilityData abil = m_rotTimers->getSelfHeal();
			if(abil.key != 0)
			{
				iSim->pressButtonDown(abil.key);
				iSim->pressButtonUp(abil.key);	
			}
		}
	}
	if(m_localPlayer.playerData.fHealth >= 100.0 && partnerHealth >= 100.0 && m_recoverKeyPressed)
	{
		currentPhase = targetCheck;
		m_recoverKeyPressed = false;
	}
	if(!m_recoverKeyPressed)
	{
		currentPhase = looting;
	}
}

void PathSystem::attackMoveSlot()
{
	bool energyAttack = false;
	if(beginOfAttack)
	{
		iSim->pressButtonDown(m_PveBotConfig.openingAttack);
		iSim->pressButtonUp(m_PveBotConfig.openingAttack);
		beginOfAttack = false;
		currentAbility = 0;
		return;
	}

	if(currentAbility >= m_PveBotConfig.mv_abilities.size())
		currentAbility = 0;

	for(int i = currentAbility; i < m_PveBotConfig.mv_abilities.size(); i++)
	{																																																						
		if(m_player.heroData.energy >= m_PveBotConfig.mv_abilities[i].energyRequired)
		{
			iSim->pressButtonDown(m_PveBotConfig.mv_abilities[i].abilityKey);
			Sleep(20);
			iSim->pressButtonUp(m_PveBotConfig.mv_abilities[i].abilityKey);
			
			currentAbility++;
			if(m_PveBotConfig.alwaysDefaultAttacks)
				return;
			break;
		}
	}

	Sleep(50);
	iSim->pressButtonDown(m_PveBotConfig.defaultAttack);
	iSim->pressButtonUp(m_PveBotConfig.defaultAttack);
	
}

void PathSystem::moveRandom()
{
	timer->stop();
	srand ( time(NULL) );
	int num = rand() % 2;
	int num2 = rand () % 300 + 500;
	int num3 = rand () % 500 + 1800;

	if(num == 0)
	{
		iSim->pressButtonDown(VK_RIGHT);
		Sleep(num2);
		iSim->pressButtonUp(VK_RIGHT);
		Sleep(num3);
	}
	else
	{
		iSim->pressButtonDown(VK_LEFT);
		Sleep(num2);
		iSim->pressButtonUp(VK_LEFT);
		Sleep(num3);
	}
}

void PathSystem::Loot(PointCoords *coords,float angle,  PointCoords *targetCoords, float dist, float JumpDelay)
{
	if(m_lastTarget.x == 0 || m_LootEnable == false)
	{
		currentPhase = targetCheck;
		return;
	}

	bool finished = followTargetPosition(coords,angle,targetCoords,dist,JumpDelay,true);

	if(!finished)
		return;

	POINT p;
	p.x = 0;
	p.y = 10;
	iSim->mouseRightClickOnCenter(&p,false);
	Sleep(2000);

	p.x = -500;
	p.y = 0;
	iSim->mouseLeftClickOnCenter(&p,true,true);
	Sleep(500);

	currentPhase = targetCheck;
	m_lastTarget.x = 0;
}

void PathSystem::selfBuff()
{
	return;
	if(m_PveBotConfig.selfBuffKey != 'N')
	{
		iSim->pressButtonDown(m_PveBotConfig.selfBuffKey);
		iSim->pressButtonUp(m_PveBotConfig.selfBuffKey);
		Sleep(300);
	}
}

void PathSystem::sellItemsSlot()
{
	if(currentPhase == sellingItems || !m_isPetPresent)
	{
		currentPhase = PathFollowing;
		sellItemsTimer->start(1500000);
		return;
	}
	QVector<autoProcedure> proc = m_setupBotDlg->getSellProcedure();

	for(int i = 0; i < proc.size(); i++)
	{
		if(proc[i].leftClick)
			iSim->mouseLeftClick(&proc[i].pos);
		else
			iSim->mouseRightClick(&proc[i].pos);
		Sleep(proc[i].waitMilliseconds);
	}

	iSim->pressButtonDown(VK_ESCAPE);
	iSim->pressButtonUp(VK_ESCAPE);

	currentPhase = sellingItems;
	sellItemsTimer->start(62000);
}

void PathSystem::revivePet()
{
	
	QVector<autoProcedure> proc = m_setupBotDlg->getRessProcedure();

	for(int i = 0; i < proc.size(); i++)
	{
		if(proc[i].leftClick)
			iSim->mouseLeftClick(&proc[i].pos);
		else
			iSim->mouseRightClick(&proc[i].pos);
		Sleep(proc[i].waitMilliseconds);
	}
}

void PathSystem::ressWithProbe(float angle, float dist, float JumpDelay)
{
	stopTimers();
	
	currentPhase = playerDead;

	if(m_player.heroData.hp > 0)
	{
		PointCoords localPlayerCoord;
		localPlayerCoord.x =  m_player.baseData.mob_x;
		localPlayerCoord.y =  m_player.baseData.mob_y;
		localPlayerCoord.z =  m_player.baseData.mob_z;
		 
		int nPoint = searchNearestWayPoint(&localPlayerCoord);
		if(nPoint >= 0)
		{
			currentWaypoint = nPoint;
			bool finished = followTargetPosition(&localPlayerCoord,angle,&mv_waypoints[currentWaypoint],dist,JumpDelay,true);
			if(!finished)
				return;
		}
		
		Sleep(1000);
		iSim->pressButtonDown(m_PveBotConfig.recoverAbility);
		iSim->pressButtonUp(m_PveBotConfig.recoverAbility);	
		Sleep(8000);
		iSim->pressButtonDown(m_PveBotConfig.selfBuffKey);
		iSim->pressButtonUp(m_PveBotConfig.selfBuffKey);
		Sleep(300);
		startBot();

	}

	POINT p;
	p.x = 0;
	p.y = 20;

	iSim->mouseLeftClickOnCenter(&p,false,false);
	Sleep(8000);

}

void PathSystem::targetTimeOutSlot()
{
	iSim->pressButtonDown(VK_ESCAPE);
	iSim->pressButtonUp(VK_ESCAPE);

	channeledAbilityOn = false;

	m_OldTargetID = 0;

	m_rotTimers->stop();
	PointCoords localPlayerCoord;
	localPlayerCoord.x =  m_player.baseData.mob_x;
	localPlayerCoord.y =  m_player.baseData.mob_y;
	localPlayerCoord.z =  m_player.baseData.mob_z;

	m_lastTarget.x = 0;

		
	int nPoint = searchNearestWayPoint(&localPlayerCoord);
	if(!tabSpamTimer->isActive())
		tabSpamTimer->start(1000);
	enemiesAttackingTimer->stop();
	if(nPoint >= 0)
	{
		currentWaypoint = nPoint;
		beginOfAttack = true;
	}

	startBot();
}

void PathSystem::nextTargetTimeOutSlot()
{
	return;
	if(m_target.isValid)
		return;
	//se ci sono ancora dei mob nel gruppo devo ucciderli
	if(currentPullMobs.size() > 0)
		currentPullMobs.pop_front();

	if(currentPullMobs.size() > 0)
	{
		currentPhase = nextGroupTargeting;
		return;
	}

	nextTargetTimer->stop();
	currentPhase = PathFollowing;
}

void PathSystem::abilityTriggeredSlot(int index,const abilityData *ability)
{
	iSim->pressButtonDown(ability->key);
	Sleep(50);
	iSim->pressButtonUp(ability->key);

	if(ability->channeled > 0)
		channeledAbilityOn = true;
	else
		channeledAbilityOn = false;

	m_rotTimers->setAbilityUsed(index);
}


