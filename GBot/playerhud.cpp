#include "playerhud.h"
#include "baseSettings.h"
#include <Windows.h>
#include "c:\progetti\include\swlib.h"

PlayerHud::PlayerHud(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_TranslucentBackground,true);
	setAttribute(Qt::WA_ShowWithoutActivating);
	//this->setWindowTitle(randomizeName());
	
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog);
	setSizeGripEnabled(false);

	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->restoreDlgGeometry(this);

	long exStyle = GetWindowLong(winId(), GWL_EXSTYLE) | WS_EX_NOACTIVATE;
	SetWindowLong(winId(), GWL_EXSTYLE, exStyle);

	moveState = false;
	StartPosX = StartPosY = 0;

	hpGeometry = new QRect(ui.hpLabel->geometry());
	energyGeometry = new QRect(ui.energylabel->geometry());
}

PlayerHud::~PlayerHud()
{

}

void PlayerHud::mousePressEvent( QMouseEvent * event )
{
	if (event->button() == Qt::LeftButton)
	{
		StartPosX = event->pos().x();
		StartPosY = event->pos().y();
		moveState = true;
	}
}

void PlayerHud::mouseMoveEvent( QMouseEvent * event )
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
}

void PlayerHud::mouseReleaseEvent( QMouseEvent * event )
{
	if(moveState)
	{
		moveState = false;
		DialogBaseSettings *settings = DialogBaseSettings::get_instance();
		settings->saveDlgGeometry(this);
	}
}

void PlayerHud::setData(MobData *mData, float len)
{
	
	//int lenght = hpGeometry->width()/mData->heroData.realMaxHP;
	//hpGeometry->setWidth(lenght * mData->heroData.hp);
	//ui.hpLabel->setGeometry(*hpGeometry);
	ui.hpPointLabel->setText(QString("%1").arg((int)mData->heroData.hp));
	ui.energyPointLabel->setText(QString("%1").arg((int)mData->heroData.maxEnergy));
	ui.levelLabel->setText(QString("%1").arg(mData->heroData.Level));
	ui.infoLabel->setText(QString("Distance: %1").arg(len));

}

void PlayerHud::cancelData()
{
}
