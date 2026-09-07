// CannonTower.h: interface for the CCannonTower class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CCannonTower
{
	CCannonTower();
	virtual ~CCannonTower();
	SingletonInstance(CCannonTower)
public:
	void FindTarget(int aIndex);
};

#define gCannonTower SingNull(CCannonTower)