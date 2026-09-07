// CastleSiege.cpp: implementation of the CCastleSiege class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "GameMain.h"
#include "Guild.h"
#include "GuildManager.h"
#include "LifeStone.h"
#include "Map.h"
#include "MapServerManager.h"
#include "ReadFile.h"
#include "Mercenary.h"
#include "Message.h"
#include "Monster.h"
#include "Move.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "UnionManager.h"
#include "User.h"
#include "Util.h"
#include "Viewport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleSiege::CCastleSiege() // OK
{
	this->Init();
}

CCastleSiege::~CCastleSiege() // OK
{

}

void CCastleSiege::Init() // OK
{
	this->m_CastleSiegeSwitch = 1;
	this->m_CastleSiegeLoadFile = 0;
	this->m_CastleSiegeLoadDatabase = 0;
	this->m_CastleSiegeDoRun = 0;

	this->m_Cycle = 7;
	this->m_State = -1;

	this->m_bFixCastleCycleStartDate = 0;

	this->m_TributeMoney = 0;

	memset(&this->m_FixCastleCycleStartDate,0,sizeof(this->m_FixCastleCycleStartDate));

	this->m_iFixCastleSpecificState = 0;
	this->m_bFixCastleStateStartDate = 0;

	memset(&this->m_FixCastleStateStartDate,0,sizeof(this->m_FixCastleStateStartDate));

	this->m_CastleSiegeDataLoad = 1;
	this->m_CastleSiegeDataLoadTime = 0;
	this->m_RemainTime = -1;
	this->m_TargetTime = -1;

	memset(&this->m_StateStartDate,0,sizeof(this->m_StateStartDate));
	memset(&this->m_LeftCastleSiegeDate,0,sizeof(this->m_LeftCastleSiegeDate));

	this->m_BasicGuildDataLoad = 0;
	this->m_SiegeGuildDataLoad = 0;

	this->m_CastleGuildListLoad = 0;
	this->m_CastleOccupied = 0;

	memset(this->m_CastleOwner,0,sizeof(this->m_CastleOwner));

	this->m_HuntEnabled = 0;

	this->m_SaveNpcTime = GetTickCount();
	this->m_CalcLeftTime = 0;
	this->m_MessageTime = 0;
	this->m_CheckGateTime = 0;
	this->m_LeftTime = 0;
	this->m_LeftStartTime = 0;
	this->m_UpdateSideTime = 0;
	this->m_MiniMapTime = 0;

	this->m_CastleTowerEnable = 0;
	this->m_CrownAvailable = 0;

	this->m_CrownAccessUser = -1;

	this->m_CrownAccessUserX = 0;
	this->m_CrownAccessUserY = 0;

	this->m_CrownSwitchUser1 = -1;
	this->m_CrownSwitchUser2 = -1;

	this->m_CrownAccessTime = 0;

	memset(this->m_MiddleOwner,0,sizeof(this->m_MiddleOwner));

	this->m_StateTimeData.clear();

	this->m_NpcDataList.clear();

	this->m_CastleNpcCreated = 0;
}

void CCastleSiege::Load(char* path)
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

	this->Init();

	this->ClearAllNpc();

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

					this->m_Cycle = lpReadFile->GetNumber();
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CASTLE_STATE_TIME_DATA info;

					info.Active = 1;

					info.State = lpReadFile->GetNumber();

					info.Day = lpReadFile->GetAsNumber();

					info.Hour = lpReadFile->GetAsNumber();

					info.Minute = lpReadFile->GetAsNumber();

					this->m_StateTimeData.push_back(info);
				}
				else if(section == 2)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CASTLE_SIEGE_NPC_DATA info;

					info.Class = lpReadFile->GetNumber();
					
					info.Index = lpReadFile->GetAsNumber();
					
					info.DbSave = lpReadFile->GetAsNumber();

					info.Side = lpReadFile->GetAsNumber();

					info.DefaultDefenseLevel = lpReadFile->GetAsNumber();

					info.DefaultRegenLevel = lpReadFile->GetAsNumber();

					info.DefaultLifeLevel = lpReadFile->GetAsNumber();

					info.DefaultLife = info.DefaultLifeLevel;;

					info.DefaultX = lpReadFile->GetAsNumber();

					info.DefaultY = lpReadFile->GetAsNumber();

					info.DefaultDir = lpReadFile->GetAsNumber();

					info.SetBaseValue();

					this->m_NpcDataList.push_back(info);
				}
				else if(section == 3)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					NpcUpgradeGateDefense[level][0] = lpReadFile->GetAsNumber();

					NpcUpgradeGateDefense[level][1] = lpReadFile->GetAsNumber();

					NpcUpgradeGateDefense[level][2] = lpReadFile->GetAsNumber();
				}
				else if(section == 4)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					NpcUpgradeStatueDefense[level][0] = lpReadFile->GetAsNumber();

					NpcUpgradeStatueDefense[level][1] = lpReadFile->GetAsNumber();

					NpcUpgradeStatueDefense[level][2] = lpReadFile->GetAsNumber();
				}
				else if(section == 5)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					NpcUpgradeGateLife[level][0] = lpReadFile->GetAsNumber();

					NpcUpgradeGateLife[level][1] = lpReadFile->GetAsNumber();

					NpcUpgradeGateLife[level][2] = lpReadFile->GetAsNumber();
				}
				else if(section == 6)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					NpcUpgradeStatueLife[level][0] = lpReadFile->GetAsNumber();

					NpcUpgradeStatueLife[level][1] = lpReadFile->GetAsNumber();

					NpcUpgradeStatueLife[level][2] = lpReadFile->GetAsNumber();
				}
				else if(section == 7)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					NpcUpgradeStatueRegen[level][0] = lpReadFile->GetAsNumber();

					NpcUpgradeStatueRegen[level][1] = lpReadFile->GetAsNumber();

					NpcUpgradeStatueRegen[level][2] = lpReadFile->GetAsNumber();
				}
				else if(section == 8)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					NpcDefense[0][0] = lpReadFile->GetNumber();

					NpcDefense[0][1] = lpReadFile->GetAsNumber();

					NpcDefense[0][2] = lpReadFile->GetAsNumber();

					NpcDefense[0][3] = lpReadFile->GetAsNumber();
				}
				else if(section == 9)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					NpcDefense[1][0] = lpReadFile->GetNumber();

					NpcDefense[1][1] = lpReadFile->GetAsNumber();

					NpcDefense[1][2] = lpReadFile->GetAsNumber();

					NpcDefense[1][3] = lpReadFile->GetAsNumber();
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

	sort(this->m_StateTimeData.begin(),this->m_StateTimeData.end(),this->ScheduleStateCompFunc);

	if(this->m_StateTimeData.empty() == 0)
	{
		for(size_t n = 0; n < this->m_StateTimeData.size(); n++)
		{
			if((n+1) < this->m_StateTimeData.size())
			{
				int second1 = (this->m_StateTimeData[n].Day*86400)+(this->m_StateTimeData[n].Hour*3600)+(this->m_StateTimeData[n].Minute*60);
				int second2 = (this->m_StateTimeData[n+1].Day*86400)+(this->m_StateTimeData[n+1].Hour*3600)+(this->m_StateTimeData[n+1].Minute*60);

				int result = second2 -second1;

				if(result < 0)
				{
					LogAdd(LOG_RED,"[Castle Siege] CCastleSiege::LoadData() - Date Order is wrong (sort fail) : %d-%d",this->m_StateTimeData[n].State,this->m_StateTimeData[n+1].State);
				}
				else
				{
					this->m_StateTimeData[n].Second = result;
				}
			}
			else
			{
				this->m_StateTimeData[n].Second = 0;
			}
		}
	}

	this->m_CastleSiegeLoadFile = 1;

	_SYSTEMTIME tmToDay;

	GetLocalTime(&tmToDay);

	wsprintf(path,".\\Data\\GameServerInfo - Event.dat");

	this->m_CastleSiegeSwitch = GetPrivateProfileInt("GameServerInfo","CastleSiegeEvent",0,path);
	this->m_FixCastleCycleStartDate.wYear = GetPrivateProfileInt("GameServerInfo","CastleSiegeCycleStartYear",0,path);
	this->m_FixCastleCycleStartDate.wMonth = GetPrivateProfileInt("GameServerInfo","CastleSiegeCycleStartMonth",0,path);
	this->m_FixCastleCycleStartDate.wDay = GetPrivateProfileInt("GameServerInfo","CastleSiegeCycleStartDay",0,path);

	this->m_iFixCastleSpecificState = GetPrivateProfileInt("GameServerInfo","CastleSiegeSpecificState",0,path);
	this->m_FixCastleStateStartDate.wYear = GetPrivateProfileInt("GameServerInfo","CastleSiegeStateStartYear",0,path);
	this->m_FixCastleStateStartDate.wMonth = GetPrivateProfileInt("GameServerInfo","CastleSiegeStateStartMonth",0,path);
	this->m_FixCastleStateStartDate.wDay = GetPrivateProfileInt("GameServerInfo","CastleSiegeStateStartDay",0,path);
	this->m_FixCastleStateStartDate.wHour = GetPrivateProfileInt("GameServerInfo","CastleSiegeStateStartHour",0,path);
	this->m_FixCastleStateStartDate.wMinute = GetPrivateProfileInt("GameServerInfo","CastleSiegeStateStartMinute",0,path);

	if(this->m_FixCastleCycleStartDate.wYear == 0 || this->m_FixCastleCycleStartDate.wMonth == 0 || this->m_FixCastleCycleStartDate.wDay == 0)
	{
		this->m_bFixCastleCycleStartDate = 0;
	}
	else
	{
		_SYSTEMTIME tmEndDate = this->m_FixCastleCycleStartDate;
		
		this->GetNextDay(&tmEndDate,this->m_Cycle,0,0,0);

		int TodayDate = MAKELONG(MAKEWORD(tmToDay.wDay,tmToDay.wMonth),tmToDay.wYear);
		int EventEndDate = MAKELONG(MAKEWORD(tmEndDate.wDay,tmEndDate.wMonth),tmEndDate.wYear);

		if(EventEndDate <= TodayDate)
		{
			this->m_bFixCastleCycleStartDate = 0;
		}
		else
		{
			this->m_bFixCastleCycleStartDate = 1;
		}

		if(CHECK_RANGE(this->m_FixCastleCycleStartDate.wMonth,13) == 0 || CHECK_RANGE(this->m_FixCastleCycleStartDate.wDay-1,31) == 0)
		{
			this->m_bFixCastleCycleStartDate = 0;
		}
	}

	if(this->m_FixCastleStateStartDate.wYear == 0 || this->m_FixCastleStateStartDate.wMonth == 0 || this->m_FixCastleStateStartDate.wDay == 0)
	{
		this->m_bFixCastleStateStartDate = 0;
	}
	else if(this->m_iFixCastleSpecificState < CS_STATE_SIGN1 || this->m_iFixCastleSpecificState > CS_STATE_END_CYCLE)
	{
		this->m_bFixCastleStateStartDate = 0;
	}
	else
	{
		QWORD TodayDate = MAKE_NUMBERLL(MAKELONG(tmToDay.wMinute, tmToDay.wHour), (MAKELONG(MAKEWORD(tmToDay.wDay, tmToDay.wMonth), tmToDay.wYear)));
		QWORD EventEndDate = MAKE_NUMBERLL(MAKELONG(this->m_FixCastleStateStartDate.wMinute,this->m_FixCastleStateStartDate.wHour),(MAKELONG(MAKEWORD(this->m_FixCastleStateStartDate.wDay,this->m_FixCastleStateStartDate.wMonth),this->m_FixCastleStateStartDate.wYear)));

		if(EventEndDate <= TodayDate)
		{
			this->m_bFixCastleStateStartDate = 0;
		}
		else
		{
			this->m_bFixCastleStateStartDate = 1;
		}

		if(this->m_iFixCastleSpecificState == CS_STATE_END)
		{
			this->m_bFixCastleStateStartDate = 0;
		}

		if(CHECK_RANGE(this->m_FixCastleStateStartDate.wMonth,13) == 0 || CHECK_RANGE(this->m_FixCastleStateStartDate.wDay - 1,31) == 0 || CHECK_RANGE(this->m_FixCastleStateStartDate.wHour,24) == 0 || CHECK_RANGE(this->m_FixCastleStateStartDate.wMinute,60) == 0)
		{
			this->m_bFixCastleStateStartDate = 0;
		}
	}

	WritePrivateProfileString("GameServerInfo","CastleSiegeCycleStartYear","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeCycleStartMonth","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeCycleStartDay","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeSpecificState","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeStateStartYear","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeStateStartMonth","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeStateStartDay","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeStateStartHour","0",path);
	WritePrivateProfileString("GameServerInfo","CastleSiegeStateStartMinute","0",path);

	this->m_CastleSiegeDataLoad = 2;
}

void CCastleSiege::MainProc() // OK
{
	if(this->m_CastleSiegeDoRun == 0)
	{
		return;
	}

	if(this->m_CastleSiegeSwitch == 0)
	{
		return;
	}

	switch(this->m_State)
	{
		case CS_STATE_BLANK:
			this->ProcState_BLANK();
			break;
		case CS_STATE_IDLE_1:
			this->ProcState_IDLE1();
			break;
		case CS_STATE_SIGN1:
			this->ProcState_SIGN1();
			break;
		case CS_STATE_IDLE_2:
			this->ProcState_IDLE2();
			break;
		case CS_STATE_SIGN2:
			this->ProcState_SIGN2();
			break;
		case CS_STATE_IDLE_3:
			this->ProcState_IDLE3();
			break;
		case CS_STATE_NOTIFY:
			this->ProcState_NOTIFY();
			break;
		case CS_STATE_READY:
			this->ProcState_READY();
			break;
		case CS_STATE_START:
			this->ProcState_START();
			break;
		case CS_STATE_END:
			this->ProcState_END();
			break;
		case CS_STATE_END_CYCLE:
			this->ProcState_END_CYCLE();
			break;
	}

	if((GetTickCount()-this->m_SaveNpcTime) > 300000)
	{
		if(this->GetState() != CS_STATE_START)
		{
			this->GDCastleSiegeNpcSaveSend();
		}

		this->m_SaveNpcTime = GetTickCount();
	}

	if((GetTickCount()-this->m_CalcLeftTime) > 10000)
	{
		this->m_CalcLeftTime = GetTickCount();
		this->GetLeftSiegeDate();
	}

	static int LastTickCount = 0;

	if((GetTickCount()-LastTickCount) > 3600000)
	{
		LastTickCount = GetTickCount();

		int LeftSiegeDate = this->GetLeftSiegeDate();

		if(LeftSiegeDate >= 0)
		{
			LogAdd(LOG_BLACK,"[Castle Siege] Sync Start Time. [%02d] days remain",(LeftSiegeDate/60/60/24));
		}
	}
}

void CCastleSiege::ProcState_BLANK() // OK
{

}

void CCastleSiege::ProcState_IDLE1() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_SIGN1,1);
	}
}

void CCastleSiege::ProcState_SIGN1() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();

		if((GetTickCount()-this->m_MessageTime) > 1800000)
		{
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(111));
			this->m_MessageTime = GetTickCount();
		}
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_IDLE_2,1);
	}
}

void CCastleSiege::ProcState_IDLE2() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_SIGN2,1);
	}
}

void CCastleSiege::ProcState_SIGN2() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();

		if((GetTickCount()-this->m_MessageTime) > 1800000)
		{
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(112));
			this->m_MessageTime = GetTickCount();
		}
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_IDLE_3,1);
	}
}

void CCastleSiege::ProcState_IDLE3() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();

		if((GetTickCount()-this->m_MessageTime) > 7200000)
		{
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(113));
			this->m_MessageTime = GetTickCount();
		}
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_NOTIFY,1);
	}
}

void CCastleSiege::ProcState_NOTIFY() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();

		if((GetTickCount()-this->m_MessageTime) > 1800000)
		{
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(114));
			this->m_MessageTime = GetTickCount();
		}
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_READY,1);
	}
}

void CCastleSiege::ProcState_READY() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();

		if((GetTickCount()-this->m_MessageTime) > 1800000)
		{
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(115));
			this->m_MessageTime = GetTickCount();
		}

		if(this->m_RemainTime <= 300000)
		{
			if((GetTickCount()-this->m_LeftStartTime) > 60000)
			{
				GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(116),(this->m_RemainTime/60000)+1);

				this->m_LeftStartTime = GetTickCount();
			}
		}
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_START,1);
	}
}

void CCastleSiege::ProcState_START() // OK
{
	this->CheckMiddleWinnerGuild();

	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();

		if((GetTickCount()-this->m_CheckGateTime) > 5000)
		{
			this->CheckCastleNPCLive();
			this->m_CheckGateTime = GetTickCount();
		}

		if((GetTickCount()-this->m_LeftTime) > 180000)
		{
			if(this->m_RemainTime > 180000)
			{
				this->GCCastleSiegeLeftTimeSendToAll();
				this->m_LeftTime = GetTickCount();
			}
		}

		if(this->m_RemainTime <= 180000)
		{
			if((GetTickCount()-this->m_LeftTime) > 60000)
			{
				this->GCCastleSiegeLeftTimeSendToAll();
				this->m_LeftTime = GetTickCount();
			}
		}

		if((GetTickCount()-this->m_UpdateSideTime) > 5000)
		{
			this->SetCastleJoinSide();
			this->m_UpdateSideTime = GetTickCount();
		}

		if((GetTickCount()-this->m_MiniMapTime) > 3000)
		{
			this->OperateMiniMapWork();
			this->m_MiniMapTime = GetTickCount();
		}

		if((GetTickCount()-this->m_MessageTime) > 1800000)
		{
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(117));
			this->m_MessageTime = GetTickCount();
		}
	}

	if(this->m_RemainTime <= 0)
	{
		if(this->CheckCastleSiegeResult() == 1)
		{
			this->GDCastleSiegeResetTaxInfoSend();
		}

		this->RespawnPlayers(0);

		if(this->m_CastleOccupied == 1)
		{
			this->GDCastleSiegeOwnerChangeSend();
		}

		this->GDCastleSiegeEndSend();

		this->GDCastleSiegeNpcSaveSend();
		this->GCCastleSiegeStartStateSendToAll(0);

		this->GDCastleSiegeParticipantClearSend();

		this->SetState(CS_STATE_END,1);
	}
}

void CCastleSiege::ProcState_END() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_END_CYCLE,1);
	}
}

