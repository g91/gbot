#ifndef MAPDLG_H
#define MAPDLG_H

#include <QDialog>
#include "ui_mapdlg.h"

class MapDlg : public QDialog
{
	Q_OBJECT

public:
	MapDlg(QWidget *parent = 0);
	~MapDlg();

	QString m_fileName;
	QString mapName;

private:
	Ui::MapDlg ui;

protected slots:
	virtual void openImageSlot();
	virtual void accept();
	virtual void reject();

};

#endif // MAPDLG_H
