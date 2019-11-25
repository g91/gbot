#include "gbot.h"
#include "c:\progetti\include\swlib.h"
#include <qmessagebox.h>
#include <qgraphicswidget.h>
#include <qlineedit.h>
#include <time.h>
#include "WorldSimulator.h"
#include <math.h>
#include "baseSettings.h"
#include <QValidator>
#include <qbuffer.h>
#include "focusmap.h"
#include "gbothud.h"
#include "pvebotsetup.h"
#include "rotationconfigdlg.h"

HHOOK kb_hook = 0;
bool StickKey = false;
bool TeleportOnTargetKey = false;
bool HyperJumpKey = false;
bool ForwardJumpKey = false;
bool BackJumpKey = false;
bool AppShow = false;
bool EnableSpeed = false;

bool SpeedhackPressed = false;

unsigned char StickKeyVal = 0;
unsigned char TeleportOnTargetKeyVal = 0;
unsigned char HyperJumpKeyVal = 0;
unsigned char ForwardJumpKeyVal = 0;
unsigned char BackJumpKeyVal = 0;
unsigned char SpeedHackKeyVal = 0;

float HyperJumpLength = 0;
float HyperJumpHeight = 0;
float ForwardJumpLength = 0;
float BackJumpLength = 0;
float StickDistance = 0;
float StickJumpDelayTime = 1500;
float SpeedHackVal = 1.1f;

QString bannerMsg = "GBot Swtor Edition v1.4.1 Beta.                                  Visit us at www.gamebotdev.com.                          ";



LRESULT CALLBACK keyb_proc(int code, WPARAM w, LPARAM l)
{	

	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)l;
	if(w ==WM_KEYDOWN)
	{
		if(p->vkCode == StickKeyVal)
		{
			StickKey = !StickKey;
		}
		else if(p->vkCode == TeleportOnTargetKeyVal)
		{
			TeleportOnTargetKey = true;
		}
		else if(p->vkCode == HyperJumpKeyVal)
		{
			HyperJumpKey = true;
		}
		else if(p->vkCode == ForwardJumpKeyVal)
		{
			ForwardJumpKey = true;
		}
		else if(p->vkCode == BackJumpKeyVal)
		{
			BackJumpKey = true;
		}
		else if(p->vkCode == VK_F12)
		{
			AppShow = !AppShow;
		}
		else if(p->vkCode == SpeedHackKeyVal)
		{
			SpeedhackPressed = true;
			EnableSpeed = !EnableSpeed;
		}
	}
	return CallNextHookEx (kb_hook, code, w, l);

	return 0;

}

