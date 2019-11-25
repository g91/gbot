#include "focusmapviews.h"
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QSettings>
#include <QProgressBar>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include "focusmap.h"
#include "playerhud.h"
#include <QGraphicsEllipseItem>

int unitSizeMultiplier = 2300;

MobEntity::MobEntity(QGraphicsItem * parent)
	: QGraphicsPolygonItem(parent)
{
	setAcceptHoverEvents(true);
	_drawCountour = false;
	oldDrawContour = false;
	itemClicked = false;
}

void MobEntity::setType(EntityType type)
{
	if(m_type == type)
		return;

	m_type = type;

	if(type == mainPlayer)
	{
		int Size = 11;
		int midunitSize = 4;
		QPolygon polygon;
		polygon << QPoint(midunitSize, 0) << QPoint(Size-midunitSize,Size) << QPoint(0,Size) << QPoint(midunitSize, 0);
		setPolygon(polygon);
		setPen(QColor(Qt::green).light());
		setBrush(QBrush(QColor(Qt::green).dark(),Qt::SolidPattern));
	}
	else if(type == Player)
	{
		int Size = 11;
		int midunitSize = 4;
		QPolygon polygon;
		polygon << QPoint(midunitSize, 0) << QPoint(Size-midunitSize,Size) << QPoint(0,Size) << QPoint(midunitSize, 0);
		setPolygon(polygon);
		setPen(QColor(Qt::red).light());
		setBrush(QBrush(QColor(Qt::red).dark(),Qt::SolidPattern));
	}
	else if(type == Mob)
	{
		int Size = 8;
		QPolygon polygon;
		polygon << QPoint(0, 0) << QPoint(Size,0) << QPoint(Size,Size) << QPoint(0, Size) << QPoint(0, 0);
		setPolygon(polygon);
		setPen(QColor(Qt::blue).light());
		setBrush(QBrush(QColor(Qt::blue),Qt::SolidPattern));
	}
	else if(type == targetType)
	{
		int Size = 12;
		QPolygon polygon;
		polygon << QPoint(0, 0) << QPoint(Size,0) << QPoint(0, Size) << QPoint(Size,Size) << QPoint(0, 0);
		setPolygon(polygon);
		setPen(QColor(Qt::yellow).light());
		setBrush(QBrush(QColor(Qt::yellow).dark(),Qt::SolidPattern));
	}
}

MobEntity::~MobEntity()
{
}

void MobEntity::hoverEnterEvent  ( QGraphicsSceneHoverEvent * event )
{
	if(m_type == Player)
		_parentGraphic->itemHovered(this);
}

void MobEntity::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
	if(m_type == Player)
		_parentGraphic->cancelItemHovered();
}

void MobEntity::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	if(event->button() == Qt::LeftButton)
	{
		_parentGraphic->itemClicked(data(5).toUInt(),data(6).toUInt(),this);
	}
}

FocusMapGraphicsView::FocusMapGraphicsView(QWidget *parent)
: QGraphicsView(parent)
{

	scene = new QGraphicsScene(this);
	setStyleSheet("background: transparent");
	setScene(scene);	

	resizeButtonPressed = false;
	zoomSize = 400;
	showPlayers = showMobs = true;
	firstDraw = false;
	divFactor = 1.0;
	m_CurrenttargetID = 0;
	m_playerData = 0;
	
	rect = new QRectF(0,0,zoomSize,zoomSize);

	playerHud = new PlayerHud();

	playerHudTimer = new QTimer(this);
	connect(playerHudTimer,SIGNAL(timeout()),this,SLOT(playerHudTimerFinished()));

}

FocusMapGraphicsView::~FocusMapGraphicsView()
{	
}

int FocusMapGraphicsView::findIndexOf(unsigned long id)
{
	for(int i = 0; i < mv_Mobs.size(); i++)
	{
		if(mv_Mobs[i]->mobData.mobAddress == id)
		{
			return i;
		}
	}

	return -1;
}

