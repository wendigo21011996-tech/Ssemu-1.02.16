// CustomComboSkill.h: interface for the CCustomComboSkill class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "DefaultClassInfo.h"

struct COMBO_SKILL_INFO
{
	int Class;
	int Index;
	int Stage;
	int Delay;
};

class CCustomComboSkill
{
	CCustomComboSkill();
	virtual ~CCustomComboSkill();
	SingletonInstance(CCustomComboSkill)
public:
	void Load(char* path);
	void SetInfo(COMBO_SKILL_INFO info);
	int CheckSkill(int Class,WORD skill,int* delay);
private:
	std::map<int,COMBO_SKILL_INFO> m_ComboSkillInfo[MAX_CLASS];
};

#define gCustomComboSkill SingNull(CCustomComboSkill)