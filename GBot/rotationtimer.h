#ifndef ROTATIONTIMER_H
#define ROTATIONTIMER_H

#include <QTimer>
#include <qvector.h>
#include "Structures.h"

class MobData;

class rotationTimers : public QObject
{
	Q_OBJECT

public:
	rotationTimers(QObject *parent);
	~rotationTimers();

	void setMobsData(MobData *playerdata, MobData *target);
	void setAbilityUsed(int index); //devo sapere se l ho realmente utilizzata per far ripsrtire il timer
	void setAbilitiesData(QVector<abilityData> *abilities){mv_abilities = abilities;}
	void start();
	void stop();
	bool isActive(){return delayTimer->isActive();}
	abilityData getSelfHeal();
	void setDefaultUsed(bool used){m_defaultUsed = used;}

private:
	QVector<abilityData> *mv_abilities;

	MobData *m_playerdata;
	MobData *m_target;

	QVector<QTimer *> timers;
	QTimer *delayTimer;

	bool waitForDelay;
	int nextComboAttack;
	bool m_defaultUsed;

	bool checkAbilityReady(int index);

protected slots:
	void delayTimerSlot();

	void timer1();
	void timer2();
	void timer3();
	void timer4();
	void timer5();
	void timer6();
	void timer7();
	void timer8();
	void timer9();
	void timer10();
	void timer11();
	void timer12();

signals:
	void abilityTriggered(int index,const abilityData *ability);
	
};

#endif // ROTATIONTIMER_H