void FocusMapGraphicsView::setPlayerData(MobData *playerData, float angle, MobData *targetData)
{
	if(targetData->isValid)
		m_CurrenttargetID = targetData->baseData.id;
	else
		m_CurrenttargetID = 0;

	if(!m_playerData)
	{
		m_playerData = new MobEntity();
		m_playerData->setType(MobEntity::mainPlayer);
		m_playerData->setZValue(1.0);
		scene->addItem(m_playerData);
	}

	rect->setRect(0,0,zoomSize,zoomSize);
	rect->moveCenter(QPointF(playerData->baseData.mob_x, playerData->baseData.mob_y));
	if(!firstDraw)
	{
		//firstDraw = true;
		//scene->addRect(rect);
		scene->setSceneRect(*rect);
	}
			
	angle = -angle;
	int centerx = m_playerData->boundingRect().center().x();
	int centery =  m_playerData->boundingRect().center().y();
	m_playerData->setZValue(2.0);
	m_playerData->mobData = *playerData;
	m_playerData->setPos(playerData->baseData.mob_x, playerData->baseData.mob_y);
	m_playerData->setTransform(QTransform().translate(centerx, centery).rotate(angle+90).translate(-centerx, -centery));

	fitInView(*rect,Qt::KeepAspectRatio);
}

void FocusMapGraphicsView::setMobsData(QVector<MobData> uData)
{
	for(int i = 0; i <  mv_Mobs.size(); i++)
	{
		mv_Mobs[i]->setData(1,0);
	}
	for(int i = 1; i < uData.size()-1; i++)
	{
		QLineF l(rect->center().x(), rect->center().y(),uData[i].baseData.mob_x,uData[i].baseData.mob_y);
		int index = findIndexOf(uData[i].mobAddress);		
		if(index >= 0)
		{
			
			if(l.length() >= zoomSize/2.8)
			{
				mv_Mobs[index]->setData(1,0);
				continue;
				
			}
			mv_Mobs[index]->setData(1,1);
			mv_Mobs[index]->setPos(uData[i].baseData.mob_x, uData[i].baseData.mob_y);

			if(uData[i].baseData.id != m_CurrenttargetID)
			{
				if(uData[i].isPlayer)
					mv_Mobs[index]->setType(MobEntity::Player);
				else
					mv_Mobs[index]->setType(MobEntity::Mob);
			}
			else
			{
				mv_Mobs[index]->setType(MobEntity::targetType);
			}

			if(mv_Mobs[index]->mobData.isPlayer)
			{
				float angle = uData[i].baseData.angle;
				angle = -angle - PI/2;
				while ( angle > PI )
				angle -= 2*PI;
				while (angle < -PI )
				angle += 2*PI;

				
				angle = angle = (angle * 180) / PI;
				
				if(angle < 0)
					angle = fabs(angle);
				else
					angle = 360-angle;

				int centerx = mv_Mobs[index]->boundingRect().center().x();
				int centery =  mv_Mobs[index]->boundingRect().center().y();
				angle = -angle;
				mv_Mobs[index]->setTransform(QTransform().translate(centerx, centery).rotate(angle+90).translate(-centerx, -centery));

				
			}

		}
		else if(l.length() <= zoomSize/2.8)
		{
			MobEntity *item;

			if(!uData[i].isPlayer && !showMobs)
				continue;
			if(uData[i].isPlayer && !showPlayers)
				continue;
			if(!uData[i].isPlayer)
			{
				item = new MobEntity();
				item->setType(MobEntity::Mob);
				item->setZValue(1.0);
			}
			else
			{
				item = new MobEntity();
				item->setType(MobEntity::Player);
				item->setZValue(1.5);
			}
			item->mobData = uData[i];
			item->setPos(uData[i].baseData.mob_x, uData[i].baseData.mob_y);
			scene->addItem(item);
			item->setData(1,1);
			item->setClassParent(this);
			mv_Mobs.push_back(item);
		}

		
	}

	//delete dead or out of range targets
	for(int i = 0; i < mv_Mobs.size(); i++)
	{
		if(mv_Mobs[i]->data(1) == 0 || (mv_Mobs[i]->mobData.isPlayer && !showPlayers) || (!mv_Mobs[i]->mobData.isPlayer && !showMobs))
		{
			scene->removeItem(mv_Mobs[i]);
			mv_Mobs.remove(i);
			i--;
		}
	}
	
}

