// MonsterSkillElement.h: interface for the CMonsterSkillElement class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterSkillElementInfo.h"

#define MAX_MONSTER_SKILL_ELEMENT_INFO 100

class CMonsterSkillElement
{
	CMonsterSkillElement();
	virtual ~CMonsterSkillElement();
	SingletonInstance(CMonsterSkillElement)
public:
	void Load(char* path);
	void DelAllSkillElement();
	CMonsterSkillElementInfo* FindSkillElementInfo(int ElementNumber);
public:
	bool m_DataLoad;
	CMonsterSkillElementInfo m_MonsterSkillElementInfoArray[MAX_MONSTER_SKILL_ELEMENT_INFO];
};

#define gMonsterSkillElement SingNull(CMonsterSkillElement)