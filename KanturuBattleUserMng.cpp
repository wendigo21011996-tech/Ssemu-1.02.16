// KanturuBattleUserMng.cpp: implementation of the CKanturuBattleUserMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuBattleUserMng.h"
#include "Map.h"
#include "User.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuBattleUserMng::CKanturuBattleUserMng() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->ResetAllData();
	
	#endif
}

CKanturuBattleUserMng::~CKanturuBattleUserMng() // OK
{

}

void CKanturuBattleUserMng::ResetAllData() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_KANTURU_BATTLE_USER;n++)
	{
		this->m_BattleUser[n].ResetData();
	}

	this->m_BattleUserCount = 0;

	this->m_BattleMaxUser = MAX_KANTURU_BATTLE_USER;
	
	#endif
}

bool CKanturuBattleUserMng::AddUserData(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return 0;
	}

	if(this->IsOverMaxUser() != 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_KANTURU_BATTLE_USER;n++)
	{
		if(this->m_BattleUser[n].IsUseData() == 0)
		{
			this->m_BattleUser[n].SetIndex(aIndex);
			this->m_BattleUserCount++;
			return 1;
		}
	}
	
	#endif

	return 0;
}

bool CKanturuBattleUserMng::DelUserData(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_KANTURU_BATTLE_USER;n++)
	{
		if(this->m_BattleUser[n].IsUseData() != 0 && this->m_BattleUser[n].GetIndex() == aIndex)
		{
			this->m_BattleUser[n].ResetData();
			this->m_BattleUserCount--;
			return 1;
		}
	}
	
	#endif

	return 0;
}

void CKanturuBattleUserMng::CheckUserState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_KANTURU_BATTLE_USER;n++)
	{
		if(this->m_BattleUser[n].IsUseData() == 0)
		{
			continue;
		}

		if(gObjIsConnectedGP(this->m_BattleUser[n].GetIndex()) == 0)
		{
			this->DelUserData(this->m_BattleUser[n].GetIndex());
			continue;
		}

		if(gObj[this->m_BattleUser[n].GetIndex()].Map != MAP_KANTURU3)
		{
			this->DelUserData(this->m_BattleUser[n].GetIndex());
			continue;
		}
	}

	#endif
}

void CKanturuBattleUserMng::MoveAllUser(int gate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_KANTURU_BATTLE_USER;n++)
	{
		if(this->m_BattleUser[n].IsUseData() != 0)
		{
			gObjMoveGate(this->m_BattleUser[n].GetIndex(),gate);
		}
	}

	#endif
}

int CKanturuBattleUserMng::GetUserCount() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_BattleUserCount;
	
	#else

	return 0;
	
	#endif
}

int CKanturuBattleUserMng::GetMaxUser() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_BattleMaxUser;
	
	#else

	return 0;
	
	#endif
}

int CKanturuBattleUserMng::IsEmpty() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return ((this->m_BattleUserCount==0)?1:0);
	
	#else

	return 0;
	
	#endif
}

int CKanturuBattleUserMng::IsOverMaxUser() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return ((this->m_BattleUserCount>=MAX_KANTURU_BATTLE_USER)?1:0);

	#else

	return 0;
	
	#endif
}
