#include "rotationtimer.h"
#include <QLineF>
#include "c:\progetti\include\swlib.h"

#define _AttackDelay_ 1950


rotationTimers::rotationTimers(QObject *parent)
	:QObject(parent)
{
	waitForDelay = false;
	nextComboAttack = -1;
	mv_abilities = 0;
	m_defaultUsed = false;
	
	for(int i = 0; i < 13; i++)
	{
		QTimer *timer = new QTimer(this);
		timers.push_back(timer);
	}

	delayTimer = new QTimer(this);
	connect(delayTimer,SIGNAL(timeout()),this,SLOT(delayTimerSlot()));

	connect(timers[0],SIGNAL(timeout()),this,SLOT(timer1()));
	connect(timers[1],SIGNAL(timeout()),this,SLOT(timer2()));
	connect(timers[2],SIGNAL(timeout()),this,SLOT(timer3()));
	connect(timers[3],SIGNAL(timeout()),this,SLOT(timer4()));
	connect(timers[4],SIGNAL(timeout()),this,SLOT(timer5()));
	connect(timers[5],SIGNAL(timeout()),this,SLOT(timer6()));
	connect(timers[6],SIGNAL(timeout()),this,SLOT(timer7()));
	connect(timers[7],SIGNAL(timeout()),this,SLOT(timer8()));
	connect(timers[8],SIGNAL(timeout()),this,SLOT(timer9()));
	connect(timers[9],SIGNAL(timeout()),this,SLOT(timer10()));
	connect(timers[10],SIGNAL(timeout()),this,SLOT(timer11()));
	connect(timers[11],SIGNAL(timeout()),this,SLOT(timer12()));
	
}

rotationTimers::~rotationTimers()
{

}

void rotationTimers::timer1(){timers[0]->stop();}
void rotationTimers::timer2(){timers[1]->stop();}
void rotationTimers::timer3(){timers[2]->stop();}
void rotationTimers::timer4(){timers[3]->stop();}
void rotationTimers::timer5(){timers[4]->stop();}
void rotationTimers::timer6(){timers[5]->stop();}
void rotationTimers::timer7(){timers[6]->stop();}
void rotationTimers::timer8(){timers[7]->stop();}
void rotationTimers::timer9(){timers[8]->stop();}
void rotationTimers::timer10(){timers[9]->stop();}
void rotationTimers::timer11(){timers[10]->stop();}
void rotationTimers::timer12(){timers[11]->stop();}

void rotationTimers::start()
{
	delayTimer->start(300);
}

void rotationTimers::stop()
{
	m_defaultUsed = false;
	delayTimer->stop();
}

void rotationTimers::setAbilityUsed(int index)
{
	if(index > timers.size()-1)
		return;
	
	waitForDelay = true;

	if(index == 1)
		m_defaultUsed = true;

	if(mv_abilities->at(index).channeled > 0)
	{
		delayTimer->start(mv_abilities->at(index).channeled*1000);
	}
	else
		delayTimer->start(_AttackDelay_);
	
	if(mv_abilities->at(index).cooldown == 0)
		timers[index]->stop(); //default ability o abilità cd 0
	else
		timers[index]->start(mv_abilities->at(index).cooldown*1000);

	if(!mv_abilities->at(index).triggered.isEmpty())
	{
		//search for the next ability to use
		nextComboAttack = -1;
		for(int i = 0; i < mv_abilities->size(); i++)
		{
			if(mv_abilities->at(i).name == mv_abilities->at(index).triggered)
			{
				nextComboAttack = i;
				break;
			}
		}
		
	}

}

void rotationTimers::setMobsData(MobData *playerdata, MobData *target)
{
	m_playerdata = playerdata;
	m_target = target;
}

abilityData rotationTimers::getSelfHeal()
{
	abilityData data;
	for(int i = 1; i < mv_abilities->size(); i++)
	{
		if(mv_abilities->at(i).type == "Selfheal")
			return mv_abilities->at(i);
	}

	return data;
}

bool rotationTimers::checkAbilityReady(int index)
{
	if(!m_target)
		return false;

	if(m_defaultUsed && index == 1)
		return false;

	if( mv_abilities->at(index).type == "Selfheal")
		return false;

	if(timers[index]->isActive())
		return false;

	if(m_playerdata->heroData.energy < mv_abilities->at(index).energy)
		return false;

	float distance = QLineF(m_playerdata->baseData.mob_x,m_playerdata->baseData.mob_y,m_target->baseData.mob_x,m_target->baseData.mob_y).length();

	if(distance < mv_abilities->at(index).rangeMax || distance > mv_abilities->at(index).rangeMin)
		return false;

	waitForDelay = true;

	return true;
}

void rotationTimers::delayTimerSlot()
{
	if(!mv_abilities)
		return;
	if(nextComboAttack >= 0)
	{
		if(checkAbilityReady(nextComboAttack))
		{
			emit abilityTriggered(nextComboAttack,&mv_abilities->at(nextComboAttack));
			waitForDelay = false;
			nextComboAttack = -1;
			return;
		}
	}

	for(int i = 1; i < mv_abilities->size(); i++)
	{
		if(checkAbilityReady(i))
		{
			emit abilityTriggered(i,&mv_abilities->at(i));
			waitForDelay = false;
			return;
		}
	}

	if(checkAbilityReady(0))
		emit abilityTriggered(0,&mv_abilities->at(0));

	waitForDelay = false;
}
