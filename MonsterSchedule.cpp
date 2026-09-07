// MonsterSchedule.cpp: implementation of the CMonsterSchedule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterSchedule.h"
#include "ReadFile.h"
#include "Message.h"
#include "Notice.h"
#include "ScheduleManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterSchedule::CMonsterSchedule() // OK
{
	for(int n=0;n < MAX_MONSTER_SCHEDULE;n++)
	{
		MONSTER_SCHEDULE_INFO* lpInfo = &this->m_MonsterScheduleInfo[n];

		lpInfo->Index = n;
		lpInfo->State = MONSTER_SCHEDULE_STATE_BLANK;
		lpInfo->RemainTime = 0;
		lpInfo->TargetTime = 0;
		lpInfo->TickCount = GetTickCount();

		lpInfo->StartTime.clear();

		lpInfo->ValueInfo.clear();
	}
}

CMonsterSchedule::~CMonsterSchedule() // OK
{

}

void CMonsterSchedule::Init() // OK
{
	for(int n=0;n < MAX_MONSTER_SCHEDULE;n++)
	{
		this->SetState(&this->m_MonsterScheduleInfo[n],MONSTER_SCHEDULE_STATE_EMPTY);
	}
}

void CMonsterSchedule::Load(char* path) // OK
{
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

	for(int n=0;n < MAX_MONSTER_SCHEDULE;n++)
	{
		this->m_MonsterScheduleInfo[n].StartMessage = -1;
		this->m_MonsterScheduleInfo[n].FinalMessage = -1;
		this->m_MonsterScheduleInfo[n].EnableTime = 0;
		this->m_MonsterScheduleInfo[n].StartTime.clear();
		this->m_MonsterScheduleInfo[n].ValueInfo.clear();
	}

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

					MONSTER_SCHEDULE_START_TIME info;

					int index = lpReadFile->GetNumber();

					info.Year = lpReadFile->GetAsNumber();

					info.Month = lpReadFile->GetAsNumber();

					info.Day = lpReadFile->GetAsNumber();

					info.DayOfWeek = lpReadFile->GetAsNumber();

					info.Hour = lpReadFile->GetAsNumber();

					info.Minute = lpReadFile->GetAsNumber();

					info.Second = lpReadFile->GetAsNumber();

					this->m_MonsterScheduleInfo[index].StartTime.push_back(info);
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int index = lpReadFile->GetNumber();

					this->m_MonsterScheduleInfo[index].StartMessage = lpReadFile->GetAsNumber();

					this->m_MonsterScheduleInfo[index].FinalMessage = lpReadFile->GetAsNumber();

					this->m_MonsterScheduleInfo[index].NpcMessage = lpReadFile->GetAsNumber();

					this->m_MonsterScheduleInfo[index].EnableTime = lpReadFile->GetAsNumber();
				}
				else if(section == 2)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					MONSTER_SCHEDULE_VALUE_INFO info;

					int index = lpReadFile->GetNumber();

					info.Index = lpReadFile->GetAsNumber();

					info.MapNumber = lpReadFile->GetAsNumber();

					info.PosX = lpReadFile->GetAsNumber();

					info.PosY = lpReadFile->GetAsNumber();

					this->m_MonsterScheduleInfo[index].ValueInfo.push_back(info);
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

	delete lpReadFile;
}

void CMonsterSchedule::MainProc() // OK
{
	for(int n=0;n < MAX_MONSTER_SCHEDULE;n++)
	{
		MONSTER_SCHEDULE_INFO* lpInfo = &this->m_MonsterScheduleInfo[n];

		if((GetTickCount()-lpInfo->TickCount) >= 1000)
		{
			lpInfo->TickCount = GetTickCount();

			lpInfo->RemainTime = (int)difftime(lpInfo->TargetTime,time(0));

			switch(lpInfo->State)
			{
				case MONSTER_SCHEDULE_STATE_BLANK:
					this->ProcState_BLANK(lpInfo);
					break;
				case MONSTER_SCHEDULE_STATE_EMPTY:
					this->ProcState_EMPTY(lpInfo);
					break;
				case MONSTER_SCHEDULE_STATE_START:
					this->ProcState_START(lpInfo);
					break;
			}
		}
	}
}

