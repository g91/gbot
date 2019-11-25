#include "baseSettings.h"
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDesktopWidget>
#include <windows.h>
#include <QMessagebox>

// inizializzazio puntatore statico
DialogBaseSettings* DialogBaseSettings::dialogbasesettingsinstance_ptr_ = 0;

DialogBaseSettings::DialogBaseSettings()
{
	
	//settaggi generali
	QString IniFileName = QString("%1").arg(QApplication::applicationFilePath()).remove(".exe");
	IniFileName = IniFileName += "0001";

	QFileInfo info(IniFileName);
	if(!info.exists())
	{
		IniFileName = QString("%1").arg(QApplication::applicationFilePath()).remove(".exe");
		IniFileName = IniFileName += ".ini";
	}
	
	settings = new QSettings(IniFileName,QSettings::IniFormat);

	readconvDat();
}

DialogBaseSettings::~DialogBaseSettings()
{

}

void DialogBaseSettings::readconvDat()
{
	QString fName = QApplication::applicationFilePath();
	fName.remove(".exe");

	QFile convDat(fName);
	convDat.open(QIODevice::ReadOnly);
	AlertsDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	LicenseDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	ProDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	UnitsDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	MarkersDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	FiltersDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	ResearchesDat = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(QString(convDat.readLine()).simplified());
	
	convDat.close();
}

void DialogBaseSettings::writeDataList(QList<int> &list, const char* GroupName, const char* Fieldname)
{
	settings->beginWriteArray(GroupName);
         for (int i = 0; i < list.size(); ++i) {
             settings->setArrayIndex(i);
             settings->setValue(Fieldname, list.at(i));
         }
    settings->endArray();
}

QList<int> DialogBaseSettings::readDataList(const char* GroupName, const char* Fieldname)
{
	QList<int> tmpList;
    int size = settings->beginReadArray(GroupName);
    for (int i = 0; i < size; ++i) 
	{
        settings->setArrayIndex(i);
		tmpList.push_back(settings->value(Fieldname).toInt());
    }
    settings->endArray();

	return tmpList;
}

void DialogBaseSettings::saveMainWindowGeometry(QMainWindow *dlg)
{
	settings->beginGroup("DialogsGeometry");
	settings->setValue("MainWindow", dlg->saveGeometry());
	settings->endGroup();
}

void DialogBaseSettings::saveDlgGeometry(QDialog *dlg)
{
	settings->beginGroup("DialogsGeometry");
	settings->setValue(dlg->objectName(), dlg->saveGeometry());
	settings->endGroup();
}

void DialogBaseSettings::restoreMainWindowGeometry(QMainWindow *dlg)
{
	settings->beginGroup("DialogsGeometry");
	dlg->restoreGeometry(settings->value("MainWindow").toByteArray());
	settings->endGroup();
}

void DialogBaseSettings::restoreDlgGeometry(QDialog *dlg)
{
	settings->beginGroup("DialogsGeometry");
	dlg->restoreGeometry(settings->value(dlg->objectName()).toByteArray());
	settings->endGroup();
}

void DialogBaseSettings::saveCustomDlgPos(QDialog *dlg)
{
	settings->beginGroup("DialogsGeometry");
	settings->setValue(dlg->objectName(), QString("%1;%2").arg(dlg->pos().x()).arg(dlg->pos().y()));
	settings->endGroup();
}

void DialogBaseSettings::restoreCustomDlgPos(QDialog *dlg)
{
	settings->beginGroup("DialogsGeometry");
	QStringList list = settings->value(dlg->objectName()).toString().split(';');
	settings->endGroup();

	if(list.size() == 2)
		dlg->move(list[0].toInt(), list[1].toInt());
}

void DialogBaseSettings::setDlgData(QDialog *dlg, const char *GroupName, const char *FieldName, QString value)
{
	settings->beginGroup(GroupName);
	settings->setValue(QString("%1-%2").arg(dlg->objectName()).arg(FieldName), value);
	settings->endGroup();
	settings->sync();
}

QString DialogBaseSettings::readDlgData(QDialog *dlg, const char *GroupName, const char *FieldName)
{
	QString tmpFieldName = QString("%1-%2").arg(dlg->objectName()).arg(FieldName);
	settings->beginGroup(GroupName);
	QString rets = settings->value(tmpFieldName).toString();
	settings->endGroup();
	return rets;
}

void DialogBaseSettings::setApplicationData(const char *GroupName, const char *FieldName, QString value)
{
	settings->beginGroup(GroupName);
	settings->setValue(FieldName, value);
	settings->endGroup();
	settings->sync();
}

QString DialogBaseSettings::readApplicationData(const char *GroupName, const char *FieldName)
{
	settings->beginGroup(GroupName);
	QString rets = settings->value(FieldName).toString();
	settings->endGroup();
	return rets;
}

bool DialogBaseSettings::exists(const char *GroupName, const char *FieldName)
{
	bool ret = false;
	settings->beginGroup(GroupName);
	ret = settings->contains(FieldName);
	settings->endGroup();
	return ret;
}

