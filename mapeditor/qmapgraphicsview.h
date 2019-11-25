#ifndef QMAPGRAPHICSVIEW_H
#define QMAPGRAPHICSVIEW_H

#include <QGraphicsView>
#include <qgraphicsscene.h>
#include <qvector.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneContextMenuEvent>

class QMapGraphicsView;
class WayPoint : public QGraphicsPixmapItem
{
public:
	WayPoint(QMapGraphicsView *parent);

	enum e_type{None, Movement, Jump, AttackArea};
	void clear(){index = 0; type = None;}

	int index;
	e_type type;

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

private:
	QMenu *menu;
	QMapGraphicsView *m_parent;
};

class QMenu;
class QMapGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	QMapGraphicsView(QWidget *parent);
	~QMapGraphicsView();

	void resize();
	void wayPointLeftClicked(WayPoint *);
	inline QVector<WayPoint *> getWayPoints(){return mv_WayPoints;}
	void setWayPoints(QVector<WayPoint *> wPoints);
	void deleteWayPoints();
	QString getImageName();
	void setMapImage(QString imageName);

private:
	QGraphicsScene *scene;
	QGraphicsPixmapItem *mapItem;
	QString m_imageName;

	bool addWayPointActive;
	QVector<WayPoint *> mv_WayPoints;

protected slots:
	virtual void mousePressEvent ( QMouseEvent * event );
	virtual void mouseReleaseEvent ( QMouseEvent * event );
	void triggered(QAction *);
};

#endif // QMAPGRAPHICSVIEW_H
