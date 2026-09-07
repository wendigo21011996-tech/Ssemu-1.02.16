// MonsterSkillUnit.cpp: implementation of the CMonsterSkillUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterSkillUnit.h"
#include "ReadFile.h"
#include "MonsterSkillElement.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterSkillUnit::CMonsterSkillUnit() // OK
{
	this->DelAllSkillUnit();
}

CMonsterSkillUnit::~CMonsterSkillUnit() // OK
{

}

void CMonsterSkillUnit::Load(char* path) // OK
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

	this->DelAllSkillUnit();

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

					CMonsterSkillUnitInfo info;

					info.m_UnitNumber = lpReadFile->GetNumber();

					strcpy_s(info.m_UnitName,lpReadFile->GetAsString());

					info.m_UnitTargetType = lpReadFile->GetAsNumber();

					info.m_UnitScopeType = lpReadFile->GetAsNumber();

					info.m_UnitScopeValue = lpReadFile->GetAsNumber();

					info.m_Delay = lpReadFile->GetAsNumber();

					info.m_ElementsCount = 0;

					for(int n=0;n < MAX_MONSTER_SKILL_ELEMENT;n++)
					{
						info.m_ElementsSlot[n] = gMonsterSkillElement->FindSkillElementInfo(lpReadFile->GetAsNumber());

						info.m_ElementsCount = ((info.m_ElementsSlot[n]==0)?info.m_ElementsCount:(info.m_ElementsCount+1));
					}

					if(info.m_UnitNumber >= 0 && info.m_UnitNumber < MAX_MONSTER_SKILL_UNIT_INFO_ARRAY)
					{
						this->m_MonsterSkillUnitInfoArray[info.m_UnitNumber] = info;
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

void CMonsterSkillUnit::DelAllSkillUnit() // OK
{
	this->m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_SKILL_UNIT_INFO_ARRAY;n++)
	{
		this->m_MonsterSkillUnitInfoArray[n].Reset();
	}
}

CMonsterSkillUnitInfo* CMonsterSkillUnit::FindSkillUnitInfo(int UnitNumber) // OK
{
	if(UnitNumber < 0 || UnitNumber >= MAX_MONSTER_SKILL_UNIT_INFO_ARRAY)
	{
		return 0;
	}

	if(this->m_MonsterSkillUnitInfoArray[UnitNumber].m_UnitNumber == UnitNumber)
	{
		return &this->m_MonsterSkillUnitInfoArray[UnitNumber];
	}

	return 0;
}