void CCastleSiege::ProcState_END_CYCLE() // OK
{
	if((GetTickCount()-this->m_TickCount) >= 1000)
	{
		this->m_RemainTime = (int)(difftime(this->m_TargetTime,time(0))*1000);
		this->m_TickCount = GetTickCount();
	}

	if(this->m_RemainTime <= 0)
	{
		this->SetState(CS_STATE_IDLE_1,1);
	}
}

void CCastleSiege::SetState(int state,bool reset) // OK
{
	if(state < CS_STATE_BLANK || state > CS_STATE_END_CYCLE)
	{
		return;
	}

	this->m_State = state;

	switch(this->m_State)
	{
		case CS_STATE_BLANK:
			this->SetState_BLANK(reset);
			break;
		case CS_STATE_IDLE_1:
			this->SetState_IDLE1(reset);
			break;
		case CS_STATE_SIGN1:
			this->SetState_SIGN1(reset);
			break;
		case CS_STATE_IDLE_2:
			this->SetState_IDLE2(reset);
			break;
		case CS_STATE_SIGN2:
			this->SetState_SIGN2(reset);
			break;
		case CS_STATE_IDLE_3:
			this->SetState_IDLE3(reset);
			break;
		case CS_STATE_NOTIFY:
			this->SetState_NOTIFY(reset);
			break;
		case CS_STATE_READY:
			this->SetState_READY(reset);
			break;
		case CS_STATE_START:
			this->SetState_START(reset);
			break;
		case CS_STATE_END:
			this->SetState_END(reset);
			break;
		case CS_STATE_END_CYCLE:
			this->SetState_END_CYCLE(reset);
			break;
	}

	if(this->m_CastleNpcCreated == 0)
	{
		this->m_CastleNpcCreated = 1;
		this->CreateNpcDB();
	}

	GetLocalTime(&this->m_StateStartDate);

	this->m_MessageTime = 0;
	this->m_CheckGateTime = 0;
}

void CCastleSiege::SetState_BLANK(bool reset) // OK
{

}

void CCastleSiege::SetState_IDLE1(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState IDLE1");

	if(reset == 1)
	{
		time_t ttSTime;
		time_t ttETime;
		tm tmETime;
		double dResultSecond;

		memset(&tmETime,0,sizeof(tmETime));

		tmETime.tm_year = this->m_StartDate.wYear - 1900;
		tmETime.tm_mon = this->m_StartDate.wMonth - 1;
		tmETime.tm_mday = this->m_StartDate.wDay;

		ttETime = mktime(&tmETime);
		time(&ttSTime);

		dResultSecond = difftime(ttETime,ttSTime);
		this->m_RemainTime = (int)(dResultSecond*(double)1000.0);
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}
}

void CCastleSiege::SetState_SIGN1(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState SIGN1");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_SIGN1);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}

	this->GDCastleSiegeResetStateSend();
}

void CCastleSiege::SetState_IDLE2(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState IDLE2");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_IDLE_2);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}
}

void CCastleSiege::SetState_SIGN2(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState SIGN2");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_SIGN2);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}
}

void CCastleSiege::SetState_IDLE3(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState IDLE3");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_IDLE_3);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}
}

void CCastleSiege::SetState_NOTIFY(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState NOTIFY");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_NOTIFY);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}

	this->ClearAndLoadGuildList();
}

void CCastleSiege::SetState_READY(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState READY");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_READY);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}

	this->ClearAndLoadGuildList();
}

void CCastleSiege::SetState_START(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState START");

	if(reset == 1)
	{
		int NextRemainTime = this->GetNextRemainTime(CS_STATE_START);

		if(NextRemainTime < 0)
		{
			this->m_CastleSiegeDoRun = 0;
			ErrorMessageBox("[Castle Siege] NextRemainTime < 0");
		}

		this->m_RemainTime = NextRemainTime*1000;
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}

	this->m_CastleTowerEnable = 0;
	this->m_CrownAvailable = 0;

	this->m_CrownAccessUser = -1;

	this->m_CrownAccessUserX = 0;
	this->m_CrownAccessUserY = 0;

	this->m_CrownSwitchUser1 = -1;
	this->m_CrownSwitchUser2 = -1;

	this->m_CrownAccessTime = 0;

	this->m_CheckGateTime = 0;
	this->m_LeftStartTime = 0;
	this->m_LeftTime = GetTickCount();
	this->m_UpdateSideTime = GetTickCount();
	this->m_MiniMapTime = GetTickCount();

	memset(this->m_MiddleOwner,0,sizeof(this->m_MiddleOwner));

	if(this->m_SiegeGuildDataMap.empty() == 1)
	{
		this->CheckCastleSiegeResult();
		this->SetState(CS_STATE_END,1);
		LogAdd(LOG_RED,"[Castle Siege] SetState START - No Attack Guild");
	}
	else
	{
		this->GDCastleSiegeNpcSaveSend();
		this->ClearNpcNoDB();
		this->CheckReviveNonDbNPC();

		if(this->CheckGuardianStatueExist() == 0)
		{
			this->ClearCastleTowerBarrier();
			this->m_CastleTowerEnable = 1;
		}

		this->AdjustDbNpcLevel();
		this->CloseAllCastleGates();
		this->SetCastleJoinSide();
		this->RespawnPlayers(1);
		this->GCCastleSiegeStartStateSendToAll(1);
	}
}

void CCastleSiege::SetState_END(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState END");

	if(reset == 1)
	{
		tm tmETime;

		memset(&tmETime,0,sizeof(tmETime));

		tmETime.tm_year = this->m_EndDate.wYear-1900;
		tmETime.tm_mon = this->m_EndDate.wMonth-1;
		tmETime.tm_mday = this->m_EndDate.wDay;

		time_t ttSTime;

		time(&ttSTime);

		time_t ttETime = mktime(&tmETime);

		double dResultSecond = difftime(ttETime,ttSTime);
		this->m_RemainTime = (int)(dResultSecond*(double)1000.0);
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
	}

	this->ClearNpcNoDB();
	this->ResetPlayerJoinSide();
	GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(110));
}

void CCastleSiege::SetState_END_CYCLE(bool reset) // OK
{
	LogAdd(LOG_BLACK,"[Castle Siege] SetState END_CYCLE");

	SYSTEMTIME tmNowDate;

	GetLocalTime(&tmNowDate);

	tmNowDate.wHour = 0;
	tmNowDate.wMinute = 0;
	tmNowDate.wSecond = 0;
	tmNowDate.wMilliseconds = 0;

	tmNowDate = this->m_SiegeEndSchedule;

	this->m_StartDate = this->m_SiegeEndSchedule;

	this->GetNextDay(&tmNowDate,this->m_Cycle,0,0,0);

	this->GetNextDay(&this->m_SiegeEndSchedule,this->m_Cycle,0,0,0);

	this->m_EndDate = tmNowDate;

	this->GDCastleSiegeDateChangeSend();

	this->GDCastleSiegeRegisteredClearSend();
	
	this->GDCastleSiegeParticipantClearSend();

	this->SetState(CS_STATE_SIGN1,1);
}

int CCastleSiege::GetState() // OK
{
	return this->m_State;
}

int CCastleSiege::CheckSync() // OK
{
	if(this->m_CastleSiegeLoadFile == 0)
	{
		LogAdd(LOG_RED,"[Castle Siege] CCastleSiege::CheckSync() - m_CastleSiegeLoadFile == 0");
		return 0;
	}

	this->m_TickCount = GetTickCount();

	_SYSTEMTIME tmToDay;

	GetLocalTime(&tmToDay);

	if(this->m_bFixCastleCycleStartDate != 0)
	{
		this->m_StartDate.wYear = this->m_FixCastleCycleStartDate.wYear;
		this->m_StartDate.wMonth = this->m_FixCastleCycleStartDate.wMonth;
		this->m_StartDate.wDay = this->m_FixCastleCycleStartDate.wDay;

		this->m_EndDate = this->m_StartDate;
		this->GetNextDay(&this->m_EndDate,this->m_Cycle,0,0,0);
	}

	int iTODAY_DATE_NUM = MAKELONG(MAKEWORD(tmToDay.wDay,tmToDay.wMonth),tmToDay.wYear);
	int iEVENT_END_DATE_NUM = MAKELONG(MAKEWORD(this->m_EndDate.wDay,this->m_EndDate.wMonth),this->m_EndDate.wYear);

	if(iEVENT_END_DATE_NUM <= iTODAY_DATE_NUM)
	{
		this->m_StartDate = tmToDay;

		this->m_EndDate = tmToDay;
		
		this->GetNextDay(&this->m_EndDate,this->m_Cycle,0,0,0);

		this->GDCastleSiegeDateChangeSend();

		return this->CheckSync();
	}

	if(this->m_bFixCastleCycleStartDate != 0)
	{
		this->GDCastleSiegeDateChangeSend();
	}

	if(this->m_bFixCastleStateStartDate != 0)
	{
		bool bSpecificStateExist = 0;

		std::vector<CASTLE_STATE_TIME_DATA>::iterator it = this->m_StateTimeData.begin();

		while (it != this->m_StateTimeData.end())
		{
			CASTLE_STATE_TIME_DATA & pScheduleData = (CASTLE_STATE_TIME_DATA &)*it;

			if(pScheduleData.State == this->m_iFixCastleSpecificState)
			{
				bSpecificStateExist = 1;
				break;
			}

			it++;
		}

		if(bSpecificStateExist != 0)
		{
			time_t ttSTime;
			time_t ttETime;
			tm tmETime;
			double dResultSecond;
			memset(&tmETime,0,sizeof(tmETime));

			tmETime.tm_year = this->m_FixCastleStateStartDate.wYear-1900;
			tmETime.tm_mon = this->m_FixCastleStateStartDate.wMonth-1;
			tmETime.tm_mday = this->m_FixCastleStateStartDate.wDay;
			tmETime.tm_hour = this->m_FixCastleStateStartDate.wHour;
			tmETime.tm_min = this->m_FixCastleStateStartDate.wMinute;

			time(&ttSTime);
			ttETime = mktime(&tmETime);

			dResultSecond = difftime(ttETime,ttSTime);

			this->m_RemainTime = (int)(dResultSecond*(double)1000.0);
			this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
			this->m_State = this->m_iFixCastleSpecificState - 1;
			this->SetState(this->m_State,0);
			return 1;
		}
		else
		{
			ErrorMessageBox("[Castle Siege] CCastleSiege::CheckSync() - FIXED STATE:%d NOT FOUND IN SCHEDULE (P.S.> Check File 'MuCastleData.dat','commonserver.cfg')",this->m_iFixCastleSpecificState);
			this->m_bFixCastleStateStartDate = 0;
		}
	}

	if(this->m_CastleSiegeEnded != 0)
	{
		this->SetState(CS_STATE_END,1);
		return 1;
	}

	int iEVENT_START_DATE_NUM = MAKELONG(MAKEWORD(this->m_StartDate.wDay,this->m_StartDate.wMonth),this->m_StartDate.wYear);

	if(iEVENT_START_DATE_NUM > iTODAY_DATE_NUM)
	{
		this->SetState(CS_STATE_IDLE_1,1);
		return 1;
	}
	else
	{
		int iCastleInitState = 0;
		bool bStateSetted = 0;
		CASTLE_STATE_TIME_DATA pScheData_FR;
		CASTLE_STATE_TIME_DATA pScheData_RR;
		std::vector<CASTLE_STATE_TIME_DATA>::iterator it;

		it = this->m_StateTimeData.begin();

		while (it != this->m_StateTimeData.end())
		{
			CASTLE_STATE_TIME_DATA & pScheduleData = (CASTLE_STATE_TIME_DATA &)*it;;
			_SYSTEMTIME tmSchduleDate;

			tmSchduleDate = this->m_StartDate;
			GetNextDay(&tmSchduleDate,pScheduleData.Day,0,0,0);
			tmSchduleDate.wHour = pScheduleData.Hour;
			tmSchduleDate.wMinute = pScheduleData.Minute;

			INT64 i64ScheduleDateNum = ((INT64)((INT64)MAKELONG(tmSchduleDate.wMinute,tmSchduleDate.wHour) | (INT64)((INT64)MAKELONG(MAKEWORD(tmSchduleDate.wDay,tmSchduleDate.wMonth),tmSchduleDate.wYear) << 32)));
			INT64 i64ToDayDateNum = ((INT64)((INT64)MAKELONG(tmToDay.wMinute,tmToDay.wHour) | (INT64)((INT64)MAKELONG(MAKEWORD(tmToDay.wDay,tmToDay.wMonth),tmToDay.wYear) << 32)));

			if(i64ScheduleDateNum > i64ToDayDateNum)
			{
				bStateSetted = 1;
				pScheData_RR = pScheduleData;
				this->m_State = iCastleInitState;
				break;
			}

			iCastleInitState = pScheduleData.State;
			pScheData_FR = pScheduleData;

			it++;
		}

		_SYSTEMTIME tmSchduleDate;

		time_t ttSTime;
		time_t ttETime;
		struct tm tmETime;
		double dResultSecond;

		if(bStateSetted == 0)
		{
			ErrorMessageBox("[Castle Siege] CCastleSiege::CheckSync() - bStateSetted == 0  START_DATE (%04d-%02d-%02d),END_DATE (%04d-%02d-%02d)",this->m_StartDate.wYear,this->m_StartDate.wMonth,this->m_StartDate.wDay,this->m_EndDate.wYear,this->m_EndDate.wMonth,this->m_EndDate.wDay);
			return 0;
		}

		tmSchduleDate = this->m_StartDate;
		GetNextDay(&tmSchduleDate,pScheData_RR.Day,0,0,0);
		tmSchduleDate.wHour = pScheData_RR.Hour;
		tmSchduleDate.wMinute = pScheData_RR.Minute;

		memset(&tmETime,0,sizeof(tmETime));

		tmETime.tm_year = tmSchduleDate.wYear - 1900;
		tmETime.tm_mon = tmSchduleDate.wMonth - 1;
		tmETime.tm_mday = tmSchduleDate.wDay;
		tmETime.tm_hour = tmSchduleDate.wHour;
		tmETime.tm_min = tmSchduleDate.wMinute;

		time(&ttSTime);
		ttETime = mktime(&tmETime);

		dResultSecond = difftime(ttETime,ttSTime);
		this->m_RemainTime = (int)(dResultSecond*(double)1000.0);
		this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);

		if(this->GetState() == CS_STATE_START)
		{
			this->m_State = CS_STATE_READY;
			this->m_RemainTime = 900000;
			this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
		}
		else if(this->GetState() == CS_STATE_READY)
		{
			if(m_RemainTime < 900000)
			{
				this->m_State = CS_STATE_READY;
				this->m_RemainTime = 900000;
				this->m_TargetTime = (int)(time(0)+this->m_RemainTime/1000);
			}
		}

		this->SetState(this->m_State,0);
	}
	return 1;
}

CASTLE_SIEGE_NPC_DATA* CCastleSiege::GetNpcData(int NpcClass,int NpcIndex,int aIndex) // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(NpcClass != -1 && it->Class != NpcClass)
		{
			continue;
		}

		if(NpcIndex != -1 && it->Index != NpcIndex)
		{
			continue;
		}

		if(aIndex != -1 && it->aIndex != aIndex)
		{
			continue;
		}

		return &(*it);
	}

	return 0;
}

void CCastleSiege::GetNextDay(SYSTEMTIME*st,int day,int hour,int minute,int second) // OK
{
	FILETIME fst;

	SystemTimeToFileTime(st,&fst);

	QWORD date = MAKE_NUMBERLL(fst.dwLowDateTime,fst.dwHighDateTime);

	date += ((QWORD)day*864000000000)+((QWORD)hour*36000000000)+((QWORD)minute*600000000)+((QWORD)second*10000000);

	fst.dwHighDateTime = SET_NUMBERHDW(date);

	fst.dwLowDateTime = SET_NUMBERLDW(date);

	FileTimeToSystemTime(&fst,st);
}

int CCastleSiege::GetNextRemainTime(int state) // OK
{
	for(std::vector<CASTLE_STATE_TIME_DATA>::iterator it = this->m_StateTimeData.begin(); it != this->m_StateTimeData.end(); it++)
	{
		CASTLE_STATE_TIME_DATA & pScheduleData = *it;

		if(it->State == state)
		{
			return it->Second;
		}
	}

	return -1;
}

int CCastleSiege::GetLeftSiegeDate() // OK
{
	memset(&this->m_LeftCastleSiegeDate,0,sizeof(this->m_LeftCastleSiegeDate));

	if(this->GetState() < CS_STATE_IDLE_1 || this->GetState() > CS_STATE_END_CYCLE)
	{
		return -3;
	}

	if(this->m_CastleSiegeDoRun == 0)
	{
		return -3;
	}

	if(this->GetState() == CS_STATE_START)
	{
		return -1;
	}

	if(this->m_CastleSiegeEnded == 1 || this->GetState() >= CS_STATE_END)
	{
		return -2;
	}

	int second = -1;

	bool AddTime = 0;

	for(std::vector<CASTLE_STATE_TIME_DATA>::iterator it = this->m_StateTimeData.begin(); it != this->m_StateTimeData.end(); it++)
	{
		if(it->State == CS_STATE_START)
		{
			break;
		}

		if(AddTime == 1)
		{
			second += it->Second;
		}

		if(it->State == this->m_State)
		{
			second = this->m_RemainTime/1000;
			AddTime = 1;
		}
	}

	if(second >= 0)
	{
		SYSTEMTIME tmLocalTime;
		
		GetLocalTime(&tmLocalTime);

		this->GetNextDay(&tmLocalTime,0,0,0,second);

		this->m_LeftCastleSiegeDate = tmLocalTime;

		if(second == 0)
		{
			LogAdd(LOG_RED,"[Castle Siege] CCastleSiege::CalcCastleLeftSiegeDate() - second == 0");
		}

		return second;
	}

	return -3;
}

bool CCastleSiege::GetCastleStateTerm(SYSTEMTIME* StartTime,SYSTEMTIME* EndTime) // OK
{
	if(this->GetState() < CS_STATE_IDLE_1 || this->GetState() > CS_STATE_END_CYCLE || this->m_CastleSiegeDoRun == 0)
	{
		return 0;
	}

	*StartTime = this->m_StateStartDate;

	GetLocalTime(EndTime);

	GetNextDay(EndTime,0,0,0,this->m_RemainTime/1000);

	return 1;
}

void CCastleSiege::ClearAllNpc() // OK
{
	for(int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if(gObj[n].CsNpcType != 0)
		{
			gObjDel(n);
		}
	}
}

