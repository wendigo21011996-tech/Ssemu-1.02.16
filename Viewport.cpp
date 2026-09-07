// Viewport.cpp: implementation of the CViewport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Viewport.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "CustomArena.h"
#include "Duel.h"
#include "EffectManager.h"
#include "GameMaster.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Map.h"
#include "MapManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewport::CViewport() // OK
{

}

CViewport::~CViewport() // OK
{

}

bool CViewport::CheckViewportObjectPosition(int aIndex,int map,int x,int y,int view) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != map)
	{
		return 0;
	}

	if(lpObj->X < (x-view) || lpObj->X > (x+view) || lpObj->Y < (y-view) || lpObj->Y > (y+view))
	{
		return 0;
	}

	return 1;
}

bool CViewport::CheckViewportObject1(int aIndex,int bIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_NONE && lpObj->VpPlayer[n].index == bIndex && lpObj->VpPlayer[n].type == type)
		{
			return 0;
		}
	}

	return 1;
}

bool CViewport::CheckViewportObject2(int aIndex,int bIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].index == bIndex && lpObj->VpPlayer2[n].type == type)
		{
			return 0;
		}
	}

	return 1;
}

bool CViewport::CheckViewportObjectItem(int aIndex,int bIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayerItem[n].state != VIEWPORT_NONE && lpObj->VpPlayerItem[n].index == bIndex && lpObj->VpPlayerItem[n].type == type)
		{
			return 0;
		}
	}

	return 1;
}

bool CViewport::AddViewportObject1(int aIndex,int bIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckViewportObject1(aIndex,bIndex,type) == 0 || (type == OBJECT_MONSTER && gObj[bIndex].Class == 523) || gObj[bIndex].Class == 603)
	{
		return 0;
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state == VIEWPORT_NONE)
		{
			this->AddViewportObjectAgro(aIndex,bIndex,type);
			lpObj->VpPlayer[n].state = VIEWPORT_SEND;
			lpObj->VpPlayer[n].index = bIndex;
			lpObj->VpPlayer[n].type = type;
			lpObj->VPCount++;
			return 1;
		}
	}

	return 0;
}

bool CViewport::AddViewportObject2(int aIndex,int bIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckViewportObject2(aIndex,bIndex,type) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			lpObj->VpPlayer2[n].state = VIEWPORT_SEND;
			lpObj->VpPlayer2[n].index = bIndex;
			lpObj->VpPlayer2[n].type = type;
			lpObj->VPCount2++;
			return 1;
		}
	}

	return 0;
}

bool CViewport::AddViewportObjectItem(int aIndex,int bIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckViewportObjectItem(aIndex,bIndex,type) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayerItem[n].state == VIEWPORT_NONE)
		{
			lpObj->VpPlayerItem[n].state = VIEWPORT_SEND;
			lpObj->VpPlayerItem[n].index = bIndex;
			lpObj->VpPlayerItem[n].type = type;
			lpObj->VPCountItem++;
			return 1;
		}
	}

	return 0;
}

void CViewport::AddViewportObjectAgro(int aIndex,int bIndex,int type) // OK
{
	if(gObj[aIndex].Type == OBJECT_MONSTER && gObj[aIndex].CurrentAI != 0)
	{
		if(type == OBJECT_USER)
		{
			gObj[aIndex].Agro.SetAgro(bIndex,1000);
		}
	}
}

void CViewport::DelViewportObjectAgro(int aIndex,int bIndex,int type) // OK
{
	if(gObj[aIndex].Type == OBJECT_MONSTER && gObj[aIndex].CurrentAI != 0)
	{
		if(type == OBJECT_USER)
		{
			gObj[aIndex].Agro.DelAgro(bIndex);
		}
	}
}

void CViewport::DestroyViewportPlayer1(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND && lpObj->VpPlayer[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_USER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(lpTarget->State == OBJECT_CREATE && lpTarget->Live == 0)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			continue;
		}

		if(lpTarget->Connected != OBJECT_ONLINE || lpTarget->Teleport != 0)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			continue;
		}

		if(lpTarget->State == OBJECT_DIECMD || lpTarget->State == OBJECT_DELCMD)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,lpTarget->Map,lpTarget->X,lpTarget->Y,MapViewRange) == 0)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			continue;
		}
	}
}

