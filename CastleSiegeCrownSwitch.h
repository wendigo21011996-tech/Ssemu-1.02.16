// CastleSiegeCrownSwitch.h: interface for the CCastleSiegeCrownSwitch class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CCastleSiegeCrownSwitch
{
	CCastleSiegeCrownSwitch();
	virtual ~CCastleSiegeCrownSwitch();
	SingletonInstance(CCastleSiegeCrownSwitch)
public:
	void CastleSiegeCrownSwitchAct(int aIndex);
};

#define gCastleSiegeCrownSwitch SingNull(CCastleSiegeCrownSwitch)