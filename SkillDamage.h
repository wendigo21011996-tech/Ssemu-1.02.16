// SkillDamage.h: interface for the CSkillDamage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Skill.h"

struct SKILL_DAMAGE_INFO
{
	int Index;
	int Rate;
};

class CSkillDamage
{
	CSkillDamage();
	virtual ~CSkillDamage();
	SingletonInstance(CSkillDamage)
public:
	void Load(char* path);
	void SetInfo(int type,SKILL_DAMAGE_INFO info);
	int GetDamageRate(CSkill* lpSkill,int damage);
	int GetEffectRate(CSkill* lpSkill);
private:
	std::map<int,SKILL_DAMAGE_INFO> m_SkillDamageRateInfo;
	std::map<int,SKILL_DAMAGE_INFO> m_SkillEffectRateInfo;
};

#define gSkillDamage SingNull(CSkillDamage)