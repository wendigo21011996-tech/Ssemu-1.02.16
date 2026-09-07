// DamageTable.h: interface for the CDamageTable class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct MONSTER_DAMAGE_TABLE_INFO
{
	int MapNumber;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int MinDefense;
	int MaxDefense;
	int DamageRate;
};

struct PLAYER_DAMAGE_TABLE_INFO
{
	int MapNumber;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int MonsterIndex;
	int MonsterMinLevel;
	int MonsterMaxLevel;
	int DamageRate;
};

class CDamageTable
{
	CDamageTable();
	virtual ~CDamageTable();
	SingletonInstance(CDamageTable)
public:
	void Load(char* path);
	int GetMonsterDamage(LPOBJ lpObj);
	int GetPlayerDamage(LPOBJ lpObj,LPOBJ lpTarget);
private:
	std::vector<MONSTER_DAMAGE_TABLE_INFO> m_MonsterDamageInfo;
	std::vector<PLAYER_DAMAGE_TABLE_INFO> m_PlayerDamageInfo;
};

#define gDamageTable SingNull(CDamageTable)