// 380ItemOption.cpp: implementation of the C380ItemOption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "380ItemOption.h"
#include "380ItemType.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C380ItemOption::C380ItemOption() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->Init();

	#endif
}

C380ItemOption::~C380ItemOption() // OK
{

}

void C380ItemOption::Init() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_380_ITEM_OPTION;n++)
	{
		this->m_380ItemOptionInfo[n].Index = -1;
	}

	#endif
}

void C380ItemOption::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=201)

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

	this->Init();

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
			
			ITEM_380_OPTION_INFO info;

			memset(&info,0,sizeof(info));

			info.Index = lpReadFile->GetNumber();

			strcpy_s(info.Name,lpReadFile->GetAsString());

			info.Value = lpReadFile->GetAsNumber();

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

void C380ItemOption::SetInfo(ITEM_380_OPTION_INFO info) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(info.Index < 0 || info.Index >= MAX_380_ITEM_OPTION)
	{
		return;
	}

	this->m_380ItemOptionInfo[info.Index] = info;

	#endif
}

ITEM_380_OPTION_INFO* C380ItemOption::GetInfo(int index) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(index < 0 || index >= MAX_380_ITEM_OPTION)
	{
		return 0;
	}

	if(this->m_380ItemOptionInfo[index].Index != index)
	{
		return 0;
	}

	return &this->m_380ItemOptionInfo[index];

	#else

	return 0;

	#endif
}

bool C380ItemOption::Is380Item(CItem* lpItem) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if((lpItem->m_ItemOptionEx & 0x80) == 0)
	{
		return 0;
	}

	return 1;

	#else

	return 0;

	#endif
}

void C380ItemOption::Calc380ItemOption(LPOBJ lpObj,bool flag) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < INVENTORY_WEAR_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() == 0 || lpObj->Inventory[n].m_IsValidItem == 0 || lpObj->Inventory[n].Is380Item() == 0)
		{
			continue;
		}

		for(int i=0;i < MAX_380_ITEM_OPTION_INDEX;i++)
		{
			int ItemOption = g380ItemType->Get380ItemOptionIndex(lpObj->Inventory[n].m_Index,i);

			if(ItemOption == ITEM_380_OPTION_NONE)
			{
				continue;
			}

			ITEM_380_OPTION_INFO* lpInfo = this->GetInfo(ItemOption);

			if(lpInfo == 0)
			{
				continue;
			}

			int OptionValue = g380ItemType->Get380ItemOptionValue(lpObj->Inventory[n].m_Index,i);

			OptionValue = ((OptionValue==-1)?lpInfo->Value:OptionValue);

			this->InsertOption(lpObj,lpInfo->Index,OptionValue,flag);
		}
	}

	#endif
}

void C380ItemOption::InsertOption(LPOBJ lpObj,int index,int value,bool flag) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(flag != 0)
	{
		return;
	}

	switch(index)
	{
		case ITEM_380_OPTION_ADD_ATTACK_SUCCESS_RATE_PVP:
			lpObj->AttackSuccessRatePvP += value;
			break;
		case ITEM_380_OPTION_ADD_DAMAGE_PVP:
			lpObj->DamagePvP += value;
			break;
		case ITEM_380_OPTION_ADD_DEFENSE_SUCCESS_RATE_PVP:
			lpObj->DefenseSuccessRatePvP += value;
			break;
		case ITEM_380_OPTION_ADD_DEFENSE_PVP:
			lpObj->DefensePvP += value;
			break;
		case ITEM_380_OPTION_ADD_MAX_HP:
			lpObj->AddLife += value;
			break;
		case ITEM_380_OPTION_ADD_MAX_SD:
			lpObj->AddShield += value;
			break;
		case ITEM_380_OPTION_SET_SD_RECOVERY_TYPE:
			lpObj->SDRecoveryType = value;
			break;
		case ITEM_380_OPTION_ADD_SD_RECOVERY_RATE:
			lpObj->SDRecoveryRate += value;
			break;
	}

	#endif
}
