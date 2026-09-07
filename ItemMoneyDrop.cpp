// ItemMoneyDrop.cpp: implementation of the CItemMoneyDrop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemMoneyDrop.h"
#include "BonusManager.h"
#include "CrywolfSync.h"
#include "DSProtocol.h"
#include "Map.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "Monster.h"
#include "RandomManager.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemMoneyDrop::CItemMoneyDrop() // OK
{
	this->m_ItemMoneyDropInfo.clear();
}

CItemMoneyDrop::~CItemMoneyDrop() // OK
{

}

void CItemMoneyDrop::Load(char* path) // OK
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

	this->m_ItemMoneyDropInfo.clear();

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

			ITEM_MONEY_DROP_INFO info;

			memset(&info,0,sizeof(info));

			info.MapNumber = lpReadFile->GetNumber();

			info.MonsterClass = lpReadFile->GetAsNumber();

			info.MonsterLevelMin = lpReadFile->GetAsNumber();

			info.MonsterLevelMax = lpReadFile->GetAsNumber();

			info.AccountLevel = lpReadFile->GetAsNumber();

			info.MoneyAmount = lpReadFile->GetAsNumber();

			info.MoneyDropRate = lpReadFile->GetAsNumber();

			this->m_ItemMoneyDropInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CItemMoneyDrop::DropMoney(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(gMapManager->GetMapItemDrop(lpTarget->Map) == 0)
	{
		return 0;
	}

	CRandomManager RandomManager;

	for(std::vector<ITEM_MONEY_DROP_INFO>::iterator it=this->m_ItemMoneyDropInfo.begin();it != this->m_ItemMoneyDropInfo.end();it++)
	{
		int DropRate;

		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		if(it->MonsterClass != -1 && it->MonsterClass != lpObj->Class)
		{
			continue;
		}

		if(it->MonsterLevelMin != -1 && it->MonsterLevelMin > lpObj->Level)
		{
			continue;
		}

		if(it->MonsterLevelMax != -1 && it->MonsterLevelMax < lpObj->Level)
		{
			continue;
		}

		if(it->AccountLevel != -1 && it->AccountLevel != lpTarget->AccountLevel)
		{
			continue;
		}

		if((DropRate=it->MoneyDropRate) == -1 || (GetLargeRand()%1000000) < (DropRate=it->MoneyDropRate))
		{
			int rate = (1000000/((DropRate==-1)?1000000:DropRate));

			RandomManager.AddElement((int)(&(*it)),rate);
		}
	}

	ITEM_MONEY_DROP_INFO* lpItemDropInfo;

	if(RandomManager.GetRandomElement((int*)&lpItemDropInfo) == 0)
	{
		return 0;
	}
	else
	{
		int Amount = (lpItemDropInfo->MoneyAmount==-1)?((lpObj->Money==0)?1:lpObj->Money):lpItemDropInfo->MoneyAmount;

		Amount = (Amount*lpTarget->MoneyAmountDropRate)/100;

		gMap[lpObj->Map].MoneyItemDrop((Amount>0)?Amount:1,lpObj->X,lpObj->Y);

		return 1;
	}
}