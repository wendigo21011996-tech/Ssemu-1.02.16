// ItemBagManager.cpp: implementation of the CItemBagManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemBagManager.h"
#include "Console.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Path.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemBagManager::CItemBagManager() // OK
{
	this->m_ItemBagManagerInfo.clear();
}

CItemBagManager::~CItemBagManager() // OK
{

}

void CItemBagManager::Load(char* path) // OK
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

	this->m_ItemBagManagerInfo.clear();

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

			ITEM_BAG_MANAGER_INFO info;

			info.ItemIndex = lpReadFile->GetNumber();

			info.ItemLevel = lpReadFile->GetAsNumber();

			info.MonsterClass = lpReadFile->GetAsNumber();

			info.SpecialValue = lpReadFile->GetAsNumber();

			info.SpecialLevel = lpReadFile->GetAsNumber();

			info.SpecialState = lpReadFile->GetAsNumber();

			strcpy_s(info.BagPath,lpReadFile->GetAsString());

			this->m_ItemBagManagerInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CItemBagManager::LoadEventItemBag() // OK
{
	for(std::vector<ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		char path[MAX_PATH];

		wsprintf(path,"EventItemBag\\%s.txt",it->BagPath);

		it->ItemBag.Load(gPath->GetFullPath(path));
	}
}

bool CItemBagManager::GetItemBySpecialValue(int SpecialValue,int SpecialLevel,int SpecialState,LPOBJ lpObj,CItem* lpItem) // OK
{
	for(std::vector<ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if(it->SpecialValue == SpecialValue && (it->SpecialLevel == -1 || it->SpecialLevel == SpecialLevel) && (it->SpecialState == -1 || it->SpecialState == SpecialState))
		{
			return it->ItemBag.GetItem(lpObj,lpItem);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemByItemIndex(int ItemIndex,int ItemLevel,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::vector<ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if(it->ItemIndex == ItemIndex && (it->ItemLevel == -1 || it->ItemLevel == ItemLevel))
		{
			Console(2, "[DropByItemIndex][%s][%s] - (I:%d L:%d) (Map:%d X:%d Y:%d)", lpObj->Account, lpObj->Name, ItemIndex, ItemLevel, map, x, y);
			return it->ItemBag.DropItem(lpObj,map,x,y);
		}
	}

	return 0;
}

bool CItemBagManager::DropItemByMonsterClass(int MonsterClass,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::vector<ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if(it->MonsterClass == MonsterClass)
		{
			Console(2, "[DropByMonsterClass][%s][%s] - (C:%d) (Map:%d X:%d Y:%d)", lpObj->Account, lpObj->Name, MonsterClass, map, x, y);
			return it->ItemBag.DropItem(lpObj,map,x,y);
		}
	}
	
	return 0;
}

bool CItemBagManager::DropItemBySpecialValue(int SpecialValue,int SpecialLevel,int SpecialState,LPOBJ lpObj,int map,int x,int y) // OK
{
	for(std::vector<ITEM_BAG_MANAGER_INFO>::iterator it = this->m_ItemBagManagerInfo.begin(); it != this->m_ItemBagManagerInfo.end(); it++)
	{
		if(it->SpecialValue == SpecialValue && (it->SpecialLevel == -1 || it->SpecialLevel == SpecialLevel) && (it->SpecialState == -1 || it->SpecialState == SpecialState))
		{
			Console(2, "[DropBySpecialValue][%s][%s] - (V:%d L:%d S:%d) (Map:%d X:%d Y:%d)", lpObj->Account, lpObj->Name, SpecialValue, SpecialLevel, SpecialState, map, x, y);
			return it->ItemBag.DropItem(lpObj,map,x,y);
		}
	}

	return 0;
}