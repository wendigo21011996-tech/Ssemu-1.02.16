// ItemDrop.cpp: implementation of the CItemDrop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemDrop.h"
#include "BonusManager.h"
#include "CrywolfSync.h"
#include "Console.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "ItemOptionRate.h"
#include "Log.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "Monster.h"
#include "RandomManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDrop::CItemDrop() // OK
{
	this->m_ItemDropInfo.clear();
}

CItemDrop::~CItemDrop() // OK
{

}

void CItemDrop::Load(char* path) // OK
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

	this->m_ItemDropInfo.clear();

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

			ITEM_DROP_INFO info;

			memset(&info,0,sizeof(info));

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.Level = lpReadFile->GetAsNumber();

			info.Durability = lpReadFile->GetAsNumber();

			info.Option0 = lpReadFile->GetAsNumber();

			info.Option1 = lpReadFile->GetAsNumber();

			info.Option2 = lpReadFile->GetAsNumber();

			info.Option3 = lpReadFile->GetAsNumber();

			info.Option4 = lpReadFile->GetAsNumber();

			info.Option5 = lpReadFile->GetAsNumber();

			#if(GAMESERVER_UPDATE>=401)

			info.Option6 = lpReadFile->GetAsNumber();

			#endif

			info.Duration = lpReadFile->GetAsNumber();

			info.MapNumber = lpReadFile->GetAsNumber();

			info.MonsterClass = lpReadFile->GetAsNumber();

			info.MonsterLevelMin = lpReadFile->GetAsNumber();

			info.MonsterLevelMax = lpReadFile->GetAsNumber();

			info.AccountLevel = lpReadFile->GetAsNumber();

			info.DropRate = lpReadFile->GetAsNumber();

			this->m_ItemDropInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	Console(2,"[ItemDrop] %d Items Loaded",this->m_ItemDropInfo.size());
}

int CItemDrop::DropItem(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(gMapManager->GetMapItemDrop(lpTarget->Map) == 0)
	{
		return 0;
	}

	CRandomManager RandomManager;

	for(std::vector<ITEM_DROP_INFO>::iterator it=this->m_ItemDropInfo.begin();it != this->m_ItemDropInfo.end();it++)
	{
		int DropRate;

		ITEM_INFO ItemInfo;

		if(gItemManager->GetInfo(it->ItemIndex,&ItemInfo) == 0)
		{
			continue;
		}

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

		if((DropRate=it->DropRate) == -1 || (GetLargeRand()%1000000) < (DropRate=this->GetItemDropRate(lpObj,lpTarget,it->ItemIndex,it->DropRate)))
		{
			int rate = (1000000/((DropRate==-1)?1000000:DropRate));

			RandomManager.AddElement((int)(&(*it)),rate);
		}
	}

	ITEM_DROP_INFO* lpItemDropInfo;

	if(RandomManager.GetRandomElement((int*)&lpItemDropInfo) == 0)
	{
		return 0;
	}
	else
	{
		WORD ItemIndex = lpItemDropInfo->ItemIndex;
		BYTE ItemLevel = lpItemDropInfo->Level;
		BYTE ItemDur = lpItemDropInfo->Durability;
		BYTE ItemOption1 = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;
		BYTE ItemSetOption = 0;
		BYTE ItemSocketOption[MAX_SOCKET_OPTION] = {0xFF,0xFF,0xFF,0xFF,0xFF};

		gItemOptionRate->GetItemOption0(lpItemDropInfo->Option0,&ItemLevel);

		gItemOptionRate->GetItemOption1(lpItemDropInfo->Option1,&ItemOption1);

		gItemOptionRate->GetItemOption2(lpItemDropInfo->Option2,&ItemOption2);

		gItemOptionRate->GetItemOption3(lpItemDropInfo->Option3,&ItemOption3);

		gItemOptionRate->GetItemOption4(lpItemDropInfo->Option4,&ItemNewOption);

		gItemOptionRate->GetItemOption5(lpItemDropInfo->Option5,&ItemSetOption);

		#if(GAMESERVER_UPDATE>=401)

		gItemOptionRate->GetItemOption6(lpItemDropInfo->Option6,&ItemSocketOption[0]);

		#endif

		gItemOptionRate->MakeNewOption(ItemIndex,1,ItemNewOption,&ItemNewOption);

		gItemOptionRate->MakeSetOption(ItemIndex,ItemSetOption,&ItemSetOption);

		gItemOptionRate->MakeSocketOption(ItemIndex,ItemSocketOption[0],&ItemSocketOption[0]);

		GDCreateItemSend(lpTarget->Index,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,ItemIndex,ItemLevel,ItemDur,ItemOption1,ItemOption2,ItemOption3,lpTarget->Index,ItemNewOption,ItemSetOption,0,0,ItemSocketOption,0xFF,((lpItemDropInfo->Duration>0)?((DWORD)time(0)+lpItemDropInfo->Duration):0));

		return 1;
	}
}

int CItemDrop::GetItemDropRate(LPOBJ lpObj,LPOBJ lpTarget,int ItemIndex,int DropRate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(gItemManager->IsJewelItem(ItemIndex) != 0 && gCrywolfSync->CheckApplyPenalty() != 0 && gCrywolfSync->GetOccupationState() == 1)
	{
		if((GetLargeRand()%100) >= gCrywolfSync->GetGemDropPenaltiyRate())
		{
			return 0;
		}
	}

	#endif

	return DropRate;
}