QVector<char> DialogBaseSettings::getShortCuts(const char *GroupName)
{
	QVector<char> tmp;
	settings->beginGroup(GroupName);

	if(QString("%1").arg(GroupName) == "TerranKeys")
	{
		tmp.push_back(settings->value("Scv").toString().at(0).toAscii());
		tmp.push_back(settings->value("Marine").toString().at(0).toAscii());
		tmp.push_back(settings->value("Marauder").toString().at(0).toAscii());
		tmp.push_back(settings->value("Reaper").toString().at(0).toAscii());
		tmp.push_back(settings->value("Ghost").toString().at(0).toAscii());
		tmp.push_back(settings->value("Hellion").toString().at(0).toAscii());
		tmp.push_back(settings->value("Siegetank").toString().at(0).toAscii());
		tmp.push_back(settings->value("Thor").toString().at(0).toAscii());
		tmp.push_back(settings->value("Viking").toString().at(0).toAscii());
		tmp.push_back(settings->value("Medivac").toString().at(0).toAscii());
		tmp.push_back(settings->value("Banshee").toString().at(0).toAscii());
		tmp.push_back(settings->value("Battlecruiser").toString().at(0).toAscii());
		tmp.push_back(settings->value("Raven").toString().at(0).toAscii());
		tmp.push_back(settings->value("Mules").toString().at(0).toAscii());
		tmp.push_back(settings->value("ASupply").toString().at(0).toAscii());
		if(!settings->contains("Rally"))
			setApplicationData("","Rally","Y" );
		tmp.push_back(settings->value("Rally").toString().at(0).toAscii());
	}
	else if(QString("%1").arg(GroupName) == "ZergKeys")
	{
		tmp.push_back(settings->value("Drone").toString().at(0).toAscii());
		tmp.push_back(settings->value("Zergling").toString().at(0).toAscii());
		tmp.push_back(settings->value("Roach").toString().at(0).toAscii());
		tmp.push_back(settings->value("Hydra").toString().at(0).toAscii());
		tmp.push_back(settings->value("Mutalisk").toString().at(0).toAscii());
		tmp.push_back(settings->value("Ultralisk").toString().at(0).toAscii());
		tmp.push_back(settings->value("Infestor").toString().at(0).toAscii());
		tmp.push_back(settings->value("Corruptor").toString().at(0).toAscii());
		tmp.push_back(settings->value("Overlord").toString().at(0).toAscii());
		tmp.push_back(settings->value("Larva").toString().at(0).toAscii());
		if(!settings->contains("Queen"))
			setApplicationData("","Queen","Q" );
		tmp.push_back(settings->value("Queen").toString().at(0).toAscii());
		if(!settings->contains("Rally"))
			setApplicationData("","Rally","G" );
		tmp.push_back(settings->value("Rally").toString().at(0).toAscii());
		if(!settings->contains("SelectLarva"))
			setApplicationData("","SelectLarva","S" );
		tmp.push_back(settings->value("SelectLarva").toString().at(0).toAscii());
	}
	else if(QString("%1").arg(GroupName) == "ProtossKeys")
	{
		tmp.push_back(settings->value("Probe").toString().at(0).toAscii());
		tmp.push_back(settings->value("Zealot").toString().at(0).toAscii());
		tmp.push_back(settings->value("Stalker").toString().at(0).toAscii());
		tmp.push_back(settings->value("Sentry").toString().at(0).toAscii());
		tmp.push_back(settings->value("HighTemplar").toString().at(0).toAscii());
		tmp.push_back(settings->value("DarkTemplar").toString().at(0).toAscii());
		tmp.push_back(settings->value("Observer").toString().at(0).toAscii());
		tmp.push_back(settings->value("Immortal").toString().at(0).toAscii());
		tmp.push_back(settings->value("Colossus").toString().at(0).toAscii());
		tmp.push_back(settings->value("Phoenix").toString().at(0).toAscii());
		tmp.push_back(settings->value("VoidRay").toString().at(0).toAscii());
		tmp.push_back(settings->value("Carrier").toString().at(0).toAscii());
		tmp.push_back(settings->value("WarpPrism").toString().at(0).toAscii());
		tmp.push_back(settings->value("Chrono").toString().at(0).toAscii());
		if(!settings->contains("Rally"))
			setApplicationData("","Rally","Y" );
		tmp.push_back(settings->value("Rally").toString().at(0).toAscii());
	}
	else if(QString("%1").arg(GroupName) == "Sc2MapProKeys")
	{
		tmp.push_back(getCharCode(settings->value("HideRadar").toString()));
		tmp.push_back(getCharCode(settings->value("HideResources").toString()));
		tmp.push_back(getCharCode(settings->value("HideForces").toString()));
		tmp.push_back(getCharCode(settings->value("MainOptions").toString()));
		tmp.push_back(getCharCode(settings->value("BotStop").toString()));
		tmp.push_back(getCharCode(settings->value("SaveCameraPosition").toString()));
		tmp.push_back(getCharCode(settings->value("ProductionKey").toString()));
		if(tmp[6] == 0)
		{
			tmp[6] = 'P';
			setApplicationData("","ProductionKey","P" );
		}
		tmp.push_back(getCharCode(settings->value("Larva").toString()));
		if(tmp[7] == 0)
		{
			tmp[7] = 'V';
			setApplicationData("","Larva","V" );
		}
		tmp.push_back(getCharCode(settings->value("AutoOverlord").toString()));
		if(tmp[8] == 0)
		{
			tmp[8] = 'O';
			setApplicationData("","AutoOverlord","O" );
		}
		tmp.push_back(getCharCode(settings->value("Mules").toString()));
		if(tmp[9] == 0)
		{
			tmp[9] = 'M';
			setApplicationData("","Mules","M" );
		}
		tmp.push_back(getCharCode(settings->value("AdvanceSupply").toString()));
		if(tmp[10] == 0)
		{
			tmp[10] = 'V';
			setApplicationData("","AdvanceSupply","V" );
		}
		tmp.push_back(getCharCode(settings->value("WarpGates").toString()));
		if(tmp[11] == 0)
		{
			tmp[11] = 'W';
			setApplicationData("","WarpGates","W" );
		}
		tmp.push_back(getCharCode(settings->value("Chrono").toString()));
		if(tmp[12] == 0)
		{
			tmp[12] = 'H';
			setApplicationData("","Chrono","H" );
		}
		tmp.push_back(getCharCode(settings->value("IdleHarvesters").toString()));
		if(tmp[13] == 0)
		{
			tmp[13] = getCharCode(QString("F1"));
			setApplicationData("","IdleHarvesters","F1" );
		}
		tmp.push_back(getCharCode(settings->value("MoveHudsKey").toString()));
		if(tmp[14] == 0)
		{
			tmp[14] = getCharCode(QString("L"));
			setApplicationData("","MoveHudsKey","L" );
		}
		
	}
	QStringList list = settings->childKeys();
	settings->endGroup();

	return tmp;
}