void CCastleSiege::ClearNpcNoDB() // OK
{
	for(int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if(gObjIsConnected(n) != 0)
		{
			if(gObj[n].Map == MAP_CASTLE_SIEGE && gObj[n].CsNpcType != 0 && gObj[n].CsNpcType != 1)
			{
				if(gObj[n].Class == 219) // Castle Gate Switch
				{
					continue;
				}
				
				if(gObj[n].Class == 278) // Life Stone
				{
					gLifeStone->DeleteLifeStone(n);
				}
				else if(gObj[n].Class == 286 || gObj[n].Class == 287) // Archer, Spearman
				{
					gMercenary->DeleteMercenary(n);
				}

				gObjDel(n);
			}			
		}
	}
}

void CCastleSiege::ClearNpcLifeStone() // OK
{
	for (int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if(gObjIsConnected(n) != 0)
		{
			if(gObj[n].Map == MAP_CASTLE_SIEGE && gObj[n].CsNpcType != 0 && gObj[n].CsNpcType != 1)
			{
				if(gObj[n].Class == 278) // Life Stone
				{
					gLifeStone->DeleteLifeStone(n);
					gObjDel(n);
				}
			}
		}
	}
}

void CCastleSiege::CreateNpcDB() // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Live == 1)
		{
			int index = gObjAddMonster(MAP_CASTLE_SIEGE);

			if(OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			LPOBJ lpMonster = &gObj[index];

			if(gObjSetMonster(index,it->Class) == 0)
			{
				gObjDel(index);
				continue;
			}

			lpMonster->PosNum = -1;
			lpMonster->X = it->X;
			lpMonster->Y = it->Y;
			lpMonster->Map = MAP_CASTLE_SIEGE;
			lpMonster->TX = lpMonster->X;
			lpMonster->TY = lpMonster->Y;
			lpMonster->OldX = lpMonster->X;
			lpMonster->OldY = lpMonster->Y;
			lpMonster->Dir = it->Dir;
			lpMonster->StartX = (BYTE)lpMonster->X;
			lpMonster->StartY = (BYTE)lpMonster->Y;
			lpMonster->DieRegen = 0;
			lpMonster->MaxRegenTime = 0;
			lpMonster->Life = (float)it->Life;
			lpMonster->MaxLife = (float)it->LifeLevel;
			lpMonster->CsNpcType = 1;

			if(it->Class == 277)
			{
				lpMonster->Defense = NpcDefense[0][it->DefenseLevel];
				lpMonster->MagicDefense = NpcDefense[0][it->DefenseLevel];
				lpMonster->CastleDefenseLevel = it->DefenseLevel;

				this->SetGateBlockState(it->X,it->Y,1);

				gEffectManager->AddEffect(lpMonster,0,EFFECT_CASTLE_GATE_STATE,0,0,0,0,0);

				lpMonster->CastleGateState = 1;

				if(gObjIsConnected(it->SwitchIndex) == 0 || gObj[it->SwitchIndex].Class != 219)
				{
					it->SwitchIndex = this->CreateNpcGateSwitch(it->X,it->Y+4);

					if(OBJECT_RANGE(it->SwitchIndex) != 0)
					{
						gObj[it->SwitchIndex].CastleSwitchIndex = index;
						gObj[index].CastleSwitchIndex = it->SwitchIndex;
					}
				}
				else
				{
					gObj[it->SwitchIndex].CastleSwitchIndex = index;
					gObj[index].CastleSwitchIndex = it->SwitchIndex;
				}
			}
			else if(it->Class == 283)
			{
				lpMonster->Defense = NpcDefense[1][it->DefenseLevel];
				lpMonster->MagicDefense = NpcDefense[1][it->DefenseLevel];
				lpMonster->CastleDefenseLevel = it->DefenseLevel;
				lpMonster->CastleRegenLevel = it->RegenLevel;
			}

			it->aIndex = index;
			it->Live = 2;
		}
	}
}

void CCastleSiege::CreateAllGateSwitch() // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == 277)
		{
			if(gObjIsConnected(it->SwitchIndex) == 0 || gObj[it->SwitchIndex].Class != 219)
			{
				it->SwitchIndex = this->CreateNpcGateSwitch(it->X,it->Y+4);
			}
		}
	}
}

int CCastleSiege::CreateNpcGateSwitch(int x,int y) // OK
{
	if(CHECK_RANGE(x,256) == 0 || CHECK_RANGE(y,256) == 0)
	{
		return -1;
	}

	int index = gObjAddMonster(MAP_CASTLE_SIEGE);

	if(OBJECT_RANGE(index) == 0)
	{
		return -1;
	}

	LPOBJ lpMonster = &gObj[index];

	if(gObjSetMonster(index,219) == 0)
	{
		gObjDel(index);
		return -1;
	}

	lpMonster->PosNum = -1;
	lpMonster->X = x;
	lpMonster->Y = y;
	lpMonster->Map = MAP_CASTLE_SIEGE;
	lpMonster->TX = lpMonster->X;
	lpMonster->TY = lpMonster->Y;
	lpMonster->OldX = lpMonster->X;
	lpMonster->OldY = lpMonster->Y;
	lpMonster->Dir = 1;
	lpMonster->StartX = (BYTE)lpMonster->X;
	lpMonster->StartY = (BYTE)lpMonster->Y;
	lpMonster->DieRegen = 0;
	lpMonster->MaxRegenTime = 0;
	lpMonster->Life = 10000;
	lpMonster->MaxLife = 10000;
	lpMonster->CsNpcType = 2;
	lpMonster->CastleSwitchIndex = -1;

	return index;
}

void CCastleSiege::CheckCastleNPCLive() // OK
{
	if(this->GetState() != CS_STATE_START)
	{
		return;
	}

	if(this->m_NpcDataList.empty() == 0)
	{
		for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
		{
			CASTLE_SIEGE_NPC_DATA & NpcData = CASTLE_SIEGE_NPC_DATA(*it);

			if((NpcData.Class == 277 || NpcData.Class == 283) && NpcData.aIndex != -1)
			{
				if(gObjIsConnected(NpcData.aIndex) == 0 || (gObj[NpcData.aIndex].Class != 277 && gObj[NpcData.aIndex].Class != 283) || gObj[NpcData.aIndex].Live == 0)
				{
					NpcData.Life = NpcData.DefaultLife;
					NpcData.LifeLevel = NpcData.DefaultLifeLevel;
					NpcData.RegenLevel = NpcData.DefaultRegenLevel;
					NpcData.DefenseLevel = NpcData.DefaultDefenseLevel;

					if(NpcData.Class == 277)
					{
						this->SetGateBlockState(NpcData.X,NpcData.Y,1);
						gEffectManager->AddEffect(&gObj[NpcData.aIndex],0,EFFECT_CASTLE_GATE_STATE,0,0,0,0,0);
						LogAdd(LOG_BLACK,"[Castle Siege] Castle Gate has been destroyed. (Index:%d, X:%d, Y:%d)", it->Index, it->X, it->Y);
					}
					else if(NpcData.Class == 283)
					{
						LogAdd(LOG_BLACK,"[Castle Siege] Defend Statue has been destroyed. (Index:%d, X:%d, Y:%d)", it->Index, it->X, it->Y);

						if(this->CheckGuardianStatueExist() == 0)
						{
							this->ClearCastleTowerBarrier();
							this->m_CastleTowerEnable = 1;
							LogAdd(LOG_BLACK,"[Castle Siege] Crown Room entrance is open");
						}
					}

					it->aIndex = -1;
				}
			}
		}
	}
}

void CCastleSiege::AdjustDbNpcLevel() // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Live > 0 && gObjIsConnected(it->aIndex) != 0 && it->Class == gObj[it->aIndex].Class && gObj[it->aIndex].CsNpcType != 0)
		{
			switch(gObj[it->aIndex].Class)
			{
				case 277:
					gObj[it->aIndex].CastleDefenseLevel = it->DefenseLevel;
					gObj[it->aIndex].Defense = NpcDefense[0][it->DefenseLevel];
					break;
				case 283:
					gObj[it->aIndex].Defense = NpcDefense[1][it->DefenseLevel];
					gObj[it->aIndex].CastleDefenseLevel = it->DefenseLevel;
					gObj[it->aIndex].CastleRegenLevel = it->RegenLevel;
					break;
			}
		}
	}
}

void CCastleSiege::MonsterDie(LPOBJ lpObj,bool remove) // OK
{
	if(OBJECT_RANGE(lpObj->Index) == 0)
	{
		return;
	}

	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == lpObj->Class && it->aIndex == lpObj->Index && it->Live > 0)
		{
			it->SetBaseValue();

			it->aIndex = -1;
			it->Live = 0;

			if(it->Class == 277 || it->Class == 283)
			{
				it->Life = it->DefaultLife;
				it->LifeLevel = it->DefaultLifeLevel;
				it->RegenLevel = it->DefaultRegenLevel;
				it->DefenseLevel = it->DefaultDefenseLevel;
			}

			if(it->Class == 277)
			{
				this->SetGateBlockState(it->X,it->Y,1);

				gEffectManager->AddEffect(lpObj,0,EFFECT_CASTLE_GATE_STATE,0,0,0,0,0);

				LogAdd(LOG_BLACK,"[Castle Siege] Castle Gate has been destroyed. (Index:%d, X:%d, Y:%d)", it->Index, it->X, it->Y);
			}
			else if(it->Class == 283)
			{
				LogAdd(LOG_BLACK,"[Castle Siege] Defend Statue has been destroyed. (Index:%d, X:%d, Y:%d)", it->Index, it->X, it->Y);

				if(this->GetState() == CS_STATE_START)
				{
					if(this->CheckGuardianStatueExist() == 0)
					{
						this->ClearCastleTowerBarrier();
						this->m_CastleTowerEnable = 1;
						LogAdd(LOG_BLACK,"[Castle Siege] Crown Room entrance is open");
					}
				}
			}

			if(remove == 1 && this->GetState() != CS_STATE_START)
			{
				SDHP_CASTLE_SIEGE_NPC_REMOVE_SEND pMsg;

				pMsg.header.set(0x80,0x16,sizeof(pMsg));

				pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

				pMsg.NpcClass = it->Class;

				pMsg.NpcIndex = it->Index;

				gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
			}

			return;
		}
	}
}

void CCastleSiege::UpgradeDefenseLevel(int NpcClass,int NpcIndex,int UpgradeValue) // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == NpcClass && it->Index == NpcIndex && it->Live > 0)
		{
			if(gObjIsConnected(it->aIndex) != 0 && gObj[it->aIndex].CsNpcType != 0)
			{
				it->DefenseLevel = UpgradeValue;

				gObj[it->aIndex].Defense = NpcDefense[0][UpgradeValue];
				gObj[it->aIndex].MagicDefense = NpcDefense[0][UpgradeValue];
				gObj[it->aIndex].CastleDefenseLevel = UpgradeValue;
				return;
			}
		}
	}
}

void CCastleSiege::UpgradeRegenLevel(int NpcClass,int NpcIndex,int UpgradeValue) // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == NpcClass && it->Index == NpcIndex && it->Live > 0)
		{
			if(gObjIsConnected(it->aIndex) != 0 && gObj[it->aIndex].CsNpcType != 0)
			{
				it->RegenLevel = UpgradeValue;

				gObj[it->aIndex].CastleRegenLevel = UpgradeValue;
				return;
			}
		}
	}
}

void CCastleSiege::UpgradeLifeLevel(int NpcClass,int NpcIndex,int UpgradeValue) // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == NpcClass && it->Index == NpcIndex && it->Live > 0)
		{
			if(gObjIsConnected(it->aIndex) != 0 && gObj[it->aIndex].CsNpcType != 0)
			{
				it->LifeLevel = UpgradeValue;
				it->Life = UpgradeValue;

				gObj[it->aIndex].Life = (float)UpgradeValue;
				gObj[it->aIndex].MaxLife = (float)UpgradeValue;
				return;
			}
		}
	}
}

void CCastleSiege::SetCrownIndex(int aIndex) // OK
{
	this->m_CrownIndex = aIndex;
}

bool CCastleSiege::GetRegCrownAvailable() // OK
{
	return m_CrownAvailable;
}

void CCastleSiege::SetRegCrownAvailable(bool state) // OK
{
	this->m_CrownAvailable = state;
}

int CCastleSiege::CheckCastleTowerEnable() // OK
{
	return m_CastleTowerEnable;
}

bool CCastleSiege::CheckCsGateAlive(int aIndex) // OK
{
	if(gObjIsConnected(aIndex) == 0 || gObj[aIndex].Class != 277)
	{
		return 0;
	}

	return 1;
}

bool CCastleSiege::CheckCastleHasMoney(int money) // OK
{
	if(this->m_TributeMoney >= money)
	{
		return 1;
	}

	return 0;
}

QWORD CCastleSiege::GetCastleMoney() // OK
{
	return m_TributeMoney;
}

bool CCastleSiege::CheckGuildOwnCastle(char* GuildName) // OK
{
	if(strcmp(this->m_CastleOwner,GuildName) == 0)
	{
		return 1;
	}

	return 0;
}

void CCastleSiege::SetGateBlockState(int x,int y,BYTE gate) // OK
{
	if(CHECK_RANGE(x,256) == 0 || CHECK_RANGE(y,256) == 0)
	{
		return;
	}

	switch(gate)
	{
		case 0:
		{
			for (int i = x - 2; i <= x + 3; i++)
			{
				for (int j = y; j <= y + 2; j++)
				{
					gMap[MAP_CASTLE_SIEGE].m_MapAttr[j * 256 + i] |= 16;
				}
			}
		}
		break;
		case 1:
		{
			for (int i = x - 2; i <= x + 3; i++)
			{
				for (int j = y; j <= y + 2; j++)
				{
					gMap[MAP_CASTLE_SIEGE].m_MapAttr[j * 256 + i] &= ~16;
				}
			}
		}
		break;
	}
}

bool CCastleSiege::GetHuntZoneEnter() // OK
{
	return m_HuntEnabled;
}

void CCastleSiege::ClearAndLoadGuildList() // OK
{
	this->m_BasicGuildDataLoad = 0;
	this->m_SiegeGuildDataLoad = 0;

	this->m_BasicGuildDataMap.clear();

	this->m_SiegeGuildDataMap.clear();

	if(this->m_CastleGuildListLoad == 1)
	{
		this->GDCastleSiegeLoadGuildListSend();
	}
	else
	{
		this->GDCastleSiegeBuildGuildListSend();
	}
}

void CCastleSiege::SetPlayerJoinSide(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];


	if(lpObj->Guild == 0)
	{
		return;
	}

	if(this->m_CastleSiegeDoRun == 0 || this->m_SiegeGuildDataLoad == 0)
	{
		return;
	}

	if(this->GetState() != CS_STATE_START)
	{
		return;
	}

	std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.find(lpObj->GuildName);

	if(it != this->m_SiegeGuildDataMap.end())
	{
		lpObj->CastleJoinSide = it->second.Side;

		lpObj->CastleInvolved = it->second.Involved;
	}

	this->GCCastleSiegeJoinSideSend(aIndex);

	lpObj->AccumulatedCrownAccessTime = 0;
}

void CCastleSiege::ResetPlayerJoinSide() // OK
{
	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		lpObj->CastleJoinSide = 0;
		lpObj->CastleInvolved = 0;
		lpObj->LifeStoneCount = 0;

		this->GCCastleSiegeJoinSideSend(n);

		lpObj->AccumulatedCrownAccessTime = 0;
	}
}

void CCastleSiege::SetCastleJoinSide() // OK
{
	if(this->m_SiegeGuildDataLoad == 0)
	{
		return;
	}

	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		if(lpObj->Guild != 0)
		{
			std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.find(lpObj->GuildName);

			if(it != this->m_SiegeGuildDataMap.end())
			{
				lpObj->CastleJoinSide = it->second.Side;
				lpObj->CastleInvolved = it->second.Involved;
			}
		}

		this->GCCastleSiegeJoinSideSend(n);
	}
}

bool CCastleSiege::CheckCastleOwnerMember(int aIndex) // OK
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Guild == 0)
	{
		return 0;
	}

	if(strcmp(this->m_CastleOwner,lpObj->GuildName) != 0)
	{
		return 0;
	}

	return 1;
}

bool CCastleSiege::CheckCastleOwnerUnionMember(int aIndex) // OK
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Guild == 0)
	{
		return 0;
	}

	UNION_INFO* lpUnion = gUnionManager->GetUnion(lpObj->Guild->GuildUnion);

	if(lpUnion == 0)
	{
		return 0;
	}

	if(strcmp(lpUnion->Name,this->m_CastleOwner) != 0)
	{
		return 0;
	}

	return 1;
}

bool CCastleSiege::CheckGuardianStatueExist() // OK
{
	for (int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		if(gObj[n].Class == 283 && gObj[n].Map == MAP_CASTLE_SIEGE && gObj[n].Live == 1)
		{
			return 1;
		}
	}

	return 0;
}

void CCastleSiege::ChangeWinnerGuild(int side) // OK
{
	char GuildTo[9] = {0},GuildFrom[9] = {0};

	for(std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.begin(); it != this->m_SiegeGuildDataMap.end(); it++)
	{
		if(it->second.Side == side)
		{
			it->second.Side = 1;

			if(it->second.Involved != 0)
			{
				memcpy(GuildTo,it->first.c_str(),sizeof(GuildTo));
			}
		}
		else if(it->second.Side == 1)
		{
			it->second.Side = side;

			if(it->second.Involved != 0)
			{
				memcpy(GuildFrom,it->first.c_str(),sizeof(GuildFrom));
			}
		}
	}

	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		if(gObj[n].Guild == 0)
		{
			continue;
		}

		std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.find(gObj[n].GuildName);

		if(it != this->m_SiegeGuildDataMap.end())
		{
			gObj[n].CastleJoinSide = it->second.Side;

			gObj[n].CastleInvolved = it->second.Involved;
		}

		gObj[n].AccumulatedCrownAccessTime = 0;

		this->GCCastleSiegeJoinSideSend(n);
	}
}

