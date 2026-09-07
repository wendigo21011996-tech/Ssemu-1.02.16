// DamageTable.cpp: interface for the CDamageTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DamageTable.h"
#include "Attack.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDamageTable::CDamageTable() // OK
{
	this->m_MonsterDamageInfo.clear();

	this->m_PlayerDamageInfo.clear();
}

CDamageTable::~CDamageTable() // OK
{

}

void CDamageTable::Load(char* path) // OK
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

	this->m_MonsterDamageInfo.clear();

	this->m_PlayerDamageInfo.clear();

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

					MONSTER_DAMAGE_TABLE_INFO info;

					info.MapNumber = lpReadFile->GetNumber();

					info.MinLevel = lpReadFile->GetAsNumber();

					info.MaxLevel = lpReadFile->GetAsNumber();

					info.MinReset = lpReadFile->GetAsNumber();

					info.MaxReset = lpReadFile->GetAsNumber();

					info.MinDefense = lpReadFile->GetAsNumber();

					info.MaxDefense = lpReadFile->GetAsNumber();

					info.DamageRate = lpReadFile->GetAsNumber();

					this->m_MonsterDamageInfo.push_back(info);
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					PLAYER_DAMAGE_TABLE_INFO info;

					info.MapNumber = lpReadFile->GetNumber();

					info.MinLevel = lpReadFile->GetAsNumber();

					info.MaxLevel = lpReadFile->GetAsNumber();

					info.MinReset = lpReadFile->GetAsNumber();

					info.MaxReset = lpReadFile->GetAsNumber();

					info.MonsterIndex = lpReadFile->GetAsNumber();

					info.MonsterMinLevel = lpReadFile->GetAsNumber();

					info.MonsterMaxLevel = lpReadFile->GetAsNumber();

					info.DamageRate = lpReadFile->GetAsNumber();

					this->m_PlayerDamageInfo.push_back(info);
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

int CDamageTable::GetMonsterDamage(LPOBJ lpObj) // OK
{
	DWORD defense = 0;

	gAttack->GetPreviewDefense(lpObj,&defense);

	for(std::vector<MONSTER_DAMAGE_TABLE_INFO>::iterator it= this->m_MonsterDamageInfo.begin();it != this->m_MonsterDamageInfo.end();it++)
	{
		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		if(it->MinLevel != -1 && it->MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->MaxLevel != -1 && it->MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->MinReset != -1 && it->MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->MaxReset != -1 && it->MaxReset < lpObj->Reset)
		{
			continue;
		}

		if(it->MinDefense != -1 && it->MinDefense > (int)defense)
		{
			continue;
		}

		if(it->MaxDefense != -1 && it->MaxDefense < (int)defense)
		{
			continue;
		}

		return it->DamageRate;
	}

	return 100;
}

int CDamageTable::GetPlayerDamage(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	for(std::vector<PLAYER_DAMAGE_TABLE_INFO>::iterator it= this->m_PlayerDamageInfo.begin();it != this->m_PlayerDamageInfo.end();it++)
	{
		if(it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		if(it->MonsterIndex != -1 && it->MonsterIndex != lpTarget->Class)
		{
			continue;
		}

		if(it->MinLevel != -1 && it->MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->MaxLevel != -1 && it->MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->MinReset != -1 && it->MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->MaxReset != -1 && it->MaxReset < lpObj->Reset)
		{
			continue;
		}

		if(it->MonsterMinLevel != -1 && it->MonsterMinLevel > lpTarget->Level)
		{
			continue;
		}

		if(it->MonsterMaxLevel != -1 && it->MonsterMaxLevel < lpTarget->Level)
		{
			continue;
		}

		return it->DamageRate;
	}

	return 100;
}