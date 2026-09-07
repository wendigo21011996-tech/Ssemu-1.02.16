// SkillUseArea.cpp: interface for the CSkillUseArea class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillUseArea.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillUseArea::CSkillUseArea() // OK
{
	this->m_SkillUseInfo.clear();

	this->m_SkillUseAreaInfo.clear();
}

CSkillUseArea::~CSkillUseArea() // OK
{

}

void CSkillUseArea::Load(char* path) // OK
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

	this->m_SkillUseInfo.clear();

	this->m_SkillUseAreaInfo.clear();

	try
	{
		while (true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while (true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					SKILL_USE_INFO info;

					info.Index = lpReadFile->GetNumber();

					this->m_SkillUseInfo.insert(std::pair<int,SKILL_USE_INFO>(info.Index,info));
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					SKILL_USE_AREA_INFO info;

					info.Map = lpReadFile->GetNumber();

					info.X = lpReadFile->GetAsNumber();

					info.Y = lpReadFile->GetAsNumber();

					info.TX = lpReadFile->GetAsNumber();

					info.TY = lpReadFile->GetAsNumber();

					this->m_SkillUseAreaInfo.push_back(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CSkillUseArea::CheckUseSkill(LPOBJ lpObj,int Index) // OK
{
	std::map<int,SKILL_USE_INFO>::iterator it = this->m_SkillUseInfo.find(Index);

	if(it == this->m_SkillUseInfo.end())
	{
		return 0;
	}

	for(std::vector<SKILL_USE_AREA_INFO>::iterator it = this->m_SkillUseAreaInfo.begin(); it != this->m_SkillUseAreaInfo.end(); it++)
	{
		if(it->Map != lpObj->Map)
		{
			continue;
		}

		if((lpObj->X < it->X || lpObj->X > it->TX) || (lpObj->Y < it->Y || lpObj->Y > it->TY))
		{
			continue;
		}

		return 1;
	}

	return 0;
}