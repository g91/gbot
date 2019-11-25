#pragma once

#include "types.h"
#include <Psapi.h>
#include <qstring.h>
#include <QThread>
#include <qobject>
#include <qevent.h>
#include <qvector.h>

#define ERROR_EVENT   QEvent::User+1
#define MESSAGE_EVENT   QEvent::User+2

#define PROCESS_READ_EVENT QEvent::User+101

#define IS_POINTER(a) \
	( (a)!=0 && ( (a)&0x3 ) == 0 )

class ErrorMsg : public QEvent
{
public:
    ErrorMsg(QString Msg)
		: QEvent(static_cast<QEvent::Type>(ERROR_EVENT)),_Msg(Msg){};
	QString getErrorText(){return _Msg;}
private:
	QString _Msg;
};

class MobData
{
public:
	MobData(){clear();}
	void clear(){heroAddress = mobAddress = 0; isPlayer = false; isValid = false;}
	uint32 heroAddress;
	uint32 mobAddress;
	bool isPlayer;
	bool isValid;
	MobLinkedList baseData;
	HeroData heroData;

	bool operator==(const MobData &group)
	{
		return group.baseData.id == baseData.id;
	}
};

class LocalPlayer
{
public:
	LocalPlayer();
	void clear();

	uint64 playerID;
	LocalPlayerData playerData;
	PointCoords targetCoords;
	HeroData heroData;
};

class CommunicationMsg : public QEvent
{
public:
    CommunicationMsg(QString Msg, int msgNum)
		: QEvent(static_cast<QEvent::Type>(MESSAGE_EVENT)),_Msg(Msg), _MsgNum(msgNum){};
	QString getMsgText(){return _Msg;}
	int getMsgNum(){return _MsgNum;}
private:
	int _MsgNum;
	QString _Msg;
};

class ProcessManager : public QThread
{
	Q_OBJECT
public:
	ProcessManager(QObject *receiver, QString windowName, QString procName);
	bool isOpen();
	void sendKeyDown( UINT vkey );
	void sendKeyUp( UINT vkey );
	virtual void run();
	void enable(){waitForMemory = false;}

	MobData getLocalPlayerData(){if(mv_Mobs.size() > 0) return mv_Mobs[0];}
	MobData getCurrentTarget(){return targetMob;};
	QVector<MobData> getMobs(){return mv_Mobs;};
	QString getCurrentMapName(){return m_mapName;}
	LocalPlayer getPlayerExtraData(){return m_localPlayerData;}
	
signals:
	void Signal_SendData(QEvent *ev);

private:
	void displayModuleInfo();
	void getModuleInfo();
	bool openWindowProc( const char* windowName );
	QString getProcessName( DWORD processID );
	void openDataProc( const char* procName );

	void open( const char* windowName, const char *procName );
	void close();
	void addPrivileges();
	void getPlayerCoords( PointCoords* coords);
	void getTargetCoords( PointCoords* coords);
	bool getPlayerAngle( float* angle );
	bool getMobLinkData( MobLinkedList* mobData );
	bool getPlayerdata();
	bool read( addr address, void* str, int size );
	bool readFromSwtorExeBase( addr address, void* str, int size );
	bool getHeroNodeByID( uint64 id, addr* heroNode );
	addr getHeroData( uint64 id, uint32* exp, uint32* maxExp, uint32* level,
		float* force, float* hp);
	bool getPlayerID( uint64* id );
	addr getSwtorExeBase();
	bool getPlayerData(LocalPlayerData *lData);
	addr getMemAddressFromPointers(QVector<int> pointers, bool addAppBase);
	QString getMapName();
	bool getAllMobs();

	void readData();

	DWORD windowProcessID;
	HWND windowProcessHWND;
	bool m_isOpen;

	ModuleInfo moduleInfo[1024];
	addr swtorBase;

	HANDLE dataProcessHandle;
	QObject *m_receiver;

	int m_refresh;
	bool waitForMemory;
	bool pauseThread;
	QString m_windowName;
	QString m_procName;

	LocalPlayer m_localPlayerData;
	QVector<MobData> mv_Mobs;
	MobData targetMob;
	QString m_mapName;
};