GBot::GBot(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	currentTrayIconMsg = "";
	companionSearched = 0;
	pSystem = 0;
	oldTragetID = 0;
	mapItem = 0;
	targetOn = false;
	m_FocusMap = 0;
	m_GBotHud = 0;
	QStringList list;
	list.push_back(QString("231294784290462"));
	m_sharedMemory = new QSharedMemory(list[0],this);

	bool moveState = false;
	int	StartPosX = 0;
	int	StartPosY = 0;
	currentBannerTextPosition = 10;

	ui.groupBox_4->raise();
	ui.groupBox_5->raise();
	ui.groupBox_5->raise();

#ifndef _HACKS_
	ui.groupBox_5->setTitle("Advanced hacks (disabled)");
#endif

	//RotationConfigDlg dlg(this);
	//dlg.exec();

	setAttribute(Qt::WA_TranslucentBackground,true);
	setAttribute(Qt::WA_ShowWithoutActivating);
	//this->setWindowTitle(randomizeName());
	
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog);
	setSizeGripEnabled(false);

	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->restoreDlgGeometry(this);

	//long exStyle = GetWindowLong(winId(), GWL_EXSTYLE) | WS_EX_NOACTIVATE;
	//SetWindowLong(winId(), GWL_EXSTYLE, exStyle);

	/*
	ui.lineEdit_2->setValidator(new QIntValidator(3,60));
	ui.lineEdit_6->setValidator(new QIntValidator(1,33));
	ui.lineEdit_7->setValidator(new QIntValidator(1,12));
	ui.lineEdit_9->setValidator(new QIntValidator(1,33));
	ui.lineEdit_10->setValidator(new QIntValidator(1,33));
	*/

	kb_hook = SetWindowsHookEx (WH_KEYBOARD_LL,&keyb_proc,GetModuleHandle (NULL),0);

	
	readConfigIni();
	createActions();
	createTrayIcon();

	scrollTextTimer = new QTimer(this);
	connect(scrollTextTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	scrollTextTimer->start(100);

	SharedMemoryTimer = new QTimer(this);
	connect(SharedMemoryTimer, SIGNAL(timeout()), this, SLOT(sharedMemoryTimerSlot()));
	SharedMemoryTimer->start(3000);
	
	srand ( time(0) );

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		 this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->show();
	QIcon icon(QPixmap(QString::fromUtf8(":/GBot/Resources/gbot.png")));
	setWindowIcon(icon);
	trayIcon->setIcon(icon);

	m_ProcMan = new ProcessManager(this,"Star Wars™: The Old Republic™","swtor.exe");
	//m_ProcMan = new ProcessManager(this,"Calcolatrice","calc.exe");
	
	connect(m_ProcMan,SIGNAL(Signal_SendData(QEvent *)),this,SLOT(dataReadFromThread(QEvent *)));
	/*if(!retMsg.isEmpty())
	{
		QMessageBox::critical(this,tr("Critical error!"),retMsg);
		QApplication::quit();
		exit(0);
	}
	*/

	//scene = new QGraphicsScene(ui.graphicsView);
	//ui.graphicsView->setScene(scene);

	//scene->setSceneRect(QRectF(-75,-75,75,75));
	

	//wSimulator = new WorldSimulator(scene);

	m_FocusMap = new FocusMap(this);

	m_ProcMan->start();
	/*
	MobData localPlayer;
	localPlayer.baseData.mob_x = -42;
	localPlayer.baseData.mob_y = 100;

	QVector<MobData> mobs;
	MobData mob1;
	MobData mob2;
	MobData mob3;
	MobData mob4;

	mob1.mobAddress = 1044;
	mob1.baseData.mob_x = -426;
	mob1.baseData.mob_y = 900;
	mob1.isPlayer = true;
	mob1.baseData.angle = 45;
	mob1.heroData.hp = 8767;
	mob1.heroData.energy = 100;
	mob1.baseData.id = 3000;
	mob1.isValid = true;

	mob2.mobAddress = 101;
	mob2.baseData.mob_x = -426;
	mob2.baseData.mob_y = 900;
	mob2.isPlayer = true;
	mob2.baseData.angle = 25;
	mob2.heroData.hp = 8767;
	mob2.heroData.energy = 100;
	mob2.baseData.id = 3000;
	mob2.isValid = true;

	mob3.mobAddress = 102;
	mob3.baseData.mob_x = -440;
	mob3.baseData.mob_y = -500;
	mob3.isPlayer = false;

	mob4.mobAddress = 103;
	mob4.baseData.mob_x = -480;
	mob4.baseData.mob_y = -600;
	mob4.isPlayer = false;

	mobs.push_back(mob1);
	mobs.push_back(mob2);
	mobs.push_back(mob3);
	mobs.push_back(mob4);

	m_FocusMap->setPlayerData(&localPlayer,48.0,&mob1);
	
	m_FocusMap->setMobsData(mobs);
	*/
	//setMapImage("Hutta.png");
	
	//m_GBotHud = new GBotHud(0,0,this);
	//m_GBotHud->show();
}

GBot::~GBot()
{
	UnhookWindowsHookEx(kb_hook);
}

