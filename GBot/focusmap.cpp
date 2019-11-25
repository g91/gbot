#include "focusmap.h"
#include "baseSettings.h"
#include <QMouseEvent>
#include <Windows.h>

FocusMap::FocusMap(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	moveState = false;

	StartPosX = StartPosY = 0;

	setAttribute(Qt::WA_TranslucentBackground,true);
	setAttribute(Qt::WA_ShowWithoutActivating);
	//this->setWindowTitle(randomizeName());
	
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog);
	setSizeGripEnabled(false);

	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->restoreDlgGeometry(this);

	resized = false;

	long exStyle = GetWindowLong(winId(), GWL_EXSTYLE) | WS_EX_NOACTIVATE;
	SetWindowLong(winId(), GWL_EXSTYLE, exStyle);

	showLooking = false;

	ui.MapGraphicsView->setParent(this);
}

FocusMap::~FocusMap()
{

}

void FocusMap::resizeEvent(QResizeEvent *ev)
{
	ui.MapGraphicsView->setFit();
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->saveDlgGeometry(this);

	QDialog::resizeEvent(ev);
}

/*
void FocusMap::setPlayerData(QVector<PlayersData> *playersData,  int MainPlayerIndex, QVector<int> *slotsToShow, bool showMapDescr)
{
	if(this->isVisible() && !resized)
	{
		this->resize(width()-1,height());
		this->resize(width()+1,height());
		resized = true;

	}
	ui.MapGraphicsView->setPlayerData(playersData, MainPlayerIndex,slotsToShow,showMapDescr);
	//ui.NavigatorView->setPlayerData(playersData, mpData, MainPlayerIndex);
}
*/

void FocusMap::mouseDoubleClickEvent( QMouseEvent * event )
{
	if(event->button() == Qt::LeftButton)
	{
		if(!this->isMaximized())
			this->showMaximized();
		else
			this->showNormal();
	}

	QDialog::mouseDoubleClickEvent(event);
}

void FocusMap::mousePressEvent( QMouseEvent * event )
{

	if (event->button() == Qt::LeftButton)
	{
		StartPosX = event->pos().x();
		StartPosY = event->pos().y();
		moveState = true;
	}

	//QDialog::mousePressEvent(event);
	
}

void FocusMap::mouseMoveEvent( QMouseEvent * event )
{

	int x1, y1 = 0;

	if (moveState)
	{
		x1 = event->pos().x() - StartPosX;
		y1 = event->pos().y() - StartPosY;
		int xn = this->x() + x1;
		int yn = this->y() + y1;
		move(xn, yn);
	}

	//QDialog::mouseMoveEvent(event);
}

void FocusMap::mouseReleaseEvent( QMouseEvent * event )
{
	
	if(moveState)
	{
		moveState = false;
		DialogBaseSettings *settings = DialogBaseSettings::get_instance();
		settings->saveDlgGeometry(this);
	}

	//QDialog::mouseReleaseEvent(event);
}

void FocusMap::mouseCustomEvent(QMouseEvent * event, int type)
{
	if(type == 0)
		mousePressEvent(event);
	else if(type == 1)
		mouseMoveEvent(event);
	else if(type == 2)
		mouseReleaseEvent(event);
	else if(type == 3)
	{
		float divFactor = float((530+503)) / float((ui.MapGraphicsView->width()+ui.MapGraphicsView->height()));
		QPoint p1 = event->pos();
		p1.setX(p1.x()+24.0*divFactor);
		p1.setY(p1.x()+24.0*divFactor);
		int a = rect().bottomRight().x()- (rect().bottomRight().x()-p1.x());
		if(a >= 200)
			resize(a,a);
	}
}
