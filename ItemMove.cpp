// ItemMove.cpp: implementation of the CItemMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemMove.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemMove::CItemMove() // OK
{
	this->m_ItemMoveInfo.clear();
}

CItemMove::~CItemMove() // OK
{

}

void CItemMove::Load(char* path) // OK
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

	this->m_ItemMoveInfo.clear();

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

			ITEM_MOVE_INFO info;

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.AllowDrop = lpReadFile->GetAsNumber();

			info.AllowSell = lpReadFile->GetAsNumber();

			info.AllowTrade = lpReadFile->GetAsNumber();

			info.AllowVault = lpReadFile->GetAsNumber();

			this->m_ItemMoveInfo.insert(std::pair<int,ITEM_MOVE_INFO>(info.ItemIndex,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CItemMove::CheckItemMoveAllowDrop(int index) // OK
{
	std::map<int,ITEM_MOVE_INFO>::iterator it = this->m_ItemMoveInfo.find(index);

	if(it == this->m_ItemMoveInfo.end())
	{
		return 1;
	}
	else
	{
		return ((it->second.AllowDrop==0)?0:1);
	}
}

bool CItemMove::CheckItemMoveAllowSell(int index) // OK
{
	std::map<int,ITEM_MOVE_INFO>::iterator it = this->m_ItemMoveInfo.find(index);

	if(it == this->m_ItemMoveInfo.end())
	{
		return 1;
	}
	else
	{
		return ((it->second.AllowSell==0)?0:1);
	}
}

bool CItemMove::CheckItemMoveAllowTrade(int index) // OK
{
	std::map<int,ITEM_MOVE_INFO>::iterator it = this->m_ItemMoveInfo.find(index);

	if(it == this->m_ItemMoveInfo.end())
	{
		return 1;
	}
	else
	{
		return ((it->second.AllowTrade==0)?0:1);
	}
}

bool CItemMove::CheckItemMoveAllowVault(int index) // OK
{
	std::map<int,ITEM_MOVE_INFO>::iterator it = this->m_ItemMoveInfo.find(index);

	if(it == this->m_ItemMoveInfo.end())
	{
		return 1;
	}
	else
	{
		return ((it->second.AllowVault==0)?0:1);
	}
}
