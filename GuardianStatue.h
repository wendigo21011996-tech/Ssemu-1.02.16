// GuardianStatue.h: interface for the CGuardianStatue class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CGuardianStatue
{
	CGuardianStatue();
	virtual ~CGuardianStatue();
	SingletonInstance(CGuardianStatue)
public:
	void FindTarget(int aIndex);
};

#define gGuardianStatue SingNull(CGuardianStatue)