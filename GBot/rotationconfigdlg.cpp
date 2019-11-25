#include "rotationconfigdlg.h"
#include <QScrollArea>
#include <QTableWidget>
#include <qdir.h>
#include <qcombobox.h>
#include <qsettings.h>
#include <QDirIterator>
#include "enternamedlg.h"

Q_DECLARE_METATYPE(abilityData)

abilityDropLabel::abilityDropLabel(QWidget * parent, Qt::WindowFlags f)
	:QLabel(parent,f)
{
	setMouseTracking(true);
}

void abilityDropLabel::setText( const QString & s)
{
	if(!s.isEmpty())
		originalText = s;
	QLabel::setText(s);
	
}

void abilityDropLabel::setPixmap( const QPixmap &pix )
{
	if(!text().isEmpty())
		originalText = text();
	QLabel::setPixmap(pix);
}

void abilityDropLabel::clear()
{
	setPixmap(0);
	aData.clear();

	if(originalText.isEmpty())
		originalText = this->text();

	if(originalTooltip.isEmpty())
		originalTooltip = this->toolTip();
	
	setText(originalText);
	setToolTip(originalTooltip);

	
}

void abilityDropLabel::dragEnterEvent(QDragEnterEvent *event)
 {
	 if (event->mimeData()->text().count(";") == 8)
         event->acceptProposedAction();
 }

void abilityDropLabel::setAbilityData(abilityData data)
{
	aData = data;
	QPixmap pix(aData.imageName);
	setPixmap(pix);
}

void abilityDropLabel::dropEvent(QDropEvent *event)
 {
	QStringList list = event->mimeData()->text().split(";");

	if(list.size() != 9)
		return;

	if(list[2].isEmpty())
		return;

	aData.imageName = list[0] + ".png";
	aData.name = list[1];
	aData.type = list[2];
	aData.energy = list[3].toInt();
	aData.channeled = list[4].toInt();
	aData.range = list[5];
	aData.cooldown = list[6].toInt();
	aData.triggered = list[7];
	aData.descritpion = list[8];

	if(text().isEmpty())
		aData.key = originalText.at(0).toAscii();
	else
	{
		originalText = text();
		aData.key = text().at(0).toAscii();
	}

	if(aData.key == '-')
		aData.key = 189;
	if(aData.key == '+')
		aData.key = 187;


	QPixmap pix(aData.imageName);
	setPixmap(pix);

	event->acceptProposedAction();
 }

void abilityDropLabel::mouseMoveEvent ( QMouseEvent * ev )
{
	if(originalTooltip.isEmpty())
		originalTooltip = toolTip();
	setToolTip(QString("%1\n%2").arg(originalTooltip).arg(aData.name));
}

void abilityDropLabel::mousePressEvent ( QMouseEvent * ev )
{
	if(ev->button() == Qt::RightButton)
	{
		aData.clear();
		setPixmap(0);
		setText(originalText);
		setToolTip(originalTooltip);
	}
}

////////////////////////////////////////////////////////
abilityLabel::abilityLabel(abilityData abil, QWidget * parent, Qt::WindowFlags f)
	:QLabel(parent,f)
{
	setMouseTracking(true);
	aData = abil;
}

void abilityLabel::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
         dragStartPosition = event->pos();
		 dragStartPosition.setX(dragStartPosition.x()+rect().width()/2);
		 dragStartPosition.setY(dragStartPosition.y()+rect().height()/2);
	}
}