void CCastleSiege::CheckMiddleWinnerGuild() // OK
{
	if(this->m_CrownAccessTime == 0)
	{
		return;
	}

	if(gObjIsConnected(this->m_CrownAccessUser) == 0 || gObjIsConnected(this->m_CrownSwitchUser1) == 0 || gObjIsConnected(this->m_CrownSwitchUser2) == 0)
	{
		return;
	}

	LPOBJ lpCrownAccess = &gObj[this->m_CrownAccessUser];
	
	LPOBJ lpSwitchUser1 = &gObj[this->m_CrownSwitchUser1];

	LPOBJ lpSwitchUser2 = &gObj[this->m_CrownSwitchUser2];

	if(lpCrownAccess->Guild == 0 || lpSwitchUser1->Guild == 0 || lpSwitchUser2->Guild == 0)
	{
		return;
	}

	if(lpCrownAccess->CastleJoinSide >= 2 && lpSwitchUser1->CastleJoinSide >= 2 && lpSwitchUser2->CastleJoinSide >= 2)
	{
		if(lpCrownAccess->CastleInvolved != 0)
		{
			if(lpCrownAccess->CastleJoinSide == lpSwitchUser1->CastleJoinSide && lpCrownAccess->CastleJoinSide == lpSwitchUser2->CastleJoinSide)
			{
				if(((GetTickCount()-this->m_CrownAccessTime)+lpCrownAccess->AccumulatedCrownAccessTime) >= MAX_CASTLE_SIEGE_CROWN_TIME)
				{
					LogAdd(LOG_BLACK,"[Castle Siege] Guild %s has taken possession of the castle.", lpCrownAccess->GuildName);

					this->GCCastleSiegeCrownAccessStateSend(this->m_CrownAccessUser,1);

					this->GCCastleSiegeCrownStateSend(2);

					memset(this->m_MiddleOwner,0,sizeof(this->m_MiddleOwner));

					memcpy(this->m_MiddleOwner,lpCrownAccess->GuildName,sizeof(this->m_MiddleOwner));

					this->ClearNpcLifeStone();
					
					this->ChangeWinnerGuild(lpCrownAccess->CastleJoinSide);
					
					this->GCCastleSiegeNotifyProgressSendToAll(1,lpCrownAccess->GuildName);
					
					this->ResetCrownAccessTickCount();
					
					this->SetCrownUserIndex(-1);
					
					this->SetCrownAccessPosition(0,0);
					
					this->ResetCrownSwitchUserIndex(217);
					
					this->ResetCrownSwitchUserIndex(218);
					
					this->m_CrownAvailable = 0;
					
					this->RespawnPlayers(0);

					this->GCCastleSiegeLeftTimeSendToAll();

					this->m_LeftTime = GetTickCount();
				}
			}
		}
	}
}

bool CCastleSiege::CheckCastleSiegeResult() // OK
{
	if(this->m_CastleOccupied == 1 && strcmp(this->m_CastleOwner,"") != 0)
	{
		if(strcmp(this->m_MiddleOwner,"") == 0 || strcmp(this->m_MiddleOwner,this->m_CastleOwner) == 0)
		{
			this->m_CastleOccupied = 1;
			this->m_CastleSiegeEnded = 1;
			
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(118),this->m_CastleOwner);
			
			return 0;
		}
		else
		{
			this->m_CastleOccupied = 1;
			this->m_CastleSiegeEnded = 1;
			
			memset(this->m_CastleOwner,0,sizeof(this->m_CastleOwner));
			memcpy(this->m_CastleOwner,this->m_MiddleOwner,sizeof(this->m_CastleOwner));
			
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(119),this->m_CastleOwner);

			LogAdd(LOG_BLACK,"[Castle Siege] Guild %s is the new owner of the castle.",this->m_CastleOwner);
			
			return 1;
		}
	}
	else
	{
		if(strcmp(this->m_MiddleOwner,"") == 0)
		{
			this->m_CastleOccupied = 0;
			
			this->m_CastleSiegeEnded = 1;
			
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(120));
			
			return 0;
		}
		else
		{
			this->m_CastleOccupied = 1;
			this->m_CastleSiegeEnded = 1;
			
			memset(this->m_CastleOwner,0,sizeof(this->m_CastleOwner));
			memcpy(this->m_CastleOwner,this->m_MiddleOwner,sizeof(this->m_CastleOwner));
			
			GDGlobalNoticeSend(gMapServerManager->GetMapServerGroup(),0,0,0,0,0,0,gMessage->GetMessage(119),this->m_CastleOwner);

			LogAdd(LOG_BLACK,"[Castle Siege] Guild %s is the new owner of the castle.",this->m_CastleOwner);
			
			return 1;
		}
	}
}

bool CCastleSiege::CheckUnionGuildMaster(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return 0;
	}

	if(lpObj->Guild == 0)
	{
		return 0;
	}

	if(lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		return 0;
	}

	if(lpObj->GuildIndex == lpObj->Guild->GuildUnion || lpObj->Guild->GuildUnion == 0)
	{
		return 1;
	}


	return 0;
}

int CCastleSiege::GetCrownUserIndex() // OK
{
	return m_CrownAccessUser;
}

void CCastleSiege::SetCrownUserIndex(int aIndex) // OK
{
	this->m_CrownAccessUser = aIndex;
}

bool CCastleSiege::CheckCrownAccessPosition(short X,short Y) // OK
{
	if(this->m_CrownAccessUserX != X || this->m_CrownAccessUserY != Y)
	{
		return 1;
	}

	return 0;
}

void CCastleSiege::SetCrownAccessPosition(BYTE X,BYTE Y) // OK
{
	this->m_CrownAccessUserX = X;

	this->m_CrownAccessUserY = Y;
}

int CCastleSiege::GetCrownSwitchUserIndex(int MonsterClass) // OK
{
	switch(MonsterClass)
	{
		case 217:
			return this->m_CrownSwitchUser1;
		case 218:
			return this->m_CrownSwitchUser2;
	}

	return -1;
}

void CCastleSiege::ResetCrownSwitchUserIndex(int MonsterClass) // OK
{
	switch(MonsterClass)
	{
		case 217:
			this->m_CrownSwitchUser1 = -1;
			break;
		case 218:
			this->m_CrownSwitchUser2 = -1;
			break;
	}
}

void CCastleSiege::SetCrownSwitchUserIndex(int MonsterClass,int aIndex) // OK
{
	switch(MonsterClass)
	{
		case 217:
			this->m_CrownSwitchUser1 = aIndex;
			break;
		case 218:
			this->m_CrownSwitchUser2 = aIndex;
			break;
	}
}

DWORD CCastleSiege::GetCrownAccessTickCount() // OK
{
	return m_CrownAccessTime;
}

void CCastleSiege::ResetCrownAccessTickCount() // OK
{
	this->m_CrownAccessTime = 0;
}

void CCastleSiege::SetCrownAccessTickCount() // OK
{
	this->m_CrownAccessTime = GetTickCount();
}

void CCastleSiege::CheckTeleportMagicAxisY(int x,int y,BYTE* ty) // OK
{
	if(x < 144)
	{
		int WallAxisY[3] = {113,160,203};

		for(int n = 0; n < 3; n++)
		{
			if((y >= (WallAxisY[n] - 5) && y <= (WallAxisY[n] + 1)) && ((*ty) >= (WallAxisY[n] + 1)))
			{
				(*ty) = (WallAxisY[n] - 1);
				return;
			}

			if((y >= (WallAxisY[n] + 3) && y <= (WallAxisY[n] + 9)) && ((*ty) <= (WallAxisY[n] + 3)))
			{
				(*ty) = (WallAxisY[n] + 3);
				return;
			}
		}
	}
}

void CCastleSiege::CheckReviveNonDbNPC() // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->DbSave != 0)
		{
			continue;
		}

		int index = gObjAddMonster(MAP_CASTLE_SIEGE);

		if(OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ lpMonster = &gObj[index];

		if(gObjSetMonster(index,it->Class) == 0)
		{
			gObjDel(index);
			continue;
		}

		lpMonster->PosNum = -1;
		lpMonster->X = it->X;
		lpMonster->Y = it->Y;
		lpMonster->Map = MAP_CASTLE_SIEGE;
		lpMonster->TX = lpMonster->X;
		lpMonster->TY = lpMonster->Y;
		lpMonster->OldX = lpMonster->X;
		lpMonster->OldY = lpMonster->Y;
		lpMonster->Dir = it->Dir;
		lpMonster->StartX = (BYTE)lpMonster->X;
		lpMonster->StartY = (BYTE)lpMonster->Y;
		lpMonster->DieRegen = 0;
		lpMonster->MaxRegenTime = 0;
		lpMonster->Life = (float)it->Life;
		lpMonster->MaxLife = (float)it->LifeLevel;

		if(it->Class == 221)
		{
			lpMonster->CsNpcType = 3;
		}
		else
		{
			lpMonster->CsNpcType = 2;
		}
	}
}

void CCastleSiege::CheckReviveGuardianStatue() // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == 283 && it->Live == 0)
		{
			int index = gObjAddMonster(MAP_CASTLE_SIEGE);

			if(OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			LPOBJ lpMonster = &gObj[index];

			if(gObjSetMonster(index,it->Class) == 0)
			{
				gObjDel(index);
				continue;
			}

			lpMonster->PosNum = -1;
			lpMonster->X = it->X;
			lpMonster->Y = it->Y;
			lpMonster->Map = MAP_CASTLE_SIEGE;
			lpMonster->TX = lpMonster->X;
			lpMonster->TY = lpMonster->Y;
			lpMonster->OldX = lpMonster->X;
			lpMonster->OldY = lpMonster->Y;
			lpMonster->Dir = it->Dir;
			lpMonster->StartX = (BYTE)lpMonster->X;
			lpMonster->StartY = (BYTE)lpMonster->Y;
			lpMonster->DieRegen = 0;
			lpMonster->MaxRegenTime = 0;
			lpMonster->Life = (float)it->Life;
			lpMonster->MaxLife = (float)it->LifeLevel;
			lpMonster->CsNpcType = 1;

			lpMonster->Defense = NpcDefense[1][it->DefaultDefenseLevel];
			lpMonster->CastleDefenseLevel = it->DefaultDefenseLevel;
			lpMonster->CastleRegenLevel = it->DefaultRegenLevel;

			it->aIndex = index;
			it->Live = 2;
		}
	}
}

void CCastleSiege::RespawnPlayers(bool defense) // OK
{
	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		LPOBJ lpObj = &gObj[n];

		if(gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
		{
			continue;
		}

		if(lpObj->CastleJoinSide == 1 && defense == 0)
		{
			continue;
		}

		if(this->GetRespawnCoord(&lpObj->X,&lpObj->Y,lpObj->CastleJoinSide) != 0)
		{
			lpObj->State = OBJECT_DELCMD;
			lpObj->TX = lpObj->X;
			lpObj->TY = lpObj->Y;
			lpObj->Map = MAP_CASTLE_SIEGE;
			lpObj->PathCount = 0;
			lpObj->Teleport = 0;
			lpObj->ViewState = VIEW_STATE_NONE;

			gObjClearViewport(lpObj);

			gMove->GCTeleportSend(lpObj->Index,-1,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,lpObj->Dir);

			gObjViewportListProtocolCreate(lpObj);

			gObjectManager->CharacterUpdateMapEffect(lpObj);

			lpObj->RegenMapNumber = MAP_CASTLE_SIEGE;
			lpObj->RegenMapX = (BYTE)lpObj->X;
			lpObj->RegenMapY = (BYTE)lpObj->Y;
			lpObj->RegenOk = 1;
		}

		lpObj->AccumulatedCrownAccessTime = 0;
	}
}

bool CCastleSiege::GetRespawnCoord(short* x,short* y,BYTE side) // OK
{
	if(side > 2)
	{
		side = 2;
	}

	short RespawnArea[3][4] = { {35,17,144,48},{74,144,115,154},{35,17,144,48} };

	for(int n = 0; n < 100; n++)
	{
		(*x) = ((GetLargeRand()%(RespawnArea[side][2]-RespawnArea[side][0]))+RespawnArea[side][0]);
		(*y) = ((GetLargeRand()%(RespawnArea[side][3]-RespawnArea[side][1]))+RespawnArea[side][1]);

		if(gMap[MAP_CASTLE_SIEGE].CheckAttr((*x),(*y),4) == 0 && gMap[MAP_CASTLE_SIEGE].CheckAttr((*x),(*y),8) == 0)
		{
			return 1;
		}
	}

	return 0;
}

void CCastleSiege::ClearCastleTowerBarrier() // OK
{
	for(int n = 0; n < MAX_OBJECT_MONSTER; n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		if(gObj[n].Class == 215)
		{
			gObjDel(n);
		}
	}
}

void CCastleSiege::CloseAllCastleGates() // OK
{
	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class != 277)
		{
			continue;
		}

		if(gObjIsConnected(it->aIndex) == 0)
		{
			continue;
		}

		LPOBJ lpMonster = &gObj[it->aIndex];

		this->SetGateBlockState(it->X,it->Y,0);
		
		gEffectManager->DelEffect(lpMonster,EFFECT_CASTLE_GATE_STATE);
		
		lpMonster->CastleGateState = 0;
	}
}

void CCastleSiege::OperateMiniMapWork()
{
	if(this->m_BasicGuildDataLoad == 0)
	{
		return;
	}

	std::map<int,CASTLE_SIEGE_MINI_MAP_DATA> mapMiniMapData;

	BYTE cBUFFER1[4096] = { 0 };

	PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE_SEND* lpMsgSend1 = (PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE_SEND*)cBUFFER1;
	PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE* lpMsgSendBody1 = (PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE*)&cBUFFER1[sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE_SEND)];

	BYTE cBUFFER2[4096] = { 0 };

	PMSG_CASTLE_SIEGE_MINI_MAP_SEND* lpMsgSend2 = (PMSG_CASTLE_SIEGE_MINI_MAP_SEND*)cBUFFER2;

	PMSG_CASTLE_SIEGE_MINI_MAP* lpMsgSendBody2 = (PMSG_CASTLE_SIEGE_MINI_MAP*)&cBUFFER2[sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SEND)];

	for (std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_BasicGuildDataMap.begin(); it != this->m_BasicGuildDataMap.end(); it++)
	{
		CASTLE_SIEGE_GUILD_FINAL* GuildData = &it->second;
		CASTLE_SIEGE_MINI_MAP_DATA MiniMapData;

		memcpy(MiniMapData.GuildName,&it->first[0],sizeof(MiniMapData.GuildName));

		mapMiniMapData.insert(std::pair<int,CASTLE_SIEGE_MINI_MAP_DATA>(GuildData->Side,MiniMapData));
	}

	if(mapMiniMapData.empty())
	{
		return;
	}

	lpMsgSend2->count = 0;

	if(this->m_NpcDataList.empty() == 0)
	{
		for (std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
		{
			if(lpMsgSend2->count >= 150)
			{
				break;
			}

			CASTLE_SIEGE_NPC_DATA & NpcData = CASTLE_SIEGE_NPC_DATA(*it);

			if((NpcData.Class == 277 || NpcData.Class == 283) && NpcData.aIndex != -1)
			{
				if(!gObjIsConnected(NpcData.aIndex) || (gObj[NpcData.aIndex].Class != 277 && gObj[NpcData.aIndex].Class != 283))
				{
					BYTE btNpcType = 0;

					switch(NpcData.Class)
					{
						case 277:
							btNpcType = 0;
							break;
						case 283:
							btNpcType = 1;
							break;
						default:
							continue;
							break;
					}

					lpMsgSendBody2[lpMsgSend2->count].npc = btNpcType;
					lpMsgSendBody2[lpMsgSend2->count].x = (BYTE)gObj[NpcData.aIndex].X;
					lpMsgSendBody2[lpMsgSend2->count].y = (BYTE)gObj[NpcData.aIndex].Y;

					lpMsgSend2->count++;
				}
			}
		}
	}

	lpMsgSend2->header.set(0xBB,(WORD)(lpMsgSend2->count * sizeof(PMSG_CASTLE_SIEGE_MINI_MAP)) + sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SEND));

	this->m_MiniMapDataMap.clear();

	std::map<int,CASTLE_SIEGE_MINI_MAP_DATA>::iterator it2(mapMiniMapData.begin());

	for (; it2 != mapMiniMapData.end(); it2++)
	{
		this->m_MiniMapDataMap.insert(std::pair<int,CASTLE_SIEGE_MINI_MAP_DATA>(it2->first,it2->second));
	}

	for (int iIDX = OBJECT_START_USER; iIDX < MAX_OBJECT; iIDX++)
	{
		if(gObjIsConnected(iIDX) == 0)
		{
			continue;
		}

		if(gObj[iIDX].Map != MAP_CASTLE_SIEGE)
		{
			continue;
		}

		if(gObj[iIDX].CastleJoinSide == 0)
		{
			continue;
		}

		std::map<int,CASTLE_SIEGE_MINI_MAP_DATA>::iterator it3(this->m_MiniMapDataMap.find(gObj[iIDX].CastleJoinSide));

		if(it3 != this->m_MiniMapDataMap.end())
		{
			if(it3->second.PointsCount >= 1000)
			{
				continue;
			}

			if(gObj[iIDX].GuildRank == GUILD_STATUS_MASTER && strcmp(gObj[iIDX].GuildName,"") != 0 && strcmp(gObj[iIDX].GuildName,it3->second.GuildName) == 0)
			{
				it3->second.GuildMasterIndex = iIDX;
				continue;
			}

			it3->second.MinimapPoints[it3->second.PointsCount].x = (BYTE)gObj[iIDX].X;
			it3->second.MinimapPoints[it3->second.PointsCount].y = (BYTE)gObj[iIDX].Y;
			it3->second.PointsCount++;
		}
	}

	std::map<int,CASTLE_SIEGE_MINI_MAP_DATA>::iterator it4(this->m_MiniMapDataMap.begin());

	for (; it4 != this->m_MiniMapDataMap.end(); it4++)
	{
		lpMsgSend1->count = 0;

		if(gObjIsConnected(it4->second.GuildMasterIndex) == 0)
		{
			continue;
		}

		lpMsgSend1->count = it4->second.PointsCount;

		if(lpMsgSend1->count > 1000)
		{
			lpMsgSend1->count = 1000;
		}

		memcpy(lpMsgSendBody1,it4->second.MinimapPoints,(lpMsgSend1->count * sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE)));

		lpMsgSend1->header.set(0xB6,(WORD)(lpMsgSend1->count * sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE)) + sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE_SEND));

		DataSend(it4->second.GuildMasterIndex,cBUFFER1,(lpMsgSend1->count * sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE)) + sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE_SEND));
		DataSend(it4->second.GuildMasterIndex,cBUFFER2,(lpMsgSend2->count * sizeof(PMSG_CASTLE_SIEGE_MINI_MAP)) + sizeof(PMSG_CASTLE_SIEGE_MINI_MAP_SEND));

	}
}

