#ifndef ENTERNAMEDLG_H
#define ENTERNAMEDLG_H

#include <QDialog>
#include "ui_enternamedlg.h"

class enterNameDlg : public QDialog
{
	Q_OBJECT

public:
	enterNameDlg(QWidget *parent = 0);
	~enterNameDlg();

	QString getNameEntered(){return ui.lineEdit->text();}

private:
	Ui::enterNameDlg ui;
};

#endif // ENTERNAMEDLG_H
