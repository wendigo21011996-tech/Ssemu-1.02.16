// CastleSiegeCrownSwitch.cpp: implementation of the CCastleSiegeCrownSwitch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CastleSiegeCrownSwitch.h"
#include "CastleSiege.h"
#include "Map.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleSiegeCrownSwitch::CCastleSiegeCrownSwitch() // OK
{

}

CCastleSiegeCrownSwitch::~CCastleSiegeCrownSwitch() // OK
{

}

void CCastleSiegeCrownSwitch::CastleSiegeCrownSwitchAct(int aIndex) // OK
{
	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_NPC || (lpObj->Class != 217 && lpObj->Class != 218))
	{
		return;
	}

	int bIndex = gCastleSiege->GetCrownSwitchUserIndex(lpObj->Class);

	if(gObjIsConnected(bIndex) == 0)
	{
		if(gCastleSiege->GetRegCrownAvailable() != 0)
		{
			gCastleSiege->SetRegCrownAvailable(0);
			gCastleSiege->GCCastleSiegeCrownStateSend(1);
		}

		gCastleSiege->ResetCrownSwitchUserIndex(lpObj->Class);
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	gCastleSiege->GCCastleSiegeSwitchInfoSend(aIndex);

	if(lpTarget->Map != MAP_CASTLE_SIEGE || lpTarget->CastleJoinSide < 2)
	{
		gCastleSiege->GCCastleSiegeSwitchStateSend(bIndex,aIndex,-1,0);

		if(gCastleSiege->GetRegCrownAvailable() != 0)
		{
			gCastleSiege->SetRegCrownAvailable(0);
			gCastleSiege->GCCastleSiegeCrownStateSend(1);
		}

		gCastleSiege->ResetCrownSwitchUserIndex(lpObj->Class);
		return;
	}

	if(lpObj->X < (lpTarget->X-3) || lpObj->X > (lpTarget->X+3) || lpObj->Y < (lpTarget->Y-3) || lpObj->Y > (lpTarget->Y+3))
	{
		gCastleSiege->GCCastleSiegeSwitchStateSend(bIndex,aIndex,-1,0);

		if(gCastleSiege->GetRegCrownAvailable() != 0)
		{
			gCastleSiege->SetRegCrownAvailable(0);
			gCastleSiege->GCCastleSiegeCrownStateSend(1);
		}

		gCastleSiege->ResetCrownSwitchUserIndex(lpObj->Class);
		return;
	}

	if(gObjIsConnected(gCastleSiege->GetCrownSwitchUserIndex(217)) != 0 && gObjIsConnected(gCastleSiege->GetCrownSwitchUserIndex(218)) != 0)
	{
		if(gObj[gCastleSiege->GetCrownSwitchUserIndex(217)].CastleJoinSide == gObj[gCastleSiege->GetCrownSwitchUserIndex(218)].CastleJoinSide)
		{
			if(gCastleSiege->GetRegCrownAvailable() == 0)
			{
				gCastleSiege->SetRegCrownAvailable(1);
				gCastleSiege->GCCastleSiegeCrownStateSend(0);
			}
		}
	}
}