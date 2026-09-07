// MonsterManager.cpp: implementation of the CMonsterManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterManager.h"
#include "380ItemType.h"
#include "BloodCastle.h"
#include "BonusManager.h"
#include "CastleDeep.h"
#include "CastleSiege.h"
#include "ChaosCastle.h"
#include "Crywolf.h"
#include "CustomArena.h"
#include "CustomEventDrop.h"
#include "CustomLukeHelper.h"
#include "DevilSquare.h"
#include "DropEvent.h"
#include "IllusionTemple.h"
#include "InvasionManager.h"
#include "ItemManager.h"
#include "Map.h"
#include "ReadFile.h"
#include "Monster.h"
#include "MonsterSetBase.h"
#include "ObjectManager.h"
#include "ServerInfo.h"
#include "User.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterManager::CMonsterManager() // OK
{
	this->Init();
}

CMonsterManager::~CMonsterManager() // OK
{

}

void CMonsterManager::Init() // OK
{
	for(int n=0;n < MAX_MONSTER_INFO;n++)
	{
		this->m_MonsterInfo[n].Index = -1;
	}

	for(int n=0;n < MAX_MONSTER_LEVEL;n++)
	{
		memset(this->m_MonsterItemInfo[n].IndexTable,-1,sizeof(this->m_MonsterItemInfo[n].IndexTable));

		this->m_MonsterItemInfo[n].IndexCount = 0;
	}
}

void CMonsterManager::Load(char* path) // OK
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

			MONSTER_INFO info;

			memset(&info,0,sizeof(info));

			info.Index = lpReadFile->GetNumber();

			info.Type = lpReadFile->GetAsNumber();

			strcpy_s(info.Name,lpReadFile->GetAsString());

			info.Level = lpReadFile->GetAsNumber();

			info.Life = lpReadFile->GetAsNumber();

			info.Mana = lpReadFile->GetAsNumber();

			info.DamageMin = lpReadFile->GetAsNumber();

			info.DamageMax = lpReadFile->GetAsNumber();

			info.Defense = lpReadFile->GetAsNumber();

			info.MagicDefense = lpReadFile->GetAsNumber();

			info.AttackRate = lpReadFile->GetAsNumber();

			info.DefenseRate = lpReadFile->GetAsNumber();

			info.MoveRange = lpReadFile->GetAsNumber();

			info.AttackType = lpReadFile->GetAsNumber();

			info.AttackRange = lpReadFile->GetAsNumber();

			info.ViewRange = lpReadFile->GetAsNumber();

			info.MoveSpeed = lpReadFile->GetAsNumber();

			info.AttackSpeed = lpReadFile->GetAsNumber();

			info.RegenTime = lpReadFile->GetAsNumber();

			info.Attribute = lpReadFile->GetAsNumber();

			info.ItemRate = lpReadFile->GetAsNumber();

			info.MoneyRate = lpReadFile->GetAsNumber();

			info.MaxItemLevel = lpReadFile->GetAsNumber();

			info.MonsterSkill = lpReadFile->GetAsNumber();

			for(int n=0;n < MAX_RESISTANCE_TYPE;n++){info.Resistance[n] = lpReadFile->GetAsNumber();}

			#if(GAMESERVER_UPDATE>=701)

			info.ElementalAttribute = lpReadFile->GetAsNumber();

			info.ElementalPattern = lpReadFile->GetAsNumber();

			info.ElementalDefense = lpReadFile->GetAsNumber();

			info.ElementalDamageMin = lpReadFile->GetAsNumber();

			info.ElementalDamageMax = lpReadFile->GetAsNumber();

			info.ElementalAttackRate = lpReadFile->GetAsNumber();

			info.ElementalDefenseRate = lpReadFile->GetAsNumber();

			#endif

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	this->InitMonsterItem();

	delete lpReadFile;
}

void CMonsterManager::SetInfo(MONSTER_INFO info) // OK
{
	if(info.Index < 0 || info.Index >= MAX_MONSTER_INFO)
	{
		return;
	}

	info.Life = GET_MAX_VALUE(((__int64)info.Life*gServerInfo->m_MonsterMaxLifeRate)/100);
	info.Defense = GET_MAX_VALUE(((__int64)info.Defense*gServerInfo->m_MonsterDefenseRate)/100);
	info.DefenseRate = GET_MAX_VALUE(((__int64)info.DefenseRate*gServerInfo->m_MonsterDefenseSuccessRateRate)/100);
	info.DamageMin = GET_MAX_VALUE(((__int64)info.DamageMin*gServerInfo->m_MonsterPhysiDamageRate)/100);
	info.DamageMax = GET_MAX_VALUE(((__int64)info.DamageMax*gServerInfo->m_MonsterPhysiDamageRate)/100);
	info.AttackRate = GET_MAX_VALUE(((__int64)info.AttackRate*gServerInfo->m_MonsterAttackSuccessRateRate)/100);

	#if(GAMESERVER_UPDATE>=701)

	info.ElementalDefense = GET_MAX_VALUE(((__int64)info.ElementalDefense*gServerInfo->m_MonsterDefenseRate)/100);
	info.ElementalDamageMin = GET_MAX_VALUE(((__int64)info.ElementalDamageMin*gServerInfo->m_MonsterPhysiDamageRate)/100);
	info.ElementalDamageMax = GET_MAX_VALUE(((__int64)info.ElementalDamageMax*gServerInfo->m_MonsterPhysiDamageRate)/100);
	info.ElementalAttackRate = GET_MAX_VALUE(((__int64)info.ElementalAttackRate*gServerInfo->m_MonsterAttackSuccessRateRate)/100);
	info.ElementalDefenseRate = GET_MAX_VALUE(((__int64)info.ElementalDefenseRate*gServerInfo->m_MonsterDefenseSuccessRateRate)/100);

	#endif

	info.ScriptLife = info.Life;

	this->m_MonsterInfo[info.Index] = info;
}

