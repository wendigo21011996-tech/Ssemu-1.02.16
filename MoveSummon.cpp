// MoveSummon.cpp: implementation of the CMoveSummon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MoveSummon.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "DefaultClassInfo.h"
#include "Gate.h"
#include "ItemManager.h"
#include "Map.h"
#include "ReadFile.h"
#include "ServerInfo.h"
#include "Util.h"

CMoveSummon ;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoveSummon::CMoveSummon() // OK
{
	this->m_MoveSummonInfo.clear();
}

CMoveSummon::~CMoveSummon() // OK
{

}

void CMoveSummon::Load(char* path) // OK
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

	this->m_MoveSummonInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			MOVE_SUMMON_INFO info;

			info.Map = lpReadFile->GetNumber();

			info.X = lpReadFile->GetAsNumber();

			info.Y = lpReadFile->GetAsNumber();

			info.TX = lpReadFile->GetAsNumber();

			info.TY = lpReadFile->GetAsNumber();

			info.MinLevel = lpReadFile->GetAsNumber();

			info.MaxLevel = lpReadFile->GetAsNumber();

			info.MinReset = lpReadFile->GetAsNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.Enable[0] = lpReadFile->GetAsNumber();

			info.Enable[1] = lpReadFile->GetAsNumber();

			info.Enable[2] = lpReadFile->GetAsNumber();

			info.Enable[3] = lpReadFile->GetAsNumber();

			this->m_MoveSummonInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CMoveSummon::CheckMoveSummon(LPOBJ lpObj,int map,int x,int y) // OK
{
	if(lpObj->Permission[12] != 0)
	{
		return 0;
	}

	if(lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		return 0;
	}

	if(gServerInfo->m_PKLimitFree == 0 && lpObj->PKLevel > 4 && gServerInfo->m_PKLimitMoveSummon != 0)
	{
		return 0;
	}

	if(map == MAP_CASTLE_SIEGE && gCastleSiegeSync->GetState() != CS_STATE_START && (x > 160 && x < 192 && y > 187 && y < 217))
	{
		return 0;
	}

	if(map == MAP_ATLANS && (lpObj->Inventory[INVENTORY_SLOT_HELPER].IsItem() != 0 && (lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,2) || lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,3)))) // Uniria,Dinorant
	{
		return 0;
	}

	#if(GAMESERVER_UPDATE>=201)

	if((map == MAP_ICARUS || map == MAP_KANTURU3) && (lpObj->Inventory[INVENTORY_SLOT_WING].IsItem() == 0 && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,3) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,37))) // Dinorant,Fenrir
	{
		return 0;
	}

	#else

	if(map == MAP_ICARUS && lpObj->Inventory[INVENTORY_SLOT_WING].IsItem() == 0 && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,3) && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index != GET_ITEM(13,37)) // Dinorant,Fenrir
	{
		return 0;
	}

	#endif

	if(lpObj->Attack.Offline != 0 || lpObj->PShopCustomOffline != 0 || lpObj->Helper.Offline != 0)
	{
		return 0;
	}

	for(int n=0;n < MAX_SELF_DEFENSE;n++)
	{
		if(lpObj->SelfDefense[n] >= 0)
		{
			if(GetTickCount() < lpObj->SelfDefenseTime[n])
			{
				return 0;
			}
		}
	}

	for(std::vector<MOVE_SUMMON_INFO>::iterator it=this->m_MoveSummonInfo.begin();it != this->m_MoveSummonInfo.end();it++)
	{
		if(it->Map != map)
		{
			continue;
		}

		if((it->X > x || it->TX < x) || (it->Y > y || it->TY < y))
		{
			continue;
		}

		if(it->MinLevel != -1 && lpObj->Level < gGate->GetMoveLevel(lpObj,it->Map,it->MinLevel))
		{
			return 0;
		}

		if(it->MaxLevel != -1 && lpObj->Level > it->MaxLevel)
		{
			return 0;
		}

		if(it->MinReset != -1 && lpObj->Reset < it->MinReset)
		{
			return 0;
		}

		if(it->MaxReset != -1 && lpObj->Reset > it->MaxReset)
		{
			return 0;
		}

		if(it->Enable[lpObj->AccountLevel] == 0)
		{
			return 0;
		}
		
		return 1;
	}

	return 0;
}