void CMonsterSchedule::ProcState_BLANK(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{

}

void CMonsterSchedule::ProcState_EMPTY(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{
	if(lpInfo->RemainTime <= 0)
	{
		if(lpInfo->StartMessage != -1){gNotice->GCNoticeSendToAll(0,0,0,0,0,0,gMessage->GetMessage(lpInfo->StartMessage));}
		this->SetState(lpInfo,MONSTER_SCHEDULE_STATE_START);
	}
}

void CMonsterSchedule::ProcState_START(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{
	if(lpInfo->RemainTime <= 0)
	{
		if(lpInfo->FinalMessage != -1){gNotice->GCNoticeSendToAll(0,0,0,0,0,0,gMessage->GetMessage(lpInfo->FinalMessage));}
		this->SetState(lpInfo,MONSTER_SCHEDULE_STATE_EMPTY);
	}
}

void CMonsterSchedule::SetState(MONSTER_SCHEDULE_INFO* lpInfo,int state) // OK
{
	switch((lpInfo->State=state))
	{
		case MONSTER_SCHEDULE_STATE_BLANK:
			this->SetState_BLANK(lpInfo);
			break;
		case MONSTER_SCHEDULE_STATE_EMPTY:
			this->SetState_EMPTY(lpInfo);
			break;
		case MONSTER_SCHEDULE_STATE_START:
			this->SetState_START(lpInfo);
			break;
	}
}

void CMonsterSchedule::SetState_BLANK(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{

}

void CMonsterSchedule::SetState_EMPTY(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{
	this->CheckSync(lpInfo);
}

void CMonsterSchedule::SetState_START(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{
	lpInfo->RemainTime = lpInfo->EnableTime;

	lpInfo->TargetTime = (int)(time(0)+lpInfo->RemainTime);
}

void CMonsterSchedule::CheckSync(MONSTER_SCHEDULE_INFO* lpInfo) // OK
{
	if(lpInfo->StartTime.empty() != 0)
	{
		this->SetState(lpInfo,MONSTER_SCHEDULE_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for(std::vector<MONSTER_SCHEDULE_START_TIME>::iterator it=lpInfo->StartTime.begin();it != lpInfo->StartTime.end();it++)
	{
		ScheduleManager.AddSchedule(it->Year,it->Month,it->Day,it->Hour,it->Minute,it->Second,it->DayOfWeek);
	}

	if(ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(lpInfo,MONSTER_SCHEDULE_STATE_BLANK);
		return;
	}

	lpInfo->RemainTime = (int)difftime(ScheduleTime.GetTime(),time(0));

	lpInfo->TargetTime = (int)ScheduleTime.GetTime();
}

int CMonsterSchedule::CheckEnableNpc(LPOBJ lpObj,LPOBJ lpNpc) // OK
{
	for(int n=0;n < MAX_MONSTER_SCHEDULE;n++)
	{
		for(std::vector<MONSTER_SCHEDULE_VALUE_INFO>::iterator it=this->m_MonsterScheduleInfo[n].ValueInfo.begin();it != this->m_MonsterScheduleInfo[n].ValueInfo.end();it++)
		{
			if(it->Index != lpNpc->Class)
			{
				continue;
			}

			if(it->MapNumber != -1 && it->MapNumber != lpNpc->Map)
			{
				continue;
			}

			if(it->PosX != -1 && it->PosX != lpNpc->X)
			{
				continue;
			}

			if(it->PosY != -1 && it->PosY != lpNpc->Y)
			{
				continue;
			}

			if(this->m_MonsterScheduleInfo[n].State == MONSTER_SCHEDULE_STATE_EMPTY)
			{
				if(this->m_MonsterScheduleInfo[n].NpcMessage != -1)
				{
					GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetMessage(this->m_MonsterScheduleInfo[n].NpcMessage));
				}

				return 1;
			}
			else if(this->m_MonsterScheduleInfo[n].State == MONSTER_SCHEDULE_STATE_START)
			{
				return 0;
			}
		}
	}

	return 0;
}