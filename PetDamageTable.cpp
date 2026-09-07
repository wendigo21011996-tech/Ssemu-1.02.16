// PetDamageTable.cpp: interface for the CPetDamageTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PetDamageTable.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPetDamageTable::CPetDamageTable() // OK
{
	this->m_PetDamageTableInfo.clear();
}

CPetDamageTable::~CPetDamageTable() // OK
{

}

void CPetDamageTable::Load(char* path) // OK
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

	this->m_PetDamageTableInfo.clear();

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

			PET_DAMAGE_TABLE_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.MinLevel = lpReadFile->GetAsNumber();

			info.MaxLevel = lpReadFile->GetAsNumber();

			info.MinReset = lpReadFile->GetAsNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.DamageRate = lpReadFile->GetAsNumber();

			this->m_PetDamageTableInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CPetDamageTable::GetPetDamage(LPOBJ lpObj,int index,int* damage) // OK
{
	for(std::vector<PET_DAMAGE_TABLE_INFO>::iterator it= this->m_PetDamageTableInfo.begin();it != this->m_PetDamageTableInfo.end();it++)
	{
		if(it->Index != index)
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

		(*damage) = ((*damage)*it->DamageRate)/1000;

		return 1;
	}

	return 0;
}