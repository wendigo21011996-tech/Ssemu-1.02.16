// CustomWing.cpp: implementation of the CCustomWing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WingOption.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWingOption::CWingOption() // OK
{
	this->m_WingOptionInfo.clear();
}

CWingOption::~CWingOption() // OK
{

}


void CWingOption::Load(char* path) // OK
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

	this->m_WingOptionInfo.clear();

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

			WING_OPTION_INFO info;

			memset(&info,0,sizeof(info));

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.DefenseConstA = lpReadFile->GetAsNumber();

			info.IncDamageConstA = lpReadFile->GetAsNumber();

			info.IncDamageConstB = lpReadFile->GetAsNumber();

			info.DecDamageConstA = lpReadFile->GetAsNumber();

			info.DecDamageConstB = lpReadFile->GetAsNumber();

			info.MaxNewOption = lpReadFile->GetAsNumber();

			this->m_WingOptionInfo.insert(std::pair<int,WING_OPTION_INFO>(info.ItemIndex,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CWingOption::CheckWingByItem(int ItemIndex) // OK
{
	std::map<int, WING_OPTION_INFO>::iterator it = this->m_WingOptionInfo.find(ItemIndex);

	if(it == this->m_WingOptionInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int CWingOption::GetWingDefense(int ItemIndex,int ItemLevel) // OK
{
	std::map<int, WING_OPTION_INFO>::iterator it = this->m_WingOptionInfo.find(ItemIndex);

	if(it == this->m_WingOptionInfo.end())
	{
		return 0;
	}

	return (it->second.DefenseConstA*ItemLevel);
}

int CWingOption::GetWingIncDamage(int ItemIndex,int ItemLevel) // OK
{
	std::map<int, WING_OPTION_INFO>::iterator it = this->m_WingOptionInfo.find(ItemIndex);

	if(it == this->m_WingOptionInfo.end())
	{
		return 100;
	}

	return (it->second.IncDamageConstA+(ItemLevel*it->second.IncDamageConstB));
}

int CWingOption::GetWingDecDamage(int ItemIndex,int ItemLevel) // OK
{
	std::map<int, WING_OPTION_INFO>::iterator it = this->m_WingOptionInfo.find(ItemIndex);

	if(it == this->m_WingOptionInfo.end())
	{
		return 100;
	}

	return (it->second.DecDamageConstA-(ItemLevel*it->second.DecDamageConstB));
}

int CWingOption::GetWingMaxNewOption(int ItemIndex) // OK
{
	std::map<int, WING_OPTION_INFO>::iterator it = this->m_WingOptionInfo.find(ItemIndex);

	if(it == this->m_WingOptionInfo.end())
	{
		return 0;
	}

	return (it->second.MaxNewOption > MAX_EXC_OPTION) ? MAX_EXC_OPTION : it->second.MaxNewOption;
}