void CViewport::DestroyViewportPlayer2(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_SEND && lpObj->VpPlayer2[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer2[n].index];

		if(lpTarget->Connected != OBJECT_ONLINE)
		{
			lpObj->VpPlayer2[n].state = VIEWPORT_NONE;
			lpObj->VpPlayer2[n].index = -1;
			lpObj->VPCount2--;
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,lpTarget->Map,lpTarget->X,lpTarget->Y,MapViewRange) == 0)
		{
			lpObj->VpPlayer2[n].state = VIEWPORT_NONE;
			lpObj->VpPlayer2[n].index = -1;
			lpObj->VPCount2--;
			continue;
		}
	}
}

void CViewport::DestroyViewportMonster1(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND && lpObj->VpPlayer[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_MONSTER && lpObj->VpPlayer[n].type != OBJECT_NPC)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(lpTarget->State == OBJECT_CREATE && lpTarget->Live == 0)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			this->DelViewportObjectAgro(aIndex,lpTarget->Index,lpTarget->Type);
			continue;
		}

		if(lpTarget->Connected != OBJECT_ONLINE || lpTarget->Teleport != 0)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			this->DelViewportObjectAgro(aIndex,lpTarget->Index,lpTarget->Type);
			continue;
		}

		if(lpTarget->State == OBJECT_DIECMD || lpTarget->State == OBJECT_DELCMD)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			this->DelViewportObjectAgro(aIndex,lpTarget->Index,lpTarget->Type);
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,lpTarget->Map,lpTarget->X,lpTarget->Y,MapViewRange) == 0)
		{
			lpObj->VpPlayer[n].state = VIEWPORT_DESTROY;
			this->DelViewportObjectAgro(aIndex,lpTarget->Index,lpTarget->Type);
			continue;
		}
	}
}

void CViewport::DestroyViewportMonster2(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_SEND && lpObj->VpPlayer2[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer2[n].type != OBJECT_MONSTER && lpObj->VpPlayer2[n].type != OBJECT_NPC)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer2[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer2[n].index];

		if(lpTarget->Connected != OBJECT_ONLINE)
		{
			lpObj->VpPlayer2[n].state = VIEWPORT_NONE;
			lpObj->VpPlayer2[n].index = -1;
			lpObj->VPCount2--;
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,lpTarget->Map,lpTarget->X,lpTarget->Y,MapViewRange) == 0)
		{
			lpObj->VpPlayer2[n].state = VIEWPORT_NONE;
			lpObj->VpPlayer2[n].index = -1;
			lpObj->VPCount2--;
			continue;
		}
	}
}

void CViewport::DestroyViewportItem(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayerItem[n].state != VIEWPORT_SEND && lpObj->VpPlayerItem[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayerItem[n].type != OBJECT_ITEM)
		{
			continue;
		}

		if(MAP_ITEM_RANGE(lpObj->VpPlayerItem[n].index) == 0)
		{
			continue;
		}

		CMapItem* lpMapItem = &gMap[lpObj->Map].m_Item[lpObj->VpPlayerItem[n].index];

		if(lpMapItem->IsItem() == 0)
		{
			lpObj->VpPlayerItem[n].state = VIEWPORT_DESTROY;
			continue;
		}

		if(lpMapItem->m_State == OBJECT_DIECMD)
		{
			lpObj->VpPlayerItem[n].state = VIEWPORT_DESTROY;
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,lpObj->Map,lpMapItem->m_X,lpMapItem->m_Y,MapViewRange) == 0)
		{
			lpObj->VpPlayerItem[n].state = VIEWPORT_DESTROY;
			continue;
		}
	}
}

void CViewport::DestroyViewportGuild(int aIndex) // OK
{
	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	PMSG_GUILD_VIEWPORT_DELETE_SEND pMsg;

	pMsg.header.set(0x5D,sizeof(pMsg));

	pMsg.index[0] = SET_NUMBERHB(aIndex);

	pMsg.index[1] = SET_NUMBERLB(aIndex);

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	MsgSendV2(&gObj[aIndex],(BYTE*)&pMsg,pMsg.header.size);
}

