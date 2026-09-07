// LifeStone.h: interface for the CLifeStone class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CLifeStone
{
	CLifeStone();
	virtual ~CLifeStone();
	SingletonInstance(CLifeStone)
public:
	bool CreateLifeStone(int aIndex);
	void DeleteLifeStone(int aIndex);
	bool SetReSpawnUserXY(int aIndex);
	void FindTarget(int aIndex);
};

#define gLifeStone SingNull(CLifeStone)