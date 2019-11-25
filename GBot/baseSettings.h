#ifndef DIALOGBASESETTINGS_H
#define DIALOGBASESETTINGS_H
#include <QSettings>
#include <QList>
#include <QVariant>
#include <QDialog.h>
#include <QFileInfo>
#include <QApplication>
#include <QMainWindow>

class DialogBaseSettings
{
private:
    static DialogBaseSettings* dialogbasesettingsinstance_ptr_;
    DialogBaseSettings();

public:
    ~DialogBaseSettings();

    static DialogBaseSettings* get_instance()
    {
        if (dialogbasesettingsinstance_ptr_ == 0)
        {
			dialogbasesettingsinstance_ptr_ = new DialogBaseSettings;
        }

        return dialogbasesettingsinstance_ptr_;
    }
	void recreateSettings()
	{
		if(settings != 0)
			delete settings;

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

	}

	void writeDataList(QList<int> &list, const char* GroupName, const char* Fieldname);
	QList<int> readDataList(const char* GroupName, const char* Fieldname);

	void saveDlgGeometry(QDialog *dlg);
	void restoreDlgGeometry(QDialog *dlg);

	void restoreMainWindowGeometry(QMainWindow *dlg);
	void saveMainWindowGeometry(QMainWindow *dlg);

	void saveCustomDlgPos(QDialog *dlg);
	void restoreCustomDlgPos(QDialog *dlg);

	//utilizzo generico per qualsiasi tipo di salvataggio
	void setApplicationData(const char *GroupName, const char *FieldName, QString value);
	QString readApplicationData(const char *GroupName, const char *FieldName);

	void setDlgData(QDialog *dlg, const char *GroupName, const char *FieldName, QString value);
	QString readDlgData(QDialog *dlg, const char *GroupName, const char *FieldName);

	bool exists(const char *GroupName, const char *FieldName);

	QVector<char> getShortCuts(const char *GroupName);
	char getCharCode(QString &value);

	QVector<QColor> getSlotColors();

	void createDefaultIni();

	QString AlertsDat;
	QString LicenseDat;
	QString ProDat;
	QString UnitsDat;
	QString ConvDat;
	QString MarkersDat;
	QString FiltersDat;
	QString ResearchesDat;

private:
	QSettings *settings;

	
	void readconvDat();
};
#endif // DIALOGBASESETTINGS_H