void CViewport::CreateViewportPlayer(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER)
	{
		return;
	}

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=OBJECT_START_MONSTER;n < MAX_OBJECT;n++)
	{
		if(gObj[n].Connected != OBJECT_ONLINE || n == aIndex || gObj[n].Map != lpObj->Map)
		{
			continue;
		}

		if(gObj[n].State != OBJECT_CREATE && gObj[n].State != OBJECT_PLAYING)
		{
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,gObj[n].Map,gObj[n].X,gObj[n].Y,MapViewRange) != 0)
		{
			this->AddViewportObject1(aIndex,n,gObj[n].Type);
			this->AddViewportObject2(n,aIndex,gObj[aIndex].Type);
		}
	}
}

void CViewport::CreateViewportMonster(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_MONSTER && lpObj->Type != OBJECT_NPC)
	{
		return;
	}

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=MAX_OBJECT_MONSTER;n < MAX_OBJECT;n++)
	{
		if(gObj[n].Connected != OBJECT_ONLINE || n == aIndex || gObj[n].Map != lpObj->Map)
		{
			continue;
		}

		if(gObj[n].State != OBJECT_CREATE && gObj[n].State != OBJECT_PLAYING)
		{
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,gObj[n].Map,gObj[n].X,gObj[n].Y,MapViewRange) != 0)
		{
			this->AddViewportObject1(aIndex,n,gObj[n].Type);
			this->AddViewportObject2(n,aIndex,gObj[aIndex].Type);
		}
	}
}

void CViewport::CreateViewportItem(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER)
	{
		return;
	}

	int MapViewRange = gMapManager->GetMapViewRange(lpObj->Map);

	for(int n=0;n < MAX_MAP_ITEM;n++)
	{
		if(gMap[lpObj->Map].m_Item[n].m_Live == 0)
		{
			continue;
		}

		if(gMap[lpObj->Map].m_Item[n].m_State != OBJECT_CREATE && gMap[lpObj->Map].m_Item[n].m_State != OBJECT_PLAYING)
		{
			continue;
		}

		if(this->CheckViewportObjectPosition(aIndex,lpObj->Map,gMap[lpObj->Map].m_Item[n].m_X,gMap[lpObj->Map].m_Item[n].m_Y,MapViewRange) != 0)
		{
			this->AddViewportObjectItem(aIndex,n,OBJECT_ITEM);
		}
	}
}

void CViewport::GCViewportDestroySend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[256];

	PMSG_VIEWPORT_DESTROY_SEND pMsg;

	pMsg.header.set(0x14,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_DESTROY info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_DESTROY)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_USER && lpObj->VpPlayer[n].type != OBJECT_MONSTER && lpObj->VpPlayer[n].type != OBJECT_NPC)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpObj->VpPlayer[n].index);
		info.index[1] = SET_NUMBERLB(lpObj->VpPlayer[n].index);

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size = size;

		memcpy(send,&pMsg,sizeof(pMsg));

		DataSend(aIndex,send,size);
	}
}

void CViewport::GCViewportDestroyItemSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[1024];

	PMSG_VIEWPORT_DESTROY_ITEM_SEND pMsg;

	pMsg.header.set(0x21,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_DESTROY info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayerItem[n].state != VIEWPORT_DESTROY)
		{
			continue;
		}

		if(lpObj->VpPlayerItem[n].type != OBJECT_ITEM)
		{
			continue;
		}

		if(MAP_ITEM_RANGE(lpObj->VpPlayerItem[n].index) == 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpObj->VpPlayerItem[n].index);
		info.index[1] = SET_NUMBERLB(lpObj->VpPlayerItem[n].index);

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