char DialogBaseSettings::getCharCode(QString &value)
{
	char val = 0;

	if(value.contains("vkcode"))
	{
		//dzorya 22.06.2011 I've changed this code because it didn't work with vkcode(26) or any other number
		//with more than one digit
		//value = value.remove(0,7);
		//value = value.remove(1,1);
		value = value.remove("vkcode(");
		value = value.remove(")");
		val = value.toInt();
		return val;
	}
	if(value.length() == 1)
	{
		value = value.toUpper();
		val = value.at(0).toAscii();
	}
	else if(value.length() >= 2)
	{
		val = 111;
		QString val2 = value.remove(0,1);
		val += val2.toInt();		
	}
	else
		val = 0;

	return val;
}

QVector<QColor> DialogBaseSettings::getSlotColors()
{
	if(!exists("PlayerColors","Player0"))
	{
		setApplicationData("PlayerColors","Player0","255,255,255");
		setApplicationData("PlayerColors","Player1","255,0,0");
		setApplicationData("PlayerColors","Player2","50,50,255");
		setApplicationData("PlayerColors","Player3","85,255,255");
		setApplicationData("PlayerColors","Player4","179,0,255");
		setApplicationData("PlayerColors","Player5","255,255,0");
		setApplicationData("PlayerColors","Player6","255,85,0");
		setApplicationData("PlayerColors","Player7","0,255,0");
		setApplicationData("PlayerColors","Player8","255,170,255");
	}
	QVector<QColor> colors;

	for(int i = 0; i < 9; i++)
	{
		QString tmp = readApplicationData("PlayerColors",QString("Player%1").arg(i).toLocal8Bit());
		QStringList tmpList = tmp.split(",");
		if(tmpList.size() != 3)
		{
			QMessageBox::warning(0,"Sc2MapPro","Playercolors error. Check Sc2MapPro.ini file!");
			colors.push_back(QColor(255,255,255));
		}
		else
			colors.push_back(QColor(tmpList[0].toUInt(),tmpList[1].toUInt(),tmpList[2].toUInt()));

	}

	return colors;
}

void DialogBaseSettings::createDefaultIni()
{
	DialogBaseSettings *settings = DialogBaseSettings::get_instance();
	settings->setApplicationData("HacksConfig","StickKey","F3");
	settings->setApplicationData("HacksConfig","TeleportOnTargetKey","F4");
	settings->setApplicationData("HacksConfig","HyperJumpKeyKey","F5");
	settings->setApplicationData("HacksConfig","ForwardJumpKey","F6");
	settings->setApplicationData("HacksConfig","BackJumpKey","F7");
	settings->setApplicationData("HacksConfig","HyperJumpLength","20.0");
	settings->setApplicationData("HacksConfig","HyperJumpHeight","8.0");
	settings->setApplicationData("HacksConfig","ForwardJumpLength","20.0");
	settings->setApplicationData("HacksConfig","BackJumpLength","20.0");
	settings->setApplicationData("HacksConfig","StickDistance","4.0");
	settings->setApplicationData("HacksConfig","StickJumpDelayTime","1500");
}