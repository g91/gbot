#include "setupbotdlg.h"
#include "basesettings.h"
#include <Windows.h>

HHOOK mo_hook = 0;
bool waitForLeft = false;
bool waitForRight = false;
int xCoord = 0;
int yCoord = 0;

LRESULT CALLBACK mouse_proc (int code, WPARAM w, LPARAM l)
{
	if(!waitForLeft && !waitForRight)
		return CallNextHookEx (mo_hook, code, w, l);

	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)l;
	if(w == WM_RBUTTONDOWN && waitForRight)
	{
		xCoord = pMouseStruct->pt.x;
		yCoord = pMouseStruct->pt.y;
		waitForRight = false;
		return CallNextHookEx (mo_hook, code, w, l);
	}
	//else if(w == WM_LBUTTONDBLCLK)
	//	return 1;
	//else if(w == WM_MOUSEMOVE && moveStop)
	//	return 1;
	else if(w == WM_LBUTTONDOWN && waitForLeft)
	{
		xCoord = pMouseStruct->pt.x;
		yCoord = pMouseStruct->pt.y;
		waitForLeft = false;
		return CallNextHookEx (mo_hook, code, w, l);
	}
		

	return CallNextHookEx (mo_hook, code, w, l);

}

SetupBotDlg::SetupBotDlg(HWND *swHwnd, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	m_swHwnd = swHwnd;
	loadConfig();

	phase = 0;

	sellTimer = new QTimer(this);
	connect(sellTimer,SIGNAL(timeout()),this,SLOT(sellTimerSlot()));

	ressTimer = new QTimer(this);
	connect(ressTimer,SIGNAL(timeout()),this,SLOT(ressTimerSlot()));
}

SetupBotDlg::~SetupBotDlg()
{
	UnhookWindowsHookEx(mo_hook);
}

void SetupBotDlg::accept()
{
}
void SetupBotDlg::sellSlot()
{
	if(GetForegroundWindow() != *m_swHwnd)
			SetForegroundWindow(*m_swHwnd);
	phase = 0;
	mo_hook = SetWindowsHookEx (WH_MOUSE_LL,&mouse_proc,GetModuleHandle (NULL),0);
	sellTimer->start(500);
	
}
void SetupBotDlg::ressSlot()
{
	if(GetForegroundWindow() != *m_swHwnd)
			SetForegroundWindow(*m_swHwnd);
	phase = 0;
	mo_hook = SetWindowsHookEx (WH_MOUSE_LL,&mouse_proc,GetModuleHandle (NULL),0);
	ressTimer->start(500);
}


void SetupBotDlg::sellTimerSlot()
{
	if(phase == 0)
	{
		ui.messageLabel->setText("Left click on your Open Crew Window");
		phase++;
		waitForLeft = true;
	}

	if(phase == 1 && !waitForLeft)
	{
		ui.lineEdit->setText(QString("%1").arg(xCoord));
		ui.lineEdit_2->setText(QString("%1").arg(yCoord));
		ui.messageLabel->setText("Left click on sell low quality items");
		phase++;
		waitForLeft = true;
	}
	if(phase == 2 && !waitForLeft)
	{
		ui.lineEdit_4->setText(QString("%1").arg(xCoord));
		ui.lineEdit_3->setText(QString("%1").arg(yCoord));
		ui.messageLabel->setText("Left click on accept to sell items button");
		phase++;
		waitForLeft = true;
	}
	if(phase == 3 && !waitForLeft)
	{
		ui.lineEdit_9->setText(QString("%1").arg(xCoord));
		ui.lineEdit_10->setText(QString("%1").arg(yCoord));
		phase++;
	}
	if(phase == 4)
	{
		ui.messageLabel->setText("Procedure completed!");
		UnhookWindowsHookEx(mo_hook);
		sellTimer->stop();
		saveSellConfig();
	}

	
}

void SetupBotDlg::ressTimerSlot()
{
	if(phase == 0)
	{
		ui.messageLabel->setText("Right click on your Companion portrait");
		phase++;
		waitForRight = true;
	}

	if(phase == 1 && !waitForRight)
	{
		ui.lineEdit_5->setText(QString("%1").arg(xCoord));
		ui.lineEdit_6->setText(QString("%1").arg(yCoord));
		ui.messageLabel->setText("Left click on Dismiss companion menu");
		phase++;
		waitForLeft = true;
	}
	if(phase == 2 && !waitForLeft)
	{
		ui.lineEdit_7->setText(QString("%1").arg(xCoord));
		ui.lineEdit_8->setText(QString("%1").arg(yCoord));
		phase++;
	}
	if(phase == 3)
	{
		ui.messageLabel->setText("Procedure completed!");
		UnhookWindowsHookEx(mo_hook);
		ressTimer->stop();
		saveRessConfig();
	}
}

