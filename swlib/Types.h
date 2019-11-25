#pragma once

#include <windows.h>

#define PI 3.141592f

#define MIN_DIST_WAYPOINTS 0.5

typedef signed short sint16;
typedef signed int sint32;
typedef unsigned char byte;
typedef unsigned __int64 uint64;
typedef signed __int64 sint64;
typedef unsigned int addr;
typedef unsigned int uint32;

typedef struct
{
	char unknown0[36]; //0x0000
    uint64 dwLastTarget; //0x0028  
    char unknown44[16]; //0x002C
    float fHealth; //0x0040  
    float fPartner_Health; //0x0044  
    char unknown72[0x60]; //0x0048
    float fDist_2_Target; //0x0080  
    char unknown132[116]; //0x0084
    float fX_Cord; //0x0100  
    float fY_Cord; //0x0104  
    float fZ_Cord; //0x0108  
    char unknown268[8]; //0x010C
    float fAngle; //0x0114  
    char unknown280[8]; //0x0118
    float fPartnerX_Cord; //0x0120  
    float fPartnerY_Cord; //0x0124  
    float fPartnerZ_Cord; //0x0128

} LocalPlayerData;

typedef struct
{
	char name[65];
} Mapname;

typedef struct
{
	float x;
	float z;
	float y;
} PointCoords;

typedef struct
{
	char* name;
	addr address;
} ModuleInfo;

typedef struct
{
	float hp; //0x0
	float maxHp;
	char unknown[0x14];
	float energy; //0x1C
	float maxEnergy; //0x1C
	float unk1_1;
	float unk2_2;
	uint32 exp;
	uint32 unk;
	uint32 maxexp;
	uint32 unk2;
	uint32 Level;

	//1d4
	char unknown3[0x194];//6a8
	uint32 ArmaOn;
	char unknown4[0x40];
	char combat;
	float realHP;
	float realMaxHP;
	float realEnergy; //0x1C
	float realMaxEnergy; //0x1C

} HeroData;

typedef struct
{
	uint64 id;
	uint32 nextMob;
	uint32 prevMob;
	UINT32 unknown2;
	UINT32 unknown3;
	uint32 mobStruct;
	float mob_x;
	float mob_z;
	float mob_y;
	uint32 unknown5;
	float angle;
	char unknown4[0x288];
	uint64 targetId; //2c8

} MobLinkedList;

