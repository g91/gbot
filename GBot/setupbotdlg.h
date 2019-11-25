#ifndef SETUPBOTDLG_H
#define SETUPBOTDLG_H

#include <QDialog>
#include "ui_setupbotdlg.h"
#include <qtimer.h>
#include <qvector.h>
#include <Windows.h>

class autoProcedure
{
public:
	autoProcedure(){clear();}
	void clear(){leftClick = 0; pos.x = 0; pos.y = 0; waitMilliseconds = 0;}

	bool leftClick;
	POINT pos;
	int waitMilliseconds;
};

class SetupBotDlg : public QDialog
{
	Q_OBJECT

public:
	SetupBotDlg(HWND *swHwnd, QWidget *parent = 0);
	~SetupBotDlg();

	QVector<autoProcedure> getSellProcedure(){return mv_SellProcedure;}
	QVector<autoProcedure> getRessProcedure(){return mv_RessProcedure;}

private:
	Ui::SetupBotDlg ui;

	QTimer *sellTimer;
	QTimer *ressTimer;

	int phase;

	void saveSellConfig();
	void saveRessConfig();
	void loadConfig();

	QVector<autoProcedure> mv_SellProcedure;
	QVector<autoProcedure> mv_RessProcedure;

	HWND *m_swHwnd;

protected slots:
	virtual void accept();
	virtual void sellSlot();
	virtual void ressSlot();

	void sellTimerSlot();
	void ressTimerSlot();
};

#endif // SETUPBOTDLG_H
