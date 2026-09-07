// PetDamageTable.h: interface for the CPetDamageTable class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct PET_DAMAGE_TABLE_INFO
{
	int Index;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int DamageRate;
};

class CPetDamageTable
{
	CPetDamageTable();
	virtual ~CPetDamageTable();
	SingletonInstance(CPetDamageTable)
public:
	void Load(char* path);
	bool GetPetDamage(LPOBJ lpObj,int index,int* damage);
private:
	std::vector<PET_DAMAGE_TABLE_INFO> m_PetDamageTableInfo;
};

#define gPetDamageTable SingNull(CPetDamageTable)