void GBot::setMapImage(QString imageName)
{
	if(mapItem)
		scene->removeItem(mapItem);

	QString m_imageName = QString("%1/Images/%2").arg(QApplication::applicationDirPath()).arg(imageName);

	QPixmap *pix = new QPixmap();

	scene->setSceneRect(pix->rect());
	pix->load(m_imageName);
	int w = pix->width();
	mapItem = new QGraphicsPixmapItem();
	mapItem->setPixmap(*pix);
	mapItem->setZValue(0.0);
	scene->addItem(mapItem);
	
	mapItem->show();
}

void GBot::resizeEvent( QResizeEvent * event )
{
	//if(mapItem)
	//	ui.graphicsView->fitInView(mapItem,Qt::KeepAspectRatio);

	QDialog::resizeEvent(event);
}

void GBot::dataReadFromThread(QEvent *event)
{
	if(AppShow && !isVisible())
		show();
	else if(!AppShow && isVisible())
		hide();
	if(event->type() == ERROR_EVENT)
	{
		ErrorMsg *qev = (ErrorMsg *)event;
		QString msg = qev->getErrorText();
		QMessageBox::critical(this,"Error!",msg);
		QApplication::quit();
	}
	else if(event->type() == MESSAGE_EVENT)
	{
		CommunicationMsg *qev = (CommunicationMsg *)event;
		QString msg = qev->getMsgText();
		int msgNum = qev->getMsgNum();

		if(currentTrayIconMsg != msg && msgNum == 0)
		{
			trayIcon->showMessage("GBot", msg);
			currentTrayIconMsg = msg;
		}
		else if(msgNum == PROCESS_READ_EVENT)
		{
			MobData localPlayer = m_ProcMan->getLocalPlayerData();

			if(IsIconic(*m_ProcMan->getClientHwnd()))
			{
				m_ProcMan->enable();
				m_FocusMap->hide();
				if(m_GBotHud)
					m_GBotHud->hide();
				return;
			}

			if(msg == "No Data" || !localPlayer.isValid)
			{
				companionSearched = 0;
				m_ProcMan->enable();
				m_FocusMap->hide();
				if(m_GBotHud)
					m_GBotHud->hide();
				m_FocusMap->resetData();

				QString mapName = m_ProcMan->getCurrentMapName();
				if(m_currentMapName != mapName)
				{
					m_currentMapName = mapName;
					//if(pSystem)
					//	pSystem->reset();
				}
				return;
			}

			if(!pSystem)
			{
				pSystem = new PathSystem(m_ProcMan->getClientHwnd(), m_ProcMan);
				m_GBotHud = new GBotHud(m_ProcMan->getClientHwnd(),pSystem,this);
				pSystem->setSetupBotDlg(m_GBotHud->getSetupBotDlg());
			}

			if(m_GBotHud->isHidden())
				m_GBotHud->show();

			if(m_FocusMap->isHidden())
				m_FocusMap->show();

			MobData target =  m_ProcMan->getCurrentTarget();
			QVector<MobData> mobs = m_ProcMan->getMobs();
	        
			LocalPlayer lPlayer = m_ProcMan->getPlayerExtraData();

			//search for companion
			if(companionSearched == 0 && m_GBotHud->isPetOptionEnabled())
			{
				companionSearched = getCompanion(&mobs);				
			}

			if(companionSearched)
				pSystem->setCompanion(&mobs[companionSearched]);

			float angle;
			m_ProcMan->getPlayerAngle(&angle);

			m_FocusMap->setPlayerData(&localPlayer,angle,&target);
			m_FocusMap->setMobsData(mobs);

			PointCoords coord;
			coord.x = localPlayer.baseData.mob_x;
			coord.y = localPlayer.baseData.mob_y;
			coord.z = localPlayer.baseData.mob_z;

			m_GBotHud->setLocalPlayerPosition(coord,angle,StickJumpDelayTime);

			mv_WayPoints = pSystem->getFollowPathPoint();
			int currentWayPoint  = pSystem->getCurrentFollowPathPoint();

			m_FocusMap->setCurrentWayPoints(mv_WayPoints, currentWayPoint);

			pSystem->setCurrentTargetData(&lPlayer, &mobs[0],&target,angle,StickDistance,StickJumpDelayTime);				
		
			if(!target.isValid)
			{
				//pSystem->setCurrentTargetData(0,0,0,0,0,0);
				
				if(targetOn)
				{
					pSystem->cancelTarget();
					targetOn = false;
				}
			}
			else if(target.isValid && StickKey)
			{
				//if(GetForegroundWindow() != *m_ProcMan->getClientHwnd())
				//	SetForegroundWindow(*m_ProcMan->getClientHwnd());
				m_GBotHud->setTextInfo("Stickied on target");
				targetOn = true;
				PointCoords coord;
				coord.x = target.baseData.mob_x;
				coord.y = target.baseData.mob_y;
				coord.z = target.baseData.mob_z;

				PointCoords localPlayerCoord;
				localPlayerCoord.x = localPlayer.baseData.mob_x;
				localPlayerCoord.y = localPlayer.baseData.mob_y;
				localPlayerCoord.z = localPlayer.baseData.mob_z;

				float angle;
				m_ProcMan->getPlayerAngle(&angle);				
				pSystem->followTargetPosition(&localPlayerCoord,angle,&coord,StickDistance,StickJumpDelayTime,true);
				
			}
			else if(targetOn)
			{
					pSystem->cancelTarget();
					targetOn = false;
					m_GBotHud->setTextInfo("Stick Off");
			}

			if(TeleportOnTargetKey && target.isValid)
			{
				PointCoords coord;
				coord.x = target.baseData.mob_x;
				coord.y = target.baseData.mob_y;
				coord.z = target.baseData.mob_z;

				PointCoords localPlayerCoord;
				localPlayerCoord.x = localPlayer.baseData.mob_x;
				localPlayerCoord.y = localPlayer.baseData.mob_y;
				localPlayerCoord.z = localPlayer.baseData.mob_z;

#ifdef _HACKS_
				m_ProcMan->teleport(&localPlayerCoord, &coord);
#endif
				TeleportOnTargetKey = false;
			}
			if(HyperJumpKey)
			{
				PointCoords coord;
				coord.x = target.baseData.mob_x;
				coord.y = target.baseData.mob_y;
				coord.z = target.baseData.mob_z;

				PointCoords localPlayerCoord;
				localPlayerCoord.x = localPlayer.baseData.mob_x;
				localPlayerCoord.y = localPlayer.baseData.mob_y;
				localPlayerCoord.z = localPlayer.baseData.mob_z;

				float angle;
				m_ProcMan->getPlayerAngle(&angle);

#ifdef _HACKS_
				HyperJump(&localPlayerCoord,angle);
#endif

				HyperJumpKey = false;
			}
			else if(ForwardJumpKey)
			{
				PointCoords coord;
				coord.x = target.baseData.mob_x;
				coord.y = target.baseData.mob_y;
				coord.z = target.baseData.mob_z;

				PointCoords localPlayerCoord;
				localPlayerCoord.x = localPlayer.baseData.mob_x;
				localPlayerCoord.y = localPlayer.baseData.mob_y;
				localPlayerCoord.z = localPlayer.baseData.mob_z;

				float angle;
				m_ProcMan->getPlayerAngle(&angle);
#ifdef _HACKS_
				ForwardJump(&localPlayerCoord,angle);
#endif
				ForwardJumpKey = false;
			}
			else if(BackJumpKey)
			{
				PointCoords coord;
				coord.x = target.baseData.mob_x;
				coord.y = target.baseData.mob_y;
				coord.z = target.baseData.mob_z;

				PointCoords localPlayerCoord;
				localPlayerCoord.x = localPlayer.baseData.mob_x;
				localPlayerCoord.y = localPlayer.baseData.mob_y;
				localPlayerCoord.z = localPlayer.baseData.mob_z;

				float angle;
				m_ProcMan->getPlayerAngle(&angle);
#ifdef _HACKS_
				BackJump(&localPlayerCoord,angle);
#endif

				BackJumpKey = false;
			}
			
			if(SpeedhackPressed)
			{

				SpeedhackPressed = false;

#ifdef _HACKS_
				if(EnableSpeed)
					m_ProcMan->setSpeed(SpeedHackVal);
				else
					m_ProcMan->resetSpeed();
#endif

			}
			


			m_ProcMan->enable();

		}
	}
}

