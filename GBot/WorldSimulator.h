#pragma once
#include <qvector.h>
#include <QString>
#include <QGraphicsEllipseItem>

class QGraphicsScene;
class QGraphicsPixmapItem;
class SimPlayer
{
public:
	SimPlayer(){movx = movy = 0.0;index = 0;line = 0;}

	QGraphicsPixmapItem *pItem;
	QGraphicsLineItem *line;
	float energy;
	float hp;
	int level;
	float angle;
	float movx;
	float movy;
	int index;
};

class SimMob
{
public:
	SimMob(){}

	QGraphicsPixmapItem *pItem;
};

class WorldSimulator
{
public:
	WorldSimulator(QGraphicsScene *scene);
	~WorldSimulator(void);

	void addPlayer();
	void addMob();
	void moveLocalPlayer(QPointF point, float angle);
	void moveTarget(QPointF point);
	QString getMapName();

	void movePlayers();
	void moveMobs();

private:
	QGraphicsScene *m_Scene;
	QVector<SimPlayer> mv_Players;
	QVector<SimMob> mv_Mobs;
	SimPlayer *player;
	SimPlayer *target;

};

