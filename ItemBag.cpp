// ItemBag.cpp: implementation of the CItemBag class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemBag.h"
#include "CustomWing.h"
#include "DSProtocol.h"
#include "ExcellentOptionRate.h"
#include "ItemOptionRate.h"
#include "ItemManager.h"
#include "JewelOfHarmonyOption.h"
#include "Log.h"
#include "Map.h"
#include "ReadFile.h"
#include "Protocol.h"
#include "ServerInfo.h"
#include "SetItemType.h"
#include "Util.h"
#include "WingOption.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemBag::CItemBag() // OK
{
	memset(this->m_EventName,0,sizeof(this->m_EventName));

	this->m_DropZen = 0;
	this->m_ItemDropRate = 0;
	this->m_ItemDropCount = 0;
	this->m_SetItemDropRate = 0;
	this->m_RandomOption = 0;
	this->m_BagUseEffect = 0;

	this->m_ItemBagInfo.clear();
}

CItemBag::~CItemBag() // OK
{

}

void CItemBag::Init() // OK
{
	
}

void CItemBag::Load(char* path) // OK
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

	this->m_ItemBagInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					strcpy_s(this->m_EventName,lpReadFile->GetString());

					this->m_DropZen = lpReadFile->GetAsNumber();

					this->m_ItemDropRate = lpReadFile->GetAsNumber();

					this->m_ItemDropCount = lpReadFile->GetAsNumber();

					this->m_SetItemDropRate = lpReadFile->GetAsNumber();

					this->m_RandomOption = lpReadFile->GetAsNumber();

					this->m_BagUseEffect = lpReadFile->GetAsNumber();
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_INFO info;

					memset(&info,0,sizeof(info));

					info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

					info.MinLevel = lpReadFile->GetAsNumber();

					info.MaxLevel = lpReadFile->GetAsNumber();

					info.Option1 = lpReadFile->GetAsNumber();

					info.Option2 = lpReadFile->GetAsNumber();

					info.Option3 = lpReadFile->GetAsNumber();

					info.NewOption = lpReadFile->GetAsNumber();

					info.SetOption = lpReadFile->GetAsNumber();

					info.Duration = lpReadFile->GetAsNumber();

					this->m_ItemBagInfo.push_back(info);
				}
				else
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	this->m_ItemBagEx.Load(path);

	delete lpReadFile;
}

bool CItemBag::GetItem(LPOBJ lpObj,CItem* lpItem) // OK
{
	if(this->m_ItemBagEx.m_ItemBagInfo.empty() == 0)
	{
		return this->m_ItemBagEx.GetItem(lpObj,lpItem);
	}

	if(this->m_ItemBagInfo.size() == 0)
	{
		return 0;
	}

	int index = GetLargeRand()%this->m_ItemBagInfo.size();

	ITEM_BAG_INFO* lpInfo = &this->m_ItemBagInfo[index];

	if(lpInfo == 0)
	{
		return 0;
	}

	int Option1 = lpInfo->Option1;
	int Option2 = lpInfo->Option2;
	int Option3 = lpInfo->Option3;
	int SetOption = lpInfo->SetOption;
	int SocketSlot = lpInfo->SocketOption;

	if(this->m_RandomOption != 0)
	{
		if(Option1 != 0)
		{
			Option1 = GetLargeRand()%2;
		}

		if(Option2 != 0)
		{
			Option2 = GetLargeRand()%2;
		}

		if(Option3 != 0)
		{
			Option3 = GetLargeRand()%Option3;

			Option3 = ((Option3>gServerInfo->m_MaxItemOption)?gServerInfo->m_MaxItemOption:Option3);
		}

		if(SetOption != 0)
		{
			SetOption = GetLargeRand()%2;
		}

		if(SocketSlot != 0)
		{
			SocketSlot = GetLargeRand()%5;
		}
	}

	gItemOptionRate->MakeLevelOption(lpInfo->MinLevel,lpInfo->MaxLevel,&lpItem->m_Level);
	gItemOptionRate->MakeNewOption(lpInfo->ItemIndex,this->m_RandomOption,lpInfo->NewOption,&lpItem->m_NewOption);
	gItemOptionRate->MakeSetOption(lpInfo->ItemIndex,SetOption,&lpItem->m_SetOption);
	gItemOptionRate->MakeSocketSlot(lpInfo->ItemIndex,SocketSlot,&lpItem->m_SocketOption[0]);

	if(lpInfo->Duration > 0)
	{
		int duration = ((lpInfo->Duration>0)?((DWORD)time(0)+lpInfo->Duration):0);

		lpItem->m_IsPeriodicItem = ((duration>0)?1:0);

		lpItem->m_LoadPeriodicItem = ((duration>0)?1:0);

		lpItem->m_PeriodicItemTime = ((duration>0)?duration:0);
	}

	lpItem->Convert(lpInfo->ItemIndex,Option1,Option2,Option3,lpItem->m_NewOption,lpItem->m_SetOption,0,0,lpItem->m_SocketOption,0xFF);

	return 1;
}

bool CItemBag::DropItem(LPOBJ lpObj,int map,int x,int y) // OK
{
	if(this->m_ItemBagEx.m_ItemBagInfo.empty() == 0)
	{
		return this->m_ItemBagEx.DropItem(lpObj,map,x,y);
	}

	for(int n=0;n < this->m_ItemDropCount;n++)
	{
		int px = x;
		int py = y;

		if(this->m_ItemDropCount > 1 || (gMap[map].CheckAttr(px,py,4) != 0 || gMap[map].CheckAttr(px,py,8) != 0))
		{
			if(gObjGetRandomFreeLocation(map,&px,&py,2,2,10) == 0)
			{
				px = lpObj->X;
				py = lpObj->Y;
			}
		}

		if((GetLargeRand()%100) >= this->m_ItemDropRate)
		{
			gMap[map].MoneyItemDrop(this->m_DropZen,px,py);
			continue;
		}

		if((GetLargeRand()%10000) < this->m_SetItemDropRate)
		{
			gSetItemType->MakeRandomSetItem(lpObj->Index,map,px,py);
			continue;
		}

		CItem item;

		if(this->GetItem(lpObj,&item) != 0)
		{
			GDCreateItemSend(lpObj->Index,map,px,py,item.m_Index,(BYTE)item.m_Level,0,item.m_Option1,item.m_Option2,item.m_Option3,lpObj->Index,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,item.m_PeriodicItemTime);
			continue;
		}
	}

	switch(this->m_BagUseEffect)
	{
		case 1:
			GCServerCommandSend(lpObj->Index,0,lpObj->X,lpObj->Y,1);
			break;
		case 2:
			GCServerCommandSend(lpObj->Index,2,lpObj->X,lpObj->Y,1);
			break;
		case 3:
			GCServerCommandSend(lpObj->Index,58,SET_NUMBERHB(lpObj->Index),SET_NUMBERLB(lpObj->Index),1);
			break;
	}

	return 1;
}