void GBot::iconActivated(QSystemTrayIcon::ActivationReason reason)
 {
     switch (reason) {
     case QSystemTrayIcon::DoubleClick:
		 if(!isVisible())
		 {
			show();
			AppShow = true;
		 }
		 else
		 {
			 hide();
			 AppShow = false;
		 }
         break;
     default:
         ;
     }
 }

void GBot::closeEvent(QCloseEvent *event)
 {
     if (trayIcon->isVisible()) 
	 {
         hide();
         event->ignore();
     }
 }

void GBot::createActions()
 {
     restoreAction = new QAction(tr("&Show"), this);
     connect(restoreAction, SIGNAL(triggered()), this, SLOT(showHideSlot()));

     quitAction = new QAction(tr("&Quit"), this);
     connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
 }

 void GBot::createTrayIcon()
 {
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(restoreAction);
     trayIconMenu->addSeparator();
     trayIconMenu->addAction(quitAction);

     trayIcon = new QSystemTrayIcon(this);
     trayIcon->setContextMenu(trayIconMenu);
 }

 void GBot::timerUpdate()
 {
	 QString tmpMessage = bannerMsg.left(currentBannerTextPosition);
	 ui.bannerLabel->setText(tmpMessage);

	 currentBannerTextPosition++;
	 if(currentBannerTextPosition >= bannerMsg.length())
		currentBannerTextPosition = 10;
 }

 void GBot::sharedMemoryTimerSlot()
 {
	if (m_sharedMemory->isAttached())
		m_sharedMemory->detach();

	if(m_sharedMemory->attach())
	{
		if(map_Adresses.size() > 1)
			return;

		QString data;
		m_sharedMemory->lock();
		QBuffer buffer;
		QDataStream in(&buffer);
		buffer.setData((char *)m_sharedMemory->constData(),m_sharedMemory->size());
		buffer.open(QBuffer::ReadOnly);
		in >> data;
		m_sharedMemory->unlock();
		parseSharedMemory(data);
		if(bannerMsg.contains("(Not Activated)"))
			bannerMsg = "GBot Swtor Edition v1.4.1 Beta.                                  Visit us at www.gamebotdev.com.                            ";
	}
	else
	{
		map_Adresses.clear();
		m_ProcMan->clearAddresses();
		if(!bannerMsg.contains("(Not Activated)"))
			bannerMsg = "GBot Swtor Edition v1.4.1 Beta (Not Activated).                                  Visit us at www.gamebotdev.com.                          Download Gbd License Client to activate it!                      ";
	}
 }

