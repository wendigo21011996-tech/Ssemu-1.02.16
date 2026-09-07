// MonsterAI.h: interface for the CMonsterAI class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CMonsterAI
{
	CMonsterAI();
	virtual ~CMonsterAI();
	SingletonInstance(CMonsterAI)
public:
	void MonsterAIProc();
	bool RunAI(int aIndex,int MonsterClass);
	void MonsterStateMsgProc(int aIndex);
	void ProcessStateMsg(LPOBJ lpObj,int code,int aIndex,int SubCode);
	bool UpdateCurrentAIUnit(int aIndex);
	void MonsterMoveProc();
	void MonsterMove(int aIndex);
};

#define gMonsterAI SingNull(CMonsterAI)