// 380ItemType.cpp: implementation of the C380ItemType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "380ItemType.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C380ItemType::C380ItemType() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_380ItemTypeInfo.clear();

	#endif
}

C380ItemType::~C380ItemType() // OK
{

}

void C380ItemType::Load(char* path) // OK
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

	this->m_380ItemTypeInfo.clear();

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

			ITEM_380_TYPE_INFO info;

			memset(&info,0,sizeof(info));

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			for(int n=0;n < MAX_380_ITEM_OPTION_INDEX;n++)
			{
				info.OptionIndex[n] = lpReadFile->GetAsNumber();

				info.OptionValue[n] = lpReadFile->GetAsNumber();
			}

			this->m_380ItemTypeInfo.insert(std::pair<int,ITEM_380_TYPE_INFO>(info.ItemIndex,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

bool C380ItemType::Check380ItemType(int index) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	std::map<int,ITEM_380_TYPE_INFO>::iterator it = this->m_380ItemTypeInfo.find(index);

	if(it == this->m_380ItemTypeInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}

	#else

	return 0;

	#endif
}

int C380ItemType::Get380ItemOptionIndex(int index,int number) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	std::map<int,ITEM_380_TYPE_INFO>::iterator it = this->m_380ItemTypeInfo.find(index);

	if(it == this->m_380ItemTypeInfo.end())
	{
		return -1;
	}
	else
	{
		return ((CHECK_RANGE(number,MAX_380_ITEM_OPTION_INDEX)==0)?-1:it->second.OptionIndex[number]);
	}

	#else

	return -1;

	#endif
}

int C380ItemType::Get380ItemOptionValue(int index,int number) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	std::map<int,ITEM_380_TYPE_INFO>::iterator it = this->m_380ItemTypeInfo.find(index);

	if(it == this->m_380ItemTypeInfo.end())
	{
		return -1;
	}
	else
	{
		return ((CHECK_RANGE(number,MAX_380_ITEM_OPTION_INDEX)==0)?-1:it->second.OptionValue[number]);
	}

	#else

	return -1;

	#endif
}
