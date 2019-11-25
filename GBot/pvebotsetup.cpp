#include "pvebotsetup.h"
#include "baseSettings.h"
#include <QMouseEvent>
#include <Windows.h>
#include <qcombobox.h>
#include <qcheckbox.h>

bool Ascending(const RotationAbility &s1, const RotationAbility &s2)
{
	return s1.energyRequired >= s2.energyRequired;
}

PveBotSetup::PveBotSetup(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	moveState = false;
	StartPosX = StartPosY = 0;

	setAttribute(Qt::WA_TranslucentBackground,true);
	//setAttribute(Qt::WA_ShowWithoutActivating);
	//this->setWindowTitle(randomizeName());

	setFixedSize(size());
	
	setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog);
	setSizeGripEnabled(false);

	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->restoreDlgGeometry(this);

	//long exStyle = GetWindowLong(winId(), GWL_EXSTYLE) | WS_EX_NOACTIVATE;
	//SetWindowLong(winId(), GWL_EXSTYLE, exStyle);

	loadInidata();
}

PveBotSetup::~PveBotSetup()
{

}

void PveBotSetup::mousePressEvent( QMouseEvent * event )
{
	if (event->button() == Qt::LeftButton)
	{
		StartPosX = event->pos().x();
		StartPosY = event->pos().y();
		moveState = true;
	}
}

void PveBotSetup::mouseMoveEvent( QMouseEvent * event )
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

void PveBotSetup::mouseReleaseEvent( QMouseEvent * event )
{
	if(moveState)
	{
		moveState = false;
		DialogBaseSettings *settings = DialogBaseSettings::get_instance();
		settings->saveDlgGeometry(this);
	}
}

void PveBotSetup::accept()
{
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();

	settings->setApplicationData("PveBotConfig","MinimumLocalPlayerHealth",QString("%1").arg(ui.spinBox->value()));
	settings->setApplicationData("PveBotConfig","MinimumLocalPlayerEnergy",QString("%1").arg(ui.spinBox_2->value()));
	settings->setApplicationData("PveBotConfig","MinimumCompanionHealth",QString("%1").arg(ui.spinBox_3->value()));
	settings->setApplicationData("PveBotConfig","MinimumLifeForSteamPack",QString("%1").arg(ui.spinBox_4->value()));

	settings->setApplicationData("PveBotConfig","BotMaxDistance",ui.maxDistanceLineEdit->text());
	settings->setApplicationData("PveBotConfig","ZLevelDistance",ui.ZLevelLineEdit->text());

	m_config.clear();
	loadInidata();

	QDialog::accept();
}

void PveBotSetup::loadInidata()
{
	m_config.mv_abilities.clear();
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();

	if(!settings->exists("MinimumLocalPlayerHealth","OpeningAttack"))
	{
		QDialog::accept();
	}

	ui.spinBox->setValue(settings->readApplicationData("PveBotConfig","MinimumLocalPlayerHealth").toInt());
	ui.spinBox_2->setValue(settings->readApplicationData("PveBotConfig","MinimumLocalPlayerEnergy").toInt());
	ui.spinBox_3->setValue(settings->readApplicationData("PveBotConfig","MinimumCompanionHealth").toInt());
	ui.spinBox_4->setValue(settings->readApplicationData("PveBotConfig","MinimumLifeForSteamPack").toInt());

	ui.maxDistanceLineEdit->setText(settings->readApplicationData("PveBotConfig","BotMaxDistance"));
	ui.ZLevelLineEdit->setText(settings->readApplicationData("PveBotConfig","ZLevelDistance"));

	m_config.lifeStimPackhealth = ui.spinBox_4->value();

	m_config.minimumCopmanionHealth = ui.spinBox_3->value();
	m_config.minimumPlayerHealh = ui.spinBox->value();
	m_config.minimumPlayerEnergy = ui.spinBox_2->value();


	m_config.BotMaxDistance = ui.maxDistanceLineEdit->text().toFloat();
	m_config.BotMaxZValue = ui.ZLevelLineEdit->text().toFloat();

	if(m_config.BotMaxDistance == 0)
		m_config.BotMaxDistance = 45;

	if(m_config.BotMaxZValue == 0)
		m_config.BotMaxZValue = 6;

	//qSort(m_config.mv_abilities.begin(),m_config.mv_abilities.end(),Ascending);
}