void GBot::parseSharedMemory(const QString &data)
{
	QStringList list1 = data.split("*");
	for(int i = 2; i < list1.size(); i++)
	{
		QStringList list2 = list1[i].split("=");
		if(list2.size() == 2)
		{
			unsigned int address = list2[1].remove("0X").toUInt(0,16);
			map_Adresses.insert(list2[0],address);
		}
	}

	m_ProcMan->setAddresses(map_Adresses);
}

 bool GBot::HyperJump(PointCoords *current_coord, float angle)
 {
	 PointCoords Coord1; //coordinata salto

	 Coord1.x = current_coord->x;
	 Coord1.y = current_coord->y;
	 Coord1.z = current_coord->z+10.0;

	 QLineF line(Coord1.x,Coord1.y,Coord1.x+10,Coord1.y+10);
	 line.setAngle(angle);
	 line.setLength(20);

	 Coord1.x = line.p2().x();
	 Coord1.y = line.p2().y();
	

	 return m_ProcMan->teleport(current_coord, &Coord1);
 }

 bool GBot::ForwardJump(PointCoords *current_coord, float angle)
 {
	 PointCoords Coord1;

	 Coord1.x = current_coord->x;
	 Coord1.y = current_coord->y;
	 Coord1.z = current_coord->z+1.0f;

	 QLineF line(Coord1.x,Coord1.y,Coord1.x+10,Coord1.y+10);
	 line.setAngle(angle);
	 line.setLength(20);

	 Coord1.x = line.p2().x();
	 Coord1.y = line.p2().y();
	

	 return m_ProcMan->teleport(current_coord, &Coord1);
 }

 bool GBot::BackJump(PointCoords *current_coord, float angle)
 {
	 PointCoords Coord1;

	 Coord1.x = current_coord->x;
	 Coord1.y = current_coord->y;
	 Coord1.z = current_coord->z+1.0f;

	 QLineF line(Coord1.x,Coord1.y,Coord1.x+10,Coord1.y+10);
	 line.setAngle(angle);
	 line.setLength(-20);

	 Coord1.x = line.p2().x();
	 Coord1.y = line.p2().y();
	

	 return m_ProcMan->teleport(current_coord, &Coord1);
 }

 void GBot::showHideSlot()
 {
	if(AppShow && !isVisible())
	{
		show();
		
	}
	else if(!AppShow && isVisible())
	{
		AppShow = false;
		hide();
	}

	AppShow = !AppShow;
	
 }

 void GBot::saveHacksConfigSlot()
 {
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->setApplicationData("HacksConfig","StickKey",ui.lineEdit->text());
	settings->setApplicationData("HacksConfig","TeleportOnTargetKey",ui.lineEdit_4->text());
	settings->setApplicationData("HacksConfig","HyperJumpKeyKey",ui.lineEdit_5->text());
	settings->setApplicationData("HacksConfig","ForwardJumpKey",ui.lineEdit_8->text());
	settings->setApplicationData("HacksConfig","BackJumpKey",ui.lineEdit_11->text());
	settings->setApplicationData("HacksConfig","HyperJumpLength",ui.lineEdit_6->text());
	settings->setApplicationData("HacksConfig","HyperJumpHeight",ui.lineEdit_7->text());
	settings->setApplicationData("HacksConfig","ForwardJumpLength",ui.lineEdit_9->text());
	settings->setApplicationData("HacksConfig","BackJumpLength",ui.lineEdit_10->text());
	settings->setApplicationData("HacksConfig","StickDistance",ui.lineEdit_2->text());
	settings->setApplicationData("HacksConfig","StickJumpDelayTime",ui.jumpDelayEdit->text());

	settings->setApplicationData("HacksConfig","SpeedHackKey",ui.lineEdit_12->text());
	settings->setApplicationData("HacksConfig","SpeedHackVal",QString("%1").arg((float)ui.spinBox->value()/100.0));

	readConfigIni();

	QMessageBox::information(this,"Hack Configuration", "Configuration saved");
 }

 void GBot::readConfigIni()
 {
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();

	if(!settings->exists("HacksConfig","StickKey"))
		settings->createDefaultIni();

	QString key = settings->readApplicationData("HacksConfig","StickKey");
	ui.lineEdit->setText(QString("%1").arg(key));
	StickKeyVal = settings->getCharCode(key);
	key = settings->readApplicationData("HacksConfig","TeleportOnTargetKey");
	ui.lineEdit_4->setText(QString("%1").arg(key));
	TeleportOnTargetKeyVal = settings->getCharCode(key);
	key = settings->readApplicationData("HacksConfig","HyperJumpKeyKey");
	ui.lineEdit_5->setText(QString("%1").arg(key));
	HyperJumpKeyVal = settings->getCharCode(key);
	key = settings->readApplicationData("HacksConfig","ForwardJumpKey");
	ui.lineEdit_8->setText(QString("%1").arg(key));
	ForwardJumpKeyVal = settings->getCharCode(key);
	key = settings->readApplicationData("HacksConfig","BackJumpKey");
	ui.lineEdit_11->setText(QString("%1").arg(key));
	BackJumpKeyVal = settings->getCharCode(key);
	key = settings->readApplicationData("HacksConfig","SpeedHackKey");
	ui.lineEdit_12->setText(QString("%1").arg(key));
	SpeedHackKeyVal = settings->getCharCode(key);

	HyperJumpLength = settings->readApplicationData("HacksConfig","HyperJumpLength").toFloat();
	HyperJumpHeight = settings->readApplicationData("HacksConfig","HyperJumpHeight").toFloat();
	ForwardJumpLength = settings->readApplicationData("HacksConfig","ForwardJumpLength").toFloat();
	BackJumpLength = settings->readApplicationData("HacksConfig","BackJumpLength").toFloat();
	StickDistance = settings->readApplicationData("HacksConfig","StickDistance").toFloat();
	StickJumpDelayTime  = settings->readApplicationData("HacksConfig","StickJumpDelayTime").toFloat();
	SpeedHackVal = settings->readApplicationData("HacksConfig","SpeedHackVal").toFloat();

	ui.lineEdit_6->setText(QString("%1").arg(HyperJumpLength));
	ui.lineEdit_7->setText(QString("%1").arg(HyperJumpHeight));
	ui.lineEdit_9->setText(QString("%1").arg(ForwardJumpLength));
	ui.lineEdit_10->setText(QString("%1").arg(BackJumpLength));
	ui.lineEdit_2->setText(QString("%1").arg(StickDistance));
	ui.jumpDelayEdit->setText(QString("%1").arg(StickJumpDelayTime));
	ui.spinBox->setValue(SpeedHackVal*100);
 }

 void GBot::mousePressEvent( QMouseEvent * event )
{

	if (event->button() == Qt::LeftButton)
	{
		StartPosX = event->pos().x();
		StartPosY = event->pos().y();
		moveState = true;
	}

	//QDialog::mousePressEvent(event);
	
}