void CViewport::GCViewportPlayerSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[8192];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x12,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_PLAYER info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_USER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(gObjIsChangeSkin(lpTarget->Index) != 0)
		{
			continue;
		}

		if(lpTarget->Authority == AUTHORITY_INVISIBLE)
		{
			continue;
		}

		if(gEffectManager->CheckEffect(lpTarget,EFFECT_INVISIBILITY) != 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);
		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		if(lpTarget->State == OBJECT_CREATE && lpTarget->Teleport == 0)
		{
			info.index[0] |= 0x80;
		}

		info.x = (BYTE)lpTarget->X;
		info.y = (BYTE)lpTarget->Y;

		lpTarget->CharSet[0] &= 0xF8;
		lpTarget->CharSet[0] |= lpTarget->ViewState & 7;

		memcpy(info.CharSet,lpTarget->CharSet,sizeof(info.CharSet));

		info.ViewSkillState = lpTarget->ViewSkillState;

		memcpy(info.name,lpTarget->Name,sizeof(info.name));

		info.tx = (BYTE)lpTarget->TX;
		info.ty = (BYTE)lpTarget->TY;

		info.DirAndPkLevel = (lpTarget->Dir*16) | (((gCustomArena->CheckPlayerJoined(lpTarget,lpObj)==0)?lpTarget->PKLevel:6) & 0x0F);

		int InfoSize = sizeof(info);

		memcpy(&send[size],&info,sizeof(info));
		size += InfoSize;

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

void CViewport::GCViewportMonsterSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[8192];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x13,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_MONSTER info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_MONSTER && lpObj->VpPlayer[n].type != OBJECT_NPC)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(OBJECT_RANGE(lpTarget->SummonIndex) != 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);
		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		if(lpTarget->State == OBJECT_CREATE)
		{
			info.index[0] |= 0x80;

			if(lpTarget->Teleport != 0)
			{
				info.index[0] |= 0x40;
			}
		}

		info.type[0] = SET_NUMBERHB(lpTarget->Class);
		info.type[1] = SET_NUMBERLB(lpTarget->Class);

		if(lpTarget->Class == 278)
		{
			if(gCastleSiegeSync->GetState() == CS_STATE_START && lpObj->CastleJoinSide == lpTarget->CastleJoinSide)
			{
				info.type[0] |= 0x80;
			}

			if(lpObj->Guild != 0 && lpTarget->Guild != 0 && lpObj->Guild == lpTarget->Guild)
			{
				info.type[0] |= 0x80;
			}

			info.type[0] |= ((lpTarget->CreationState << 4) & 0x70);
		}

		info.ViewSkillState = lpTarget->ViewSkillState;

		info.x = (BYTE)lpTarget->X;
		info.y = (BYTE)lpTarget->Y;

		info.tx = (BYTE)lpTarget->TX;
		info.ty = (BYTE)lpTarget->TY;

		info.DirAndPkLevel = (lpTarget->Dir*16) | (lpTarget->PKLevel & 0x0F);

		if(lpTarget->Class == 216)
		{
			if(gCastleSiege->GetRegCrownAvailable() == 0)
			{
				gEffectManager->DelEffect(lpTarget,EFFECT_CASTLE_CROWN_STATE);
			}
			else
			{
				gEffectManager->AddEffect(lpTarget,0,EFFECT_CASTLE_CROWN_STATE,0,0,0,0,0);
			}
		}

		if(lpTarget->Class == 277)
		{
			if(lpTarget->CastleGateState == 0)
			{
				gEffectManager->DelEffect(lpTarget,EFFECT_CASTLE_GATE_STATE);
			}
			else
			{
				gEffectManager->AddEffect(lpTarget,0,EFFECT_CASTLE_GATE_STATE,0,0,0,0,0);
			}
		}

		int InfoSize = sizeof(info);

		memcpy(&send[size],&info,sizeof(info));
		size += InfoSize;

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

void CViewport::GCViewportSummonSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[8192];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x1F,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_SUMMON info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_MONSTER && lpObj->VpPlayer[n].type != OBJECT_NPC)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(OBJECT_RANGE(lpTarget->SummonIndex) == 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);
		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		if(lpTarget->State == OBJECT_CREATE)
		{
			info.index[0] |= 0x80;
		}

		info.type[0] = SET_NUMBERHB(lpTarget->Class);
		info.type[1] = SET_NUMBERLB(lpTarget->Class);

		info.ViewSkillState = lpTarget->ViewSkillState;

		info.x = (BYTE)lpTarget->X;
		info.y = (BYTE)lpTarget->Y;

		info.tx = (BYTE)lpTarget->TX;
		info.ty = (BYTE)lpTarget->TY;

		info.DirAndPkLevel = (lpTarget->Dir*16) | (lpTarget->PKLevel & 0x0F);

		memcpy(info.name,gObj[lpTarget->SummonIndex].Name,sizeof(info.name));

		int InfoSize = sizeof(info);

		memcpy(&send[size],&info,sizeof(info));
		size += InfoSize;

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

