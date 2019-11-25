#include "gbothud.h"
#include "baseSettings.h"
#include <Windows.h>
#include "c:\progetti\include\swlib.h"
#include <QMouseEvent>
#include "PathSystem.h"
#include <qfiledialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include "setupbotdlg.h"
#include "rotationconfigdlg.h"

GBotHud::GBotHud(HWND *swHwnd, PathSystem *pathSystem, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	m_swHwnd = swHwnd;

	s_dlg = new SetupBotDlg(swHwnd,this);

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
	m_pathSystem = pathSystem;
	PathMoving = false;

	ui.infoLabel1->setText("Record new path");
	ui.infoLabel2->setText("");

	loadConfig();
	configChanged();

	r_dlg = new RotationConfigDlg();
	QStringList list = r_dlg->getConfigEntries();

	ui.configComboBox->addItem(tr("None"));
	for(int i = 0; i < list.size(); i++)
		ui.configComboBox->addItem(list[i]);

}

GBotHud::~GBotHud()
{

}

void GBotHud::setLocalPlayerPosition(PointCoords coords, float angle,float jumpDelay)
{
	lCoords = coords;
	m_angle = angle;
	m_jumpDelay = jumpDelay;

	if(PathMoving)
	{
		m_pathSystem->followPath("",&lCoords,m_angle,m_jumpDelay);
		ui.infoLabel1->setText(QString("Bot Started. Waypoint %1").arg(m_pathSystem->getCurrentFollowPathPoint()));
	}

}

void GBotHud::mousePressEvent( QMouseEvent * event )
{
	if (event->button() == Qt::LeftButton)
	{
		StartPosX = event->pos().x();
		StartPosY = event->pos().y();
		moveState = true;
	}
}

void GBotHud::mouseMoveEvent( QMouseEvent * event )
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

void GBotHud::mouseReleaseEvent( QMouseEvent * event )
{
	if(moveState)
	{
		moveState = false;
		DialogBaseSettings *settings = DialogBaseSettings::get_instance();
		settings->saveDlgGeometry(this);
	}
}

void GBotHud::startSlot()
{
	PathMoving = true;
	m_pathSystem->startBot();
	m_pathSystem->followPath("",&lCoords,m_angle,m_jumpDelay);
	SetForegroundWindow(*m_swHwnd);
}

void GBotHud::stopSlot()
{
	PathMoving = false;
	m_pathSystem->stopBot();
	ui.infoLabel1->setText("Bot Stopped");
	SetForegroundWindow(*m_swHwnd);
}

void GBotHud::addWSlot()
{
	m_pathSystem->addWayPoint(lCoords);
	ui.infoLabel1->setText(QString("Path waypoints added (%1)").arg(m_pathSystem->getNumWaypoints()));
}

void GBotHud::resetSlot()
{
	m_pathSystem->reset();
	ui.infoLabel1->setText("Record new path");
}

void GBotHud::loadSlot()
{
	QDir dir(QString("%1/Paths/").arg(QApplication::applicationDirPath()));
	if(!dir.exists())
		dir.mkdir(QString("%1/Paths/").arg(QApplication::applicationDirPath()));
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Path File"),QString("%1/Paths/").arg(QApplication::applicationDirPath()),
                                                 tr("Path File (*.pts)"));

	if(!fileName.isEmpty())
		if(!m_pathSystem->loadPath(fileName))
			ui.infoLabel1->setText("Wrong Path file!");
		else
			ui.infoLabel1->setText(QString("Path Loaded (%1 Waypoints)").arg(m_pathSystem->getNumWaypoints()));
}

void GBotHud::saveSlot()
{
	QDir dir(QString("%1/Paths/").arg(QApplication::applicationDirPath()));
	if(!dir.exists())
		dir.mkdir(QString("%1/Paths/").arg(QApplication::applicationDirPath()));
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Path File"),
                            QString("%1/Paths/").arg(QApplication::applicationDirPath()),
                            tr("Path File (*.pts)"));

	if(!fileName.isEmpty())
		if(!m_pathSystem->savePath(fileName))
			ui.infoLabel1->setText("Error during Path saving!");
		else
			ui.infoLabel1->setText("Path correctly saved.");
}

void GBotHud::configChanged()
{
	if(!m_pathSystem)
		return;
	m_pathSystem->setBotConfig(ui.comboBox->currentIndex(),ui.petCheckBox->isChecked(),ui.pveCheckBox->isChecked(),ui.pvpCheckBox->isChecked(), ui.lootCheckBox->isChecked(), ui.sellCheckBox->isChecked(), ui.reviveCheckBox->isChecked());
	saveConfig();
}

void GBotHud::loadConfig()
{
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	ui.comboBox->setCurrentIndex(settings->readApplicationData("GBotHudConfig","Type").toInt());
	ui.petCheckBox->setChecked(settings->readApplicationData("GBotHudConfig","PetActive").toInt());
	ui.pveCheckBox->setChecked(settings->readApplicationData("GBotHudConfig","PveBotActive").toInt());
	ui.pvpCheckBox->setChecked(settings->readApplicationData("GBotHudConfig","PvpBotActive").toInt());
	ui.lootCheckBox->setChecked(settings->readApplicationData("GBotHudConfig","PveBotLoot").toInt());
	ui.sellCheckBox->setChecked(settings->readApplicationData("GBotHudConfig","PveSell").toInt());
	ui.reviveCheckBox->setChecked(settings->readApplicationData("GBotHudConfig","PveRessPet").toInt());

	if(settings->exists("WizardSetup-Sell","x1"))
		ui.sellCheckBox->setEnabled(true);
	if(settings->exists("WizardSetup-Ress","x1"))
		ui.reviveCheckBox->setEnabled(true);
}

void GBotHud::saveConfig()
{
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->setApplicationData("GBotHudConfig","Type",QString("%1").arg(ui.comboBox->currentIndex()));
	settings->setApplicationData("GBotHudConfig","PetActive",QString("%1").arg(ui.petCheckBox->isChecked()));
	settings->setApplicationData("GBotHudConfig","PveBotActive",QString("%1").arg(ui.pveCheckBox->isChecked()));
	settings->setApplicationData("GBotHudConfig","PvpBotActive",QString("%1").arg(ui.pvpCheckBox->isChecked()));
	settings->setApplicationData("GBotHudConfig","PveBotLoot",QString("%1").arg(ui.lootCheckBox->isChecked()));
	settings->setApplicationData("GBotHudConfig","PveSell",QString("%1").arg(ui.sellCheckBox->isChecked()));
	settings->setApplicationData("GBotHudConfig","PveRessPet",QString("%1").arg(ui.reviveCheckBox->isChecked()));
}

void GBotHud::pvpClickedSlot()
{
	if(ui.pvpCheckBox->isChecked())
		ui.pveCheckBox->setChecked(false);
}

void GBotHud::pveClickedSlot()
{
	if(ui.pveCheckBox->isChecked())
		ui.pvpCheckBox->setChecked(false);
}

void GBotHud::wizardSlot()
{
	s_dlg->show();
}

void GBotHud::configComboChangedSlot(int)
{
	if(ui.configComboBox->currentText() == tr("None"))
		return;

	mv_abilities = r_dlg->getRotationAbilities(ui.configComboBox->currentText());

	if(!m_pathSystem)
		return;

	bool vent =  r_dlg->isVentClass(ui.configComboBox->currentText());

	m_pathSystem->setBotAbilities(&mv_abilities,vent);
	SetForegroundWindow(*m_swHwnd);
}
