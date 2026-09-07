// CastleSiegeSync.cpp: implementation of the CCastleSiegeSync class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CastleSiegeSync.h"
#include "CastleSiege.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "GuildManager.h"
#include "MapServerManager.h"
#include "User.h"
#include "UnionManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleSiegeSync::CCastleSiegeSync() // OK
{
	this->Clear();
}

CCastleSiegeSync::~CCastleSiegeSync() // OK
{

}

void CCastleSiegeSync::MainProc() // OK
{
	if(this->m_TributeMoney <= 0)
	{
		return;
	}

	if((GetTickCount()-this->m_TributeSaveTime) >= 60000)
	{
		this->m_TributeSaveTime = GetTickCount();

		gCastleSiege->GDCastleSiegeAddTributeMoneySend(this->m_TributeMoney);

		this->m_TributeMoney = 0;
	}
}

void CCastleSiegeSync::Clear() // OK
{
	this->m_CurCastleState = -1;
	
	this->m_CurTaxRateChaos = 0;
	
	this->m_CurTaxRateStore = 0;

	memset(this->m_CastleOwnerGuild,0,sizeof(this->m_CastleOwnerGuild));

	this->m_TributeMoney = 0;

	this->m_TributeSaveTime = GetTickCount();
}

int CCastleSiegeSync::GetState() // OK
{
	return this->m_CurCastleState;
}

int CCastleSiegeSync::GetTaxRateChaos(int aIndex) // OK
{
	int CurTaxRateChaos = this->m_CurTaxRateChaos;

	if(this->CheckCastleOwnerMember(aIndex) != 0 || this->CheckCastleOwnerUnionMember(aIndex) != 0)
	{
		CurTaxRateChaos = 0;
	}

	return CurTaxRateChaos;
}

int CCastleSiegeSync::GetTaxRateStore(int aIndex) // OK
{
	int CurTaxRateStore = this->m_CurTaxRateStore;

	if(this->CheckCastleOwnerMember(aIndex) != 0 || this->CheckCastleOwnerUnionMember(aIndex) != 0)
	{
		CurTaxRateStore = 0;
	}

	return CurTaxRateStore;
}

int CCastleSiegeSync::GetTaxHuntZone(int aIndex,bool CheckOwnerGuild) // OK
{
	int CurTaxHuntZone = this->m_CurTaxHuntZone;

	if(CheckOwnerGuild != 0)
	{
		if(this->CheckCastleOwnerMember(aIndex) != 0 || this->CheckCastleOwnerUnionMember(aIndex) != 0)
		{
			CurTaxHuntZone = 0;
		}
	}

	return CurTaxHuntZone;
}

char* CCastleSiegeSync::GetCastleOwnerGuild() // OK
{
	return this->m_CastleOwnerGuild;
}

bool CCastleSiegeSync::CheckCastleOwnerMember(int aIndex) // OK
{
	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return false;
	}

	if(strcmp(this->m_CastleOwnerGuild,"") == 0)
	{
		return false;
	}

	if(strcmp(gObj[aIndex].GuildName,this->m_CastleOwnerGuild) != 0)
	{
		return false;
	}

	return true;
}

bool CCastleSiegeSync::CheckCastleOwnerUnionMember(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return false;
	}

	if(strcmp(this->m_CastleOwnerGuild,"") == 0)
	{
		return false;
	}
	
	if(lpObj->Guild == 0)
	{
		return false;
	}
	
	UNION_INFO* lpUnion = gUnionManager->GetUnion(lpObj->Guild->GuildUnion);

	if(lpUnion == 0)
	{
		return false;
	}

	if(strcmp(lpUnion->Name,this->m_CastleOwnerGuild) != 0)
	{
		return false;
	}

	return true;
}

void CCastleSiegeSync::AddTributeMoney(int money) // OK
{
	if(money <= 0 || money > MAX_MONEY)
	{
		return;
	}

	this->m_TributeMoney += money;
}

void CCastleSiegeSync::SetCastleState(int state) // OK
{
	this->m_CurCastleState = state;
}

void CCastleSiegeSync::SetTaxRateChaos(int rate) // OK
{
	this->m_CurTaxRateChaos = rate;
}

void CCastleSiegeSync::SetTaxRateStore(int rate) // OK
{
	this->m_CurTaxRateStore = rate;
}

void CCastleSiegeSync::SetTaxHuntZone(int rate) // OK
{
	this->m_CurTaxHuntZone = rate;
}

void CCastleSiegeSync::SetCastleOwnerGuild(char* GuildName) // OK
{
	memset(this->m_CastleOwnerGuild,0,sizeof(this->m_CastleOwnerGuild));

	memcpy(this->m_CastleOwnerGuild,GuildName,sizeof(this->m_CastleOwnerGuild));
}