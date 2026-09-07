// CastleSiegeCrown.h: interface for the CCastleSiegeCrown class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CCastleSiegeCrown
{
	CCastleSiegeCrown();
	virtual ~CCastleSiegeCrown();
	SingletonInstance(CCastleSiegeCrown)
public:
	void CastleSiegeCrownAct(int aIndex);
};

#define gCastleSiegeCrown SingNull(CCastleSiegeCrown)