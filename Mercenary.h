// Mercenary.h: interface for the CMercenary class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CMercenary
{
	CMercenary();
	virtual ~CMercenary();
	SingletonInstance(CMercenary)
public:
	bool CreateMercenary(int aIndex,int MonsterClass,int x,int y);
	void DeleteMercenary(int aIndex);
	void FindTarget(int aIndex);
private:
	int m_MercenaryCount;
};

#define gMercenary SingNull(CMercenary)