void abilityLabel::mouseMoveEvent ( QMouseEvent * event )
{
	if(this->toolTip().isEmpty())
	{
		if(aData.type.isEmpty())
		{
			setToolTip(tr("NOT IMPLEMENTED"));
			return;
		}

		QString tooltip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.gamebotdev.com\">\n"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n");
		tooltip += "p, li { white-space: pre-wrap; }\n";
		tooltip += "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n";
		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600; text-decoration: underline;\">%1</span></p>\n").arg(aData.name);
		tooltip += "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:600; text-decoration: underline;\"></p>\n";
		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Type: </span><span style=\" font-size:8pt; font-weight:600; color:#0000ff;\">%1</span></"
							"p>\n").arg(aData.type);
		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Range:</span><span style=\" font-size:8pt;\"> </span><span style=\" font-size:8pt; font-weight:600; color:#0000ff;\">%1 Meters</span></p>\n").arg(aData.range);
		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Cooldown: </span><span style=\" font-size:8pt; font-weight:600; color:#0000ff;\">%1 Seconds</span></p>\n").arg(aData.cooldown);
		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Energy: </span><span style=\" font-size:8pt; font-weight:600; color:#0000ff;\">%1</span></p>\n").arg(aData.energy);
	
		QString channeled;
		if(aData.channeled <= 0)
			channeled = "No";
		else
			channeled = QString("%1").arg(aData.channeled);
	
		tooltip += "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Channeled: </span><span style=\" font-size:8pt; font-weight:6";
		tooltip += QString("00; color:#0000ff;\">%1</span></p>\n").arg(channeled);

		QString triggered;
		if(aData.triggered <= 0)
			triggered = "None";
		else
			triggered = QString("%1").arg(aData.triggered);

		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Triggered by: </span><span style=\" font-size:8pt; font-weight:600; color:#0000ff;\">%1</span></p>\n").arg(triggered);
		tooltip += "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; text-decoration: underline;\"></p>\n";

		tooltip += QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">%1</p>\n").arg(aData.descritpion);
		tooltip += "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; text-decoration: underline;\"></p></body></html>";
	
		setToolTip(tooltip);

	}

	 if (!(event->buttons() & Qt::LeftButton))
         return;
     if ((event->pos() - dragStartPosition).manhattanLength()
          < QApplication::startDragDistance())
         return;

     QDrag *drag = new QDrag(this);
     QMimeData *mimeData = new QMimeData;

	 QString mimeInfo = QString("%1;%2;%3;%4;%5;%6;%7;%8;%9").arg(aData.imageName).arg(aData.name)
		 .arg(aData.type).arg(aData.energy).arg(aData.channeled).arg(aData.range).arg(aData.cooldown).arg(aData.triggered)
		 .arg(aData.descritpion);
	 mimeData->setText(mimeInfo);
     drag->setMimeData(mimeData);
	 drag->setPixmap(*pixmap());

     Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);

	
	QLabel::mouseMoveEvent(event);
}


////////////////////////////////////////////////////

WidgetList::WidgetList(QWidget *parent)
    :QWidget(parent)
      ,layout_(new QHBoxLayout(this))
{
    this->setLayout(layout_);
}

void WidgetList::clear()
{
	for(int i = 0; i < mv_Objects.size(); i++)
	{
		layout_->removeWidget(mv_Objects[i]);
		delete mv_Objects[i];
	}

	mv_Objects.clear();

}


void WidgetList::removeWidget(QWidget* widget)
{
    layout_->removeWidget(widget);
}

void WidgetList::addWidget(QWidget* widget)
{
    layout_->addWidget(widget);
	mv_Objects.push_back(widget);
}

QWidget* WidgetList::takeAt(int index) const
{
    return layout_->takeAt(index)->widget();
}

int WidgetList::count() const
{
    return layout_->count();
}


RotationConfigDlg::RotationConfigDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	wList = new WidgetList(ui.baseScrollArea);
	ui.baseScrollArea->setWidget(wList);
	ui.baseScrollArea->setWidgetResizable(true);
	ui.baseScrollArea->show();

	waList = new WidgetList(ui.advancedScrollArea);
	ui.advancedScrollArea->setWidget(waList);
	ui.advancedScrollArea->setWidgetResizable(true);
	ui.advancedScrollArea->show();

	loadClasses();
	buildConfigurationCombo();
	
}

RotationConfigDlg::~RotationConfigDlg()
{

}

void RotationConfigDlg::classChangedSlot()
{
	wList->clear();
	QStringList filters;
	filters.push_back("*.png");
	QString Path = QString("%1//Abilities//%2").arg(QApplication::applicationDirPath()).arg(ui.classComboBox->currentText());
	QDir dir(QString("%1/Abilities/%2").arg(QApplication::applicationDirPath()).arg(ui.classComboBox->currentText()));
	QStringList listEntry = dir.entryList(filters,QDir::Files);
	QString iniFilename = QString("%1//%2.ini").arg(Path).arg(ui.classComboBox->currentText());

	
	for(int i = 0; i < listEntry.size();i++)
	{
		QString name = listEntry[i].remove(".png");
		QString File = QString("%1/%2").arg(Path).arg(listEntry[i]);
		abilityData aData = readClassIni(iniFilename,listEntry[i]);
		aData.imageName = File;
		abilityLabel * label = new abilityLabel(aData);
		QPixmap pix(File);
		label->setPixmap(pix);
		label->setScaledContents(true);
		label->setGeometry(0,0,48,48);
		label->setMaximumHeight(48);
		label->setMaximumWidth(48);
		wList->addWidget(label);
	}

	int index = ui.classComboBox->currentIndex()-1;
	ui.subClassComboBox->clear();
	ui.subClassComboBox->addItem("None");

	if(subClasses.size() >= index*2)
	{
		ui.subClassComboBox->addItem(subClasses[2*index]);
		ui.subClassComboBox->addItem(subClasses[(2*index)+1]);
	}
	
}