bool CCastleSiege::ScheduleStateCompFunc(CASTLE_STATE_TIME_DATA& A,CASTLE_STATE_TIME_DATA& B) // OK
{
	if(A.State > B.State)
	{
		return 0;
	}

	return 1;
}

bool CCastleSiege::RegGuildScoreCompFunc(CASTLE_SIEGE_GUILD_DATA & A,CASTLE_SIEGE_GUILD_DATA & B) // OK
{
	if(A.TotalScore < B.TotalScore)
	{
		return 0;
	}

	return 1;
}

void CCastleSiege::DataSendToAll(BYTE* lpMsg,int size) // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0 && gObj[n].Map == MAP_CASTLE_SIEGE)
		{
			DataSend(n,lpMsg,size);
		}
	}
}

void CCastleSiege::CGCastleSiegeStateRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_STATE_SEND pMsg;

	pMsg.header.set(0x80,0x01,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.aIndex = aIndex;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeRegisterRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(this->GetState() != CS_STATE_SIGN1)
	{
		this->GCCastleSiegeRegisterGuildResultSend(aIndex,7,"");
		return;
	}

	if(lpObj->Guild == 0)
	{
		this->GCCastleSiegeRegisterGuildResultSend(aIndex,6,"");
		return;
	}

	if(this->CheckUnionGuildMaster(aIndex) == 0)
	{
		this->GCCastleSiegeRegisterGuildResultSend(aIndex,0,"");
	}
	else if(this->CheckCastleOwnerMember(aIndex) != 0 || this->CheckCastleOwnerUnionMember(aIndex) != 0)
	{
		this->GCCastleSiegeRegisterGuildResultSend(aIndex,3,"");
	}
	else
	{
		SDHP_CASTLE_SIEGE_REGISTER_SEND pMsg;

		pMsg.header.set(0x80,0x0D,sizeof(pMsg));

		pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

		pMsg.aIndex = aIndex;

		memcpy(pMsg.GuildName,lpObj->GuildName,sizeof(pMsg.GuildName));
	
		gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
	}
}

void CCastleSiege::CGCastleSiegeGiveUpGuildRecv(PMSG_CASTLE_SIEGE_GIVEUP_GUILD_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(this->GetState() < CS_STATE_SIGN1 || this->GetState() > CS_STATE_SIGN2)
	{
		this->GCCastleSiegeGiveupGuildResultSend(aIndex,3,0,"");
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}
	
	if(this->CheckUnionGuildMaster(aIndex) == 0)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_GIVEUP_GUILD_SEND pMsg;

	pMsg.header.set(0x80,0x12,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.aIndex = aIndex;

	pMsg.GiveUp = lpMsg->GiveUp;

	memcpy(pMsg.GuildName,lpObj->GuildName,sizeof(pMsg.GuildName));
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeRegisterGuildInfoRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_REGISTER_INFO_SEND pMsg;

	pMsg.header.set(0x80,0x0A,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.aIndex = aIndex;

	memcpy(pMsg.GuildName,lpObj->GuildName,sizeof(pMsg.GuildName));
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeRegisterMarkRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	if(this->GetState() != CS_STATE_SIGN2)
	{
		return;
	}
	
	if(gItemManager->GetInventoryItemCount(lpObj,GET_ITEM(14,21),3) == 0)
	{
		PMSG_CASTLE_SIEGE_REGISTER_MARK_SEND pMsg;

		pMsg.header.set(0xB2,0x04,sizeof(pMsg));
		pMsg.result = 3;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	if(((lpObj->SiegueRegisterTransaction == 0)?(lpObj->SiegueRegisterTransaction++):lpObj->SiegueRegisterTransaction) != 0)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_REGISTER_MARK_SEND pMsg;

	pMsg.header.set(0x80,0x10,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
	
	pMsg.aIndex = aIndex;
	
	memcpy(&pMsg.GuildName,lpObj->GuildName,sizeof(pMsg.GuildName));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeNpcBuyRecv(PMSG_CASTLE_SIEGE_NPC_BUY_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	if(lpObj->GuildRank != GUILD_STATUS_MASTER && lpObj->GuildRank != GUILD_STATUS_OFFICE_MASTER)
	{
		return;
	}

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0)
	{
		return;
	}

	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class == lpMsg->NpcClass && it->Index == lpMsg->NpcIndex)
		{
			if(it->Live > 0 && gObjIsConnected(it->aIndex) != 0)
			{
				if(gObj[it->aIndex].Live == 1)
				{
					this->GCCastleSiegeNpcBuyResultSend(aIndex,0,lpMsg->NpcClass,lpMsg->NpcIndex);
					return;
				}
			}

			int BuyCost = 0;

			switch(it->Class)
			{
				case 277:
					BuyCost = it->DefaultLifeLevel*5;
					break;
				case 283:
					BuyCost = it->DefaultLifeLevel*3;
					break;
			}

			if(gObj[aIndex].Money < ((DWORD)BuyCost))
			{
				this->GCCastleSiegeNpcBuyResultSend(aIndex,3,lpMsg->NpcClass,lpMsg->NpcIndex);
			}
			else
			{
				SDHP_CASTLE_SIEGE_NPC_BUY_SEND pMsg;

				pMsg.header.set(0x80,0x03,sizeof(pMsg));

				pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
				
				pMsg.aIndex = aIndex;

				pMsg.NpcClass = lpMsg->NpcClass;
				
				pMsg.NpcIndex = lpMsg->NpcIndex;
				
				pMsg.DefenseLevel = it->DefenseLevel;
				
				pMsg.RegenLevel = it->RegenLevel;
				
				pMsg.LifeLevel = it->LifeLevel;
				
				pMsg.Life = it->Life;
				
				pMsg.X = it->X;
				
				pMsg.Y = it->Y;
				
				pMsg.Dir = it->Dir;
				
				pMsg.BuyCost = BuyCost;

				gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
			}

			return;
		}
	}
}

void CCastleSiege::CGCastleSiegeNpcRepairRecv(PMSG_CASTLE_SIEGE_NPC_REPAIR_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	if(lpObj->GuildRank != GUILD_STATUS_MASTER && lpObj->GuildRank != GUILD_STATUS_OFFICE_MASTER)
	{
		return;
	}

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0)
	{
		return;
	}

	CASTLE_SIEGE_NPC_DATA* lpNpc = this->GetNpcData(lpMsg->NpcClass,lpMsg->NpcIndex,-1);

	if(lpNpc == 0)
	{
		this->GCCastleSiegeNpcRepairResultSend(aIndex,0,lpMsg->NpcClass,lpMsg->NpcIndex,0,0);
		return;
	}

	int bIndex = lpNpc->aIndex;

	LPOBJ lpMonster = &gObj[bIndex];

	if(gObjIsConnected(bIndex) == 0 || lpMonster->Live == 0)
	{
		this->GCCastleSiegeNpcRepairResultSend(aIndex,0,lpMsg->NpcClass,lpMsg->NpcIndex,0,0);
		return;
	}

	if(lpMonster->MaxLife < lpMonster->Life)
	{
		lpMonster->Life = lpMonster->MaxLife;
		this->GCCastleSiegeNpcRepairResultSend(aIndex,1,lpMsg->NpcClass,lpMsg->NpcIndex,(int)lpMonster->Life,(int)lpMonster->MaxLife);
		return;
	}
	
	DWORD RepairCost = 0;

	switch(lpMsg->NpcClass)
	{
		case 277:
			RepairCost = ((int)(lpMonster->MaxLife-lpMonster->Life)*5)+(lpMonster->CastleDefenseLevel*1000000);
			break;
		case 283:
			RepairCost = ((int)(lpMonster->MaxLife-lpMonster->Life)*3)+((lpMonster->CastleDefenseLevel+lpMonster->CastleRegenLevel)*1000000);
			break;
	}

	if(lpObj->Money < RepairCost)
	{
		this->GCCastleSiegeNpcRepairResultSend(aIndex,3,lpMsg->NpcClass,lpMsg->NpcIndex,0,0);
		return;
	}

	SDHP_CASTLE_SIEGE_NPC_REPAIR_SEND pMsg;

	pMsg.header.set(0x80,0x04,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
	
	pMsg.aIndex = aIndex;
	
	pMsg.NpcClass = lpMsg->NpcClass;
	
	pMsg.NpcIndex = lpMsg->NpcIndex;
	
	pMsg.RepairCost = RepairCost;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeNpcUpgradeRecv(PMSG_CASTLE_SIEGE_NPC_UPGRADE_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	if(lpObj->GuildRank != GUILD_STATUS_MASTER && lpObj->GuildRank != GUILD_STATUS_OFFICE_MASTER)
	{
		return;
	}

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0)
	{
		return;
	}

	CASTLE_SIEGE_NPC_DATA* lpNpc = this->GetNpcData(lpMsg->NpcClass,lpMsg->NpcIndex,-1);

	if(lpNpc == 0)
	{
		this->GCCastleSiegeNpcUpgradeResultSend(aIndex,7,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
		return;
	}

	int require_gem = 0;
	int require_money = 0;
	int current_level = 0;
	int next_level = 0;
	int next_value = 0;
	bool upgrade = 0;

	if(lpMsg->NpcClass == 277)
	{
		if(lpMsg->UpgradeType == 1)
		{
			current_level = lpNpc->DefenseLevel;
			next_level = lpMsg->UpgradeValue;

			if(CS_UPGRADE_RANGE(next_level) != 0 && next_level == (current_level+1))
			{
				upgrade = 1;
				next_value = next_level;
				require_gem = NpcUpgradeGateDefense[(next_level-1)][1];
				require_money = NpcUpgradeGateDefense[(next_level-1)][2];
			}
		}
		else if(lpMsg->UpgradeType == 3)
		{
			current_level = 0;

			next_level = 0;

			for(int n = 0; n < 3; n++)
			{
				if(lpNpc->LifeLevel >= NpcUpgradeGateLife[n][0])
				{
					current_level = (n+1);
				}

				if(lpMsg->UpgradeValue >= NpcUpgradeGateLife[n][0])
				{
					next_level = (n+1);
				}
			}

			if(next_level != (current_level+1))
			{
				LogAdd(LOG_RED,"[Castle Siege] Upgrade NPC Error: Life Gate [%d][%d]",lpNpc->LifeLevel,lpMsg->UpgradeValue);
			}

			if(CS_UPGRADE_RANGE(next_level) != 0 && next_level == (current_level+1))
			{
				upgrade = 1;
				next_value = NpcUpgradeGateLife[(next_level-1)][0];
				require_gem = NpcUpgradeGateLife[(next_level-1)][1];
				require_money = NpcUpgradeGateLife[(next_level-1)][2];
			}
		}
		else
		{
			this->GCCastleSiegeNpcUpgradeResultSend(aIndex,5,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
			return;
		}
	}
	else if(lpMsg->NpcClass == 283)
	{
		if(lpMsg->UpgradeType == 1)
		{
			current_level = lpNpc->DefenseLevel;
			next_level = lpMsg->UpgradeValue;

			if(CS_UPGRADE_RANGE(next_level) != 0 && next_level == (current_level+1))
			{
				upgrade = 1;
				next_value = next_level;
				require_gem = NpcUpgradeStatueDefense[(next_level-1)][1];
				require_money = NpcUpgradeStatueDefense[(next_level-1)][2];
			}
		}
		else if(lpMsg->UpgradeType == 2)
		{
			current_level = lpNpc->RegenLevel;
			next_level = lpMsg->UpgradeValue;

			if(CS_UPGRADE_RANGE(next_level) != 0 && next_level == (current_level+1))
			{
				upgrade = 1;
				next_value = next_level;
				require_gem = NpcUpgradeStatueRegen[(next_level-1)][1];
				require_money = NpcUpgradeStatueRegen[(next_level-1)][2];
			}
		}
		else if(lpMsg->UpgradeType == 3)
		{
			current_level = 0;

			next_level = 0;

			for(int n = 0; n < 3; n++)
			{
				if(lpNpc->LifeLevel >= NpcUpgradeStatueLife[n][0])
				{
					current_level = (n+1);
				}

				if(lpMsg->UpgradeValue >= NpcUpgradeStatueLife[n][0])
				{
					next_level = (n+1);
				}
			}

			if(next_level != (current_level+1))
			{
				LogAdd(LOG_RED,"[Castle Siege] Upgrade NPC Error: Life Statue [%d][%d]",lpNpc->LifeLevel,lpMsg->UpgradeValue);
			}

			if(CS_UPGRADE_RANGE(next_level) != 0 && next_level == (current_level+1))
			{
				upgrade = 1;
				next_value = NpcUpgradeStatueLife[(next_level-1)][0];
				require_gem = NpcUpgradeStatueLife[(next_level-1)][1];
				require_money = NpcUpgradeStatueLife[(next_level-1)][2];
			}
		}
		else
		{
			this->GCCastleSiegeNpcUpgradeResultSend(aIndex,5,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
			return;
		}
	}

	if(upgrade == 0)
	{
		this->GCCastleSiegeNpcUpgradeResultSend(aIndex,6,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
		return;
	}
	
	if(lpObj->Money < ((DWORD)require_money))
	{
		this->GCCastleSiegeNpcUpgradeResultSend(aIndex,3,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
		return;
	}

	if(gItemManager->GetInventoryItemCount(lpObj,GET_ITEM(14,31),0) < require_gem)
	{
		this->GCCastleSiegeNpcUpgradeResultSend(aIndex,4,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
		return;
	}

	gItemManager->DeleteInventoryItemCount(lpObj,GET_ITEM(14,31),0,require_gem);

	lpObj->Money -= ((DWORD)require_money);

	GCMoneySend(lpObj->Index,lpObj->Money);

	lpObj->CastleSeniorMixUpgrade = 1;

	SDHP_CASTLE_SIEGE_NPC_UPGRADE_SEND pMsg;

	pMsg.header.set(0x80,0x05,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
	
	pMsg.aIndex = aIndex;
	
	pMsg.NpcClass = lpMsg->NpcClass;
	
	pMsg.NpcIndex = lpMsg->NpcIndex;
	
	pMsg.UpgradeType = lpMsg->UpgradeType;
	
	pMsg.UpgradeValue = next_value;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeTaxMoneyInfoRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	if(lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		return;
	}

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_SEND pMsg;

	pMsg.header.set(0x80,0x06,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.aIndex = aIndex;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeTaxRateChangeRecv(PMSG_CS_TAX_RATE_CHANGE_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0 || lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		return;
	}

	int max_tax = -1;

	switch(lpMsg->TaxType)
	{
		case 1:
			max_tax = 3;
			break;
		case 2:
			max_tax = 3;
			break;
		case 3:
			max_tax = 300000;
			break;
	}

	int TaxRate = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->TaxRate[0],lpMsg->TaxRate[1]),MAKE_NUMBERW(lpMsg->TaxRate[2],lpMsg->TaxRate[3]));

	if(TaxRate < 0 || max_tax < 0 || TaxRate > max_tax)
	{
		this->GCCastleSiegeTaxRateRequestSend(aIndex,0,0,0);
	}
	else
	{
		SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_SEND pMsg;

		pMsg.header.set(0x80,0x07,sizeof(pMsg));

		pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

		pMsg.aIndex = aIndex;
		
		pMsg.TaxType = lpMsg->TaxType;

		pMsg.TaxRate = TaxRate;
	
		gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
	}
}

void CCastleSiege::CGCastleSiegeMoneyOutRecv(PMSG_CASTLE_SIEGE_MONEY_OUT_RECV*lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0 || lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		return;
	}
	
	int money = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->money[3],lpMsg->money[2]),MAKE_NUMBERW(lpMsg->money[1],lpMsg->money[0]));

	if(money <= 0)
	{
		this->GCCastleSiegeMoneyOutResultSend(aIndex,0,0);
		return;
	}

	if(this->CheckCastleHasMoney(money) == 0)
	{
		this->GCCastleSiegeMoneyOutResultSend(aIndex,0,0);
		return;
	}

	if(gObjCheckMaxMoney(lpObj->Index,money) == 0)
	{
		this->GCCastleSiegeMoneyOutResultSend(aIndex,0,0);
		return;
	}

	SDHP_CASTLE_SIEGE_MONEY_OUT_SEND pMsg;

	pMsg.header.set(0x80,0x08,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.aIndex = aIndex;

	pMsg.Money = -money;
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeMiniMapRequestRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->CastleJoinSide == 0)
	{
		this->GCCastleSiegeMiniMapRequestSend(aIndex,3);
		return;
	}

	if(this->CheckUnionGuildMaster(aIndex) == 0)
	{
		this->GCCastleSiegeMiniMapRequestSend(aIndex,3);
		return;
	}
	
	if(this->GetState() != CS_STATE_START)
	{
		this->GCCastleSiegeMiniMapRequestSend(aIndex,2);
		return;
	}

	this->GCCastleSiegeMiniMapRequestSend(aIndex,1);

	if(std::find(this->m_MiniMapRequestList.begin(),this->m_MiniMapRequestList.end(),aIndex) == this->m_MiniMapRequestList.end())
	{
		this->m_MiniMapRequestList.push_back(aIndex);
	}
}

void CCastleSiege::CGCastleSiegeMiniMapCloseRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	std::vector<int>::iterator it;

	if((it = std::find(this->m_MiniMapRequestList.begin(),this->m_MiniMapRequestList.end(),aIndex)) != this->m_MiniMapRequestList.end())
	{
		this->m_MiniMapRequestList.erase(it);
	}
}

void CCastleSiege::CGCastleSiegeCommandRequestRecv(PMSG_CASTLE_SIEGE_COMMAND_REQUEST_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->CastleJoinSide == 0)
	{
		return;
	}

	if(this->CheckUnionGuildMaster(aIndex) == 0)
	{
		return;
	}

	if(this->GetState() != CS_STATE_START)
	{
		return;
	}

	PMSG_CASTLE_SIEGE_COMMAND_REQUEST_SEND pMsg;

	pMsg.header.set(0xB2,0x1D,sizeof(pMsg));
	pMsg.team = lpMsg->team;
	pMsg.x = lpMsg->x;
	pMsg.y = lpMsg->y;
	pMsg.command = lpMsg->command;
	
	for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		if(gObj[n].Map == MAP_CASTLE_SIEGE || gObj[n].CastleJoinSide == lpObj->CastleJoinSide)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}

void CCastleSiege::CGCastleSiegeHuntZoneAllowRecv(PMSG_CASTLE_SIEGE_HUNT_ZONE_ALLOW_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}
	
	PMSG_CASTLE_SIEGE_ENABLE_HUNT_SEND pMsg;

	pMsg.header.set(0xB2,0x1F,sizeof(pMsg));

	pMsg.allow = lpMsg->allow;

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0 || lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}
	
	if(this->GetState() == CS_STATE_START)
	{
		pMsg.result = 0;
		DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
		return;
	}

	this->m_HuntEnabled = (lpMsg->allow != 0);

	pMsg.result = 1;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeNpcListRecv(PMSG_CASTLE_SIEGE_NPC_LIST_RECV*lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}

	BYTE send[2048];

	PMSG_CASTLE_SIEGE_NPC_LIST_SEND pMsg;

	pMsg.header.set(0xB3,0);

	int size = sizeof(pMsg);

	pMsg.result = 1;

	pMsg.count = 0;

	if(this->CheckGuildOwnCastle(lpObj->GuildName) == 0 || (lpObj->GuildRank != GUILD_STATUS_MASTER && lpObj->GuildRank != GUILD_STATUS_OFFICE_MASTER))
	{
		pMsg.result = 2;
		DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
		return;
	}	

	int MonsterClass = ((lpMsg->code == 1) ? 277 : (lpMsg->code == 2) ? 283 : 0);

	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->Class != MonsterClass)
		{
			continue;
		}

		LPOBJ lpMonster = &gObj[it->aIndex];

		PMSG_CASTLE_SIEGE_NPC_LIST info;

		info.NpcClass = it->Class;
		info.NpcIndex = it->Index;
		info.DefenseLevel = it->DefenseLevel;
		info.RegenLevel = it->RegenLevel;
		info.X = (BYTE)lpMonster->X;
		info.Y = (BYTE)lpMonster->Y;

		if(gObjIsConnected(it->aIndex) == 0 || lpMonster->Live == 0 || lpMonster->Life == 0 || lpMonster->CsNpcType == 0)
		{
			info.LifeLevel = it->DefaultLifeLevel;
			info.Life = 0;
			info.Live = 0;
		}
		else
		{
			info.LifeLevel = (DWORD)lpMonster->MaxLife;
			info.Life = (DWORD)lpMonster->Life;
			info.Live = 1;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}

void CCastleSiege::CGCastleSiegeGuildRegisterListRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_REGISTER_LIST_SEND pMsg;

	pMsg.header.set(0x83,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.aIndex = aIndex;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::CGCastleSiegeGuildListRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	BYTE send[2048];

	PMSG_CASTLE_SIEGE_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0xB5,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	pMsg.result = 1;

	if(this->GetState() != CS_STATE_NOTIFY && this->GetState() != CS_STATE_READY)
	{
		pMsg.result = 2;
		DataSend(lpObj->Index,(BYTE*)&pMsg,size);
		return;
	}

	if(this->m_BasicGuildDataLoad == 0 || this->m_SiegeGuildDataLoad == 0)
	{
		pMsg.result = 3;
		DataSend(lpObj->Index,(BYTE*)&pMsg,size);
		return;
	}

	PMSG_CASTLE_SIEGE_GUILD_LIST info;

	for(std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.begin();  it != this->m_SiegeGuildDataMap.end(); it++)
	{
		info.Side = it->second.Side;
		
		info.Involved = it->second.Involved;

		memset(info.GuildName,0,sizeof(info.GuildName));

		memcpy(info.GuildName,&(it->first)[0],sizeof(info.GuildName));

		info.Score = it->second.Score;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);
}

void CCastleSiege::CGCastleSiegeCastleOwnerMarkRecv(int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	GUILD_INFO* lpGuild = gGuildManager->GetGuild(this->m_CastleOwner);

	if(lpGuild != 0)
	{
		PMSG_CASTLE_SIEGE_OWNER_MARK_SEND pMsg;

		pMsg.header.set(0xB9,0x02,sizeof(pMsg));

		memcpy(pMsg.Mark,lpGuild->Mark,sizeof(pMsg.Mark));

		DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
	}
}

void CCastleSiege::CGCastleSiegeLandOfTrialsMoveRecv(PMSG_CASTLE_SIEGE_LAND_OF_TRIAL_MOVE_RECV* lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	PMSG_CASTLE_SIEGE_LAND_OF_TRIAL_MOVE_SEND pMsg;

	pMsg.header.set(0xB9,0x05,sizeof(pMsg));

	pMsg.result = 1;

	if(this->GetHuntZoneEnter() != 0 || this->CheckCastleOwnerMember(lpObj->Index) == 1 || this->CheckCastleOwnerUnionMember(lpObj->Index) == 1)
	{
		DWORD money = gCastleSiegeSync->GetTaxHuntZone(aIndex,1);

		money = ((money<0)?0:money);

		if(lpObj->Money < money)
		{
			pMsg.result = 0;
			DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
			return;
		}

		lpObj->Money -= money;

		GCMoneySend(aIndex,lpObj->Money);

		gCastleSiegeSync->AddTributeMoney(money);

		gObjMoveGate(aIndex,95);

		DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
	}
	else
	{
		pMsg.result = 0;

		DataSend(aIndex,(BYTE*)&pMsg,sizeof(pMsg));
	}
}

void CCastleSiege::CGCastleSiegeGateOperateRecv(PMSG_CASTLE_SIEGE_GATE_OPERATE_RECV*lpMsg,int aIndex) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Guild == 0)
	{
		return;
	}
	
	if(this->GetState() == CS_STATE_START)
	{
		if(lpObj->CastleJoinSide != 1)
		{
			return;
		}
	}
	else
	{
		if(this->CheckCastleOwnerMember(aIndex) == 0 && this->CheckCastleOwnerUnionMember(aIndex) == 0)
		{
			return;
		}
	}
	
	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(this->CheckCsGateAlive(bIndex) == 0)
	{
		this->GCCastleSiegeGateOperateSend(aIndex,2,-1,0);
		return;
	}

	LPOBJ lpMonster = &gObj[bIndex];

	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->aIndex != bIndex || it->Class != 277 || it->Live != 2)
		{
			continue;
		}

		this->SetGateBlockState(it->X,it->Y,lpMsg->operation);

		gEffectManager->AddEffect(lpMonster,0,EFFECT_CASTLE_GATE_STATE,0,0,0,0,0);

		lpMonster->CastleGateState = lpMsg->operation;

		this->GCCastleSiegeGateOperateSend(aIndex,1,bIndex,lpMsg->operation);

		this->GCCastleSiegeGateOperateStateSend(bIndex,lpMsg->operation);

		return;
	}

	this->GCCastleSiegeGateOperateSend(aIndex,0,-1,0);
}