void GBot::mouseMoveEvent( QMouseEvent * event )
{

	int x1, y1 = 0;

	if (moveState)
	{
		x1 = event->pos().x() - StartPosX;
		y1 = event->pos().y() - StartPosY;
		int xn = this->x() + x1;
		int yn = this->y() + y1;
		move(xn, yn);
	}

	//QDialog::mouseMoveEvent(event);
}

void GBot::mouseReleaseEvent( QMouseEvent * event )
{
	
	if(moveState)
	{
		moveState = false;
		DialogBaseSettings *settings = DialogBaseSettings::get_instance();
		settings->saveDlgGeometry(this);
	}

	//QDialog::mouseReleaseEvent(event);
}

void GBot::botSetupSlot()
{
	
	PveBotSetup pveBot(this);
	if(pveBot.exec() == QDialog::Accepted)
	{
		if(pSystem)
			pSystem->reloadConfigIni();
	}
}

void GBot::rotationSlot()
{
	RotationConfigDlg dlg(this);
	dlg.exec();
}

int GBot::getCompanion(QVector<MobData> *mobs)
{
	int index = 0;
	for(int i = 1; i < mobs->size(); i++)
	{
		float distance = QLineF(mobs->at(i).baseData.mob_x, mobs->at(i).baseData.mob_y, mobs->at(0).baseData.mob_x, mobs->at(0).baseData.mob_y).length();
		if(distance <= 3)
		{
			return i;
		}
	}

	return index;
}

