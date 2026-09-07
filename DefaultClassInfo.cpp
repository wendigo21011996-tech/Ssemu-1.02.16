// DefaultClassInfo.cpp: implementation of the CDefaultClassInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DefaultClassInfo.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaultClassInfo::CDefaultClassInfo() // OK
{
	memset(this->m_DefaultClassInfo,0,sizeof(this->m_DefaultClassInfo));
}

CDefaultClassInfo::~CDefaultClassInfo() // OK
{

}

void CDefaultClassInfo::Load(char* path) // OK
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

	memset(this->m_DefaultClassInfo,0,sizeof(this->m_DefaultClassInfo));

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

			DEFAULT_CLASS_INFO info;

			info.Class = lpReadFile->GetNumber();

			info.Strength = lpReadFile->GetAsNumber();

			info.Dexterity = lpReadFile->GetAsNumber();

			info.Vitality = lpReadFile->GetAsNumber();

			info.Energy = lpReadFile->GetAsNumber();

			info.Leadership = lpReadFile->GetAsNumber();

			info.MaxLife = lpReadFile->GetAsFloatNumber();

			info.MaxMana = lpReadFile->GetAsFloatNumber();

			info.LevelLife = lpReadFile->GetAsFloatNumber();

			info.LevelMana = lpReadFile->GetAsFloatNumber();

			info.VitalityToLife = lpReadFile->GetAsFloatNumber();

			info.EnergyToMana = lpReadFile->GetAsFloatNumber();

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CDefaultClassInfo::SetInfo(DEFAULT_CLASS_INFO info) // OK
{
	if(CHECK_RANGE(info.Class,MAX_CLASS) == 0)
	{
		return;
	}

	this->m_DefaultClassInfo[info.Class] = info;
}

int CDefaultClassInfo::GetCharacterDefaultStat(int Class,int stat) // OK
{
	if(CHECK_RANGE(Class,MAX_CLASS) == 0)
	{
		return 0;
	}

	switch(stat)
	{
		case 0:
			return this->m_DefaultClassInfo[Class].Strength;
		case 1:
			return this->m_DefaultClassInfo[Class].Dexterity;
		case 2:
			return this->m_DefaultClassInfo[Class].Vitality;
		case 3:
			return this->m_DefaultClassInfo[Class].Energy;
		case 4:
			return this->m_DefaultClassInfo[Class].Leadership;
	}

	return 0;
}
