#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QtGui/QMainWindow>
#include "ui_mapeditor.h"

class MapEditor : public QMainWindow
{
	Q_OBJECT

public:
	MapEditor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MapEditor();

protected slots:
	void resizeEvent( QResizeEvent * event );
	void splitterMovedSlot(int,int);

	virtual void addWayPointSlot();
	virtual void loadMapSlot();
	virtual void saveMapSlot();
	virtual void newMapSlot();

private:
	Ui::MapEditorClass ui;

	bool loadMap(QString fileName);
	bool saveMap(QString fileName);
};

#endif // MAPEDITOR_H
