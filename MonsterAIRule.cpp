// MonsterAIRule.cpp: implementation of the CMonsterAIRule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIRule.h"
#include "ReadFile.h"
#include "Util.h"

BOOL CMonsterAIRule::m_DataLoad;
int CMonsterAIRule::m_MonsterCurrentAIUnitTable[MAX_MONSTER_AI_RULE_TABLE];
CMonsterAIRuleInfo CMonsterAIRule::m_MonsterAIRuleInfoArray[MAX_MONSTER_AI_RULE_INFO];
int CMonsterAIRule::m_MonsterAIRuleInfoCount;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIRule::CMonsterAIRule() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIRule::DelAllAIRule();

	#endif
}

CMonsterAIRule::~CMonsterAIRule() // OK
{

}

void CMonsterAIRule::LoadData(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CReadFile* lpReadFile = new CReadFile;

	if(lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	CMonsterAIRule::DelAllAIRule();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CMonsterAIRuleInfo info;

					info.m_RuleNumber = lpReadFile->GetNumber();

					info.m_MonsterClass = lpReadFile->GetAsNumber();

					strcpy_s(info.m_RuleDesc,lpReadFile->GetAsString());

					info.m_MonsterAIUnit = lpReadFile->GetAsNumber();

					info.m_RuleCondition = lpReadFile->GetAsNumber();

					info.m_WaitTime = lpReadFile->GetAsNumber();

					info.m_ContinuanceTime = lpReadFile->GetAsNumber();

					info.m_Month = lpReadFile->GetAsNumber();

					info.m_Day = lpReadFile->GetAsNumber();

					info.m_WeekDay = lpReadFile->GetAsNumber();

					info.m_Hour = lpReadFile->GetAsNumber();

					info.m_Minute = lpReadFile->GetAsNumber();

					if(CMonsterAIRule::m_MonsterAIRuleInfoCount >= 0 && CMonsterAIRule::m_MonsterAIRuleInfoCount < MAX_MONSTER_AI_RULE_INFO)
					{
						CMonsterAIRule::m_MonsterAIRuleInfoArray[CMonsterAIRule::m_MonsterAIRuleInfoCount++] = info;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	CMonsterAIRule::m_DataLoad = 1;

	delete lpReadFile;

	#endif
}

void CMonsterAIRule::DelAllAIRule()
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIRule::m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_AI_RULE_TABLE;n++)
	{
		CMonsterAIRule::m_MonsterCurrentAIUnitTable[n] = 0;
	}

	for(int n=0;n < MAX_MONSTER_AI_RULE_INFO;n++)
	{
		CMonsterAIRule::m_MonsterAIRuleInfoArray[n].Reset();
	}

	CMonsterAIRule::m_MonsterAIRuleInfoCount = 0;

	#endif
}

int CMonsterAIRule::GetCurrentAIUnit(int MonsterClass) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(MonsterClass < 0 || MonsterClass >= MAX_MONSTER_AI_RULE_TABLE)
	{
		return 0;
	}

	return CMonsterAIRule::m_MonsterCurrentAIUnitTable[MonsterClass];

	#else

	return 0;

	#endif
}

void CMonsterAIRule::MonsterAIRuleProc() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < CMonsterAIRule::m_MonsterAIRuleInfoCount;n++)
	{
		CMonsterAIRuleInfo* lpMonsterAIRuleInfo = &CMonsterAIRule::m_MonsterAIRuleInfoArray[n];

		if(lpMonsterAIRuleInfo->m_MonsterClass < 0 || lpMonsterAIRuleInfo->m_MonsterClass >= MAX_MONSTER_AI_RULE_TABLE)
		{
			continue;
		}

		if(lpMonsterAIRuleInfo->IsValid() == 0)
		{
			CMonsterAIRule::m_MonsterCurrentAIUnitTable[lpMonsterAIRuleInfo->m_MonsterClass] = 0;
		}
		else
		{
			CMonsterAIRule::m_MonsterCurrentAIUnitTable[lpMonsterAIRuleInfo->m_MonsterClass] = lpMonsterAIRuleInfo->m_MonsterAIUnit;
		}
	}

	#endif
}
