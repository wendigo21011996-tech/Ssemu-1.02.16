// QuestObjective.cpp: implementation of the CQuestObjective class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestObjective.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Notice.h"
#include "Party.h"
#include "Quest.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestObjective::CQuestObjective() // OK
{
	this->m_QuestObjectiveInfo.clear();
}

CQuestObjective::~CQuestObjective() // OK
{

}

void CQuestObjective::Load(char* path) // OK
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

	this->m_QuestObjectiveInfo.clear();

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

			QUEST_OBJECTIVE_INFO info;

			info.Sort = lpReadFile->GetNumber();

			info.Type = lpReadFile->GetAsNumber();

			info.Index = lpReadFile->GetAsNumber();

			info.Quantity = lpReadFile->GetAsNumber();

			info.Level = lpReadFile->GetAsNumber();

			info.Option1 = lpReadFile->GetAsNumber();

			info.Option2 = lpReadFile->GetAsNumber();

			info.Option3 = lpReadFile->GetAsNumber();

			info.NewOption = lpReadFile->GetAsNumber();

			info.MapNumber = lpReadFile->GetAsNumber();

			info.DropMinLevel = lpReadFile->GetAsNumber();

			info.DropMaxLevel = lpReadFile->GetAsNumber();

			info.ItemDropRate = lpReadFile->GetAsNumber();

			info.RequireIndex = lpReadFile->GetAsNumber();

			info.RequireState = lpReadFile->GetAsNumber();

			for(int n=0;n < MAX_CLASS;n++){info.RequireClass[n] = lpReadFile->GetAsNumber();}

			this->m_QuestObjectiveInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CQuestObjective::GetQuestObjectiveCount(LPOBJ lpObj,QUEST_OBJECTIVE_INFO* lpInfo) // OK
{
	switch(lpInfo->Type)
	{
		case QUEST_OBJECTIVE_ITEM:
			return gItemManager->GetInventoryItemCount(lpObj,lpInfo->Index,lpInfo->Level);
		case QUEST_OBJECTIVE_MONEY:
			return lpObj->Money;
		#if(GAMESERVER_UPDATE>=301)
		case QUEST_OBJECTIVE_MONSTER:
			return this->GetQuestObjectiveKillCount(lpObj, lpInfo);
		#endif
	}

	return 0;
}

int CQuestObjective::GetQuestObjectiveKillCount(LPOBJ lpObj,QUEST_OBJECTIVE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	if(lpObj->QuestKillCountIndex != lpInfo->RequireIndex)
	{
		return 0;
	}

	if(lpObj->QuestKillCount[lpInfo->Sort].MonsterClass != lpInfo->Index)
	{
		return 0;
	}

	return lpObj->QuestKillCount[lpInfo->Sort].KillCount;

	#else

	return 0;

	#endif
}

bool CQuestObjective::CheckQuestObjectiveRequisite(LPOBJ lpObj,QUEST_OBJECTIVE_INFO* lpInfo) // OK
{
	if(lpInfo->RequireIndex != -1 && gQuest->CheckQuestListState(lpObj,lpInfo->RequireIndex,lpInfo->RequireState) == 0)
	{
		return 0;
	}

	if(lpInfo->RequireClass[lpObj->Class] == 0 || lpInfo->RequireClass[lpObj->Class] > (lpObj->ChangeUp+1))
	{
		return 0;
	}

	return 1;
}

bool CQuestObjective::CheckQuestObjective(LPOBJ lpObj,int QuestIndex) // OK
{
	for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex)
		{
			continue;
		}

		if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(this->GetQuestObjectiveCount(lpObj,&(*it)) < it->Quantity)
		{
			return 0;
		}
	}

	return 1;
}

bool CQuestObjective::CheckQuestObjectiveItemCount(LPOBJ lpObj,int ItemIndex,int ItemLevel) // OK
{
	for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_OBJECTIVE_ITEM)
		{
			continue;
		}

		if(it->Index == ItemIndex && it->Level == ItemLevel && it->Quantity <= this->GetQuestObjectiveCount(lpObj,&(*it)))
		{
			return 0;
		}
	}

	return 1;
}

void CQuestObjective::AddQuestObjectiveKillCount(LPOBJ lpObj,QUEST_OBJECTIVE_INFO* lpInfo) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	if(lpObj->QuestKillCountIndex != lpInfo->RequireIndex)
	{
		return;
	}

	if(lpObj->QuestKillCount[lpInfo->Sort].MonsterClass != lpInfo->Index)
	{
		return;
	}

	lpObj->QuestKillCount[lpInfo->Sort].KillCount++;

	#endif
}

void CQuestObjective::InitQuestObjectiveKillCount(LPOBJ lpObj,int QuestIndex) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex)
		{
			continue;
		}

		if(this->CheckQuestObjectiveRequisite(lpObj, &(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_OBJECTIVE_MONSTER)
		{
			continue;
		}

		lpObj->QuestKillCountIndex = QuestIndex;

		lpObj->QuestKillCount[it->Sort].MonsterClass = it->Index;

		lpObj->QuestKillCount[it->Sort].KillCount = 0;
	}

	#endif
}

void CQuestObjective::RemoveQuestObjective(LPOBJ lpObj,int QuestIndex) // OK
{
	for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex)
		{
			continue;
		}

		if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type == QUEST_OBJECTIVE_ITEM)
		{
			gItemManager->DeleteInventoryItemCount(lpObj, it->Index,it->Level,it->Quantity);
			continue;
		}

		if(it->Type == QUEST_OBJECTIVE_MONEY)
		{
			lpObj->Money -= it->Quantity;
			GCMoneySend(lpObj->Index,lpObj->Money);
			continue;
		}

		#if(GAMESERVER_UPDATE>=301)
		
		if(it->Type == QUEST_OBJECTIVE_MONSTER)
		{
			lpObj->QuestKillCountIndex = -1;
			lpObj->QuestKillCount[it->Sort].MonsterClass = -1;
			lpObj->QuestKillCount[it->Sort].KillCount = 0;
			continue;
		}

		#endif
	}
}

