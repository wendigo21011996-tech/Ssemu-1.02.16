// CustomHealthBar.cpp: implementation of the CCustomHealthBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomHealthBar.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomHealthBar::CCustomHealthBar() // OK
{

}

CCustomHealthBar::~CCustomHealthBar() // OK
{

}

void CCustomHealthBar::Load(char*path) // OK
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

	this->m_MonsterHealthBarInfo.clear();

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
			
			MONSTER_HEALTH_BAR_INFO info;

			info.MonsterIndex = lpReadFile->GetNumber();

			info.MapNumber = lpReadFile->GetAsNumber();

			info.BarSwitch = lpReadFile->GetAsNumber();

			this->m_MonsterHealthBarInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CCustomHealthBar::CheckHealthBar(int MonsterIndex,int MapNumber) // OK
{
	for(std::vector<MONSTER_HEALTH_BAR_INFO>::iterator it= this->m_MonsterHealthBarInfo.begin();it != this->m_MonsterHealthBarInfo.end();it++)
	{
		if(it->MonsterIndex != -1 && it->MonsterIndex != MonsterIndex)
		{
			continue;
		}

		if(it->MapNumber != -1 && it->MapNumber != MapNumber)
		{
			continue;
		}

		return it->BarSwitch;
	}

	return 0;
}