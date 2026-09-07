// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct CUSTOM_MONSTER_INFO
{
	int MonsterClass;
	int MapNumber;
	int MaxLife;
	int DamageMin;
	int DamageMax;
	int Defense;
	int AttackRate;
	int DefenseRate;
	int ExperienceRate;
	int KillMessage;
	int BagSpecial;
	int RewardMessage;
	int RewardCoin1;
	int RewardCoin2;
	int RewardCoin3;
};

class CCustomMonster
{
	CCustomMonster();
	virtual ~CCustomMonster();
	SingletonInstance(CCustomMonster)
public:
	void Load(char* path);
	void SetCustomMonsterInfo(LPOBJ lpObj);
	void MonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget);
	int GetCustomMonsterExperienceRate(int index,int map);
private:
	std::vector<CUSTOM_MONSTER_INFO> m_CustomMonsterInfo;
};

#define gCustomMonster SingNull(CCustomMonster)