// ItemLevel.cpp: implementation of the CItemLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemLevel.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemLevel::CItemLevel() // OK
{
	this->m_ItemLevelInfo.clear();
}

CItemLevel::~CItemLevel() // OK
{

}

void CItemLevel::Load(char* path) // OK
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

	this->m_ItemLevelInfo.clear();

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

			ITEM_LEVEL_INFO info;

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.Level = lpReadFile->GetAsNumber();

			strcpy_s(info.Name,lpReadFile->GetAsString());

			this->m_ItemLevelInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CItemLevel::CheckItemlevel(int index,int level) // OK
{
	for(std::vector<ITEM_LEVEL_INFO>::iterator it = this->m_ItemLevelInfo.begin(); it != this->m_ItemLevelInfo.end(); it++)
	{
		if(it->ItemIndex != index || it->Level != level)
		{
			continue;
		}

		return 1;
	}

	return 0;
}