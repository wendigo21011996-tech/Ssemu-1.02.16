// KanturuBattleOfNightmare.cpp: implementation of the CKanturuBattleOfNightmare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuBattleOfNightmare.h"
#include "Kanturu.h"
#include "KanturuBattleUserMng.h"
#include "KanturuMonsterMng.h"
#include "KanturuUtil.h"
#include "ReadFile.h"
#include "Message.h"
#include "MonsterAIGroup.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuBattleOfNightmare::CKanturuBattleOfNightmare() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_NONE);

	this->SetSuccessValue(0);

	this->m_NightmareAIGroupNumber = 0;

	this->m_NightmareObjIndex = -1;

	this->ResetAllData();

	#endif
}

CKanturuBattleOfNightmare::~CKanturuBattleOfNightmare() // OK
{

}

void CKanturuBattleOfNightmare::ResetAllData() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < KANTURU_NIGHTMARE_MAX;n++)
	{
		this->m_StateInfo[n].ResetTimeInfo();
	}

	this->m_StateInfoCount = 0;

	#endif
}

void CKanturuBattleOfNightmare::Load(char* path) // OK
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

	this->ResetAllData();

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
				if(section == 7)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int state = lpReadFile->GetNumber();

					int condition = lpReadFile->GetAsNumber();

					int value = lpReadFile->GetAsNumber();

					this->m_StateInfo[this->m_StateInfoCount].SetStateInfo(state);

					this->m_StateInfo[this->m_StateInfoCount].SetCondition(condition);

					this->m_StateInfo[this->m_StateInfoCount].SetValue(((condition==1)?(value*1000):value));

					this->m_StateInfoCount++;
				}
				else if(section == 8)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					this->m_NightmareAIGroupNumber = lpReadFile->GetNumber();
				}
				else
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

void CKanturuBattleOfNightmare::MainProc() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->CheckStateTime();

	this->CheckNightmareBattleUser();

	gKanturuBattleUserMng->CheckUserState();

	switch(this->m_BattleOfNightmareState)
	{
		case KANTURU_NIGHTMARE_NONE:
			this->ProcState_NONE();
			break;
		case KANTURU_NIGHTMARE_IDLE:
			this->ProcState_IDLE();
			break;
		case KANTURU_NIGHTMARE_NOTIFY:
			this->ProcState_NOTIFY();
			break;
		case KANTURU_NIGHTMARE_START:
			this->ProcState_START();
			break;
		case KANTURU_NIGHTMARE_END:
			this->ProcState_END();
			break;
		case KANTURU_NIGHTMARE_ENDCYCLE:
			this->ProcState_ENDCYCLE();
			break;
	}

	#endif
}

void CKanturuBattleOfNightmare::ProcState_NONE() // OK
{

}

void CKanturuBattleOfNightmare::ProcState_IDLE() // OK
{

}

void CKanturuBattleOfNightmare::ProcState_NOTIFY() // OK
{

}

void CKanturuBattleOfNightmare::ProcState_START() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	gKanturuUtil->NotifyKanturuObjectCount(gKanturuMonsterMng->GetAliveMonsterCount(),gKanturuBattleUserMng->GetUserCount());

	if(OBJECT_RANGE(this->m_NightmareObjIndex) != 0 && gObj[this->m_NightmareObjIndex].Live == 0)
	{
		this->SetSuccessValue(1);

		this->SetState(KANTURU_NIGHTMARE_END);
	}

	#endif
}

void CKanturuBattleOfNightmare::ProcState_END() // OK
{

}

void CKanturuBattleOfNightmare::ProcState_ENDCYCLE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIGroup::DelGroupInstance(this->m_NightmareAIGroupNumber);
	
	#endif
}

void CKanturuBattleOfNightmare::SetState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_StateInfo[state].SetConditionAppliedTime();

	gKanturuMonsterMng->ResetRegenMonsterObjData();

	this->m_NightmareObjIndex = -1;

	switch(state)
	{
		case KANTURU_NIGHTMARE_NONE:
			this->SetState_NONE();
			break;
		case KANTURU_NIGHTMARE_IDLE:
			this->SetState_IDLE();
			break;
		case KANTURU_NIGHTMARE_NOTIFY:
			this->SetState_NOTIFY();
			break;
		case KANTURU_NIGHTMARE_START:
			this->SetState_START();
			break;
		case KANTURU_NIGHTMARE_END:
			this->SetState_END();
			break;
		case KANTURU_NIGHTMARE_ENDCYCLE:
			this->SetState_ENDCYCLE();
			break;
	}

	gKanturuUtil->NotifyKanturuChangeState(KANTURU_STATE_BATTLE_OF_NIGHTMARE,this->m_BattleOfNightmareState);
	
	#endif
}

