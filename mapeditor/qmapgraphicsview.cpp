#include "qmapgraphicsview.h"
#include <qapplication.h>
#include <QMouseEvent>
#include <qmenu.h>
#include <qfileinfo.h>

WayPoint::WayPoint(QMapGraphicsView *parent)
	:QGraphicsPixmapItem()
{
	clear();

	m_parent = parent;
}

void WayPoint::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	menu = new QMenu;
	menu->addAction("Remove");
	menu->addAction("Change");
	menu->popup(event->screenPos());
     
	QObject::connect(menu, SIGNAL(triggered(QAction *)),
	m_parent, SLOT(triggered(QAction *)));
}

void WayPoint::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	if(event->button() == Qt::LeftButton)
		m_parent->wayPointLeftClicked(this);
}

QMapGraphicsView::QMapGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
	addWayPointActive = true;
	mapItem = 0;
	scene = new QGraphicsScene(this);
	setScene(scene);

	//setMapImage("the-pit-big.jpg");
}

QMapGraphicsView::~QMapGraphicsView()
{

}

void QMapGraphicsView::setMapImage(QString imageName)
{
	if(mapItem)
		scene->removeItem(mapItem);

	m_imageName = QString("%1/Images/%2").arg(QApplication::applicationDirPath()).arg(imageName);

	QPixmap *pix = new QPixmap();
	pix->load(m_imageName);
	int w = pix->width();
	mapItem = new QGraphicsPixmapItem();
	mapItem->setPixmap(*pix);
	scene->addItem(mapItem);
	
	mapItem->show();
	resize();
}

QString  QMapGraphicsView::getImageName()
{
	QFileInfo fi(m_imageName);
	return fi.fileName();
}

void QMapGraphicsView::resize()
{
	if(mapItem)
		fitInView((QGraphicsItem *)mapItem,Qt::KeepAspectRatio);
}

void QMapGraphicsView::mousePressEvent ( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton && addWayPointActive)
	{
		QPixmap *pix = new QPixmap(QString::fromUtf8(":/MapEditor/Resources/LookIncon.PNG"));
		*pix = pix->scaled(25,25,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		WayPoint *tItem = new WayPoint(this);
		QPointF pmapped = mapToScene(event->pos().x(), event->pos().y());
		tItem->setPos(pmapped.x()-pix->width()/2,pmapped.y()-pix->height()/2);
		tItem->setZValue(1.0);
		tItem->setPixmap(*pix);

		mv_WayPoints.push_back(tItem);
		scene->addItem(tItem);
		tItem->show();
	}

	QGraphicsView::mousePressEvent(event);
}

void QMapGraphicsView::mouseReleaseEvent ( QMouseEvent * event )
{
	QGraphicsView::mouseReleaseEvent(event);
}

void QMapGraphicsView::triggered(QAction *action)
{
	if(action->text() == "Remove")
	{
	}
	else if(action->text() == "Change")
	{
	}
}

void QMapGraphicsView::wayPointLeftClicked(WayPoint *wPoint)
{
}

void QMapGraphicsView::setWayPoints(QVector<WayPoint *> wPoints)
{
	for(int i = 0; i < wPoints.size(); i++)
	{
		QPixmap *pix = new QPixmap(QString::fromUtf8(":/MapEditor/Resources/LookIncon.PNG"));
		*pix = pix->scaled(25,25,Qt::KeepAspectRatio,Qt::SmoothTransformation);
		WayPoint *tItem = new WayPoint(this);
		tItem->setZValue(1.0);
		tItem->setPixmap(*pix);
		int x = wPoints.at(i)->pos().x();
		tItem->setPos(wPoints.at(i)->pos());

		mv_WayPoints.push_back(tItem);
		scene->addItem(tItem);
		tItem->show();
	}
}

void QMapGraphicsView::deleteWayPoints()
{
	for(int i = 0; i < mv_WayPoints.size(); i++)
		scene->removeItem(mv_WayPoints.at(i));

	mv_WayPoints.clear();
}
