#ifndef GBOT_H
#define GBOT_H

#include <QtGui/QMainWindow>
#include "ui_gbot.h"
#include <QSystemTrayIcon>
#include <qaction.h>
#include <qmenu.h>
#include <QCloseEvent>
#include <qresizeevent>
#include <qtimer.h>
#include <QSharedMemory>
#include "PathSystem.h"

class LocalPlayer;
class ProcessManager;
class QGraphicsScene;
class QGraphicsPixmapItem;
class WorldSimulator;
class FocusMap;
class GBotHud;
class GBot : public QDialog
{
	Q_OBJECT

public:
	GBot(QWidget *parent = 0);
	~GBot();

private:
	Ui::GBotClass ui;

	ProcessManager *m_ProcMan;

public slots:
		void dataReadFromThread(QEvent *event);
private slots:
     void iconActivated(QSystemTrayIcon::ActivationReason reason);
protected slots:
	void closeEvent(QCloseEvent *event);
	void resizeEvent( QResizeEvent * event );
	void timerUpdate();
	void sharedMemoryTimerSlot();

	virtual void saveHacksConfigSlot();
	virtual void showHideSlot();
	virtual void botSetupSlot();
	virtual void rotationSlot();

	virtual void mousePressEvent( QMouseEvent * event );	
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );

private:
	void createActions();
	void createTrayIcon();
	void readConfigIni();
	void setMapImage(QString imageName);

	bool HyperJump(PointCoords *current_coord, float angle);
	bool ForwardJump(PointCoords *current_coord, float angle);
	bool BackJump(PointCoords *current_coord, float angle);

	int getCompanion(QVector<MobData> *mobs);

	QAction *restoreAction;
	QAction *quitAction;

	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	QString currentTrayIconMsg;

	LocalPlayer *m_LocalPlayer;
	QGraphicsScene *scene;
	QGraphicsPixmapItem *pItem;
	QGraphicsPixmapItem *localPlayerItem;
	QGraphicsPixmapItem *mapItem;
	WorldSimulator *wSimulator;

	QTimer *scrollTextTimer;

	PathSystem *pSystem;
	uint64 oldTragetID;
	bool targetOn;

	FocusMap *m_FocusMap;
	GBotHud *m_GBotHud;

	QVector<PointCoords> *mv_WayPoints;

	bool moveState;
	int	StartPosX;
	int	StartPosY;

	int currentBannerTextPosition;
	QString m_currentMapName;
	int companionSearched;

	QTimer *SharedMemoryTimer;
	QSharedMemory *m_sharedMemory;
	QMap<QString,unsigned int> map_Adresses;
	
	void parseSharedMemory(const QString &data);
};

#endif // GBOT_H
