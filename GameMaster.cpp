// GameMaster.cpp: implementation of the CGameMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameMaster.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameMaster::CGameMaster() // OK
{
	this->m_GameMasterInfo.clear();
}

CGameMaster::~CGameMaster() // OK
{

}

void CGameMaster::Load(char* path) // OK
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

	this->m_GameMasterInfo.clear();

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

			GAME_MASTER_INFO info;

			memset(&info,0,sizeof(info));

			strcpy_s(info.Account,lpReadFile->GetString());

			strcpy_s(info.Name,lpReadFile->GetAsString());

			info.Level = lpReadFile->GetAsNumber();

			this->m_GameMasterInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CGameMaster::GetGameMasterLevel(LPOBJ lpObj) // OK
{
	for (std::vector<GAME_MASTER_INFO>::iterator it = this->m_GameMasterInfo.begin(); it != this->m_GameMasterInfo.end(); it++)
	{
		if(strcmp(it->Account,lpObj->Account) != 0)
		{
			continue;
		}

		if(strcmp(it->Name,lpObj->Name) == 0 || strcmp(it->Name,"*") == 0)
		{
			return it->Level;
		}
	}

	return -1;
}