MONSTER_INFO* CMonsterManager::GetInfo(int index) // OK
{
	if(index < 0 || index >= MAX_MONSTER_INFO)
	{
		return 0;
	}

	if(this->m_MonsterInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_MonsterInfo[index];
}

char* CMonsterManager::GetName(int index) // OK
{
	MONSTER_INFO* lpInfo = this->GetInfo(index);

	if(lpInfo == 0)
	{
		return 0;
	}

	return lpInfo->Name;
}

void CMonsterManager::InitMonsterItem() // OK
{
	for(int n=0;n < MAX_MONSTER_LEVEL;n++)
	{
		this->InsertMonsterItem(n);
	}
}

void CMonsterManager::InsertMonsterItem(int level) // OK
{
	for(int n=0;n < MAX_ITEM;n++)
	{
		ITEM_INFO ItemInfo;

		if(gItemManager->GetInfo(n,&ItemInfo) == 0)
		{
			continue;
		}

		if(ItemInfo.DropItem == 0)
		{
			continue;
		}

		if(ItemInfo.Level < 0)
		{
			continue;
		}

		if((ItemInfo.Level+4) >= level && (ItemInfo.Level-2) <= level)
		{
			MONSTER_ITEM_INFO* lpItem = &this->m_MonsterItemInfo[level];

			if(lpItem->IndexCount >= 0 && lpItem->IndexCount < MAX_MONSTER_ITEM)
			{
				lpItem->IndexTable[lpItem->IndexCount++] = n;
			}
		}
	}
}

long CMonsterManager::GetMonsterItem(int level,int excellent,int socket) // OK
{
	if(level < 0 || level >= MAX_MONSTER_LEVEL)
	{
		return -1;
	}

	if(this->m_MonsterItemInfo[level].IndexCount == 0)
	{
		return -1;
	}

	int IndexTable[MAX_MONSTER_ITEM];
	int IndexCount = 0;

	for(int n=0;n < this->m_MonsterItemInfo[level].IndexCount;n++)
	{
		if(excellent != 0 && this->m_MonsterItemInfo[level].IndexTable[n] >= GET_ITEM(12,0))
		{
			continue;
		}

		if(excellent != 0 && g380ItemType->Check380ItemType(this->m_MonsterItemInfo[level].IndexTable[n]) != 0)
		{
			continue;
		}

		IndexTable[IndexCount++] = this->m_MonsterItemInfo[level].IndexTable[n];
	}

	return ((IndexCount==0)?-1:IndexTable[GetLargeRand()%IndexCount]);
}

void CMonsterManager::SetMonsterData() // OK
{
	for(int n=0;n < gMonsterSetBase->m_count;n++)
	{
		MONSTER_SET_BASE_INFO* lpInfo = &gMonsterSetBase->m_MonsterSetBaseInfo[n];

		if(lpInfo->Type == 3 || lpInfo->Type == 4)
		{
			continue;
		}

		int index = gObjAddMonster(lpInfo->Map);

		if(OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		if(gObjSetPosMonster(index,n) == 0)
		{
			gObjDel(index);
			continue;
		}

		if(gObjSetMonster(index,lpInfo->MonsterClass) == 0)
		{
			gObjDel(index);
			continue;
		}

		LPOBJ lpObj = &gObj[index];

		if(lpObj->Map == MAP_CASTLE_SIEGE)
		{
			if(lpObj->Class == 216)
			{
				gCastleSiege->SetCrownIndex(index);
			}
		}

		#if(GAMESERVER_UPDATE>=201)

		if(lpObj->Map == MAP_CRYWOLF)
		{
			if(lpObj->Type == OBJECT_MONSTER)
			{
				if(gCrywolf->m_ObjCommonMonster.AddObj(index) == 0)
				{
					gObjDel(index);
					continue;
				}
			}

			if(lpObj->Type == OBJECT_NPC)
			{
				if(lpObj->Class >= 204 && lpObj->Class <= 209)
				{
					if(gCrywolf->m_ObjSpecialNPC.AddObj(index) == 0)
					{
						gObjDel(index);
						continue;
					}
				}
				else
				{
					if(lpObj->Class != 406 && lpObj->Class != 407)
					{
						if(gCrywolf->m_ObjCommonNPC.AddObj(index) == 0)
						{
							gObjDel(index);
							continue;
						}
					}
				}
			}
		}

		#endif
	}

	gBloodCastle->Init();

	gBonusManager->Init();

	gCastleDeep->Init();

	gChaosCastle->Init();

	gCustomArena->Init();

	gCustomEventDrop->Init();

	gCustomLukeHelper->Init();

	gDevilSquare->Init();

	gDropEvent->Init();

	gIllusionTemple->Init();

	gInvasionManager->Init();
}