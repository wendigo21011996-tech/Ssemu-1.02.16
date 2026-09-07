// QuestReward.cpp: implementation of the CQuestReward class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestReward.h"
#include "EffectManager.h"
#include "DSProtocol.h"
#include "ReadFile.h"
#include "ObjectManager.h"
#include "Quest.h"
#include "ServerInfo.h"
#include "SkillManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestReward::CQuestReward() // OK
{
	this->m_QuestRewardInfo.clear();
}

CQuestReward::~CQuestReward() // OK
{

}

void CQuestReward::Load(char* path) // OK
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

	this->m_QuestRewardInfo.clear();

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

			QUEST_REWARD_INFO info;

			info.Sort = lpReadFile->GetNumber();

			info.Type = lpReadFile->GetAsNumber();

			info.Index = lpReadFile->GetAsNumber();

			info.Quantity = lpReadFile->GetAsNumber();

			info.Level = lpReadFile->GetAsNumber();

			info.Option1 = lpReadFile->GetAsNumber();

			info.Option2 = lpReadFile->GetAsNumber();

			info.Option3 = lpReadFile->GetAsNumber();

			info.NewOption = lpReadFile->GetAsNumber();

			info.RequireIndex = lpReadFile->GetAsNumber();

			info.RequireState = lpReadFile->GetAsNumber();

			for(int n=0;n < MAX_CLASS;n++){info.RequireClass[n] = lpReadFile->GetAsNumber();}

			this->m_QuestRewardInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CQuestReward::CheckQuestRewardRequisite(LPOBJ lpObj,QUEST_REWARD_INFO* lpInfo) // OK
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

void CQuestReward::InsertQuestReward(LPOBJ lpObj,int QuestIndex) // OK
{
	for (std::vector<QUEST_REWARD_INFO>::iterator it = this->m_QuestRewardInfo.begin(); it != this->m_QuestRewardInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex)
		{
			continue;
		}

		if(this->CheckQuestRewardRequisite(lpObj,&(*it)) == 0)
		{
			continue;
		}

		if(it->Type == QUEST_REWARD_POINT)
		{
			lpObj->LevelUpPoint += it->Quantity;
			gQuest->GCQuestRewardSend(lpObj->Index,it->Index,it->Quantity);
			continue;
		}

		if(it->Type == QUEST_REWARD_CHANGE1)
		{
			if((lpObj->DBClass%16) < 1)
			{
				lpObj->DBClass = ((lpObj->DBClass/16)*16)+1;
				lpObj->ChangeUp = lpObj->DBClass%16;
			}

			gObjectManager->CharacterMakePreviewCharSet(lpObj->Index);

			BYTE Class = (lpObj->ChangeUp*16);
			Class -= (Class/32);
			Class += (lpObj->Class*32);

			gQuest->GCQuestRewardSend(lpObj->Index,it->Index,Class);
			continue;
		}

		if(it->Type == QUEST_REWARD_EXPERIENCE)
		{
			if(gEffectManager->CheckEffect(lpObj,EFFECT_SEAL_OF_SUSTENANCE1) != 0 || gEffectManager->CheckEffect(lpObj,EFFECT_SEAL_OF_SUSTENANCE2) != 0)
			{
				continue;
			}

			if(gObjectManager->CharacterLevelUp(lpObj,(it->Quantity*gServerInfo->m_AddQuestExperienceRate[lpObj->AccountLevel]),gServerInfo->m_MaxLevelUpQuest,EXPERIENCE_QUEST) != 0)
			{
				continue;
			}

			GCRewardExperienceSend(lpObj->Index,(it->Quantity*gServerInfo->m_AddQuestExperienceRate[lpObj->AccountLevel]));
			continue;
		}

		if(it->Type == QUEST_REWARD_HERO)
		{
			if(lpObj->Class == CLASS_FE)
			{
				gSkillManager->GCSkillAddSend(lpObj->Index,gSkillManager->AddSkill(lpObj,SKILL_INFINITY_ARROW,0),SKILL_INFINITY_ARROW,0,0);
			}

			int AddPoint = ((lpObj->Level>gServerInfo->m_PlusStatMinLevel)?(lpObj->Level- gServerInfo->m_PlusStatMinLevel):0)*gServerInfo->m_PlusStatPoint;

			lpObj->LevelUpPoint += AddPoint;

			gQuest->GCQuestRewardSend(lpObj->Index,it->Index,AddPoint);
			continue;
		}

		if(it->Type == QUEST_REWARD_MONEY)
		{
			if(gObjCheckMaxMoney(lpObj->Index,it->Quantity) == 0)
			{
				lpObj->Money = MAX_MONEY;
			}
			else
			{
				lpObj->Money += it->Quantity;
			}

			GCMoneySend(lpObj->Index,lpObj->Money);
			continue;
		}

		if(it->Type == QUEST_REWARD_ITEM)
		{
			for(int n=0;n < it->Quantity;n++)
			{
				GDCreateItemSend(lpObj->Index,0xEB,0,0,it->Index,it->Level,0,it->Option1,it->Option2,it->Option3,-1,it->NewOption,0,0,0,0,0xFF,0);
			}
			continue;
		}

		if(it->Type == QUEST_REWARD_EFFECT)
		{
			gEffectManager->AddEffect(lpObj,1,it->Index,it->Quantity,0,0,0,0);
			continue;
		}

		if(it->Type == QUEST_REWARD_COMBO)
		{
			gQuest->GCQuestRewardSend(lpObj->Index,it->Index,it->Quantity);
			continue;
		}

		if(it->Type == QUEST_REWARD_CHANGE2)
		{
			if((lpObj->DBClass%16) < 2)
			{
				lpObj->DBClass = ((lpObj->DBClass/16)*16)+2;
				lpObj->ChangeUp = lpObj->DBClass%16;
			}

			gObjectManager->CharacterMakePreviewCharSet(lpObj->Index);

			BYTE Class = (lpObj->ChangeUp*16);
			Class -= (Class/32);
			Class += (lpObj->Class*32);

			gQuest->GCQuestRewardSend(lpObj->Index,it->Index,Class);
			continue;
		}
	}
}

long CQuestReward::GetQuestRewardPoint(LPOBJ lpObj,int QuestIndex) // OK
{
	long point = 0;

	for (std::vector<QUEST_REWARD_INFO>::iterator it = this->m_QuestRewardInfo.begin(); it != this->m_QuestRewardInfo.end(); it++)
	{
		if(it->RequireIndex != QuestIndex)
		{
			continue;
		}

		if(it->Type == QUEST_REWARD_POINT)
		{
			point += it->Quantity;
			continue;
		}
	}

	return point;
}
