// JewelOfHarmonyType.cpp: implementation of the CJewelOfHarmonyType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JewelOfHarmonyType.h"
#include "ItemManager.h."
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJewelOfHarmonyType::CJewelOfHarmonyType() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_JewelOfHarmonyTypeInfo.clear();

	#endif
}

CJewelOfHarmonyType::~CJewelOfHarmonyType() // OK
{

}

void CJewelOfHarmonyType::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=201)

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

	this->m_JewelOfHarmonyTypeInfo.clear();

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

			JEWEL_OF_HARMONY_TYPE_INFO info;

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.Level = lpReadFile->GetAsNumber();

			this->m_JewelOfHarmonyTypeInfo.insert(std::pair<int,JEWEL_OF_HARMONY_TYPE_INFO>(info.ItemIndex,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

JEWEL_OF_HARMONY_TYPE_INFO* CJewelOfHarmonyType::GetInfo(int index) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	std::map<int, JEWEL_OF_HARMONY_TYPE_INFO>::iterator it = this->m_JewelOfHarmonyTypeInfo.find(index);

	if(it == this->m_JewelOfHarmonyTypeInfo.end())
	{
		return 0;
	}
	else
	{
		return &it->second;
	}

	#else

	return 0;

	#endif
};

bool CJewelOfHarmonyType::CheckJewelOfHarmonyItemType(CItem* lpItem) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	JEWEL_OF_HARMONY_TYPE_INFO* lpInfo = this->GetInfo(lpItem->m_Index);

	if(lpInfo != 0)
	{
		if(lpInfo->Level > lpItem->m_Level)
		{
			return 0;
		}
	}

	return 1;

	#else

	return 0;

	#endif
}
