#pragma once
#include <qvector>
#include "inputSim.h"
#include <qtimer.h>
#include "c:\progetti\include\swlib.h"
#include "pvebotsetup.h"
#include "Structures.h"
#include "rotationtimer.h"

class SetupBotDlg;
class inputSim;
class rotationTimers;
class PathSystem : public QObject
{
	Q_OBJECT
public:
	PathSystem(HWND *hwnd, ProcessManager *ProcMan);
	~PathSystem(void);

	enum AttackingPhase{None, PathFollowing, Attacking, CheckingLife, targetCheck, playerDead, companionDead, sellingItems, looting, nextGroupTargeting};

	bool loadPath(QString fileName);
	bool savePath(QString fileName);
	void reset();
	void addWayPoint( PointCoords &w );
	int getNumWaypoints();
	bool hasFinished();
	bool setFacing( float playerAngle, float requiredFacing, float tolerance);
	bool followTargetPosition(PointCoords *coords, float angle, PointCoords *targetCoords,float dist, float jumpDelay, bool Looting = false, bool rotateToFace = false);
	void followPath(QString pathName,PointCoords *coords, float angle,float jumpDelay);
	void cancelTarget();
	void stopBot();
	void startBot();
	QVector<PointCoords> *getFollowPathPoint(){return &mv_waypoints;}
	int getCurrentFollowPathPoint();
	void setCurrentTargetData(LocalPlayer *localPlayer, MobData *playerdata, MobData *target, float angle, float dist, float jumpDelay);

	int searchNearestWayPoint(PointCoords *playerCoords);
	void setCompanion(MobData *companion){m_Companion = companion;}

	void moveRandom();
	void reloadConfigIni();
	void setBotConfig(int type, bool pet, bool PveBot, bool pvpBot, bool lootEnable, bool sellEnabled, bool reviveEnabled)
	{
		m_Type = type; m_pveBot = PveBot; m_pvpBot = pvpBot; m_isPetPresent = pet;
		if(m_Type == 0)
			m_botDistance = 4;
		else if(m_Type == 1)
			m_botDistance =28;
		m_LootEnable = lootEnable;
		m_SellEnabled = sellEnabled;
		m_reviveEnabled = reviveEnabled;
		
	}

	void setBotAbilities(QVector<abilityData> *abilities, bool vent)
	{
		m_Vent = vent;
		mv_abilities = abilities;
		m_rotTimers->setAbilitiesData(mv_abilities);
	}

	void setSetupBotDlg(SetupBotDlg *dlg){m_setupBotDlg = dlg;}

private:
	
	void stopMove();
	void move();
	void Loot(PointCoords *coords,float angle,  PointCoords *targetCoords, float dist, float JumpDelay);


	AttackingPhase currentPhase;
	QVector<PointCoords> mv_waypoints;
	int currentWaypoint;

	bool m_hasFinished;

	bool moving;
	bool attackBot;
	inputSim *iSim;

	bool leftDown;
	bool rightDown;
	bool m_turning;

	int oldDistance;
	int movementDifference;

	void turnFine(float playerAngle, float targetR);

	QTimer *timer;
	QTimer *tabSpamTimer;
	//QTimer *attackTimer;
	QTimer *enemiesAttackingTimer;
	QTimer *selfBuffTimer;
	QTimer *sellItemsTimer;
	QTimer *targetTimeOut;
	QTimer *nextTargetTimer;

	bool m_stopBot;
	bool pathInverted;

	int m_Type;
	bool m_pveBot;
	bool m_pvpBot;
	bool m_isPetPresent;
	bool m_SellEnabled;
	bool m_reviveEnabled;

	HWND *m_hwnd;
	MobData m_target;
	MobData m_player;
	LocalPlayer m_localPlayer;
	bool m_recoverKeyPressed;
	bool beginOfAttack;
	PveBotConfig m_PveBotConfig;
	int currentAbility;
	int numofJumps;
	int m_LastWayPoint;
	PointCoords m_lastTarget;
	float m_botDistance;
	int m_totalretries;
	unsigned __int64 m_OldTargetID;
	bool m_LootEnable;
	ProcessManager *m_ProcMan;
	QVector<MobData> currentPullMobs;

	rotationTimers *m_rotTimers;
	QVector<abilityData> *mv_abilities;

	SetupBotDlg *m_setupBotDlg;

	MobData *m_Companion;
	bool channeledAbilityOn;

	bool m_Vent;

	void revivePet();
	void ressWithProbe(float angle, float dist, float JumpDelay);
	void stopTimers(){timer->stop(); tabSpamTimer->stop(); m_rotTimers->stop(); 
	enemiesAttackingTimer->stop(); selfBuffTimer->stop(); sellItemsTimer->stop();targetTimeOut->stop();channeledAbilityOn = false;}

protected slots:

	void timerFinished();
	void tabTimerSlot();
	void attackMoveSlot();
	void checkLife();
	void selfBuff();
	void sellItemsSlot();
	void targetTimeOutSlot();
	void nextTargetTimeOutSlot();
	void abilityTriggeredSlot(int index,const abilityData *ability);
};

