// EventSpawnStage.cpp: interface for the CEventSpawnStage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventSpawnStage.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventSpawnStage::CEventSpawnStage() // OK
{
	this->m_EventSpawnStageInfo.clear();
}

CEventSpawnStage::~CEventSpawnStage() // OK
{

}

void CEventSpawnStage::Load(char* path) // OK
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

	this->m_EventSpawnStageInfo.clear();

	try
	{
		while (true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int Event = lpReadFile->GetNumber();

			while (true)
			{
				if(strcmp("end",lpReadFile->GetAsString()) == 0)
				{
					break;
				}

				EVENT_SPAWN_STAGE_INFO info;

				info.Event = Event;

				info.Level = lpReadFile->GetNumber();

				info.Stage = lpReadFile->GetAsNumber();

				info.MonsterClass = lpReadFile->GetAsNumber();

				info.MaxRegenTime = lpReadFile->GetAsNumber();

				this->m_EventSpawnStageInfo.push_back(info);
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}