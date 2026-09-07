// MonsterSkillElement.cpp: implementation of the CMonsterSkillElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterSkillElement.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterSkillElement::CMonsterSkillElement() // OK
{
	this->DelAllSkillElement();
}

CMonsterSkillElement::~CMonsterSkillElement() // OK
{

}

void CMonsterSkillElement::Load(char* path) // OK
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

	this->DelAllSkillElement();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CMonsterSkillElementInfo info;

					info.m_ElementNumber = lpReadFile->GetNumber();

					strcpy_s(info.m_ElementName,lpReadFile->GetAsString());

					info.m_ElementType = lpReadFile->GetAsNumber();

					info.m_SuccessRate = lpReadFile->GetAsNumber();

					info.m_ContinuanceTime = lpReadFile->GetAsNumber();

					info.m_IncAndDecType = lpReadFile->GetAsNumber();

					info.m_IncAndDecValue = lpReadFile->GetAsNumber();

					info.m_NullifiedSkill = lpReadFile->GetAsNumber();

					info.m_CharacterClass = lpReadFile->GetAsNumber();

					info.m_CharacterLevel = lpReadFile->GetAsNumber();

					if(info.m_ElementNumber >= 0 && info.m_ElementNumber < MAX_MONSTER_SKILL_ELEMENT_INFO)
					{
						this->m_MonsterSkillElementInfoArray[info.m_ElementNumber] = info;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	this->m_DataLoad = 1;

	delete lpReadFile;
}

void CMonsterSkillElement::DelAllSkillElement() // OK
{
	this->m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_SKILL_ELEMENT_INFO;n++)
	{
		this->m_MonsterSkillElementInfoArray[n].Reset();
	}
}

CMonsterSkillElementInfo* CMonsterSkillElement::FindSkillElementInfo(int ElementNumber) // OK
{
	if(ElementNumber < 0 || ElementNumber >= MAX_MONSTER_SKILL_ELEMENT_INFO)
	{
		return 0;
	}

	if(this->m_MonsterSkillElementInfoArray[ElementNumber].m_ElementNumber == ElementNumber)
	{
		return &this->m_MonsterSkillElementInfoArray[ElementNumber];
	}

	return 0;
}
