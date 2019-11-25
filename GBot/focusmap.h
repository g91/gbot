#ifndef FOCUSMAP_H
#define FOCUSMAP_H

#include <QDialog>
#include "ui_focusmap.h"


class FocusMap : public QDialog
{
	Q_OBJECT

public:
	FocusMap(QWidget *parent = 0);
	~FocusMap();

	void setPlayerData(MobData *playerData,float angle, MobData *targetData){ui.MapGraphicsView->setPlayerData(playerData,angle,targetData);}
	void setMobsData(QVector<MobData> uData){ui.MapGraphicsView->setMobsData(uData);}
	void resetData(){ui.MapGraphicsView->resetData();}
	void mouseCustomEvent(QMouseEvent * event, int type);
	void setCurrentWayPoints(QVector<PointCoords> *points, int currentWayPoint){ui.MapGraphicsView->setCurrentWayPoints(points,currentWayPoint);}

private:
	Ui::FocusMapClass ui;

	bool moveState;
	bool resized;
	bool showLooking;

	int	StartPosX;
	int	StartPosY;

protected slots:
	void resizeEvent(QResizeEvent *);
	virtual void mousePressEvent( QMouseEvent * event );	
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	virtual void mouseDoubleClickEvent( QMouseEvent * event );



};

#endif // FOCUSMAP_H
