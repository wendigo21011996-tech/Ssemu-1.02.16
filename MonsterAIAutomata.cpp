// MonsterAIAutomata.cpp: implementation of the CMonsterAIAutomata class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIAutomata.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIAutomata::CMonsterAIAutomata() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->DelAllAutomata();

	#endif
}

CMonsterAIAutomata::~CMonsterAIAutomata() // OK
{

}

void CMonsterAIAutomata::Load(char* path) // OK
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

	this->DelAllAutomata();

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

					CMonsterAIState info;

					int AutomataNumber = lpReadFile->GetNumber();

					strcpy_s(info.m_StateTransitionDesc,lpReadFile->GetAsString());

					info.m_Priority = lpReadFile->GetAsNumber();

					info.m_CurrentState = lpReadFile->GetAsNumber();

					info.m_NextState = lpReadFile->GetAsNumber();

					info.m_TransitionType = lpReadFile->GetAsNumber();

					info.m_TransitionRate = lpReadFile->GetAsNumber();

					info.m_TransitionValueType = lpReadFile->GetAsNumber();

					info.m_TransitionValue = lpReadFile->GetAsNumber();

					info.m_DelayTime = lpReadFile->GetAsNumber();

					if((AutomataNumber >= 0 && AutomataNumber < MAX_MONSTER_AI_AUTOMATA_INFO) && (info.m_CurrentState >= 0 && info.m_CurrentState < MAX_AI_STATE) && (info.m_Priority >= 0 && info.m_Priority < MAX_AI_PRIORITY))
					{
						this->m_MonsterAIAutomataInfoArray[AutomataNumber].m_AutomataNumber = AutomataNumber;
						this->m_MonsterAIAutomataInfoArray[AutomataNumber].m_AIState[info.m_CurrentState][info.m_Priority] = info;
						this->m_MonsterAIAutomataInfoArray[AutomataNumber].m_AIStateTransCount[info.m_CurrentState]++;
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

	this->m_DataLoad = 1;

	delete lpReadFile;

	#endif
}

void CMonsterAIAutomata::DelAllAutomata() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_AI_AUTOMATA_INFO;n++)
	{
		this->m_MonsterAIAutomataInfoArray[n].Reset();
	}

	#endif
}

CMonsterAIAutomataInfo* CMonsterAIAutomata::FindAutomataInfo(int AutomataNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(AutomataNumber < 0 || AutomataNumber >= MAX_MONSTER_AI_AUTOMATA_INFO)
	{
		return 0;
	}

	if(this->m_MonsterAIAutomataInfoArray[AutomataNumber].m_AutomataNumber == AutomataNumber)
	{
		return &this->m_MonsterAIAutomataInfoArray[AutomataNumber];
	}

	#endif

	return 0;
}
