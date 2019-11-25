#include "WorldSimulator.h"
#include <qgraphicswidget.h>
#include <QGraphicsScene>
#include "c:\progetti\include\swlib.h"

WorldSimulator::WorldSimulator(QGraphicsScene *scene)
{
	m_Scene = scene;

	player = new SimPlayer();
	target = new SimPlayer();
	QPixmap *pix = new QPixmap(QString::fromUtf8(":/GBot/Resources/Down-Arrow-Icon.png"));
	*pix = pix->scaled(20,20,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	player->pItem = m_Scene->addPixmap(*pix);
	player->pItem->setZValue(1.0);

	QPixmap *pix2 = new QPixmap(QString::fromUtf8(":/GBot/Resources/target.jpg"));
	*pix2 = pix2->scaled(25,25,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	target->pItem = m_Scene->addPixmap(*pix2);
	target->pItem->setZValue(1.0);

	
}


WorldSimulator::~WorldSimulator(void)
{
}

void WorldSimulator::addPlayer()
{
	SimPlayer player;
	QPixmap *pix = new QPixmap(QString::fromUtf8(":/GBot/Resources/target.jpg"));
	*pix = pix->scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	QGraphicsPixmapItem *tItem  = new QGraphicsPixmapItem();
	player.hp = getIntRandomValue(1000,6000);
	player.energy = getIntRandomValue(0,500);
	player.level = (float)getIntRandomValue(1,50);
	player.angle = (float)getIntRandomValue(0,90);
	player.pItem = tItem;
	player.movx = (float)getIntRandomValue(-2,2);
	player.movy = (float)getIntRandomValue(-2,2);
	tItem->setPos(getIntRandomValue(50,400),getIntRandomValue(50,400));
	tItem->setZValue(1.0);
	tItem->setPixmap(*pix);

	m_Scene->addItem(tItem);


	mv_Players.push_back(player);
}

void WorldSimulator::addMob()
{
	SimMob mob;
	
	QPixmap *pix = new QPixmap(QString::fromUtf8(":/GBot/Resources/Looking.jpg"));
	*pix = pix->scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	QGraphicsPixmapItem *tItem  = new QGraphicsPixmapItem();
	
	tItem->setPos(getIntRandomValue(50,400),getIntRandomValue(50,400));
	tItem->setZValue(1.0);
	tItem->setPixmap(*pix);

	m_Scene->addItem(tItem);
	mob.pItem = tItem;
	mv_Mobs.push_back(mob);
}

void WorldSimulator::moveLocalPlayer(QPointF point, float angle)
{
	if(player->line)
		m_Scene->removeItem(player->line);

	//point.setX(point.x()+720);
	//point.setY(point.y()+550);
	

	player->pItem->setPos(point.x()- player->pItem->pixmap().width()/2, point.y()- player->pItem->pixmap().height()/2);
	QLineF tLine(0,0,30,30);
	tLine.setAngle(angle);
	player->line = m_Scene->addLine(tLine);
	player->line->setPos(point.x(), point.y());
}

void WorldSimulator::moveTarget(QPointF point)
{
	//point.setX(point.x()+720);
	//point.setY(point.y()+550);

	target->pItem->setPos(point.x()- target->pItem->pixmap().width()/2, point.y()- target->pItem->pixmap().height()/2);
}

void WorldSimulator::movePlayers()
{
	for(int i = 0; i < mv_Players.size(); i++)
	{
		mv_Players[i].pItem->moveBy(mv_Players[i].movx, mv_Players[i].movy);
	}
}

void WorldSimulator::moveMobs()
{
}