void CKanturuBattleOfNightmare::SetState_NONE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_NONE);

	CMonsterAIGroup::DelGroupInstance(this->m_NightmareAIGroupNumber);
	
	#endif
}

void CKanturuBattleOfNightmare::SetState_IDLE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_IDLE);

	this->SetSuccessValue(0);

	CMonsterAIGroup::DelGroupInstance(this->m_NightmareAIGroupNumber);

	gKanturuBattleUserMng->MoveAllUser(134);
	
	#endif
}

void CKanturuBattleOfNightmare::SetState_NOTIFY() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_NOTIFY);
	
	#endif
}

void CKanturuBattleOfNightmare::SetState_START() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_START);

	if(this->m_StateInfo[this->m_BattleOfNightmareState].GetCondition() == 1)
	{
		gKanturuUtil->NotifyKanturuBattleTime(this->m_StateInfo[this->m_BattleOfNightmareState].GetValue());
	}

	gKanturuMonsterMng->SetKanturuMonster(2);

	CMonsterAIGroup::Init(this->m_NightmareAIGroupNumber);

	CMonsterAIGroup::ChangeAIOrder(this->m_NightmareAIGroupNumber,0);

	this->m_NightmareObjIndex = CMonsterAIGroup::FindGroupLeader(this->m_NightmareAIGroupNumber);
	
	#endif
}

void CKanturuBattleOfNightmare::SetState_END() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_END);

	gKanturuUtil->NotifyKanturuResult(this->GetSuccessValue());

	if(this->GetSuccessValue() == 0)
	{
		gKanturuUtil->SendMsgKanturuBossMapUser(gMessage->GetMessage(67));
	}
	
	#endif
}

void CKanturuBattleOfNightmare::SetState_ENDCYCLE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleOfNightmareState(KANTURU_NIGHTMARE_ENDCYCLE);

	if(this->GetSuccessValue() == 0)
	{
		gKanturuBattleUserMng->MoveAllUser(137);
		gKanturuBattleUserMng->ResetAllData();
	}
	else
	{
		gKanturuUtil->SendMsgKanturuBossMapUser(gMessage->GetMessage(66));
	}
	
	#endif
}

void CKanturuBattleOfNightmare::CheckStateTime()
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->GetBattleOfNightmareState() != KANTURU_NIGHTMARE_NONE && this->m_StateInfo[this->GetBattleOfNightmareState()].GetCondition() == 1 && this->m_StateInfo[this->GetBattleOfNightmareState()].IsTimeOut() != 0)
	{
		this->SetState(((this->GetBattleOfNightmareState()==KANTURU_NIGHTMARE_ENDCYCLE)?KANTURU_NIGHTMARE_NONE:(this->GetBattleOfNightmareState()+1)));
	}
	
	#endif
}

void CKanturuBattleOfNightmare::CheckNightmareBattleUser() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->GetBattleOfNightmareState() == KANTURU_NIGHTMARE_NOTIFY || this->GetBattleOfNightmareState() == KANTURU_NIGHTMARE_START)
	{
		if(gKanturuBattleUserMng->IsEmpty() != 0)
		{
			this->SetSuccessValue(0);

			this->SetState(KANTURU_NIGHTMARE_END);

			gKanturuBattleUserMng->ResetAllData();
		}
	}
	
	#endif
}

void CKanturuBattleOfNightmare::SetBattleOfNightmareState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_BattleOfNightmareState = state;
	
	#endif
}

void CKanturuBattleOfNightmare::SetSuccessValue(int success) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_IsSucccess = success;
	
	#endif
}

int CKanturuBattleOfNightmare::GetBattleOfNightmareState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_BattleOfNightmareState;

	#else

	return 0;
	
	#endif
}

int CKanturuBattleOfNightmare::GetSuccessValue() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_IsSucccess;

	#else

	return 0;
	
	#endif
}

int CKanturuBattleOfNightmare::GetRemainTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_StateInfo[this->GetBattleOfNightmareState()].GetRemainTime();

	#else

	return 0;
	
	#endif
}
