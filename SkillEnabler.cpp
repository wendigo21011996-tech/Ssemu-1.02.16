// SkillEnabler.cpp: interface for the CSkillEnabler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillEnabler.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillEnabler::CSkillEnabler() // OK
{
	this->m_SkillEnabler.clear();
}

CSkillEnabler::~CSkillEnabler() // OK
{

}

void CSkillEnabler::Load(char* path) // OK
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

	this->m_SkillEnabler.clear();

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

			SKILL_ENABLER_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.Map = lpReadFile->GetAsNumber();

			info.PvM = lpReadFile->GetAsNumber();

			info.PvP = lpReadFile->GetAsNumber();

			this->m_SkillEnabler.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CSkillEnabler::CheckUseSkill(LPOBJ lpObj,LPOBJ lpTarget,int index) // OK
{
	for(std::vector<SKILL_ENABLER_INFO>::iterator it = this->m_SkillEnabler.begin(); it != this->m_SkillEnabler.end(); it++)
	{
		if(it->Index != index)
		{
			continue;
		}

		if(it->Map != -1 && it->Map != lpObj->Map)
		{
			continue;
		}

		if(lpTarget->Type == OBJECT_MONSTER)
		{
			return (it->PvM == 0) ? 0 : 1;
		}
		else if(lpTarget->Type == OBJECT_USER)
		{
			return (it->PvP == 0) ? 0 : 1;
		}
	}

	return 1;
}