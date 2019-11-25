#ifndef FOCUSMAPVIEWS_H
#define FOCUSMAPVIEWS_H

#include <QGraphicsView>
#include <QMap>
#include "baseSettings.h"
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <qlabel.h>
#include "c:\progetti\include\swlib.h"

class FocusMapGraphicsView;
class FocusMap;
class PlayerHud;

class MobEntity : public QGraphicsPolygonItem
{
	public:
	enum EntityType{None, Player, Mob, mainPlayer,targetType};
	MobEntity(QGraphicsItem * parent = 0);
	~MobEntity();
	void setType(EntityType type);

	void setClassParent(FocusMapGraphicsView *parentGraphic){_parentGraphic = parentGraphic;}
	void setColor(QColor &c){color = c;}
	void drawContour(bool b){_drawCountour = b;}

	MobData mobData;
	LocalPlayer playerData;

protected:
	virtual void 	hoverEnterEvent  ( QGraphicsSceneHoverEvent * event );
	virtual void 	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	virtual void 	mousePressEvent  ( QGraphicsSceneMouseEvent * event );

private:
	FocusMapGraphicsView *_parentGraphic;
	QColor color;
	bool _drawCountour;
	bool oldDrawContour;
	bool itemClicked;
	EntityType m_type;
};

class FocusMapGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	FocusMapGraphicsView(QWidget *parent);
	~FocusMapGraphicsView();

	void itemHovered(MobEntity *entity);
	void cancelItemHovered();
	void itemClicked(int token, int playerIndex, MobEntity *entity);
	void itemCanceled();

	void setParent(FocusMap *parent){m_parent = parent;}

	void setPlayerData(MobData *playerData, float angle, MobData *targetData);
	void setMobsData(QVector<MobData> uData);
	void setCurrentWayPoints(QVector<PointCoords> *points, int currentWayPoint);

	void setFit();
	void resetData();

	void resizeSprites();

	MobEntity *m_playerData;
	QVector<MobEntity *> mv_Mobs;
	QVector<QGraphicsEllipseItem *> mv_wPoints;
	bool firstDraw;
	QRectF *rect;
	PlayerHud *playerHud;
	int m_currentWayPoint;

	QTimer *playerHudTimer;
	unsigned __int64 m_CurrenttargetID;

private:
	QGraphicsScene *scene;
	QPointF _point;
	FocusMap *m_parent;

	int zoomSize;

	bool resizeButtonPressed;
	bool showPlayers;
	bool showMobs;

	int findIndexOf(unsigned long id);
	QVector<QColor> mv_colors;
	QPixmap *mobPix;
	QPixmap *playerPix;
	float divFactor;

protected slots:
	virtual void mousePressEvent( QMouseEvent * event );	
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );

	void playerHudTimerFinished();
	
};

#endif // FOCUSMAPVIEWS_H