void CViewport::GCViewportItemSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x20,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_ITEM info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayerItem[n].state != VIEWPORT_SEND)
		{
			continue;
		}

		if(lpObj->VpPlayerItem[n].type != OBJECT_ITEM)
		{
			continue;
		}

		if(MAP_ITEM_RANGE(lpObj->VpPlayerItem[n].index) == 0)
		{
			continue;
		}

		CMapItem* lpMapItem = &gMap[lpObj->Map].m_Item[lpObj->VpPlayerItem[n].index];

		info.index[0] = SET_NUMBERHB(lpObj->VpPlayerItem[n].index);
		info.index[1] = SET_NUMBERLB(lpObj->VpPlayerItem[n].index);

		if(lpMapItem->m_State == OBJECT_CREATE)
		{
			info.index[0] |= 0x80;
		}

		info.x = lpMapItem->m_X;
		info.y = lpMapItem->m_Y;

		if(lpMapItem->m_Index == GET_ITEM(14,15)) // Money
		{
			info.ItemInfo[0] = lpMapItem->m_Index & 0xFF;
			info.ItemInfo[1] = SET_NUMBERLB(SET_NUMBERHW(lpMapItem->m_BuyMoney));
			info.ItemInfo[2] = SET_NUMBERHB(SET_NUMBERLW(lpMapItem->m_BuyMoney));
			info.ItemInfo[3] = (lpMapItem->m_Index & 256) >> 1;
			info.ItemInfo[4] = SET_NUMBERLB(SET_NUMBERLW(lpMapItem->m_BuyMoney));
			info.ItemInfo[5] = (lpMapItem->m_Index & GET_ITEM(15,0)) >> 5;
			info.ItemInfo[6] = SET_NUMBERHB(SET_NUMBERHW(lpMapItem->m_BuyMoney));
		}
		else
		{
			gItemManager->ItemByteConvert(info.ItemInfo,(*lpMapItem));
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

void CViewport::GCViewportChangeSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[8192];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x45,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_CHANGE info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_USER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(gObjIsChangeSkin(lpTarget->Index) == 0)
		{
			continue;
		}

		if(lpTarget->Authority == AUTHORITY_INVISIBLE)
		{
			continue;
		}

		if(gEffectManager->CheckEffect(lpTarget,EFFECT_INVISIBILITY) != 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);
		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		if(lpTarget->State == OBJECT_CREATE && lpTarget->Teleport == 0)
		{
			info.index[0] |= 0x80;
		}

		info.x = (BYTE)lpTarget->X;
		info.y = (BYTE)lpTarget->Y;

		info.skin[0] = SET_NUMBERHB(lpTarget->Change);
		info.skin[1] = SET_NUMBERLB(lpTarget->Change);

		info.ViewSkillState = lpTarget->ViewSkillState;

		memcpy(info.name,lpTarget->Name,sizeof(info.name));

		info.tx = (BYTE)lpTarget->TX;
		info.ty = (BYTE)lpTarget->TY;

		info.DirAndPkLevel = (lpTarget->Dir*16) | (((gCustomArena->CheckPlayerJoined(lpTarget,lpObj)==0)?lpTarget->PKLevel:6) & 0x0F);

		lpTarget->CharSet[0] &= 0xF8;
		lpTarget->CharSet[0] |= lpTarget->ViewState & 7;

		memcpy(info.CharSet,lpTarget->CharSet,sizeof(info.CharSet));

		int InfoSize = sizeof(info);

		memcpy(&send[size],&info,sizeof(info));
		size += InfoSize;

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

void CViewport::GCViewportGuildSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x65,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_GUILD info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_SEND)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_USER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(lpTarget->Guild == 0)
		{
			continue;
		}

		if(lpTarget->Authority == AUTHORITY_INVISIBLE)
		{
			continue;
		}

		if(gEffectManager->CheckEffect(lpTarget,EFFECT_INVISIBILITY) != 0)
		{
			continue;
		}

		info.number = lpTarget->Guild->Index;

		info.status = lpTarget->GuildRank;

		info.type = lpTarget->Guild->Type;

		if(lpObj->Guild != 0)
		{
			info.relationship = gGuild->GetUnionType(lpObj,lpTarget);
		}
		else
		{
			info.relationship = 0;
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);
		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		if(lpTarget->GuildRank == GUILD_STATUS_MASTER)
		{
			info.index[0] |= 0x80;
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

void CViewport::GCViewportUnionSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x67,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_UNION info;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer[n].state != VIEWPORT_WAIT)
		{
			continue;
		}

		if(lpObj->VpPlayer[n].type != OBJECT_USER)
		{
			continue;
		}

		if(OBJECT_RANGE(lpObj->VpPlayer[n].index) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[lpObj->VpPlayer[n].index];

		if(lpTarget->Guild == 0)
		{
			continue;
		}

		if(lpTarget->Authority == AUTHORITY_INVISIBLE)
		{
			continue;
		}

		if(gEffectManager->CheckEffect(lpTarget,EFFECT_INVISIBILITY) != 0)
		{
			continue;
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);

		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		info.number = lpTarget->Guild->Index;

		info.relationship = gGuild->GetUnionType(lpObj,lpTarget);

		gGuild->GetUnionName(lpTarget,info.UnionName);

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

void CViewport::GCViewportCastleSiegeWeaponSend(int aIndex,int tx,int ty) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[8192];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x68,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_CASTLE_SIEGE_WEAPON info;

	for(int n=0;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[n];

		if(lpTarget->Map != lpObj->Map)
		{
			continue;
		}

		if(lpTarget->X < (tx-6) || lpTarget->X > (tx+6) || lpTarget->Y < (ty-6) || lpTarget->Y > (ty+6))
		{
			continue;
		}

		info.type = (BYTE)lpTarget->Type;

		if(lpTarget->Type == OBJECT_USER)
		{
			info.skin[0] = (lpTarget->Class << 5) & 0xE0;
			info.skin[1] = 0;
		}
		else
		{
			info.skin[0] = SET_NUMBERHB(lpTarget->Class);
			info.skin[1] = SET_NUMBERLB(lpTarget->Class);
		}

		info.index[0] = SET_NUMBERHB(lpTarget->Index);
		info.index[1] = SET_NUMBERLB(lpTarget->Index);

		info.x = (BYTE)lpTarget->X;
		info.y = (BYTE)lpTarget->Y;

		if(lpTarget->Type == OBJECT_USER)
		{
			memcpy(info.CharSet,&lpTarget->CharSet[1],sizeof(info.CharSet));
		}
		else
		{
			memset(info.CharSet,0,sizeof(info.CharSet));
		}

		int InfoSize = sizeof(info);

		info.ViewSkillState = lpTarget->ViewSkillState;

		memcpy(&send[size],&info,sizeof(info));
		size += InfoSize;

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

void CViewport::GCViewportGensSystemSend(int aIndex) // OK
{

}

void CViewport::GCViewportSimpleDestroySend(LPOBJ lpObj) // OK
{
	BYTE send[256];

	PMSG_VIEWPORT_DESTROY_SEND pMsg;

	pMsg.header.set(0x14,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_DESTROY info;

	info.index[0] = SET_NUMBERHB(lpObj->Index);
	info.index[1] = SET_NUMBERLB(lpObj->Index);

	memcpy(&send[size],&info,sizeof(info));
	size += sizeof(info);

	pMsg.count++;

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	MsgSendV2(lpObj,send,size);
}

void CViewport::GCViewportSimplePlayerSend(LPOBJ lpObj) // OK
{
	if(gObjIsChangeSkin(lpObj->Index) != 0)
	{
		return;
	}

	if(lpObj->Authority == AUTHORITY_INVISIBLE)
	{
		return;
	}

	if(gEffectManager->CheckEffect(lpObj,EFFECT_INVISIBILITY) != 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x12,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_PLAYER info;

	info.index[0] = SET_NUMBERHB(lpObj->Index);
	info.index[1] = SET_NUMBERLB(lpObj->Index);
	
	if(lpObj->State == OBJECT_CREATE && lpObj->Teleport == 0)
	{
		info.index[0] |= 0x80;
	}

	info.x = (BYTE)lpObj->X;
	info.y = (BYTE)lpObj->Y;

	lpObj->CharSet[0] &= 0xF8;
	lpObj->CharSet[0] |= lpObj->ViewState & 7;

	memcpy(info.CharSet,lpObj->CharSet,sizeof(info.CharSet));

	info.ViewSkillState = lpObj->ViewSkillState;

	memcpy(info.name,lpObj->Name,sizeof(info.name));

	info.tx = (BYTE)lpObj->TX;
	info.ty = (BYTE)lpObj->TY;

	info.DirAndPkLevel = (lpObj->Dir*16) | (((gCustomArena->CheckPlayerJoined(lpObj,lpObj)==0)?lpObj->PKLevel:6) & 0x0F);

	int InfoSize = sizeof(info);

	memcpy(&send[size],&info,sizeof(info));
	size += InfoSize;

	pMsg.count++;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);
	MsgSendV2(lpObj,send,size);
}

void CViewport::GCViewportSimpleMonsterSend(LPOBJ lpObj) // OK
{
	if(OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x13,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_MONSTER info;

	info.index[0] = SET_NUMBERHB(lpObj->Index);
	info.index[1] = SET_NUMBERLB(lpObj->Index);

	if(lpObj->State == OBJECT_CREATE)
	{
		info.index[0] |= 0x80;

		if(lpObj->Teleport != 0)
		{
			info.index[0] |= 0x40;
		}
	}

	info.type[0] = SET_NUMBERHB(lpObj->Class);
	info.type[1] = SET_NUMBERLB(lpObj->Class);

	info.ViewSkillState = lpObj->ViewSkillState;

	info.x = (BYTE)lpObj->X;
	info.y = (BYTE)lpObj->Y;

	info.tx = (BYTE)lpObj->TX;
	info.ty = (BYTE)lpObj->TY;

	info.DirAndPkLevel = (lpObj->Dir*16) | (lpObj->PKLevel & 0x0F);

	int InfoSize = sizeof(info);

	memcpy(&send[size],&info,sizeof(info));
	size += InfoSize;

	pMsg.count++;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	MsgSendV2(lpObj,send,size);
}

void CViewport::GCViewportSimpleSummonSend(LPOBJ lpObj) // OK
{
	if(OBJECT_RANGE(lpObj->SummonIndex) == 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x1F,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_SUMMON info;

	info.index[0] = SET_NUMBERHB(lpObj->Index);
	info.index[1] = SET_NUMBERLB(lpObj->Index);

	if(lpObj->State == OBJECT_CREATE)
	{
		info.index[0] |= 0x80;
	}

	info.type[0] = SET_NUMBERHB(lpObj->Class);
	info.type[1] = SET_NUMBERLB(lpObj->Class);

	info.ViewSkillState = lpObj->ViewSkillState;

	info.x = (BYTE)lpObj->X;
	info.y = (BYTE)lpObj->Y;

	info.tx = (BYTE)lpObj->TX;
	info.ty = (BYTE)lpObj->TY;

	info.DirAndPkLevel = (lpObj->Dir*16) | (lpObj->PKLevel & 0x0F);

	memcpy(info.name,gObj[lpObj->SummonIndex].Name,sizeof(info.name));

	int InfoSize = sizeof(info);

	memcpy(&send[size],&info,sizeof(info));
	size += InfoSize;

	pMsg.count++;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	MsgSendV2(lpObj,send,size);
}

void CViewport::GCViewportSimpleChangeSend(LPOBJ lpObj) // OK
{
	if(gObjIsChangeSkin(lpObj->Index) == 0)
	{
		return;
	}

	if(lpObj->Authority == AUTHORITY_INVISIBLE)
	{
		return;
	}

	if(gEffectManager->CheckEffect(lpObj,EFFECT_INVISIBILITY) != 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x45,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_CHANGE info;

	info.index[0] = SET_NUMBERHB(lpObj->Index) | 0x80;
	info.index[1] = SET_NUMBERLB(lpObj->Index);

	info.x = (BYTE)lpObj->X;
	info.y = (BYTE)lpObj->Y;

	info.skin[0] = SET_NUMBERHB(lpObj->Change);
	info.skin[1] = SET_NUMBERLB(lpObj->Change);

	info.ViewSkillState = lpObj->ViewSkillState;

	memcpy(info.name,lpObj->Name,sizeof(info.name));

	info.tx = (BYTE)lpObj->TX;
	info.ty = (BYTE)lpObj->TY;

	info.DirAndPkLevel = (lpObj->Dir*16) | (((gCustomArena->CheckPlayerJoined(lpObj,lpObj)==0)?lpObj->PKLevel:6) & 0x0F);

	lpObj->CharSet[0] &= 0xF8;
	lpObj->CharSet[0] |= lpObj->ViewState & 7;

	memcpy(info.CharSet,lpObj->CharSet,sizeof(info.CharSet));

	int InfoSize = sizeof(info);

	memcpy(&send[size],&info,sizeof(info));
	size += InfoSize;

	pMsg.count++;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);
	MsgSendV2(lpObj,send,size);
}

void CViewport::GCViewportSimpleGuildSend(LPOBJ lpObj) // OK
{
	if(lpObj->Guild == 0)
	{
		return;
	}

	if(lpObj->Authority == AUTHORITY_INVISIBLE)
	{
		return;
	}

	if(gEffectManager->CheckEffect(lpObj,EFFECT_INVISIBILITY) != 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x65,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_GUILD info;

	info.number = lpObj->Guild->Index;

	info.status = lpObj->GuildRank;

	info.type = lpObj->Guild->Type;

	info.relationship = 0;

	info.index[0] = SET_NUMBERHB(lpObj->Index);
	info.index[1] = SET_NUMBERLB(lpObj->Index);

	if(lpObj->GuildRank == GUILD_STATUS_MASTER)
	{
		info.index[0] |= 0x80;
	}

	memcpy(&send[size],&info,sizeof(info));
	size += sizeof(info);

	pMsg.count++;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);
	MsgSendV2(lpObj,send,size);
}

void CViewport::GCViewportSimpleUnionSend(LPOBJ lpObj) // OK
{
	if(lpObj->Guild == 0)
	{
		return;
	}

	if(lpObj->Authority == AUTHORITY_INVISIBLE)
	{
		return;
	}

	if(gEffectManager->CheckEffect(lpObj,EFFECT_INVISIBILITY) != 0)
	{
		return;
	}

	BYTE send[256];

	PMSG_VIEWPORT_SEND pMsg;

	pMsg.header.set(0x67,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_VIEWPORT_UNION info;

	info.index[0] = SET_NUMBERHB(lpObj->Index);

	info.index[1] = SET_NUMBERLB(lpObj->Index);

	info.number = lpObj->Guild->Index;

	info.relationship = 0;

	gGuild->GetUnionName(lpObj,info.UnionName);

	memcpy(&send[size],&info,sizeof(info));
	size += sizeof(info);

	pMsg.count++;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			if(gObj[lpObj->VpPlayer2[n].index].Guild == 0)
			{
				info.relationship = 0;

				memcpy(&send[sizeof(pMsg)],&info,sizeof(info));

				DataSend(lpObj->VpPlayer2[n].index,send,size);
			}
			else
			{
				info.relationship = gGuild->GetUnionType(&gObj[lpObj->VpPlayer2[n].index],lpObj);

				memcpy(&send[sizeof(pMsg)],&info,sizeof(info));

				DataSend(lpObj->VpPlayer2[n].index,send,size);
			}
		}
	}
}

void CViewport::GCViewportSimpleGensSystemSend(LPOBJ lpObj) // OK
{

}