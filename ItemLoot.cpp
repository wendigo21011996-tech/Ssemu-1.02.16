// ItemLoot.cpp: implementation of the CItemLoot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemLoot.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemLoot::CItemLoot() // OK
{
	this->m_ItemLootInfo.clear();
}

CItemLoot::~CItemLoot() // OK
{

}

void CItemLoot::Load(char* path) // OK
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

	this->m_ItemLootInfo.clear();

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

			ITEM_LOOT_INFO info;

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.Level = lpReadFile->GetAsNumber();

			info.AllowLoot = lpReadFile->GetAsNumber();

			this->m_ItemLootInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CItemLoot::CheckItemAllowLoot(int index,int level) // OK
{
	for(std::vector<ITEM_LOOT_INFO>::iterator it = this->m_ItemLootInfo.begin(); it != this->m_ItemLootInfo.end(); it++)
	{
		if(it->ItemIndex != index)
		{
			continue;
		}

		if(it->Level != -1 && it->Level != level)
		{
			continue;
		}

		return (it->AllowLoot == 0) ? 0 : 1;
	}

	return 1;
}