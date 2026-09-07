// MonsterAIAutomata.h: interface for the CMonsterAIAutomata class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MonsterAIAutomataInfo.h"

#define MAX_MONSTER_AI_AUTOMATA_INFO 300

class CMonsterAIAutomata
{
	CMonsterAIAutomata();
	virtual ~CMonsterAIAutomata();
	SingletonInstance(CMonsterAIAutomata)
public:
	void Load(char* path);
	void DelAllAutomata();
	CMonsterAIAutomataInfo* FindAutomataInfo(int AutomataNumber);
public:
	bool m_DataLoad;
	CMonsterAIAutomataInfo m_MonsterAIAutomataInfoArray[MAX_MONSTER_AI_AUTOMATA_INFO];
};

#define gMonsterAIAutomata SingNull(CMonsterAIAutomata)