#ifndef PVEBOTSETUP_H
#define PVEBOTSETUP_H

#include <QDialog>
#include "ui_pvebotsetup.h"

class RotationAbility
{
public:
	RotationAbility(){clear();}

	bool operator==(const RotationAbility &abil)
	{
		return energyRequired == abil.energyRequired;
	}

	void clear(){abilityKey = 0; energyRequired = 0;}

	char abilityKey;
	int energyRequired;
};

class PveBotConfig
{
public:
	PveBotConfig(){clear();}

	void clear(){mv_abilities.clear(); recoverAbility = lifeStimPackKey = 0; selfBuffKey = 0;
	minimumPlayerHealh = minimumPlayerEnergy = minimumCopmanionHealth = lifeStimPackhealth = 0;alwaysDefaultAttacks = false;
	attackDelay = 1.2;BotMaxDistance = 40.0; BotMaxZValue = 3.0;}

	QVector<RotationAbility> mv_abilities;
	char recoverAbility;
	int minimumPlayerHealh;
	int minimumPlayerEnergy;
	int minimumCopmanionHealth;
	char lifeStimPackKey;
	int lifeStimPackhealth;
	char openingAttack;
	int openingAttackEnergy;
	char defaultAttack;
	bool alwaysDefaultAttacks;
	char selfBuffKey;
	float attackDelay;
	float BotMaxDistance;
	float BotMaxZValue;
};

class PveBotSetup : public QDialog
{
	Q_OBJECT

public:
	PveBotSetup(QWidget *parent = 0);
	~PveBotSetup();

	PveBotConfig getPveBotConfig(){return m_config;}
	void loadInidata();

private:
	Ui::PveBotSetup ui;

	bool moveState;
	int	StartPosX;
	int	StartPosY;

	PveBotConfig m_config;

protected slots:
	virtual void mousePressEvent( QMouseEvent * event );	
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );

	virtual void accept();
};

#endif // PVEBOTSETUP_H