void RotationConfigDlg::subClassChangedSlot()
{
	waList->clear();
	QStringList filters;
	filters.push_back("*.png");
	QString Path = QString("%1//Abilities//%2//%3").arg(QApplication::applicationDirPath()).arg(ui.classComboBox->currentText()).arg(ui.subClassComboBox->currentText());
	QDir dir(QString("%1/Abilities/%2//%3").arg(QApplication::applicationDirPath()).arg(ui.classComboBox->currentText()).arg(ui.subClassComboBox->currentText()));
	QStringList listEntry = dir.entryList(filters,QDir::Files);
	QString iniFilename = QString("%1//%2.ini").arg(Path).arg(ui.subClassComboBox->currentText());

	
	for(int i = 0; i < listEntry.size();i++)
	{
		QString name = listEntry[i].remove(".png");
		QString File = QString("%1/%2").arg(Path).arg(listEntry[i]);

		abilityData aData = readClassIni(iniFilename,listEntry[i]);
		aData.imageName = File;
		abilityLabel * label = new abilityLabel(aData);
		QPixmap pix(File);
		label->setPixmap(pix);
		label->setScaledContents(true);
		label->setGeometry(0,0,48,48);
		label->setMaximumHeight(48);
		label->setMaximumWidth(48);
		waList->addWidget(label);
	}


}

abilityData RotationConfigDlg::readClassIni(QString fileName, QString abilname)
{
	QSettings settings(fileName, QSettings::IniFormat);

	abilityData abil;
	settings.beginGroup(abilname);
	abil.name =abilname;
	abil.type = settings.value("Type").toString();
	abil.energy = settings.value("Energy").toInt();
	abil.channeled = settings.value("Channeled").toInt();

	abil.range = settings.value("Range").toString();
	abil.rangeMax = 0;
	QStringList list = abil.range.split("-");
	if(list.size() > 1)
	{
		abil.rangeMax = list[0].toInt();
		abil.rangeMin = list[1].toInt();
	}
	else
		abil.rangeMin = list[0].toInt();
	
	abil.cooldown = settings.value("Cooldown").toInt();
	abil.triggered = settings.value("Triggered").toString();
	abil.descritpion = settings.value("Description").toString();
	settings.endGroup();	

	return abil;
}

