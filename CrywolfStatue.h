// CrywolfStatue.h: interface for the CCrywolfStatue class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CCrywolfShield
{
public:
	CCrywolfShield() // OK
	{
		this->Reset();
	}

	void Reset() // OK
	{
		this->m_PriestNumber = 0;
		this->m_ShieldHP = 0;
		this->m_ShieldMaxHP = 0;
		this->m_ShieldState = 0;
	}

	int GetHPPercentage() // OK
	{
		return ((this->m_ShieldMaxHP==0)?0:((this->m_ShieldHP*100)/this->m_ShieldMaxHP));
	}
public:
	int m_ShieldMaxHP;
	int m_ShieldHP;
	int m_ShieldState;
	int m_PriestNumber;
};

class CCrywolfStatue
{
	CCrywolfStatue();
	virtual ~CCrywolfStatue();
	SingletonInstance(CCrywolfStatue)
public:
	void CrywolfStatueAct(int aIndex);
	void SetStatueViewState(LPOBJ lpObj,int PriestNumber);
public:
	CCrywolfShield m_Shield;
};

#define gCrywolfStatue SingNull(CCrywolfStatue)