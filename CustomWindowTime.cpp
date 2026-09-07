// CustomWindowTime.cpp: implementation of the CCustomWindowTime class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomWindowTime.h"
#include "CustomArena.h"
#include "BloodCastle.h"
#include "BonusManager.h"
#include "ChaosCastle.h"
#include "CastleDeep.h"
#include "Crywolf.h"
#include "CustomLukeHelper.h"
#include "DevilSquare.h"
#include "DropEvent.h"
#include "IllusionTemple.h"
#include "InvasionManager.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomWindowTime::CCustomWindowTime() // OK
{

}

CCustomWindowTime::~CCustomWindowTime() // OK
{

}

void CCustomWindowTime::GCEventTimeSend(int aIndex) // OK
{
	if(gServerInfo->m_WindowEventSwitch == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->WindowTimeOpen == false)
	{
		return;
	}

	BYTE send[4096];

	PMSG_EVENT_TIME_LIST_SEND pMsg;

	pMsg.header.set(0xF3,0xEF,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	EVENT_TIME_INFO info;

	if(gServerInfo->m_DevilSquareEvent != 0)
	{
		strcpy_s(info.name,"Devil Square");
		info.status = EVENT_STATE_START;
		info.time = 0;

		if(gDevilSquare->GetState(0) == DS_STATE_BLANK)
		{
			info.status = EVENT_STATE_BLANK;
		}
		else if(gDevilSquare->GetState(0) == DS_STATE_EMPTY)
		{
			info.status = EVENT_STATE_STAND;

			info.time = gDevilSquare->m_DevilSquareLevel[0].RemainTime;
		}
		else if(gDevilSquare->GetState(0) == DS_STATE_READY)
		{
			info.status = EVENT_STATE_OPEN;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}
	
	if(gServerInfo->m_BloodCastleEvent != 0)
	{
		strcpy_s(info.name,"Blood Castle");
		info.status = EVENT_STATE_START;
		info.time = 0;

		if(gBloodCastle->GetState(0) == BC_STATE_BLANK)
		{
			info.status = EVENT_STATE_BLANK;
		}
		else if(gBloodCastle->GetState(0) == BC_STATE_EMPTY)
		{
			info.status = EVENT_STATE_STAND;

			info.time = gBloodCastle->m_BloodCastleLevel[0].RemainTime;
		}
		else if(gBloodCastle->GetState(0) == BC_STATE_READY)
		{
			info.status = EVENT_STATE_OPEN;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(gServerInfo->m_ChaosCastleEvent != 0)
	{
		strcpy_s(info.name,"Chaos Castle");
		info.status = EVENT_STATE_START;
		info.time = 0;

		if(gChaosCastle->GetState(0) == CC_STATE_BLANK)
		{
			info.status = EVENT_STATE_BLANK;
		}
		else if(gChaosCastle->GetState(0) == CC_STATE_EMPTY)
		{
			info.status = EVENT_STATE_STAND;

			info.time = gChaosCastle->m_ChaosCastleLevel[0].RemainTime;
		}
		else if(gChaosCastle->GetState(0) == CC_STATE_READY)
		{
			info.status = EVENT_STATE_OPEN;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(gServerInfo->m_IllusionTempleEvent != 0)
	{
		strcpy_s(info.name,"Illusion Temple");
		info.status = EVENT_STATE_START;
		info.time = 0;

		if(gIllusionTemple->GetState(0) == IT_STATE_BLANK)
		{
			info.status = EVENT_STATE_BLANK;
		}
		else if(gIllusionTemple->GetState(0) == IT_STATE_EMPTY)
		{
			info.status = EVENT_STATE_STAND;

			info.time = gIllusionTemple->m_IllusionTempleLevel[0].RemainTime;
		}
		else if(gIllusionTemple->GetState(0) == IT_STATE_READY)
		{
			info.status = EVENT_STATE_OPEN;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(gServerInfo->m_InvasionManagerSwitch != 0)
	{
		for(int n=0;n<MAX_INVASION;n++)
		{
			if(gInvasionManager->GetState(n) == INVASION_STATE_BLANK)
			{
				continue;
			}

			wsprintf(info.name,gInvasionManager->m_InvasionInfo[n].Name);
			info.status = EVENT_STATE_START;
			info.time = 0;

			if(gInvasionManager->GetState(n) == INVASION_STATE_EMPTY)
			{
				info.status = EVENT_STATE_STAND;

				info.time = gInvasionManager->m_InvasionInfo[n].RemainTime;
			}

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	if(gServerInfo->m_BonusManagerSwitch != 0)
	{
		for(int n=0;n < MAX_BONUS;n++)
		{
			if(gBonusManager->GetState(n) == BONUS_STATE_BLANK)
			{
				continue;
			}

			wsprintf(info.name,gBonusManager->m_BonusInfo[n].Name);
			info.status = EVENT_STATE_START;
			info.time = 0;

			if(gBonusManager->GetState(n) == BONUS_STATE_EMPTY)
			{
				info.status = EVENT_STATE_STAND;

				info.time = gBonusManager->m_BonusInfo[n].RemainTime;
			}

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	if(gServerInfo->m_DropEventSwitch != 0)
	{
		for(int n=0;n < MAX_DROP_EVENT;n++)
		{
			if(gDropEvent->GetState(n) == DROP_EVENT_STATE_BLANK)
			{
				continue;
			}

			wsprintf(info.name,gDropEvent->m_EventDropInfo[n].Name);
			info.status = EVENT_STATE_START;
			info.time = 0;

			if(gDropEvent->GetState(n) == DROP_EVENT_STATE_EMPTY)
			{
				info.status = EVENT_STATE_STAND;

				info.time = gDropEvent->m_EventDropInfo[n].RemainTime;
			}

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	if(gCustomArena->m_CustomArenaSwitch != 0)
	{
		for(int n=0;n < MAX_CUSTOM_ARENA;n++)
		{
			if(gCustomArena->GetState(n) == CUSTOM_ARENA_STATE_BLANK)
			{
				continue;
			}

			wsprintf(info.name,gCustomArena->m_CustomArenaInfo[n].RuleInfo.Name);
			info.status = EVENT_STATE_START;
			info.time = 0;

			if(gCustomArena->GetState(n) == CUSTOM_ARENA_STATE_EMPTY)
			{
				info.status = EVENT_STATE_STAND;

				info.time = gCustomArena->m_CustomArenaInfo[n].RemainTime;
			}
			else if(gCustomArena->GetState(n) == INVASION_STATE_START)
			{
				info.status = EVENT_STATE_OPEN;
			}

			memcpy(&send[size],&info,sizeof(info));
			size += sizeof(info);

			pMsg.count++;
		}
	}

	if(gCustomLukeHelper->m_CustomLukeHelperSwitch != 0)
	{
		strcpy_s(info.name,"Luke Helper");
		info.status = EVENT_STATE_START;
		info.time = 0;

		if(gCustomLukeHelper->m_State == CUSTOM_LUKE_HELPER_BLANK)
		{
			info.status = EVENT_STATE_BLANK;
		}
		else if(gCustomLukeHelper->m_State == CUSTOM_LUKE_HELPER_EMPTY)
		{
			info.status = EVENT_STATE_STAND;

			info.time = gCustomLukeHelper->m_RemainTime;
		}
		else if(gCustomLukeHelper->m_State == CUSTOM_LUKE_HELPER_START)
		{
			info.status = EVENT_STATE_START;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(gServerInfo->m_CastleDeepEvent != 0)
	{
		strcpy_s(info.name,"Castle Deep");
		info.status = EVENT_STATE_START;
		info.time = 0;

		if(gCastleDeep->m_State == CD_STATE_BLANK)
		{
			info.status = EVENT_STATE_BLANK;
		}
		else if(gCastleDeep->m_State == CD_STATE_EMPTY)
		{
			info.status = EVENT_STATE_STAND;

			info.time = gCastleDeep->m_RemainTime;
		}
		else if(gCastleDeep->m_State == CD_STATE_START)
		{
			info.status = EVENT_STATE_START;
		}

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);

		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		DataSend(aIndex,send,size);
	}
}