void SetupBotDlg::saveSellConfig()
{
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();

	settings->setApplicationData("WizardSetup-Sell","x1",ui.lineEdit->text());
	settings->setApplicationData("WizardSetup-Sell","y1",ui.lineEdit_2->text());

	settings->setApplicationData("WizardSetup-Sell","x2",ui.lineEdit_4->text());
	settings->setApplicationData("WizardSetup-Sell","y2",ui.lineEdit_3->text());

	settings->setApplicationData("WizardSetup-Sell","x3",ui.lineEdit_9->text());
	settings->setApplicationData("WizardSetup-Sell","y3",ui.lineEdit_10->text());
}

void SetupBotDlg::saveRessConfig()
{
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();

	settings->setApplicationData("WizardSetup-Ress","x1",ui.lineEdit_5->text());
	settings->setApplicationData("WizardSetup-Ress","y1",ui.lineEdit_6->text());

	settings->setApplicationData("WizardSetup-Ress","x2",ui.lineEdit_7->text());
	settings->setApplicationData("WizardSetup-Ress","y2",ui.lineEdit_8->text());
}

void SetupBotDlg::loadConfig()
{
	mv_SellProcedure.clear();
	mv_RessProcedure.clear();

	DialogBaseSettings *settings = DialogBaseSettings::get_instance();

	ui.lineEdit->setText(settings->readApplicationData("WizardSetup-Sell","x1"));
	ui.lineEdit_2->setText(settings->readApplicationData("WizardSetup-Sell","y1"));
	ui.lineEdit_4->setText(settings->readApplicationData("WizardSetup-Sell","x2"));
	ui.lineEdit_3->setText(settings->readApplicationData("WizardSetup-Sell","y2"));
	ui.lineEdit_9->setText(settings->readApplicationData("WizardSetup-Sell","x3"));
	ui.lineEdit_10->setText(settings->readApplicationData("WizardSetup-Sell","y3"));

	autoProcedure ap;
	ap.leftClick = true;
	ap.pos.x = ui.lineEdit->text().toInt();
	ap.pos.y = ui.lineEdit_2->text().toInt();
	ap.waitMilliseconds = 1000;
	mv_SellProcedure.push_back(ap);

	ap.leftClick = true;
	ap.pos.x = ui.lineEdit_4->text().toInt();
	ap.pos.y = ui.lineEdit_3->text().toInt();
	ap.waitMilliseconds = 1000;
	mv_SellProcedure.push_back(ap);

	ap.leftClick = true;
	ap.pos.x = ui.lineEdit_9->text().toInt();
	ap.pos.y = ui.lineEdit_10->text().toInt();
	ap.waitMilliseconds = 1000;
	mv_SellProcedure.push_back(ap);

	ui.lineEdit_5->setText(settings->readApplicationData("WizardSetup-Ress","x1"));
	ui.lineEdit_6->setText(settings->readApplicationData("WizardSetup-Ress","y1"));
	ui.lineEdit_7->setText(settings->readApplicationData("WizardSetup-Ress","x2"));
	ui.lineEdit_8->setText(settings->readApplicationData("WizardSetup-Ress","y2"));

	ap.leftClick = false;
	ap.pos.x = ui.lineEdit_5->text().toInt();
	ap.pos.y = ui.lineEdit_6->text().toInt();
	ap.waitMilliseconds = 1000;
	mv_RessProcedure.push_back(ap);

	ap.leftClick = true;
	ap.pos.x = ui.lineEdit_7->text().toInt();
	ap.pos.y = ui.lineEdit_8->text().toInt();
	ap.waitMilliseconds = 1000;
	mv_RessProcedure.push_back(ap);

	ap.leftClick = true;
	ap.pos.x = ui.lineEdit_5->text().toInt();
	ap.pos.y = ui.lineEdit_6->text().toInt();
	ap.waitMilliseconds = 1000;
	mv_RessProcedure.push_back(ap);
}