#ifndef ROTATIONCONFIGDLG_H
#define ROTATIONCONFIGDLG_H

#include <QDialog>
#include <QVBoxLayout>
#include <qlabel.h>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include "Structures.h"

class abilityDropLabel: public QLabel
{
	Q_OBJECT
public:
	abilityDropLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);

	abilityData getAbilityData(){return aData;}
	void setAbilityData(abilityData data);
	void clear();

	void setText ( const QString & );
	void setPixmap ( const QPixmap & );

protected slots:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual void mouseMoveEvent ( QMouseEvent * ev );
	virtual void mousePressEvent ( QMouseEvent * ev );
	

private:
	abilityData aData;
	QString originalTooltip;
	QString originalText;
};

class abilityLabel: public QLabel
{
	Q_OBJECT
public:
	abilityLabel(abilityData abil, QWidget * parent = 0, Qt::WindowFlags f = 0);

protected slots:
	virtual void mouseMoveEvent ( QMouseEvent * ev );
	virtual void mousePressEvent(QMouseEvent *event);

private:
	abilityData aData;
	QPoint dragStartPosition;
};

class WidgetList : public QWidget
{
    Q_OBJECT

public:
    WidgetList(QWidget *parent = 0);
	~WidgetList(){}

    void addWidget(QWidget*);
    void removeWidget(QWidget*);

    QList<QWidget*> getListWidget() const;
    QWidget* takeAt(int) const;
    int count() const;
	void clear();

private:
    QHBoxLayout* layout_;
	QVector<QWidget *> mv_Objects;
};

#include "ui_rotationconfigdlg.h"

class RotationConfigDlg : public QDialog
{
	Q_OBJECT

public:
	RotationConfigDlg(QWidget *parent = 0);
	~RotationConfigDlg();

	QVector<abilityData> getRotationAbilities(QString &rotationName);
	QStringList getConfigEntries();
	bool isVentClass(QString &rotationName);

protected slots:
	virtual void classChangedSlot();
	virtual void subClassChangedSlot();
	virtual void rotationChangedSlot();
	virtual void accept();
	virtual void saveAsSlot();

private:
	Ui::RotationConfigDlg ui;
	WidgetList* wList;
	WidgetList* waList;

	abilityData readClassIni(QString fileName, QString abilname);
	void saveConfiguration();
	void loadConfiguration();
	void loadClasses();
	void buildConfigurationCombo();

	void saveSingleAbility(QString fileName, abilityDropLabel *label);
	void getSingleAbility(QString fileName, abilityDropLabel *label=0);

	QVector<abilityData> mv_abilities;
	QVector<QString> subClasses;
	QString currentConfiguration;
};

#endif // ROTATIONCONFIGDLG_H
