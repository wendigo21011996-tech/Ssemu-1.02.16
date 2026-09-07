// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomMonster.h"
#include "DSProtocol.h"
#include "ItemBagManager.h"
#include "ReadFile.h"
#include "Message.h"
#include "Monster.h"
#include "Notice.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomMonster::CCustomMonster() // OK
{
	this->m_CustomMonsterInfo.clear();
}

CCustomMonster::~CCustomMonster() // OK
{

}

void CCustomMonster::Load(char* path) // OK
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

	this->m_CustomMonsterInfo.clear();

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

			CUSTOM_MONSTER_INFO info;

			memset(&info,0,sizeof(info));

			info.MonsterClass = lpReadFile->GetNumber();

			info.MapNumber = lpReadFile->GetAsNumber();

			info.MaxLife = lpReadFile->GetAsNumber();

			info.DamageMin = lpReadFile->GetAsNumber();

			info.DamageMax = lpReadFile->GetAsNumber();

			info.Defense = lpReadFile->GetAsNumber();

			info.AttackRate = lpReadFile->GetAsNumber();

			info.DefenseRate = lpReadFile->GetAsNumber();

			info.ExperienceRate = lpReadFile->GetAsNumber();

			info.KillMessage = lpReadFile->GetAsNumber();

			info.BagSpecial = lpReadFile->GetAsNumber();

			#if(GAMESERVER_SHOP>=1)

			info.RewardMessage = lpReadFile->GetAsNumber();

			info.RewardCoin1 = lpReadFile->GetAsNumber();

			#endif

			#if(GAMESERVER_SHOP==3)

			info.RewardCoin2 = lpReadFile->GetAsNumber();

			info.RewardCoin3 = lpReadFile->GetAsNumber();

			#endif

			this->m_CustomMonsterInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CCustomMonster::SetCustomMonsterInfo(LPOBJ lpObj) // OK
{
	for(std::vector<CUSTOM_MONSTER_INFO>::iterator it= this->m_CustomMonsterInfo.begin();it != this->m_CustomMonsterInfo.end();it++)
	{
		if(it->MonsterClass != -1 && it->MonsterClass != lpObj->Class)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		lpObj->Life = (float)((__int64)lpObj->Life*((it->MaxLife==-1)?100:it->MaxLife))/100;

		lpObj->MaxLife = (float)((__int64)lpObj->MaxLife*((it->MaxLife==-1)?100:it->MaxLife))/100;

		lpObj->PhysiDamageMin = ((__int64)lpObj->PhysiDamageMin*((it->DamageMin==-1)?100:it->DamageMin))/100;

		lpObj->PhysiDamageMax = ((__int64)lpObj->PhysiDamageMax*((it->DamageMax==-1)?100:it->DamageMax))/100;

		lpObj->Defense = ((__int64)lpObj->Defense*((it->Defense==-1)?100:it->Defense))/100;

		lpObj->AttackSuccessRate = ((__int64)lpObj->AttackSuccessRate*((it->AttackRate==-1)?100:it->AttackRate))/100;

		lpObj->DefenseSuccessRate = ((__int64)lpObj->DefenseSuccessRate*((it->DefenseRate==-1)?100:it->DefenseRate))/100;

		if(it->BagSpecial != -1)
		{
			lpObj->CustomMonsterDrop = true;
		}

		break;
	}
}

void CCustomMonster::MonsterDieProc(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	for(std::vector<CUSTOM_MONSTER_INFO>::iterator it= this->m_CustomMonsterInfo.begin();it != this->m_CustomMonsterInfo.end();it++)
	{
		if(it->MonsterClass != -1 && it->MonsterClass != lpObj->Class)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		int aIndex = gObjMonsterGetTopHitDamageUser(lpObj);

		if(OBJECT_RANGE(aIndex) != 0)
		{
			lpTarget = &gObj[aIndex];
		}

		if(it->KillMessage != -1)
		{
			gNotice->GCNoticeSendToAll(0,0,0,0,0,0,gMessage->GetMessage(it->KillMessage),lpTarget->Name);
		}

		if(it->BagSpecial != -1)
		{
			gItemBagManager->DropItemBySpecialValue(it->BagSpecial,-1,-1,lpTarget,lpObj->Map,lpObj->X,lpObj->Y);
		}

		#if(GAMESERVER_SHOP>=1)

		if(it->RewardMessage != -1)
		{
			gNotice->GCNoticeSend(lpTarget->Index,0,0,0,0,0,0,gMessage->GetMessage(it->RewardMessage),it->RewardCoin1,it->RewardCoin2,it->RewardCoin3);
		}

		if(it->RewardCoin1 > 0 || it->RewardCoin2 > 0 || it->RewardCoin3 > 0)
		{
			gObjCoinAdd(lpTarget->Index,it->RewardCoin1,it->RewardCoin2,it->RewardCoin3);
		}

		#endif

		break;
	}
}

int CCustomMonster::GetCustomMonsterExperienceRate(int index,int map) // OK
{
	for(std::vector<CUSTOM_MONSTER_INFO>::iterator it= this->m_CustomMonsterInfo.begin();it != this->m_CustomMonsterInfo.end();it++)
	{
		if(it->MonsterClass != -1 && it->MonsterClass != index)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != map)
		{
			continue;
		}

		return ((it->ExperienceRate==-1)?100:it->ExperienceRate);
	}

	return 100;
}