void RotationConfigDlg::loadClasses()
{
	//search for classes and advanced classes

	subClasses.clear();
	ui.classComboBox->clear();
	ui.subClassComboBox->clear();

	ui.classComboBox->addItem("None");

	QString Path = QString("%1/Abilities").arg(QApplication::applicationDirPath());

	QDirIterator directory_walker(Path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while(directory_walker.hasNext())
    {
          directory_walker.next();
		 
         if(directory_walker.fileInfo().completeSuffix() == "ini")
		 {
			 int level = 0;
			 QString da = directory_walker.filePath();
			 QStringList list = da.split("/");
			 for(int i = list.size()-1; i > 0; i--)
			 {
				 if(list[i] == "Abilities")
					 break;
				 level++;
			 }
			 if(level == 3)
			 {
				 subClasses.push_back(directory_walker.fileInfo().fileName().remove(".ini"));
			 }
			 else if(level == 2)
				 ui.classComboBox->addItem(directory_walker.fileInfo().fileName().remove(".ini"));
		 }
    }
}

void RotationConfigDlg::saveConfiguration()
{
	bool newConfig = false;
	if(ui.savedRotationsCombo->currentText() == tr("None"))
	{
		enterNameDlg dlg(this);
		if(dlg.exec() == QDialog::Accepted)
		{
			currentConfiguration = dlg.getNameEntered();
			newConfig = true;
		}
		else
			return;
	}

	QString iniFileName = QString("%1/Rotations.ini").arg(QApplication::applicationDirPath());
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup(currentConfiguration);
	settings.setValue("Class",ui.classComboBox->currentText());
	settings.setValue("SubClass",ui.subClassComboBox->currentText());
	settings.endGroup();


	saveSingleAbility(iniFileName, ui.abilLabel);
	saveSingleAbility(iniFileName, ui.abilLabel_2);
	saveSingleAbility(iniFileName, ui.abilLabel_3);
	saveSingleAbility(iniFileName, ui.abilLabel_4);
	saveSingleAbility(iniFileName, ui.abilLabel_5);
	saveSingleAbility(iniFileName, ui.abilLabel_6);
	saveSingleAbility(iniFileName, ui.abilLabel_7);
	saveSingleAbility(iniFileName, ui.abilLabel_8);
	saveSingleAbility(iniFileName, ui.abilLabel_9);
	saveSingleAbility(iniFileName, ui.abilLabel_10);
	saveSingleAbility(iniFileName, ui.abilLabel_11);
	saveSingleAbility(iniFileName, ui.abilLabel_12);

	int currentIndex = ui.savedRotationsCombo->findText(currentConfiguration);
	buildConfigurationCombo();

	ui.savedRotationsCombo->setCurrentIndex(ui.savedRotationsCombo->count()-1);
}

void RotationConfigDlg::saveSingleAbility(QString fileName, abilityDropLabel *label)
{
	QSettings settings(fileName, QSettings::IniFormat);
	settings.beginGroup(currentConfiguration);
	abilityData data = label->getAbilityData();
	if(data.name.isEmpty())
	{
		settings.remove(QString("Type-%1").arg(label->objectName()));
		settings.remove(QString("Energy-%1").arg(label->objectName()));
		settings.remove(QString("Channeled-%1").arg(label->objectName()));
		settings.remove(QString("Range-%1").arg(label->objectName()));
		settings.remove(QString("Cooldown-%1").arg(label->objectName()));
		settings.remove(QString("Triggered-%1").arg(label->objectName()));
		settings.remove(QString("Key-%1").arg(label->objectName()));
		settings.remove(QString("ImageName-%1").arg(label->objectName()));
		settings.remove(QString("name-%1").arg(label->objectName()));
		settings.endGroup();
		return;
	}

	settings.setValue(QString("Type-%1").arg(label->objectName()),data.type);
	settings.setValue(QString("Energy-%1").arg(label->objectName()),data.energy);
	settings.setValue(QString("Channeled-%1").arg(label->objectName()),data.channeled);
	settings.setValue(QString("Range-%1").arg(label->objectName()),data.range);
	settings.setValue(QString("Cooldown-%1").arg(label->objectName()),data.cooldown);
	settings.setValue(QString("Triggered-%1").arg(label->objectName()),data.triggered);
	settings.setValue(QString("Key-%1").arg(label->objectName()),data.key);
	settings.setValue(QString("ImageName-%1").arg(label->objectName()),data.imageName);
	settings.setValue(QString("name-%1").arg(label->objectName()),data.name);

	settings.endGroup();
}

void RotationConfigDlg::loadConfiguration()
{
}

void RotationConfigDlg::getSingleAbility(QString fileName, abilityDropLabel *label)
{
	abilityData data;
	QSettings settings(fileName, QSettings::IniFormat);
	//settings.beginGroup(ui.savedRotationsCombo->currentText());
	settings.beginGroup(currentConfiguration);
	
	data.type = settings.value(QString("Type-%1").arg(label->objectName())).toString();
	if(data.type.isEmpty())
	{
		if(label)
			label->clear();
		settings.endGroup();
		return;
	}
	
	data.name = settings.value(QString("name-%1").arg(label->objectName())).toString();
	data.energy = settings.value(QString("Energy-%1").arg(label->objectName())).toInt();
	data.channeled = settings.value(QString("Channeled-%1").arg(label->objectName())).toInt();

	data.range = settings.value(QString("Range-%1").arg(label->objectName())).toString();
	data.rangeMax = 0;
	QStringList list = data.range.split("-");
	if(list.size() > 1)
	{
		data.rangeMax = list[0].toInt();
		data.rangeMin = list[1].toInt();
	}
	else
		data.rangeMin = list[0].toInt();

	data.cooldown = settings.value(QString("Cooldown-%1").arg(label->objectName())).toInt();
	data.triggered = settings.value(QString("Triggered-%1").arg(label->objectName())).toString();
	data.key = settings.value(QString("Key-%1").arg(label->objectName())).toInt();
	data.imageName = settings.value(QString("ImageName-%1").arg(label->objectName())).toString();
	data.name = settings.value(QString("name-%1").arg(label->objectName())).toString();
	settings.endGroup();

	mv_abilities.push_back(data);

	if(label)
		label->setAbilityData(data);
}

bool RotationConfigDlg::isVentClass(QString &rotationName)
{
	currentConfiguration = rotationName;
	QString iniFileName = QString("%1/Rotations.ini").arg(QApplication::applicationDirPath());
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup(currentConfiguration);
	QString Class = settings.value("Class").toString();
	settings.endGroup();

	if(Class == "Bounty Hunter" || Class == "Trooper")
		return true;

	return false;
}

QVector<abilityData> RotationConfigDlg::getRotationAbilities(QString &rotationName)
{

	currentConfiguration = rotationName;
	QString iniFileName = QString("%1/Rotations.ini").arg(QApplication::applicationDirPath());
	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup(currentConfiguration);

	settings.endGroup();

	mv_abilities.clear();

	getSingleAbility(iniFileName, ui.abilLabel);
	getSingleAbility(iniFileName, ui.abilLabel_2);
	getSingleAbility(iniFileName, ui.abilLabel_3);
	getSingleAbility(iniFileName, ui.abilLabel_4);
	getSingleAbility(iniFileName, ui.abilLabel_5);
	getSingleAbility(iniFileName, ui.abilLabel_6);
	getSingleAbility(iniFileName, ui.abilLabel_7);
	getSingleAbility(iniFileName, ui.abilLabel_8);
	getSingleAbility(iniFileName, ui.abilLabel_9);
	getSingleAbility(iniFileName, ui.abilLabel_10);
	getSingleAbility(iniFileName, ui.abilLabel_11);
	getSingleAbility(iniFileName, ui.abilLabel_12);

	return mv_abilities;
}

void RotationConfigDlg::rotationChangedSlot()
{
	if(ui.savedRotationsCombo->currentText() == tr("None"))
		return;

	currentConfiguration = ui.savedRotationsCombo->currentText();
	QString iniFileName = QString("%1/Rotations.ini").arg(QApplication::applicationDirPath());

	QSettings settings(iniFileName, QSettings::IniFormat);
	settings.beginGroup(currentConfiguration);
	

	int index = ui.classComboBox->findText(settings.value("Class").toString());
	ui.classComboBox->setCurrentIndex(index);
	classChangedSlot();

	index = ui.subClassComboBox->findText(settings.value("SubClass").toString());
	ui.subClassComboBox->setCurrentIndex(index);
	subClassChangedSlot();

	settings.endGroup();

	mv_abilities.clear();

	getSingleAbility(iniFileName, ui.abilLabel);
	getSingleAbility(iniFileName, ui.abilLabel_2);
	getSingleAbility(iniFileName, ui.abilLabel_3);
	getSingleAbility(iniFileName, ui.abilLabel_4);
	getSingleAbility(iniFileName, ui.abilLabel_5);
	getSingleAbility(iniFileName, ui.abilLabel_6);
	getSingleAbility(iniFileName, ui.abilLabel_7);
	getSingleAbility(iniFileName, ui.abilLabel_8);
	getSingleAbility(iniFileName, ui.abilLabel_9);
	getSingleAbility(iniFileName, ui.abilLabel_10);
	getSingleAbility(iniFileName, ui.abilLabel_11);
	getSingleAbility(iniFileName, ui.abilLabel_12);
	
}

void RotationConfigDlg::buildConfigurationCombo()
{
	ui.savedRotationsCombo->clear();
	ui.savedRotationsCombo->addItem(tr("None"));

	QString iniFileName = QString("%1/Rotations.ini").arg(QApplication::applicationDirPath());
	QSettings settings(iniFileName, QSettings::IniFormat);
	QStringList list = settings.childGroups();
	for(int i = 0; i < list.size(); i++)
		ui.savedRotationsCombo->addItem(list[i]);

	
}

void RotationConfigDlg::accept()
{
	saveConfiguration();
	//QDialog::accept();
}

void RotationConfigDlg::saveAsSlot()
{
	ui.savedRotationsCombo->setCurrentIndex(0);
	saveConfiguration();
}

QStringList RotationConfigDlg::getConfigEntries()
{
	QString iniFileName = QString("%1/Rotations.ini").arg(QApplication::applicationDirPath());
	QSettings settings(iniFileName, QSettings::IniFormat);
	return settings.childGroups();
}
