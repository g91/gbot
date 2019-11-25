#include "mapeditor.h"
#include <QFileDialog>
#include <QMessageBox>
#include "mapdlg.h"

MapEditor::MapEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	QObject::connect(ui.splitter, SIGNAL(splitterMoved(int,int)),this,SLOT(splitterMovedSlot(int,int)));
}

MapEditor::~MapEditor()
{

}

void MapEditor::addWayPointSlot()
{
}

void MapEditor::loadMapSlot()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Map File"),QString("%1/Maps/").arg(QApplication::applicationDirPath()),
                                                 tr("Map File (*.map)"));

	if(!fileName.isEmpty())
		if(!loadMap(fileName))
			QMessageBox::critical(this,"Map Loading","Error during map parsing");
}

void MapEditor::saveMapSlot()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Map File"),
                            QString("%1/Maps/").arg(QApplication::applicationDirPath()),
                            tr("Map File (*.map)"));

	if(!fileName.isEmpty())
			if(!saveMap(fileName))
			QMessageBox::critical(this,"Map Saving","Error during map file save");
}

void MapEditor::splitterMovedSlot(int,int)
{
	ui.mapGraphicsView->resize();
}

void MapEditor::resizeEvent( QResizeEvent * event )
{
	ui.mapGraphicsView->resize();
	QMainWindow::resizeEvent(event);
}

bool MapEditor::loadMap(QString fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
		return false;

	QVector<WayPoint *> wPoints;

	QString mapName = file.readLine(512).simplified();

	while(!file.atEnd())
	{
		WayPoint *wP = new WayPoint(0);
		QString line = file.readLine(512);
		QStringList list = line.split(';');

		if(list.size() != 4)
		{
			file.close();
			return false;
		}
		wP->setPos(list[2].toFloat(), list[3].toFloat());
		wP->type = (WayPoint::e_type)list[1].toInt();
		wP->index = list[0].toInt();
		wPoints.push_back(wP);

	}

	file.close();

	ui.mapGraphicsView->deleteWayPoints();
	ui.mapGraphicsView->setMapImage(mapName);
	ui.mapGraphicsView->setWayPoints(wPoints);

	return true;
}

bool  MapEditor::saveMap(QString fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))
		return false;

	QVector<WayPoint *> wPoints = ui.mapGraphicsView->getWayPoints();

	QString mapline = QString("%1\n").arg(ui.mapGraphicsView->getImageName());
	file.write(mapline.toAscii(),mapline.length());

	for(int i = 0; i < wPoints.size(); i++)
	{
		QString line = QString("%1;%2;%3;%4\n").arg(wPoints.at(i)->index)
			.arg(wPoints.at(i)->Type)
			.arg(wPoints.at(i)->pos().x())
			.arg(wPoints.at(i)->pos().y());

		file.write(line.toAscii(),line.length());
	}

	file.close();

	QMessageBox::information(this,"Map Saving","Map saved");

	return true;
}

void  MapEditor::newMapSlot()
{
	MapDlg dlg(this);
	if(dlg.exec() == QDialog::Accepted)
	{
		ui.mapGraphicsView->deleteWayPoints();
		ui.mapGraphicsView->setMapImage(dlg.m_fileName);
	}
}
