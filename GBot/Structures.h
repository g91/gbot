#ifndef STRUCTURES_H
#define STRUCTURES_H

class abilityData
{
public:
	abilityData(){clear();}

	void clear(){energy = channeled = 0; range = ""; cooldown = 0;name = type = triggered = descritpion = "";imageName = "";key = 0;rangeMin = rangeMax = 0;}

	QString name;
	QString type;
	unsigned short energy;
	unsigned short channeled;
	QString range;
	unsigned short rangeMin;
	unsigned short rangeMax;
	unsigned int cooldown;
	QString triggered;
	QString descritpion;
	QString imageName;
	unsigned char key;
};

#endif