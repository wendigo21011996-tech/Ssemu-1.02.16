// KanturuBattleStanby.cpp: implementation of the CKanturuBattleStanby class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuBattleStanby.h"
#include "Kanturu.h"
#include "KanturuBattleUserMng.h"
#include "KanturuMonsterMng.h"
#include "KanturuUtil.h"
#include "ReadFile.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuBattleStanby::CKanturuBattleStanby() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleStanbyState(KANTURU_STANBY_NONE);

	this->SetSuccessValue(0);

	this->ResetAllData();
	
	#endif
}

CKanturuBattleStanby::~CKanturuBattleStanby() // OK
{

}

void CKanturuBattleStanby::ResetAllData() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < KANTURU_STANBY_MAX;n++)
	{
		this->m_StateInfo[n].ResetTimeInfo();
	}

	this->m_StateInfoCount = 0;
	
	#endif
}

void CKanturuBattleStanby::Load(char* path) // OK
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
				if(section == 1)
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

void CKanturuBattleStanby::MainProc() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->CheckStateTime();

	switch(this->m_BattleStanbyState)
	{
		case KANTURU_STANBY_NONE:
			this->ProcState_NONE();
			break;
		case KANTURU_STANBY_START:
			this->ProcState_START();
			break;
		case KANTURU_STANBY_NOTIFY:
			this->ProcState_NOTIFY();
			break;
		case KANTURU_STANBY_END:
			this->ProcState_END();
			break;
	}
	
	#endif
}

void CKanturuBattleStanby::ProcState_NONE() // OK
{

}

void CKanturuBattleStanby::ProcState_START() // OK
{

}

void CKanturuBattleStanby::ProcState_NOTIFY() // OK
{

}

void CKanturuBattleStanby::ProcState_END() // OK
{

}

void CKanturuBattleStanby::SetState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_StateInfo[state].SetConditionAppliedTime();

	gKanturuMonsterMng->ResetRegenMonsterObjData();

	switch(state)
	{
		case KANTURU_STANBY_NONE:
			this->SetState_NONE();
			break;
		case KANTURU_STANBY_START:
			this->SetState_START();
			break;
		case KANTURU_STANBY_NOTIFY:
			this->SetState_NOTIFY();
			break;
		case KANTURU_STANBY_END:
			this->SetState_END();
			break;
	}

	gKanturuUtil->NotifyKanturuChangeState(KANTURU_STATE_BATTLE_STANDBY,this->m_BattleStanbyState);
	
	#endif
}

void CKanturuBattleStanby::SetState_NONE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleStanbyState(KANTURU_STANBY_NONE);
	
	#endif
}

void CKanturuBattleStanby::SetState_START() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleStanbyState(KANTURU_STANBY_START);
	
	#endif
}

void CKanturuBattleStanby::SetState_NOTIFY() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleStanbyState(KANTURU_STANBY_NOTIFY);

	gNotice->GCNoticeSendToAll(0,0,0,0,0,0,gMessage->GetMessage(62));
	
	#endif
}

void CKanturuBattleStanby::SetState_END() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetBattleStanbyState(KANTURU_STANBY_END);
	
	#endif
}

void CKanturuBattleStanby::CheckStateTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->GetBattleStanbyState() != KANTURU_STANBY_NONE && this->m_StateInfo[this->GetBattleStanbyState()].GetCondition() == 1 && this->m_StateInfo[this->GetBattleStanbyState()].IsTimeOut() != 0)
	{
		this->SetState(((this->GetBattleStanbyState()==KANTURU_STANBY_END)?KANTURU_STANBY_NONE:(this->GetBattleStanbyState()+1)));
	}
	
	#endif
}

void CKanturuBattleStanby::SetBattleStanbyState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_BattleStanbyState = state;
	
	#endif
}

void CKanturuBattleStanby::SetSuccessValue(int success) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_IsSucccess = success;
	
	#endif
}

int CKanturuBattleStanby::GetBattleStanbyState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_BattleStanbyState;

	#else

	return 0;

	#endif
}

int CKanturuBattleStanby::GetRemainTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_StateInfo[this->GetBattleStanbyState()].GetRemainTime();

	#else

	return 0;

	#endif
}
