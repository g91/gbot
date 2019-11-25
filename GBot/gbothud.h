#ifndef GBOTHUD_H
#define GBOTHUD_H

#include <QDialog>
#include "ui_gbothud.h"
#include "c:\progetti\include\swlib.h"
#include "Structures.h"

class PathSystem;
class SetupBotDlg;
class RotationConfigDlg;
class GBotHud : public QDialog
{
	Q_OBJECT

public:
	GBotHud(HWND *swHwnd, PathSystem *pathSystem, QWidget *parent = 0);
	~GBotHud();

	void setLocalPlayerPosition(PointCoords coords, float angle,float jumpDelay);
	void setTextInfo(QString text){ui.infoLabel2->setText(text);}

	SetupBotDlg *getSetupBotDlg(){return s_dlg;}
	bool isPetOptionEnabled(){ return ui.petCheckBox->isChecked();}

private:
	Ui::GBotHud ui;

	bool moveState;
	int	StartPosX;
	int	StartPosY;

	PointCoords lCoords;
	PathSystem *m_pathSystem;

	float m_angle;
	float m_jumpDelay;

	bool PathMoving;
	SetupBotDlg *s_dlg;
	RotationConfigDlg *r_dlg;
	QVector<abilityData> mv_abilities;

	HWND *m_swHwnd;

	void loadConfig();
	void saveConfig();

protected slots:
	virtual void mousePressEvent( QMouseEvent * event );	
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );

	virtual void startSlot();
	virtual void stopSlot();
	virtual void addWSlot();
	virtual void resetSlot();
	virtual void loadSlot();
	virtual void saveSlot();
	virtual void configChanged();
	virtual void pvpClickedSlot();
	virtual void pveClickedSlot();
	virtual void wizardSlot();

	virtual void configComboChangedSlot(int);
};

#endif // GBOTHUD_H
