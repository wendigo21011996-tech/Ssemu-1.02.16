// MapServerManager.cpp: implementation of the CMapServerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapServerManager.h"
#include "Map.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapServerManager::CMapServerManager() // OK
{
	
}

CMapServerManager::~CMapServerManager() // OK
{

}

void CMapServerManager::Clear()
{
	for(int n=0;n<MAX_MAP_GROUPS;n++)
	{
		this->m_MapServerGroup[n] = 0;

		for(int i=0;i<MAX_MAP_SUBGROUPS;i++)
		{
			this->m_MapServerData[n][i].Clear(1);
		}
	}

	this->m_MapServerCode.clear();

	this->m_LocalServerData = 0;
}

void CMapServerManager::Load(char* path)
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

	this->Clear();

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

					int ServerCode = lpReadFile->GetNumber();

					int ServerGroup = lpReadFile->GetAsNumber();

					MAP_SERVER_DATA* info = &this->m_MapServerData[ServerGroup][++this->m_MapServerGroup[ServerGroup]];

					info->Clear(lpReadFile->GetAsNumber());
					
					info->Used = 1;
					
					info->ServerCode = ServerCode;
					
					info->ServerGroup = ServerGroup;
					
					strcpy_s(info->ServerIPAddress, lpReadFile->GetAsString());

					info->ServerPort = lpReadFile->GetAsNumber();
					
					this->m_MapServerCode.insert(std::pair<int,MAP_SERVER_DATA*>(ServerCode,info));
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int ServerCode = lpReadFile->GetNumber();

					int NoMove = lpReadFile->GetAsNumber();
					
					int MapNumber = lpReadFile->GetAsNumber();

					int NextServerCode = lpReadFile->GetAsNumber();

					std::map<int,MAP_SERVER_DATA*>::iterator it = this->m_MapServerCode.find(ServerCode);

					if(it == this->m_MapServerCode.end() || it->second->Used == 0 || it->second->ServerCode != ServerCode)
					{
						return;
					}

					it->second->MapMove[MapNumber] = ((NoMove == 1) ? -2 : NextServerCode);
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

	delete lpReadFile;

	std::map<int,MAP_SERVER_DATA*>::iterator it = this->m_MapServerCode.find(gServerInfo->m_ServerCode);

	if(it != this->m_MapServerCode.end())
	{
		this->m_LocalServerData = it->second;
	}
}

int CMapServerManager::GetMapServerGroup() // OK
{
	if(this->m_LocalServerData == 0)
	{
		return -1;
	}

	return this->m_LocalServerData->ServerGroup;
}

bool CMapServerManager::GetMapServerData(WORD ServerCode,char* IpAddress,WORD* ServerPort) // OK
{
	if(IpAddress == 0 || ServerPort == 0)
	{
		return 0;
	}

	std::map<int,MAP_SERVER_DATA*>::iterator it = this->m_MapServerCode.find(ServerCode);

	if(it != this->m_MapServerCode.end())
	{
		strcpy_s(IpAddress,sizeof(it->second->ServerIPAddress),it->second->ServerIPAddress);

		(*ServerPort) = it->second->ServerPort;

		return 1;
	}

	return 0;
}

bool CMapServerManager::CheckMapServer(int MapNumber) // OK
{
	if(gMapManager->IsValidMap(MapNumber) == 0)
	{
		return 0;
	}

	if(this->m_LocalServerData == 0)
	{
		return 0;
	}

	if(this->m_LocalServerData->MapMove[MapNumber] != -2)
	{
		return 0;
	}

	return 1;
}

int CMapServerManager::CheckMapServerMove(int aIndex,int MapNumber,int ServerCode) // OK
{
	if (gObjIsConnected(aIndex) == 0)
	{
		return gServerInfo->m_ServerCode;
	}

	if(gMapManager->IsValidMap(MapNumber) == 0)
	{
		return gServerInfo->m_ServerCode;
	}

	if(this->m_LocalServerData == 0)
	{
		return gServerInfo->m_ServerCode;
	}

	int MapMoveInfo = this->m_LocalServerData->MapMove[MapNumber];

	if(MapMoveInfo == -1 && ServerCode != -1)
	{
		std::map<int,MAP_SERVER_DATA*>::iterator it = this->m_MapServerCode.find(ServerCode);

		if(it != this->m_MapServerCode.end() && it->second->MapMove[MapNumber] == -2)
		{
			return ServerCode;
		}
	}
	else if(MapMoveInfo == -2)
	{
		return gServerInfo->m_ServerCode;
	}
	else if(MapMoveInfo >= 0)
	{
		std::map<int,MAP_SERVER_DATA*>::iterator it = this->m_MapServerCode.find(MapMoveInfo);

		if(it != this->m_MapServerCode.end() && it->second->Used == 1 && it->second->MapMove[MapNumber] == -2)
		{
			return it->second->ServerCode;
		}
	}

	return gServerInfo->m_ServerCode;
}