void FocusMapGraphicsView::setCurrentWayPoints(QVector<PointCoords> *points, int currentWayPoint)
{
	m_currentWayPoint = currentWayPoint;

	//reset graphical path
	if(points->size() == 0)
	{
		for(int i = 0; i < mv_wPoints.size(); i++)
			scene->removeItem(mv_wPoints[i]);
		mv_wPoints.clear();
	}

	for(int i = 0; i < mv_wPoints.size(); i++)
	{
		QLineF l(rect->center().x(), rect->center().y(),mv_wPoints[i]->pos().x(),mv_wPoints[i]->pos().y());
		float len = l.length();
		if(len >= zoomSize/2.8)
			mv_wPoints[i]->hide();
		else
			mv_wPoints[i]->show();
			

		if(i == m_currentWayPoint)
		{
			mv_wPoints[i]->setPen(QPen(QColor(Qt::green).light()));
			mv_wPoints[i]->setBrush(QBrush(QColor(Qt::green)));
		}
		else
			mv_wPoints[i]->setPen(QPen(QColor(Qt::yellow)));
		mv_wPoints[i]->setBrush(QBrush(Qt::yellow,Qt::NoBrush));

	}

	if(mv_wPoints.size() == points->size() && mv_wPoints.size() > 0)
		return;

	for(int i = mv_wPoints.size(); i < points->size(); i++)
	{
		QGraphicsEllipseItem *item = scene->addEllipse(0,0,14,14,QPen(Qt::yellow));
		item->setZValue(2.0);
		item->setPos(points->at(i).x,points->at(i).y);
		mv_wPoints.push_back(item);
	}
}

void FocusMapGraphicsView::resetData()
{
	scene->clear();
	m_playerData = 0;
	mv_Mobs.clear();
	mv_wPoints.clear();
}

void FocusMapGraphicsView::setFit()
{	
	fitInView(*rect,Qt::KeepAspectRatio);
	divFactor = float((530+503)) / float((width()+height()));
	resizeSprites();
}

void FocusMapGraphicsView::mousePressEvent( QMouseEvent * event )
{
	
	float divFactor = float((530+503)) / float((width()+height()));
	QPointF p1(event->posF());
	p1.setX(p1.x()*divFactor);
	p1.setY(p1.y()*divFactor);
	float size = 60/divFactor;
	QRectF rect(0,0,size,size);
	rect.moveCenter(QPointF(455,425));
	if(rect.contains(p1))
		resizeButtonPressed = true;
	rect.moveCenter(QPointF(74,448));
	if(rect.contains(p1))
	{
		// +
		QGraphicsView::mousePressEvent(event);
		zoomSize-=20;
		return;
	}
	rect.moveCenter(QPointF(45,418));
	if(rect.contains(p1))
	{
		// -
		QGraphicsView::mousePressEvent(event);
		zoomSize+=20;
		return;
	}
	rect.moveCenter(QPointF(82,47));
	if(rect.contains(p1))
	{
		// []
		QGraphicsView::mousePressEvent(event);
		showPlayers = !showPlayers;
		return;
	}
	rect.moveCenter(QPointF(56,82));
	if(rect.contains(p1))
	{
		// O
		QGraphicsView::mousePressEvent(event);
		showMobs = !showMobs;
		return;
	}

	m_parent->mouseCustomEvent(event, 0);
	QGraphicsView::mousePressEvent(event);
}

void FocusMapGraphicsView::mouseMoveEvent( QMouseEvent * event )
{
	if(resizeButtonPressed)
		m_parent->mouseCustomEvent(event, 3);
	else
		m_parent->mouseCustomEvent(event, 1);

	QGraphicsView::mouseMoveEvent(event);
}

void FocusMapGraphicsView::mouseReleaseEvent( QMouseEvent * event )
{
	if(resizeButtonPressed)
	{
		resizeButtonPressed = false;
	}

	m_parent->mouseCustomEvent(event, 2);
	QGraphicsView::mouseReleaseEvent(event);	
}

void FocusMapGraphicsView::resizeSprites()
{
	float divFactor = 800.0 / float((width()+height()));
		float val = 14/divFactor;
		float val2 = 16/divFactor;

		if(val < 12)
			val = 12;
		if(val2 < 13)
			val2 = 13;
		if(val > 14)
			val = 14;
		if(val2 > 16)
			val2 = 16;
}


void FocusMapGraphicsView::itemHovered(MobEntity *entity)
{
	if(entity->mobData.isPlayer)
	{
		QLineF line(entity->mobData.baseData.mob_x, entity->mobData.baseData.mob_y
		, m_playerData->mobData.baseData.mob_x, m_playerData->mobData.baseData.mob_y);
		playerHud->setData(&entity->mobData,line.length());
		playerHud->show();
		playerHudTimer->start(4000);
	}
}

void FocusMapGraphicsView::cancelItemHovered()
{
	
}

void FocusMapGraphicsView::itemClicked(int token, int playerIndex, MobEntity *entity)
{
	
}

void FocusMapGraphicsView::itemCanceled()
{
	//cancelItemHovered();
}

void FocusMapGraphicsView::playerHudTimerFinished()
{
	playerHud->hide();
}