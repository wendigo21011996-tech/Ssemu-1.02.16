// ExcellentOptionRate.cpp: implementation of the CExcellentOptionRate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExcellentOptionRate.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "RandomManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExcellentOptionRate::CExcellentOptionRate() // OK
{
	this->m_ExcellentItemOptionInfo.clear();
}

CExcellentOptionRate::~CExcellentOptionRate() // OK
{

}

void CExcellentOptionRate::Load(char* path) // OK
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

	this->m_ExcellentItemOptionInfo.clear();

	try
	{
		while (true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			EXCELLENT_ITEM_OPTION_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.ItemMinIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(),lpReadFile->GetAsNumber()));

			info.ItemMaxIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(),lpReadFile->GetAsNumber()));

			strcpy_s(info.Name,lpReadFile->GetAsString());

			info.Rate = lpReadFile->GetAsNumber();

			this->m_ExcellentItemOptionInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CExcellentOptionRate::GetNewOption(int ItemIndex)
{
	int value = 0;

	CRandomManager RandomManager;

	for(std::vector<EXCELLENT_ITEM_OPTION_INFO>::iterator it = this->m_ExcellentItemOptionInfo.begin(); it != this->m_ExcellentItemOptionInfo.end(); it++)
	{
		if(it->ItemMinIndex != -1 && it->ItemMinIndex > ItemIndex)
		{
			continue;
		}

		if(it->ItemMaxIndex != -1 && it->ItemMaxIndex < ItemIndex)
		{
			continue;
		}

		RandomManager.AddElement(it->Index,it->Rate);
	}

	RandomManager.GetRandomElement(&value);

	return value;
}