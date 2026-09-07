// CastleSiegeCrown.cpp: implementation of the CCastleSiegeCrown class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CastleSiegeCrown.h"
#include "CastleSiege.h"
#include "Map.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleSiegeCrown::CCastleSiegeCrown() // OK
{

}

CCastleSiegeCrown::~CCastleSiegeCrown() // OK
{

}

void CCastleSiegeCrown::CastleSiegeCrownAct(int aIndex) // OK
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_NPC || lpObj->Class != 216)
	{
		return;
	}

	int bIndex = gCastleSiege->GetCrownUserIndex();

	if(gObjIsConnected(bIndex) == 0)
	{
		gCastleSiege->SetCrownAccessPosition(0,0);
		gCastleSiege->SetCrownUserIndex(-1);
		gCastleSiege->ResetCrownAccessTickCount();
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Map != MAP_CASTLE_SIEGE || lpTarget->CastleJoinSide < 2 || lpTarget->CastleInvolved == 0)
	{
		gCastleSiege->GCCastleSiegeCrownAccessStateSend(bIndex,2);
		gCastleSiege->SetCrownAccessPosition(0,0);
		gCastleSiege->SetCrownUserIndex(-1);
		gCastleSiege->ResetCrownAccessTickCount();
		return;
	}

	if(gCastleSiege->CheckCrownAccessPosition(lpTarget->X,lpTarget->Y) != 0)
	{
		gCastleSiege->GCCastleSiegeCrownAccessStateSend(bIndex,2);
		gCastleSiege->SetCrownAccessPosition(0,0);
		gCastleSiege->SetCrownUserIndex(-1);
		gCastleSiege->ResetCrownAccessTickCount();
		return;
	}

	if(gObjIsConnected(gCastleSiege->GetCrownSwitchUserIndex(217)) == 0 || gObjIsConnected(gCastleSiege->GetCrownSwitchUserIndex(218)) == 0)
	{
		gCastleSiege->GCCastleSiegeCrownAccessStateSend(bIndex,2);
		gCastleSiege->SetCrownAccessPosition(0,0);
		gCastleSiege->SetCrownUserIndex(-1);
		gCastleSiege->ResetCrownAccessTickCount();
		return;
	}

	if(lpTarget->CastleJoinSide != gObj[gCastleSiege->GetCrownSwitchUserIndex(217)].CastleJoinSide || lpTarget->CastleJoinSide != gObj[gCastleSiege->GetCrownSwitchUserIndex(218)].CastleJoinSide)
	{
		gCastleSiege->GCCastleSiegeCrownAccessStateSend(bIndex,2);
		gCastleSiege->SetCrownAccessPosition(0,0);
		gCastleSiege->SetCrownUserIndex(-1);
		gCastleSiege->ResetCrownAccessTickCount();
		return;
	}
}