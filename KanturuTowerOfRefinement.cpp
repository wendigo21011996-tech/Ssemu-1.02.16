// KanturuTowerOfRefinement.cpp: implementation of the CKanturuTowerOfRefinement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuTowerOfRefinement.h"
#include "Kanturu.h"
#include "KanturuBattleUserMng.h"
#include "KanturuUtil.h"
#include "Map.h"
#include "ReadFile.h"
#include "Message.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuTowerOfRefinement::CKanturuTowerOfRefinement() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_NONE);

	this->SetEntrancePermit(0);

	this->SetUseTowerOfRefinement(0);

	this->ResetAllData();

	#endif
}

CKanturuTowerOfRefinement::~CKanturuTowerOfRefinement() // OK
{

}

void CKanturuTowerOfRefinement::ResetAllData() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < KANTURU_TOWER_OF_REFINEMENT_MAX;n++)
	{
		this->m_StateInfo[n].ResetTimeInfo();
	}

	this->m_StateInfoCount = 0;

	#endif
}

void CKanturuTowerOfRefinement::Load(char* path) // OK
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
				if(section == 9)
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

void CKanturuTowerOfRefinement::MainProc() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->CheckStateTime();

	#endif
}

void CKanturuTowerOfRefinement::SetState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_StateInfo[state].SetConditionAppliedTime();

	switch(state)
	{
		case KANTURU_TOWER_OF_REFINEMENT_NONE:
			this->SetState_NONE();
			break;
		case KANTURU_TOWER_OF_REFINEMENT_REVITALIZATION:
			this->SetState_REVITALIZATION();
			break;
		case KANTURU_TOWER_OF_REFINEMENT_NOTIFY_1:
			this->SetState_NOTIFY1();
			break;
		case KANTURU_TOWER_OF_REFINEMENT_CLOSE:
			this->SetState_CLOSE();
			break;
		case KANTURU_TOWER_OF_REFINEMENT_NOTIFY_2:
			this->SetState_NOTIFY2();
			break;
		case KANTURU_TOWER_OF_REFINEMENT_END:
			this->SetState_END();
			break;
	}

	gKanturuUtil->NotifyKanturuChangeState(KANTURU_STATE_TOWER_OF_REFINEMENT,this->m_TowerOfRefinementState);

	#endif
}

void CKanturuTowerOfRefinement::SetState_NONE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_NONE);

	this->SetEntrancePermit(0);

	this->SetUseTowerOfRefinement(0);

	gKanturuBattleUserMng->ResetAllData();

	#endif
}

void CKanturuTowerOfRefinement::SetState_REVITALIZATION() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_REVITALIZATION);

	this->SetEntrancePermit(1);

	this->SetUseTowerOfRefinement(1);

	#endif
}

void CKanturuTowerOfRefinement::SetState_NOTIFY1() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_NOTIFY_1);

	this->SetEntrancePermit(1);

	this->SetUseTowerOfRefinement(1);

	gKanturuUtil->SendMsgKanturuBossMapUser(gMessage->GetMessage(60));

	#endif
}

void CKanturuTowerOfRefinement::SetState_CLOSE() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_CLOSE);

	this->SetEntrancePermit(0);

	this->SetUseTowerOfRefinement(1);

	gKanturuUtil->SendMsgKanturuBossMapUser(gMessage->GetMessage(61));

	#endif
}

void CKanturuTowerOfRefinement::SetState_NOTIFY2() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_NOTIFY_2);

	this->SetEntrancePermit(0);

	this->SetUseTowerOfRefinement(1);

	#endif
}

void CKanturuTowerOfRefinement::SetState_END() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetTowerOfRefinementState(KANTURU_TOWER_OF_REFINEMENT_END);

	this->SetEntrancePermit(0);

	this->SetUseTowerOfRefinement(0);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0 && gObj[n].Map == MAP_KANTURU3)
		{
			gObjMoveGate(n,136);
		}
	}

	#endif
}

void CKanturuTowerOfRefinement::CheckStateTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->GetTowerOfRefinementState() != KANTURU_TOWER_OF_REFINEMENT_NONE && this->m_StateInfo[this->GetTowerOfRefinementState()].GetCondition() == 1 && this->m_StateInfo[this->GetTowerOfRefinementState()].IsTimeOut() != 0)
	{
		this->SetState(((this->GetTowerOfRefinementState()==KANTURU_TOWER_OF_REFINEMENT_END)?KANTURU_TOWER_OF_REFINEMENT_NONE:(this->GetTowerOfRefinementState()+1)));
	}

	#endif
}

void CKanturuTowerOfRefinement::SetTowerOfRefinementState(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_TowerOfRefinementState = state;

	#endif
}

void CKanturuTowerOfRefinement::SetEntrancePermit(int permit) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_EntrancePermit = permit;

	#endif
}

void CKanturuTowerOfRefinement::SetUseTowerOfRefinement(int use) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_IsUseTowerOfRefinement = use;

	#endif
}

int CKanturuTowerOfRefinement::GetTowerOfRefinementState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_TowerOfRefinementState;
	
	#else

	return 0;

	#endif
}

int CKanturuTowerOfRefinement::GetEntrancePermit() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_EntrancePermit;
	
	#else

	return 0;

	#endif
}

int CKanturuTowerOfRefinement::GetUseTowerOfRefinement() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_IsUseTowerOfRefinement;
	
	#else

	return 0;

	#endif
}

int CKanturuTowerOfRefinement::GetRemainTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_StateInfo[this->GetTowerOfRefinementState()].GetRemainTime();
	
	#else

	return 0;

	#endif
}
