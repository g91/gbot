#include "mapdlg.h"
#include <QFileDialog>
#include <qlineedit.h>
#include <qfileinfo.h>

MapDlg::MapDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

MapDlg::~MapDlg()
{

}

void MapDlg::openImageSlot()
{
	m_fileName = QFileDialog::getOpenFileName(this, tr("Open Map Image File"),QString("%1/Images/").arg(QApplication::applicationDirPath()),
                                                 tr("Image (*.png *.gif *.jpg *.bmp)"));

	if(!m_fileName.isEmpty())
	{
		QFileInfo fi(m_fileName);
		ui.lineEdit->setText(fi.fileName());
	}
}

void MapDlg::accept()
{
	mapName = ui.lineEdit_2->text();
	m_fileName =  ui.lineEdit->text();
	QDialog::accept();
}

void MapDlg::reject()
{
	mapName = "";
	m_fileName = "";

	QDialog::reject();
}
