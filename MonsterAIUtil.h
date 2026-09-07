// MonsterAIUtil.h: interface for the CMonsterAIUtil class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CMonsterAIUtil
{
	CMonsterAIUtil();
	virtual ~CMonsterAIUtil();
	SingletonInstance(CMonsterAIUtil)
public:
	bool FindPathToMoveMonster(LPOBJ lpObj,int tx,int ty,int MaxPathCount,bool PreventOverMoving);
	void SendMonsterMoveMsg(LPOBJ lpObj);
	bool CheckMovingCondition(LPOBJ lpObj);
	bool GetXYToPatrol(LPOBJ lpObj);
	bool GetXYToEascape(LPOBJ lpObj);
	bool GetXYToChase(LPOBJ lpObj);
	bool FindMonViewportObj(int aIndex,int bIndex);
	bool FindMonViewportObj2(int aIndex,int bIndex);
	int FindMonsterToHeal(LPOBJ lpObj,int rate,int distance);
};

#define gMonsterAIUtil SingNull(CMonsterAIUtil)