void CCastleSiege::GCCastleSiegeRegisterGuildResultSend(int aIndex,int result,char* GuildName) // OK
{
	PMSG_CASTLE_SIEGE_REGISTER_GUILD_SEND pMsg;

	pMsg.header.set(0xB2,0x01,sizeof(pMsg));
	
	pMsg.result = result;

	memset(pMsg.GuildName,0,sizeof(pMsg.GuildName));

	if(strlen(GuildName) > 0)
	{
		memcpy(pMsg.GuildName,GuildName,sizeof(pMsg.GuildName));
	}

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeGiveupGuildResultSend(int aIndex,int result,int giveup,char* GuildName) // OK
{
	PMSG_CASTLE_SIEGE_GIVEUP_GUILD_SEND pMsg;

	pMsg.header.set(0xB2,0x02,sizeof(pMsg));

	pMsg.result = result;

	pMsg.giveup = giveup;

	memset(pMsg.GuildName,0,sizeof(pMsg.GuildName));

	if(strlen(GuildName) > 0)
	{
		memcpy(pMsg.GuildName,GuildName,sizeof(pMsg.GuildName));
	}

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeNpcBuyResultSend(int aIndex,int result,int NpcClass,int NpcIndex) // OK
{
	PMSG_CASTLE_SIEGE_NPC_BUY_SEND pMsg;

	pMsg.header.set(0xB2,0x05,sizeof(pMsg));
	
	pMsg.result = result;
	
	pMsg.NpcClass = NpcClass;
	
	pMsg.NpcIndex = NpcIndex;
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeNpcRepairResultSend(int aIndex,int result,int NpcClass,int NpcIndex,int life,int maxlife) // OK
{
	PMSG_CASTLE_SIEGE_NPC_REPAIR_SEND pMsg;

	pMsg.header.set(0xB2,0x06,sizeof(pMsg));
	
	pMsg.result = result;
	
	pMsg.NpcClass = NpcClass;
	
	pMsg.NpcIndex = NpcIndex;
	
	pMsg.Life = life;
	
	pMsg.MaxLife = maxlife;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeNpcUpgradeResultSend(int aIndex,int result,int NpcClass,int NpcIndex,int UpgradeType,int UpgradeValue) // OK
{
	PMSG_CASTLE_SIEGE_NPC_UPGRADE_SEND pMsg;

	pMsg.header.set(0xB2,0x07,sizeof(pMsg));
	
	pMsg.result = result;
	
	pMsg.NpcClass = NpcClass;
	
	pMsg.NpcIndex = NpcIndex;
	
	pMsg.UpgradeType = UpgradeType;
	
	pMsg.UpgradeValue = UpgradeValue;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeTaxRateRequestSend(int aIndex,int result,int type,int rate) // OK
{
	PMSG_CASTLE_SIEGE_TAX_RATE_INFO_SEND pMsg;

	pMsg.header.set(0xB2,0x09,sizeof(pMsg));

	pMsg.result = result;

	pMsg.TaxType = type;

	pMsg.TaxRate[0] = SET_NUMBERHB(SET_NUMBERHW(rate));

	pMsg.TaxRate[1] = SET_NUMBERLB(SET_NUMBERHW(rate));

	pMsg.TaxRate[2] = SET_NUMBERHB(SET_NUMBERLW(rate));

	pMsg.TaxRate[3] = SET_NUMBERLB(SET_NUMBERLW(rate));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeMoneyOutResultSend(int aIndex,int result,QWORD money) // OK
{
	PMSG_CASTLE_SIEGE_MONEY_OUT_SEND pMsg;

	pMsg.header.set(0xB2,0x10,sizeof(pMsg));

	pMsg.result = result;

	pMsg.money[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(money)));
	
	pMsg.money[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(money)));
	
	pMsg.money[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(money)));
	
	pMsg.money[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(money)));
	
	pMsg.money[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(money)));
	
	pMsg.money[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(money)));
	
	pMsg.money[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(money)));
	
	pMsg.money[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(money)));
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeGateStateSend(int aIndex,int result,int bIndex) // OK
{
	PMSG_CASTLE_SIEGE_GATE_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x11,sizeof(pMsg));

	pMsg.result = result;
	
	pMsg.index[0] = SET_NUMBERHB(SET_NUMBERLW(bIndex));

	pMsg.index[1] = SET_NUMBERLB(SET_NUMBERLW(bIndex));
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeGateOperateSend(int aIndex,int result,int index,int operation) // OK
{
	PMSG_CASTLE_SIEGE_GATE_OPERATE_SEND pMsg;

	pMsg.header.set(0xB2,0x12,sizeof(pMsg));

	pMsg.result = result;
	
	pMsg.operation = operation;
	
	pMsg.index[0] = SET_NUMBERHB(SET_NUMBERLW(index));
	
	pMsg.index[1] = SET_NUMBERLB(SET_NUMBERLW(index));
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeGateOperateStateSend(int aIndex,int operation) // OK
{
	PMSG_CASTLE_SIEGE_GATE_OPERATE_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x13,sizeof(pMsg));
	
	pMsg.operation = operation;
	
	pMsg.index[0] = SET_NUMBERHB(SET_NUMBERLW(aIndex));
	
	pMsg.index[1] = SET_NUMBERLB(SET_NUMBERLW(aIndex));

	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeSwitchStateSend(int aIndex,int bIndex,int Target,int state) // OK
{
	PMSG_CASTLE_SIEGE_SWITCH_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x14,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(SET_NUMBERLW(bIndex));

	pMsg.index[1] = SET_NUMBERLB(SET_NUMBERLW(bIndex));
	
	pMsg.target[0] = SET_NUMBERHB(SET_NUMBERLW(Target));
	
	pMsg.target[1] = SET_NUMBERLB(SET_NUMBERLW(Target));
	
	pMsg.state = state;
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeCrownAccessStateSend(int aIndex,int state) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(state == 0) // Attempt
	{
		if(lpObj->AccumulatedCrownAccessTime < 0 || lpObj->AccumulatedCrownAccessTime > (MAX_CASTLE_SIEGE_CROWN_TIME+1000))
		{
			lpObj->AccumulatedCrownAccessTime = 0;
		}
	}
	else if(state == 1) // Success
	{
		lpObj->AccumulatedCrownAccessTime = 0;
	}
	else if(state == 2) // Reg. Fail
	{
		lpObj->AccumulatedCrownAccessTime += GetTickCount()-this->GetCrownAccessTickCount();

		lpObj->AccumulatedCrownAccessTime -= gServerInfo->m_CastleSiegeLowerAccumulatedTimeValue;

		if(lpObj->AccumulatedCrownAccessTime < 0 || lpObj->AccumulatedCrownAccessTime > (MAX_CASTLE_SIEGE_CROWN_TIME+1000))
		{
			lpObj->AccumulatedCrownAccessTime = 0;
		}
	}

	PMSG_CASTLE_SIEGE_CROWN_ACCESS_SEND pMsg;

	pMsg.header.set(0xB2,0x15,sizeof(pMsg));

	pMsg.state = state;

	pMsg.time = lpObj->AccumulatedCrownAccessTime;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeCrownStateSend(int state) // OK
{
	LPOBJ lpObj = &gObj[this->m_CrownIndex];

	if(gObjIsConnected(this->m_CrownIndex) == 0)
	{
		return;
	}

	if(lpObj->Class != 216)
	{
		return;
	}

	if(state == 0)
	{
		gEffectManager->AddEffect(lpObj,0,EFFECT_CASTLE_CROWN_STATE,0,0,0,0,0);
	}
	else
	{
		gEffectManager->DelEffect(lpObj,EFFECT_CASTLE_CROWN_STATE);
	}

	PMSG_CASTLE_SIEGE_CROWN_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x16,sizeof(pMsg));

	pMsg.state = state;

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeStartStateSend(int aIndex,int state) // OK
{
	PMSG_CASTLE_SIEGE_START_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x17,sizeof(pMsg));
	
	pMsg.state = state;
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeStartStateSendToAll(int state) // OK
{
	PMSG_CASTLE_SIEGE_START_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x17,sizeof(pMsg));
	
	pMsg.state = state;
	
	this->DataSendToAll((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeNotifyProgressSendToAll(int state,char* GuildName) // OK
{
	PMSG_CASTLE_SIEGE_NOTIFY_PROGRESS_SEND pMsg;
	
	pMsg.header.set(0xB2,0x18,sizeof(pMsg));
	
	pMsg.state = state;
	
	memcpy(pMsg.GuildName,GuildName,sizeof(pMsg.GuildName));

	this->DataSendToAll((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeJoinSideSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	int state = 0;

	if(lpObj->Map == MAP_CASTLE_SIEGE && (this->GetState() == CS_STATE_START || this->GetState() == CS_STATE_END) && lpObj->CastleJoinSide > 0)
	{
		switch(lpObj->CastleJoinSide)
		{
			case 1:
				state = 1;
				gEffectManager->AddEffect(lpObj,0,EFFECT_GUILD_STATE1,0,0,0,0,0);
				break;
			case 2:
				state = 2;
				gEffectManager->AddEffect(lpObj,0,EFFECT_GUILD_STATE2,0,0,0,0,0);
				break;
			case 3:
				state = 3;
				gEffectManager->AddEffect(lpObj,0,EFFECT_GUILD_STATE3,0,0,0,0,0);
				break;
			case 4:
				state = 4;
				gEffectManager->AddEffect(lpObj,0,EFFECT_GUILD_STATE4,0,0,0,0,0);
				break;
		}
	}
	else
	{
		state = 0;
		gEffectManager->DelEffect(lpObj,EFFECT_GUILD_STATE1);
		gEffectManager->DelEffect(lpObj,EFFECT_GUILD_STATE2);
		gEffectManager->DelEffect(lpObj,EFFECT_GUILD_STATE3);
		gEffectManager->DelEffect(lpObj,EFFECT_GUILD_STATE4);
	}

	PMSG_CASTLE_SIEGE_JOIN_SIDE_SEND pMsg;

	pMsg.header.set(0xB2,0x19,sizeof(pMsg));
	
	pMsg.side = state;
	
	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeMiniMapRequestSend(int aIndex,int result) // OK
{
	PMSG_CASTLE_SIEGE_MINI_MAP_OPEN_SEND pMsg;

	pMsg.header.set(0xB2,0x1B,sizeof(pMsg));

	pMsg.result = result;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeLeftTimeSend(int aIndex) // OK
{
	BYTE hour = (this->m_RemainTime/1000)/3600;

	BYTE minute = ((this->m_RemainTime/1000)%3600)/60;

	PMSG_CASTLE_SIEGE_REMAIN_TIME_SEND pMsg;

	pMsg.header.set(0xB2,0x1E,sizeof(pMsg));
	
	pMsg.hour = hour;
	
	pMsg.minute = minute;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeLeftTimeSendToAll() // OK
{
	BYTE hour = (this->m_RemainTime/1000)/3600;

	BYTE minute = ((this->m_RemainTime/1000)%3600)/60;

	PMSG_CASTLE_SIEGE_REMAIN_TIME_SEND pMsg;

	pMsg.header.set(0xB2,0x1E,sizeof(pMsg));
	
	pMsg.hour = hour;
	
	pMsg.minute = minute;

	this->DataSendToAll((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GCCastleSiegeSwitchInfoSend(int aIndex) // OK
{
	LPOBJ lpSwitch = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	if(lpSwitch->Class != 217 && lpSwitch->Class != 218)
	{
		return;
	}

	PMSG_CASTLE_SIEGE_SWITCH_INFO_SEND pMsg;

	pMsg.header.set(0xB2,0x20,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(SET_NUMBERLW(aIndex));
	
	pMsg.index[1] = SET_NUMBERLB(SET_NUMBERLW(aIndex));

	pMsg.State = 0;

	pMsg.JoinSide = 0;

	memset(pMsg.GuildName,0,sizeof(pMsg.GuildName));

	memset(pMsg.Name,0,sizeof(pMsg.Name));

	int bIndex = this->GetCrownSwitchUserIndex(lpSwitch->Class);

	if(gObjIsConnected(bIndex) != 0)
	{
		LPOBJ lpObj = &gObj[bIndex];

		pMsg.State = 1;

		pMsg.JoinSide = lpObj->CastleJoinSide;

		memcpy(pMsg.Name,lpObj->Name,sizeof(pMsg.Name));

		memcpy(pMsg.GuildName,lpObj->GuildName,sizeof(pMsg.GuildName));

		if(lpObj->Guild != 0)
		{
			char GuildName[9] = {0};

			if(gUnionManager->GetUnionName(lpObj->Guild->GuildUnion,GuildName) != 0)
			{
				memcpy(pMsg.GuildName,GuildName,sizeof(pMsg.GuildName));
			}
		}

		DataSend(bIndex,(BYTE*)&pMsg,pMsg.header.size);
	}

	MsgSendV2(lpSwitch,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::DGCastleSiegeStateRecv(SDHP_CASTLE_SIEGE_STATE_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	PMSG_CASTLE_SIEGE_STATE_SEND pMsg;

	pMsg.header.set(0xB2,0x00,sizeof(pMsg));

	pMsg.result = lpMsg->result;

	memcpy(pMsg.CastleOwner,lpMsg->CastleOwner,sizeof(pMsg.CastleOwner));

	memcpy(pMsg.MasterName,lpMsg->MasterName,sizeof(pMsg.MasterName));

	SYSTEMTIME StartTime = {0},EndTime = {0};

	if(this->GetCastleStateTerm(&StartTime,&EndTime) == 0)
	{
		pMsg.state = CS_STATE_BLANK;
	}
	else
	{
		pMsg.state = this->GetState();
	}

	pMsg.StartYear[0] = SET_NUMBERHB(StartTime.wYear);
	pMsg.StartYear[1] = SET_NUMBERLB(StartTime.wYear);
	pMsg.StartMonth = (BYTE)StartTime.wMonth;
	pMsg.StartDay = (BYTE)StartTime.wDay;
	pMsg.StartHour = (BYTE)StartTime.wHour;
	pMsg.StartMinute = (BYTE)StartTime.wMinute;

	pMsg.EndYear[0] = SET_NUMBERHB(EndTime.wYear);
	pMsg.EndYear[1] = SET_NUMBERLB(EndTime.wYear);
	pMsg.EndMonth = (BYTE)EndTime.wMonth;
	pMsg.EndDay = (BYTE)EndTime.wDay;
	pMsg.EndHour = (BYTE)EndTime.wHour;
	pMsg.EndMinute = (BYTE)EndTime.wMinute;

	pMsg.SiegeStartYear[0] = SET_NUMBERHB(this->m_LeftCastleSiegeDate.wYear);
	pMsg.SiegeStartYear[1] = SET_NUMBERLB(this->m_LeftCastleSiegeDate.wYear);
	pMsg.SiegeStartMonth = (BYTE)this->m_LeftCastleSiegeDate.wMonth;
	pMsg.SiegeStartDay = (BYTE)this->m_LeftCastleSiegeDate.wDay;
	pMsg.SiegeStartHour = (BYTE)this->m_LeftCastleSiegeDate.wHour;
	pMsg.SiegeStartMinute = (BYTE)this->m_LeftCastleSiegeDate.wMinute;

	pMsg.RemainTime[0] = SET_NUMBERHB(SET_NUMBERHW((this->m_RemainTime/1000)));
	pMsg.RemainTime[1] = SET_NUMBERLB(SET_NUMBERHW((this->m_RemainTime/1000)));
	pMsg.RemainTime[2] = SET_NUMBERHB(SET_NUMBERLW((this->m_RemainTime/1000)));
	pMsg.RemainTime[3] = SET_NUMBERLB(SET_NUMBERLW((this->m_RemainTime/1000)));

	DataSend(lpMsg->aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::DGCastleSiegeNpcBuyRecv(SDHP_CASTLE_SIEGE_NPC_BUY_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
		{
			if(it->Class == lpMsg->NpcClass && it->Index == lpMsg->NpcIndex)
			{
				if(gObjIsConnected(it->aIndex) != 0 && gObj[it->aIndex].Live == 1)
				{
					return;
				}

				int index = gObjAddMonster(MAP_CASTLE_SIEGE);

				if(OBJECT_RANGE(index) == 0)
				{
					return;
				}

				LPOBJ lpMonster = &gObj[index];

				if(gObjSetMonster(index,it->Class) == 0)
				{
					gObjDel(index);
					return;
				}

				lpMonster->PosNum = -1;
				lpMonster->X = it->X;
				lpMonster->Y = it->Y;
				lpMonster->Map = MAP_CASTLE_SIEGE;
				lpMonster->TX = lpMonster->X;
				lpMonster->TY = lpMonster->Y;
				lpMonster->OldX = lpMonster->X;
				lpMonster->OldY = lpMonster->Y;
				lpMonster->Dir = it->Dir;
				lpMonster->StartX = (BYTE)lpMonster->X;
				lpMonster->StartY = (BYTE)lpMonster->Y;
				lpMonster->DieRegen = 0;
				lpMonster->MaxRegenTime = 0;
				lpMonster->Life = (float)it->Life;
				lpMonster->MaxLife = (float)it->LifeLevel;
				lpMonster->CsNpcType = 1;

				if(it->Class == 277)
				{
					lpMonster->Defense = NpcDefense[0][it->DefenseLevel];
					lpMonster->MagicDefense = NpcDefense[0][it->DefenseLevel];
					lpMonster->CastleDefenseLevel = it->DefenseLevel;

					this->SetGateBlockState(it->X,it->Y,1);

					gEffectManager->AddEffect(lpMonster,0,EFFECT_CASTLE_GATE_STATE,0,0,0,0,0);

					lpMonster->CastleGateState = 1;

					if(gObjIsConnected(it->SwitchIndex) == 0 || gObj[it->SwitchIndex].Class != 219)
					{
						it->SwitchIndex = this->CreateNpcGateSwitch(it->X,it->Y+4);

						if(OBJECT_RANGE(it->SwitchIndex) != 0)
						{
							gObj[it->SwitchIndex].CastleSwitchIndex = index;
							gObj[index].CastleSwitchIndex = it->SwitchIndex;
						}
					}
					else
					{
						gObj[it->SwitchIndex].CastleSwitchIndex = index;
						gObj[index].CastleSwitchIndex = it->SwitchIndex;
					}
				}
				else if(it->Class == 283)
				{
					lpMonster->Defense = NpcDefense[1][it->DefenseLevel];
					lpMonster->MagicDefense = NpcDefense[1][it->DefenseLevel];
					lpMonster->CastleDefenseLevel = it->DefenseLevel;
					lpMonster->CastleRegenLevel = it->RegenLevel;
				}

				it->aIndex = index;
				it->Live = 2;
			}
		}

		lpObj->Money -= lpMsg->BuyCost;
			
		if(lpObj->Money < 0)
		{
			lpObj->Money = 0;
		}

		GCMoneySend(lpObj->Index,lpObj->Money);
	}

	this->GCCastleSiegeNpcBuyResultSend(lpMsg->aIndex,lpMsg->result,lpMsg->NpcClass,lpMsg->NpcIndex);
}

void CCastleSiege::DGCastleSiegeNpcRepairRecv(SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
		{
			if(it->Class == lpMsg->NpcClass && it->Index == lpMsg->NpcIndex && it->Live > 0)
			{
				it->Life = lpMsg->Life;

				it->LifeLevel = lpMsg->MaxLife;

				LPOBJ lpMonster = &gObj[it->aIndex];

				if(gObjIsConnected(it->aIndex) != 0 && lpMonster->CsNpcType != 0 && lpMonster->Class == lpMsg->NpcClass)
				{
					lpMonster->Life = lpMonster->MaxLife;
				}

				lpObj->Money -= lpMsg->RepairCost;

				if(lpObj->Money < 0)
				{
					lpObj->Money = 0;
				}

				GCMoneySend(lpObj->Index,lpObj->Money);

				break;
			}
		}
	}

	this->GCCastleSiegeNpcRepairResultSend(lpMsg->aIndex,lpMsg->result,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->Life,lpMsg->MaxLife);
}

void CCastleSiege::DGCastleSiegeNpcUpgradeRecv(SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result != 0)
	{
		if(lpMsg->NpcClass == 277)
		{
			switch(lpMsg->UpgradeType)
			{
				case 1:
					this->UpgradeDefenseLevel(lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeValue);
					break;
				case 3:
					this->UpgradeLifeLevel(lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeValue);
				break;
			}
		}
		else if(lpMsg->NpcClass == 283)
		{
			switch(lpMsg->UpgradeType)
			{
				case 1:
					this->UpgradeDefenseLevel(lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeValue);
					break;
				case 2:
					this->UpgradeRegenLevel(lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeValue);
					break;
				case 3:
					this->UpgradeLifeLevel(lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeValue);
					break;
			}
		}
	}

	this->GCCastleSiegeNpcUpgradeResultSend(lpMsg->aIndex,lpMsg->result,lpMsg->NpcClass,lpMsg->NpcIndex,lpMsg->UpgradeType,lpMsg->UpgradeValue);
}

void CCastleSiege::DGCastleSiegeTaxMoneyInfoRecv(SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		this->m_TributeMoney = lpMsg->TributeMoney;

		PMSG_CASTLE_SIEGE_TAX_MONEY_INFO_SEND pMsg;

		pMsg.header.set(0xB2,0x08,sizeof(pMsg));

		pMsg.result = lpMsg->result;

		pMsg.TaxRateChaos = lpMsg->TaxRateChaos;

		pMsg.TaxRateStore = lpMsg->TaxRateStore;

		pMsg.TributeMoney[0] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERHDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[1] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERHDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[2] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERHDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[3] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERHDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[4] = SET_NUMBERHB(SET_NUMBERHW(SET_NUMBERLDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[5] = SET_NUMBERLB(SET_NUMBERHW(SET_NUMBERLDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[6] = SET_NUMBERHB(SET_NUMBERLW(SET_NUMBERLDW(lpMsg->TributeMoney)));
		
		pMsg.TributeMoney[7] = SET_NUMBERLB(SET_NUMBERLW(SET_NUMBERLDW(lpMsg->TributeMoney)));

		DataSend(lpMsg->aIndex,(BYTE*)&pMsg,pMsg.header.size);
	}
}

void CCastleSiege::DGCastleSiegeTaxRateChangeRecv(SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		int taxrate = 0;

		switch(lpMsg->TaxType)
		{
			case 1:
				taxrate = 3;
				break;
			case 2:
				taxrate = 3;
				break;
			case 3:
				taxrate = 300000;
				break;
		}

		if(lpMsg->TaxRate >= 0 && lpMsg->TaxRate <= taxrate)
		{
			switch(lpMsg->TaxType)
			{
				case 1:
					this->m_TaxRateChaos = lpMsg->TaxRate;
					break;
				case 2:
					this->m_TaxRateStore = lpMsg->TaxRate;
					break;
				case 3:
					this->m_TaxRateHunt = lpMsg->TaxRate;
					break;
			}
		}

		this->GCCastleSiegeTaxRateRequestSend(lpMsg->aIndex,lpMsg->result,lpMsg->TaxType,lpMsg->TaxRate);
	}
}

void CCastleSiege::DGCastleSiegeMoneyOutRecv(SDHP_CASTLE_SIEGE_MONEY_OUT_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		LPOBJ lpObj = &gObj[lpMsg->aIndex];

		if(gObjIsConnected(lpMsg->aIndex) != 0)
		{
			if(lpMsg->Money < 0)
			{
				lpObj->Money -= lpMsg->Money;

				if(lpObj->Money > MAX_MONEY)
				{
					lpObj->Money = MAX_MONEY;
				}

				GCMoneySend(lpObj->Index,lpObj->Money);
			}
		}

		this->m_TributeMoney = lpMsg->LastMoney;
	}
	
	this->GCCastleSiegeMoneyOutResultSend(lpMsg->aIndex,lpMsg->result,lpMsg->LastMoney);
}

void CCastleSiege::DGCastleSiegeRegisterInfoRecv(SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	PMSG_CASTLE_SIEGE_REGISTER_GUILD_INFO_SEND pMsg;

	pMsg.header.set(0xB2,0x03,sizeof(pMsg));

	pMsg.result = lpMsg->result;

	pMsg.MarkCount[0] = SET_NUMBERHB(SET_NUMBERHW(lpMsg->MarkCount));

	pMsg.MarkCount[1] = SET_NUMBERLB(SET_NUMBERHW(lpMsg->MarkCount));

	pMsg.MarkCount[2] = SET_NUMBERHB(SET_NUMBERLW(lpMsg->MarkCount));

	pMsg.MarkCount[3] = SET_NUMBERLB(SET_NUMBERLW(lpMsg->MarkCount));

	pMsg.position = lpMsg->Position;

	pMsg.giveup = lpMsg->GiveUp;

	memcpy(pMsg.GuildName,lpMsg->GuildName,sizeof(pMsg.GuildName));

	DataSend(lpMsg->aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::DGCastleSiegeRegisterRecv(SDHP_CASTLE_SIEGE_REGISTER_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	this->GCCastleSiegeRegisterGuildResultSend(lpMsg->aIndex,lpMsg->result,lpMsg->GuildName);
}

void CCastleSiege::DGCastleSiegeResetStateRecv(SDHP_CASTLE_SIEGE_RESET_STATE_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		this->m_BasicGuildDataLoad = 0;

		this->m_SiegeGuildDataLoad = 0;

		this->m_BasicGuildDataMap.clear();

		this->m_SiegeGuildDataMap.clear();

		this->m_CastleTowerEnable = 0;
	
		this->m_CrownAvailable = 0;
	
		this->m_CastleGuildListLoad = 0;
	
		this->m_CastleSiegeEnded = 0;
	}
}

void CCastleSiege::DGCastleSiegeRegisterMarkRecv(SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}
	
	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	lpObj->SiegueRegisterTransaction = 0;

	if(lpMsg->result != 0)
	{
		gItemManager->DeleteInventoryItemCount(lpObj,GET_ITEM(14,21),3,1);
	}

	PMSG_CASTLE_SIEGE_REGISTER_MARK_SEND pMsg;

	pMsg.header.set(0xB2,0x04,sizeof(pMsg));
	
	pMsg.result = lpMsg->result;
	
	pMsg.MarkCount[0] = SET_NUMBERHB(SET_NUMBERHW(lpMsg->MarkCount));
	
	pMsg.MarkCount[1] = SET_NUMBERLB(SET_NUMBERHW(lpMsg->MarkCount));
	
	pMsg.MarkCount[2] = SET_NUMBERHB(SET_NUMBERLW(lpMsg->MarkCount));
	
	pMsg.MarkCount[3] = SET_NUMBERLB(SET_NUMBERLW(lpMsg->MarkCount));
	
	memcpy(pMsg.GuildName,lpMsg->GuildName,sizeof(pMsg.GuildName));
	
	DataSend(lpMsg->aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::DGCastleSiegeGiveUpGuildRecv(SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	if(lpMsg->result == 1 && lpMsg->MarkCount > 0)
	{
		lpObj->Money += (lpMsg->MarkCount*3000);

		if(lpObj->Money > MAX_MONEY)
		{
			lpObj->Money = MAX_MONEY;
		}

		GCMoneySend(lpObj->Index,lpObj->Money);
	}

	this->GCCastleSiegeGiveupGuildResultSend(lpMsg->aIndex,lpMsg->result,lpMsg->GiveUp,lpMsg->GuildName);
}

void CCastleSiege::DGCastleSiegeSyncStateRecv(SDHP_CASTLE_SIEGE_SYNC_STATE_RECV* lpMsg) // OK
{
	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	gCastleSiegeSync->SetCastleState(lpMsg->State); 
	
	gCastleSiegeSync->SetTaxRateChaos(lpMsg->TaxRateChaos);
	
	gCastleSiegeSync->SetTaxRateStore(lpMsg->TaxRateStore);
	
	gCastleSiegeSync->SetTaxHuntZone(lpMsg->TaxRateHunt);
	
	gCastleSiegeSync->SetCastleOwnerGuild(lpMsg->CastleOwner);
}

void CCastleSiege::DGCastleSiegeAddTributeMoneyRecv(SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		this->m_TributeMoney = lpMsg->money;
	}
}

void CCastleSiege::DGCastleSiegeResetTaxInfoRecv(SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 1)
	{
		this->m_TributeMoney = 0;
		this->m_TaxRateChaos = 0;
		this->m_TaxRateStore = 0;
		this->m_TaxRateHunt = 0;
	}
}

void CCastleSiege::DGCastleSiegeInitDataRecv(SDHP_CASTLE_SIEGE_INIT_DATA_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(this->m_CastleSiegeDataLoad != 2)
	{
		return;
	}

	this->m_CastleSiegeDataLoad = 3;

	if(lpMsg->result == 0)
	{
		return;
	}
	
	memset(&this->m_StartDate,0,sizeof(this->m_StartDate));

	memset(&this->m_EndDate,0,sizeof(this->m_EndDate));

	this->m_StartDate.wYear = lpMsg->StartYear;
	
	this->m_StartDate.wMonth = lpMsg->StartMonth;
	
	this->m_StartDate.wDay = lpMsg->StartDay;
	
	this->m_EndDate.wYear = lpMsg->EndYear;
	
	this->m_EndDate.wMonth = lpMsg->EndMonth;
	
	this->m_EndDate.wDay = lpMsg->EndDay;
	
	this->m_CastleGuildListLoad = lpMsg->CastleGuildListLoad;
	
	this->m_CastleSiegeEnded = lpMsg->CastleSiegeEnded;
	
	this->m_CastleOccupied = lpMsg->CastleOccupied;

	memset(&this->m_CastleOwner,0,sizeof(this->m_CastleOwner));

	memcpy(&this->m_CastleOwner,lpMsg->CastleOwner,sizeof(lpMsg->CastleOwner));

	this->m_TributeMoney = lpMsg->TributeMoney;

	this->m_TaxRateChaos = lpMsg->TaxRateChaos;

	this->m_TaxRateStore = lpMsg->TaxRateStore;

	this->m_TaxRateHunt = lpMsg->TaxRateHunt;

	this->m_SiegeEndSchedule = this->m_StartDate;

	_SYSTEMTIME EndTime;

	for(std::vector<CASTLE_STATE_TIME_DATA>::iterator it = this->m_StateTimeData.begin(); it != this->m_StateTimeData.end(); it++)
	{
		if(it->State == CS_STATE_END_CYCLE)
		{
			EndTime.wDay = it->Day;
			EndTime.wHour = it->Hour;
			EndTime.wMinute = it->Minute;
			EndTime.wSecond = 0;
			break;
		}
	}

	GetNextDay(&this->m_SiegeEndSchedule,EndTime.wDay,EndTime.wHour,EndTime.wMinute,0);

	int StartDateTime = MAKELONG(MAKEWORD(this->m_StartDate.wDay,this->m_StartDate.wMonth),this->m_StartDate.wYear);

	int EndDateTime = MAKELONG(MAKEWORD(this->m_EndDate.wDay,this->m_EndDate.wMonth),this->m_EndDate.wYear);

	if(StartDateTime > EndDateTime)
	{
		LogAdd(LOG_RED,"[Castle Siege] DGCastleSiegeInitDataRecv - StartDateTime > EndDateTime");
		return;
	}

	for(int n = 0; n < lpMsg->count; n++)
	{
		SDHP_CASTLE_SIEGE_INIT_DATA* lpInfo = (SDHP_CASTLE_SIEGE_INIT_DATA*)(((BYTE*)lpMsg) + sizeof(SDHP_CASTLE_SIEGE_INIT_DATA_RECV) + (sizeof(SDHP_CASTLE_SIEGE_INIT_DATA)*n));
		
		bool exist = 0;

		for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
		{
			CASTLE_SIEGE_NPC_DATA & NpcData = CASTLE_SIEGE_NPC_DATA(*it);

			if(NpcData.Class == lpInfo->NpcClass && NpcData.Index == lpInfo->NpcIndex)
			{
				it->Live = 1;
				it->DefenseLevel = lpInfo->DefenseLevel;
				it->RegenLevel = lpInfo->RegenLevel;
				it->LifeLevel = lpInfo->LifeLevel;
				it->Life = lpInfo->Life;
				it->X = lpInfo->X;
				it->Y = lpInfo->Y;
				it->Dir = lpInfo->Dir;
				it->Side = 1;
				exist = 1;
				break;
			}
		}

		if(exist == 0)
		{
			CASTLE_SIEGE_NPC_DATA info;

			info.Class = lpInfo->NpcClass;
			info.Index = lpInfo->NpcIndex;
			info.Live = 1;
			info.Side = 1;
			info.DefaultDefenseLevel = lpInfo->DefenseLevel;
			info.DefenseLevel = info.DefaultDefenseLevel;
			info.DefaultRegenLevel = lpInfo->RegenLevel;
			info.RegenLevel = info.DefaultRegenLevel;

			info.DefaultLifeLevel = lpInfo->LifeLevel;
			info.LifeLevel = info.DefaultLifeLevel;
			info.DefaultLife = lpInfo->Life;
			info.Life = info.DefaultLife;
			info.DefaultX = lpInfo->X;
			info.X = info.DefaultX;
			info.DefaultY = lpInfo->Y;
			info.Y = info.DefaultY;
			info.DefaultDir = lpInfo->Dir;
			info.Dir = info.DefaultDir;

			this->m_NpcDataList.push_back(info);
		}
	}

	this->m_CastleSiegeLoadDatabase = 1;

	this->m_CastleSiegeDataLoad = 4;

	if(lpMsg->FirstRun == 1)
	{
		this->GDCastleSiegeNpcCreateSend();
	}

	if(this->m_CastleSiegeLoadFile == 0 || this->m_CastleSiegeLoadDatabase == 0)
	{
		return;
	}

	this->m_CastleSiegeDataLoad = 0;

	this->CreateAllGateSwitch();

	if(this->CheckSync() == 0)
	{
		return;
	}

	this->m_CastleSiegeDoRun = 1;

	this->m_SaveNpcTime = GetTickCount();
	this->m_CalcLeftTime = 0;
	this->m_MessageTime = 0;
	this->m_CheckGateTime = 0;
	this->m_LeftStartTime = 0;
	this->m_LeftTime = 0;
	this->m_UpdateSideTime = 0;
	this->m_MiniMapTime = 0;
}

void CCastleSiege::DGCastleSiegeRegisterListRecv(SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(gObjIsConnected(lpMsg->aIndex) == 0)
	{
		return;
	}

	BYTE send[2048];

	PMSG_CASTLE_SIEGE_REGISTER_LIST_SEND pMsg;

	pMsg.header.set(0xB4,0);

	int size = sizeof(pMsg);

	pMsg.result = lpMsg->result;

	pMsg.count = 0;

	if(lpMsg->result == 1)
	{
		PMSG_CASTLE_SIEGE_REGISTER_INFO info;

		for(int n=0;n < lpMsg->count;n++)
		{
			SDHP_CASTLE_SIEGE_REGISTER_LIST* lpInfo = (SDHP_CASTLE_SIEGE_REGISTER_LIST*)(((BYTE*)lpMsg)+sizeof(SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV)+(sizeof(SDHP_CASTLE_SIEGE_REGISTER_LIST)*n));

			memcpy(info.GuildName,lpInfo->GuildName,sizeof(info.GuildName));

			info.MarkCount[0] = SET_NUMBERHB(SET_NUMBERHW(lpInfo->MarkCount));
		
			info.MarkCount[1] = SET_NUMBERLB(SET_NUMBERHW(lpInfo->MarkCount));
		
			info.MarkCount[2] = SET_NUMBERHB(SET_NUMBERLW(lpInfo->MarkCount));
		
			info.MarkCount[3] = SET_NUMBERLB(SET_NUMBERLW(lpInfo->MarkCount));

			info.giveup = lpInfo->GiveUp;

			info.position = lpInfo->Position;

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpMsg->aIndex,send,size);
}

void CCastleSiege::DGCastleSiegeNpcCreateRecv(SDHP_CASTLE_SIEGE_NPC_CREATE_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(this->m_CastleNpcCreated == 0)
	{
		this->CreateNpcDB();
		this->m_CastleNpcCreated = 1;
	}
}

void CCastleSiege::DGCastleSiegeBuildGuildListRecv(SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 0 || lpMsg->count == 0)
	{
		return;
	}

	this->m_BasicGuildDataLoad = 0;

	this->m_SiegeGuildDataLoad = 0;

	std::vector<CASTLE_SIEGE_GUILD_DATA> GuildList;

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST* lpInfo = (SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST*)(((BYTE*)lpMsg)+sizeof(SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV)+(sizeof(SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST)*n));

		CASTLE_SIEGE_GUILD_DATA info;

		memcpy(info.GuildName,lpInfo->GuildName,sizeof(info.GuildName));
		info.MarkCount = lpInfo->MarkCount;
		info.MasterLevel = lpInfo->MasterLevel;
		info.MemberCount = lpInfo->MemberCount;
		info.Position = (0x100000-lpInfo->Position);
		info.TotalScore = MAKE_NUMBERLL(lpInfo->Position,((lpInfo->MarkCount*5)+lpInfo->MemberCount+(lpInfo->MasterLevel/4)));

		GuildList.push_back(info);
	}

	if(GuildList.empty() == 1)
	{
		return;
	}

	sort(GuildList.begin(),GuildList.end(),this->RegGuildScoreCompFunc);

	this->m_BasicGuildDataMap.clear();

	for(size_t n = 0; n < GuildList.size(); n++)
	{
		if(n >= MAX_CASTLE_SIEGE_GUILD)
		{
			break;
		}

		CASTLE_SIEGE_GUILD_FINAL info;

		info.Side = (n+2);
		info.Involved = 1;
		info.Score = (GuildList[n].MarkCount*5)+(GuildList[n].MemberCount)+(GuildList[n].MasterLevel/4);

		this->m_BasicGuildDataMap.insert(std::pair<std::string,CASTLE_SIEGE_GUILD_FINAL>(GuildList[n].GuildName,info));
	}

	if(this->m_CastleOccupied == 1 && strcmp(this->m_CastleOwner,"") != 0)
	{
		CASTLE_SIEGE_GUILD_FINAL info;

		info.Side = 1;
		info.Involved = 1;
		info.Score = 0;

		this->m_BasicGuildDataMap.insert(std::pair<std::string,CASTLE_SIEGE_GUILD_FINAL>(this->m_CastleOwner,info));
	}

	this->m_BasicGuildDataLoad = 1;

	this->GDCastleSiegeLoadUnionListSend();
}

void CCastleSiege::DGCastleSiegeUnionListRecv(SDHP_CASTLE_SIEGE_UNION_LIST_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 0 || lpMsg->count == 0)
	{
		return;
	}

	if(this->m_BasicGuildDataLoad == 0)
	{
		return;
	}

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_CASTLE_SIEGE_UNION_LIST* lpInfo = (SDHP_CASTLE_SIEGE_UNION_LIST*)(((BYTE*)lpMsg)+sizeof(SDHP_CASTLE_SIEGE_UNION_LIST_RECV)+(sizeof(SDHP_CASTLE_SIEGE_UNION_LIST)*n));

		std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.find(lpInfo->GuildName);

		if(it == this->m_SiegeGuildDataMap.end())
		{
			CASTLE_SIEGE_GUILD_FINAL info;

			info.Side = lpInfo->Side;

			info.Involved = 0;
			
			info.Score = 0;

			this->m_SiegeGuildDataMap.insert(std::pair<std::string,CASTLE_SIEGE_GUILD_FINAL>(lpInfo->GuildName,info));
		}
	}

	this->m_SiegeGuildDataLoad = 1;

	this->GDCastleSiegeSaveGuildListSend();
	
	this->SetCastleJoinSide();
}

void CCastleSiege::DGCastleSiegeSaveListRecv(SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	this->m_CastleGuildListLoad = lpMsg->result;
}

void CCastleSiege::DGCastleSiegeLoadGuildListRecv(SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV* lpMsg) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(lpMsg->MapServerGroup != gMapServerManager->GetMapServerGroup())
	{
		return;
	}

	if(lpMsg->result == 0 || lpMsg->count == 0)
	{
		return;
	}

	CASTLE_SIEGE_GUILD_FINAL info;

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST* lpInfo = (SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST*)(((BYTE*)lpMsg)+sizeof(SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV)+(sizeof(SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST)*n));

		char GuildName[9] = {0};

		memcpy(GuildName,lpInfo->GuildName,sizeof(GuildName));

		info.Side = lpInfo->Side;

		info.Involved = lpInfo->Involved;

		info.Score = lpInfo->Score;

		this->m_SiegeGuildDataMap.insert(std::pair<std::string,CASTLE_SIEGE_GUILD_FINAL>(GuildName,info));

		if(info.Involved != 0)
		{
			this->m_BasicGuildDataMap.insert(std::pair<std::string,CASTLE_SIEGE_GUILD_FINAL>(GuildName,info));
		}
	}

	this->m_BasicGuildDataLoad = 1;

	this->m_SiegeGuildDataLoad = 1;

	this->SetCastleJoinSide();
}

void CCastleSiege::GDCastleSiegeDateChangeSend() // OK
{
	SDHP_CASTLE_SIEGE_DATE_CHANGE_SEND pMsg;

	pMsg.header.set(0x80,0x09,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.StartYear = this->m_StartDate.wYear;

	pMsg.StartMonth = this->m_StartDate.wMonth;
	
	pMsg.StartDay = this->m_StartDate.wDay;
	
	pMsg.EndYear = this->m_EndDate.wYear;
	
	pMsg.EndMonth = this->m_EndDate.wMonth;
	
	pMsg.EndDay = this->m_EndDate.wDay;
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeEndSend() // OK
{
	SDHP_CASTLE_SIEGE_END_SEND pMsg;

	pMsg.header.set(0x80,0x0B,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeOwnerChangeSend() // OK
{
	SDHP_CASTLE_SIEGE_OWNER_CHANGE_SEND pMsg;

	pMsg.header.set(0x80,0x0C,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.CastleOccupied = this->m_CastleOccupied;

	memcpy(pMsg.CastleOwner,this->m_CastleOwner,sizeof(pMsg.CastleOwner));
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeResetStateSend() // OK
{
	SDHP_CASTLE_SIEGE_RESET_STATE_SEND pMsg;

	pMsg.header.set(0x80,0x0E,sizeof(pMsg));
	
	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeSyncStateSend() // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	if(this->m_CastleSiegeDataLoad == 2)
	{
		if(this->m_CastleSiegeLoadFile == 0)
		{
			return;
		}

		bool result = 0;

		if(this->m_CastleSiegeDataLoadTime == 0)
		{
			result = 1;
			this->m_CastleSiegeDataLoadTime = GetTickCount();
		}
		else if(GetTickCount()-this->m_CastleSiegeDataLoadTime > 60000)
		{
			result = 1;
			this->m_CastleSiegeDataLoadTime = GetTickCount();
			LogAdd(LOG_RED,"[Castle Siege] Can not connect DataServer");
		}

		if(result == 1)
		{
			SDHP_CASTLE_SIEGE_INIT_DATA_SEND pMsg;

			pMsg.header.set(0x81,sizeof(pMsg));

			pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

			pMsg.CastleCycle = this->m_Cycle;

			gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
		}
	}

	SDHP_CASTLE_SIEGE_SYNC_STATE_SEND pMsg;

	pMsg.header.set(0x80,0x17,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();
	
	pMsg.State = this->m_State;
	
	pMsg.TaxRateChaos = this->m_TaxRateChaos;
	
	pMsg.TaxRateStore = this->m_TaxRateStore;
	
	pMsg.TaxRateHunt = this->m_TaxRateHunt;
	
	memcpy(pMsg.CastleOwner,this->m_CastleOwner,sizeof(pMsg.CastleOwner));
	
	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeAddTributeMoneySend(int money) // OK
{
	if(money <= 0 || money > MAX_MONEY)
	{
		return;
	}

	SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_SEND pMsg;

	pMsg.header.set(0x80,0x18,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.money = money;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeResetTaxInfoSend() // OK
{
	SDHP_CASTLE_SIEGE_RESET_TAX_INFO_SEND pMsg;

	pMsg.header.set(0x80,0x19,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeParticipantClearSend() // OK
{
	SDHP_CASTLE_SIEGE_PARTICIPANT_CLEAR_SEND pMsg;

	pMsg.header.set(0x80,0x1A,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeRegisteredClearSend() // OK
{
	SDHP_CASTLE_SIEGE_REGISTERED_CLEAR_SEND pMsg;

	pMsg.header.set(0x80,0x1B,sizeof(pMsg));
	
	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeNpcCreateSend() // OK
{
	BYTE send[4096];

	SDHP_CASTLE_SIEGE_NPC_CREATE_SEND pMsg;

	pMsg.header.set(0x84,0);

	int size = sizeof(pMsg);

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_NPC_CREATE info;

	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->DbSave == 1)
		{
			it->SetBaseValue();
			it->Live = 1;
			it->Side = 1;

			info.NpcClass = it->Class;
			info.NpcIndex = it->Index;
			info.NpcDefense = it->DefaultDefenseLevel;
			info.NpcRegen = it->DefaultRegenLevel;
			info.NpcMaxLife = it->DefaultLifeLevel;
			info.NpcLife = it->DefaultLife;
			info.X = it->DefaultX;
			info.Y = it->DefaultY;
			info.Dir = it->DefaultDir;

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gDataServerConnection.DataSend(send,size);
}

void CCastleSiege::GDCastleSiegeBuildGuildListSend() // OK
{
	SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0x85,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeLoadUnionListSend() // OK
{
	this->m_SiegeGuildDataMap.clear();

	BYTE send[2048];

	SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_SEND pMsg;

	pMsg.header.set(0x86,0);

	int size = sizeof(pMsg);

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_LOAD_UNION_LIST info;

	for(std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_BasicGuildDataMap.begin(); it != this->m_BasicGuildDataMap.end(); it++)
	{
		this->m_SiegeGuildDataMap.insert(std::pair<std::string,CASTLE_SIEGE_GUILD_FINAL>(it->first,it->second));

		memcpy(info.GuildName,it->first.c_str(),sizeof(info.GuildName));

		info.Side = it->second.Side;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gDataServerConnection.DataSend(send,size);
}

void CCastleSiege::GDCastleSiegeSaveGuildListSend() // OK
{
	BYTE send[2048];

	SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0x87,0);

	int size = sizeof(pMsg);

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST info;

	for(std::map<std::string,CASTLE_SIEGE_GUILD_FINAL>::iterator it = this->m_SiegeGuildDataMap.begin(); it != this->m_SiegeGuildDataMap.end(); it++)
	{
		memcpy(info.GuildName,it->first.c_str(),sizeof(info.GuildName));

		info.Side = it->second.Side;

		info.Involved = it->second.Involved;

		info.Score = it->second.Score;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gDataServerConnection.DataSend(send,size);
}

void CCastleSiege::GDCastleSiegeLoadGuildListSend() // OK
{
	SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_SEND pMsg;

	pMsg.header.set(0x88,sizeof(pMsg));

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CCastleSiege::GDCastleSiegeNpcSaveSend() // OK
{
	BYTE send[4096];

	SDHP_CASTLE_SIEGE_NPC_SAVE_SEND pMsg;

	pMsg.header.set(0x89,0);

	int size = sizeof(pMsg);

	pMsg.MapServerGroup = gMapServerManager->GetMapServerGroup();

	pMsg.count = 0;

	SDHP_CASTLE_SIEGE_NPC_SAVE info;

	for(std::vector<CASTLE_SIEGE_NPC_DATA>::iterator it = this->m_NpcDataList.begin(); it != this->m_NpcDataList.end(); it++)
	{
		if(it->DbSave == 1) 
		{
			if(gObjIsConnected(it->aIndex) != 0)
			{
				if(gObj[it->aIndex].Class == it->Class && gObj[it->aIndex].Live == 1 && gObj[it->aIndex].Life > 0)
				{
					info.NpcClass = it->Class;
					info.NpcIndex = it->Index;
					info.NpcDefense = it->DefenseLevel;
					info.NpcRegen = it->RegenLevel;
					info.NpcMaxLife = (int)gObj[it->aIndex].MaxLife;
					info.NpcLife = (int)gObj[it->aIndex].Life;
					info.X = it->X;
					info.Y = it->Y;
					info.Dir = it->Dir;

					memcpy(&send[size],&info,sizeof(info));
					size += sizeof(info);

					pMsg.count++;
				}
			}
		}
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	gDataServerConnection.DataSend(send,size);
}