bool CQuestObjective::MonsterItemDrop(LPOBJ lpMonster) // OK
{
	int aIndex = gObjMonsterGetTopHitDamageUser(lpMonster);

	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(lpObj->PartyNumber) != 0 && gServerInfo->m_QuestMonsterItemDropParty != 0)
	{
		return this->MonsterItemDropParty(lpMonster,lpObj->PartyNumber);
	}

	for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_OBJECTIVE_ITEM)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
		{
			continue;
		}

		if(it->DropMinLevel != -1 && it->DropMinLevel > lpMonster->Level)
		{
			continue;
		}

		if(it->DropMinLevel != -1 && it->DropMaxLevel != -1 && it->DropMaxLevel < lpMonster->Level)
		{
			continue;
		}

		if(it->DropMinLevel == -1 && it->DropMaxLevel != -1 && it->DropMaxLevel != lpMonster->Class)
		{
			continue;
		}

		if(it->ItemDropRate > (GetLargeRand()%10000) && it->Quantity > this->GetQuestObjectiveCount(lpObj,&(*it)))
		{
			GDCreateItemSend(aIndex,lpMonster->Map,(BYTE)lpMonster->X,(BYTE)lpMonster->Y,it->Index,it->Level,it->Option1,it->Option2,it->Option3,0,aIndex,it->NewOption,0,0,0,0,0xFF,0);
			return 1;
		}
	}

	return 0;
}

bool CQuestObjective::MonsterItemDropParty(LPOBJ lpMonster,int PartyNumber) // OK
{
	for(int n = 0; n < MAX_PARTY_USER; n++)
	{
		int aIndex = gParty->m_PartyInfo[PartyNumber].Index[n];

		if(OBJECT_RANGE(aIndex) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[aIndex];

		for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
		{
			if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
			{
				continue;
			}

			if(it->Type != QUEST_OBJECTIVE_ITEM)
			{
				continue;
			}

			if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
			{
				continue;
			}

			if(it->DropMinLevel != -1 && it->DropMinLevel > lpMonster->Level)
			{
				continue;
			}

			if(it->DropMinLevel != -1 && it->DropMaxLevel != -1 && it->DropMaxLevel < lpMonster->Level)
			{
				continue;
			}

			if(it->DropMinLevel == -1 && it->DropMaxLevel != -1 && it->DropMaxLevel != lpMonster->Class)
			{
				continue;
			}

			if(it->ItemDropRate > (GetLargeRand()%10000) && it->Quantity > this->GetQuestObjectiveCount(lpObj,&(*it)))
			{
				GDCreateItemSend(aIndex,lpMonster->Map,(BYTE)lpMonster->X,(BYTE)lpMonster->Y,it->Index,it->Level,it->Option1,it->Option2,it->Option3,0,aIndex,it->NewOption,0,0,0,0,0xFF,0);
				return 1;
			}
		}
	}

	return 0;
}

void CQuestObjective::MonsterKill(LPOBJ lpMonster) // OK
{
	int aIndex = gObjMonsterGetTopHitDamageUser(lpMonster);

	if(OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(lpObj->PartyNumber) != 0 && gServerInfo->m_QuestMonsterKillParty != 0)
	{
		return this->MonsterKillParty(lpMonster,lpObj->PartyNumber);
	}

	for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
	{
		if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type != QUEST_OBJECTIVE_MONSTER)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
		{
			continue;
		}

		#if(GAMESERVER_UPDATE>=301)
		
		if(it->Index == lpMonster->Class && it->Quantity > this->GetQuestObjectiveCount(lpObj,&(*it)))
		{
			this->AddQuestObjectiveKillCount(lpObj,&(*it));
			gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,"[Quest] %d/%d of %s",this->GetQuestObjectiveCount(lpObj,&(*it)),it->Quantity,lpMonster->MonsterName);
			return;
		}

		#endif
	}
}

void CQuestObjective::MonsterKillParty(LPOBJ lpMonster,int PartyNumber) // OK
{
	for(int n = 0; n < MAX_PARTY_USER; n++)
	{
		int aIndex = gParty->m_PartyInfo[PartyNumber].Index[n];

		if(OBJECT_RANGE(aIndex) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[aIndex];

		for (std::vector<QUEST_OBJECTIVE_INFO>::iterator it = this->m_QuestObjectiveInfo.begin(); it != this->m_QuestObjectiveInfo.end(); it++)
		{
			if(this->CheckQuestObjectiveRequisite(lpObj,&(*it)) == 0)
			{
				continue;
			}

			if(it->Type != QUEST_OBJECTIVE_MONSTER)
			{
				continue;
			}

			if(it->MapNumber != -1 && it->MapNumber != lpMonster->Map)
			{
				continue;
			}

			#if(GAMESERVER_UPDATE>=301)

			if(it->Index == lpMonster->Class && it->Quantity > this->GetQuestObjectiveCount(lpObj,&(*it)))
			{
				this->AddQuestObjectiveKillCount(lpObj,&(*it));
				gNotice->GCNoticeSend(aIndex,1,0,0,0,0,0,"[Quest] %d/%d of %s",this->GetQuestObjectiveCount(lpObj,&(*it)),it->Quantity,lpMonster->MonsterName);
				return;
			}

			#endif
		}
	}
}