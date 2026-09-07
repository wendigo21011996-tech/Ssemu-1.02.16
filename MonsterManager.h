// MonsterManager.h: interface for the CMonsterManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Item.h"

#define MAX_MONSTER_INFO 1000
#define MAX_MONSTER_LEVEL 150
#define MAX_MONSTER_ITEM 100

#define GET_MAX_VALUE(x) (((x)>2100000000)?2100000000:(((x)<0)?2100000000:(x)))

struct MONSTER_INFO
{
	int Index;
	int Type;
	char Name[32];
	int Level;
	int Life;
	int ScriptLife;
	int Mana;
	int DamageMin;
	int DamageMax;
	int Defense;
	int MagicDefense;
	int AttackRate;
	int DefenseRate;
	int MoveRange;
	int AttackRange;
	int AttackType;
	int ViewRange;
	int MoveSpeed;
	int AttackSpeed;
	int RegenTime;
	int Attribute;
	int ItemRate;
	int MoneyRate;
	int MaxItemLevel;
	int Resistance[MAX_RESISTANCE_TYPE];
	int MonsterSkill;
	#if(GAMESERVER_UPDATE>=701)
	int ElementalAttribute;
	int ElementalPattern;
	int ElementalDefense;
	int ElementalDamageMin;
	int ElementalDamageMax;
	int ElementalAttackRate;
	int ElementalDefenseRate;
	#endif
};

struct MONSTER_ITEM_INFO
{
	int IndexTable[MAX_MONSTER_ITEM];
	int IndexCount;
};

class CMonsterManager
{
	CMonsterManager();
	virtual ~CMonsterManager();
	SingletonInstance(CMonsterManager)
public:
	void Init();
	void Load(char* path);
	void SetInfo(MONSTER_INFO info);
	MONSTER_INFO* GetInfo(int index);
	char* GetName(int index);
	void InitMonsterItem();
	void InsertMonsterItem(int level);
	long GetMonsterItem(int level,int excellent,int socket);
	void SetMonsterData();
private:
	MONSTER_INFO m_MonsterInfo[MAX_MONSTER_INFO];
	MONSTER_ITEM_INFO m_MonsterItemInfo[MAX_MONSTER_LEVEL];
};

#define gMonsterManager SingNull(CMonsterManager)