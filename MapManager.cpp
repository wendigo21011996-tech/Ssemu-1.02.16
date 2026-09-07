// MapManager.cpp: implementation of the CMapManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapManager::CMapManager() // OK
{
	this->m_MapManagerInfo.clear();
}

CMapManager::~CMapManager() // OK
{

}

void CMapManager::Load(char* path) // OK
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

	this->m_MapManagerInfo.clear();

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

			MAP_MANAGER_INFO info;

			memset(&info,0,sizeof(info));

			info.Index = lpReadFile->GetNumber();

			info.NonPK = lpReadFile->GetAsNumber();

			info.ViewRange = lpReadFile->GetAsNumber();

			info.ExperienceRate = lpReadFile->GetAsNumber();

			#if(GAMESERVER_UPDATE>=401)

			info.MasterExperienceRate = lpReadFile->GetAsNumber();

			#endif

			info.ItemDropRate = lpReadFile->GetAsNumber();

			info.MoneyDropRate = lpReadFile->GetAsNumber();

			info.ExcItemDropRate = lpReadFile->GetAsNumber();

			info.SetItemDropRate = lpReadFile->GetAsNumber();

			#if(GAMESERVER_UPDATE>=401)

			info.SocketItemDrop = lpReadFile->GetAsNumber();

			#endif

			#if(GAMESERVER_UPDATE>=603)

			info.HelperEnable = lpReadFile->GetAsNumber();

			#endif

			#if(GAMESERVER_UPDATE>=501)

			info.GensBattle = lpReadFile->GetAsNumber();

			#endif

			info.ItemDrop = lpReadFile->GetAsNumber();
			
			info.PKItemDrop = lpReadFile->GetAsNumber();
			
			#if(GAMESERVER_UPDATE<=603)

			info.MonsterHPBar = lpReadFile->GetAsNumber();

			#endif
			
			info.TradeEnable = lpReadFile->GetAsNumber();
			
			info.PShopEnable = lpReadFile->GetAsNumber();
			
			info.ChatEnable = lpReadFile->GetAsNumber();
			
			info.PartyEnable = lpReadFile->GetAsNumber();
			
			info.DuelEnable = lpReadFile->GetAsNumber();

			info.GuildWarEnable = lpReadFile->GetAsNumber();

			info.SpawnGate = lpReadFile->GetAsNumber();

			info.RespawnGate = lpReadFile->GetAsNumber();

			strcpy_s(info.Name,lpReadFile->GetAsString());

			this->m_MapManagerInfo.insert(std::pair<int,MAP_MANAGER_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CMapManager::IsValidMap(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return 1;
	}

	return 0;
}

int CMapManager::GetMapNonPK(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.NonPK == -1) ? gServerInfo->m_NonPK : ((it->second.NonPK == 1) ? 1 : 0));
	}

	return gServerInfo->m_NonPK;
}

int CMapManager::GetMapNonOutlaw(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.NonPK == -1) ? 0 : ((it->second.NonPK == 2) ? 1 : 0));
	}

	return 0;
}

int CMapManager::GetMapViewRange(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.ViewRange == -1) ? 12 : it->second.ViewRange);
	}
	
	return 12;
}

int CMapManager::GetMapExperienceRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.ExperienceRate;
	}

	return 100;
}

int CMapManager::GetMapMasterExperienceRate(int index) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.MasterExperienceRate;
	}

	#endif

	return 100;
}

int CMapManager::GetMapItemDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.ItemDropRate;
		
	}

	return 100;
}

int CMapManager::GetMoneyDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.MoneyDropRate;
		
	}

	return 100;
}

int CMapManager::GetMapExcItemDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.ExcItemDropRate;
	}

	return 0;
}

int CMapManager::GetMapSetItemDropRate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.SetItemDropRate;
	}

	return 0;
}

int CMapManager::GetMapSocketItemDrop(int index) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.SocketItemDrop;
	}

	#endif

	return 0;
}

int CMapManager::GetMapHelperEnable(int index) // OK
{
	#if(GAMESERVER_UPDATE>=603)

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.HelperEnable;
	}

	#endif

	return 0;
}

int CMapManager::GetMapGensBattle(int index) // OK
{
	#if(GAMESERVER_UPDATE>=501)

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.GensBattle;
	}

	#endif

	return 0;
}

int CMapManager::GetMapItemDrop(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.ItemDrop;
	}

	return 0;
}

int CMapManager::GetMapPKItemDrop(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.PKItemDrop;
	}

	return 0;
}

int CMapManager::GetMapMonsterHPBar(int index) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.MonsterHPBar == -1) ? gServerInfo->m_MonsterHealthBarSwitch : ((it->second.MonsterHPBar == 1) ? 1 : 0));
	}

	return gServerInfo->m_MonsterHealthBarSwitch;

	#else

	return 0;

	#endif
}

int CMapManager::GetMapTradeEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.TradeEnable == -1) ? gServerInfo->m_TradeSwitch : ((it->second.TradeEnable == 1) ? 1 : 0));
	}

	return gServerInfo->m_TradeSwitch;
}

int CMapManager::GetMapPShopEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.PShopEnable == -1) ? gServerInfo->m_PersonalShopSwitch : ((it->second.PShopEnable == 1) ? 1 : 0));
	}

	return gServerInfo->m_PersonalShopSwitch;
}

int CMapManager::GetMapChatEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.ChatEnable;
	}

	return 0;
}

int CMapManager::GetMapPartyEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.PartyEnable;
	}

	return 0;
}

int CMapManager::GetMapDuelEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return ((it->second.DuelEnable == -1) ? gServerInfo->m_DuelSwitch : ((it->second.DuelEnable == 1) ? 1 : 0));
	}

	return gServerInfo->m_DuelSwitch;
}

int CMapManager::GetMapGuildWarEnable(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.GuildWarEnable;
	}

	return 0;
}

int CMapManager::GetSpawnGate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.SpawnGate;
	}

	return -1;
}

int CMapManager::GetRespawnGate(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.RespawnGate;
	}

	return -1;
}

char* CMapManager::GetMapName(int index) // OK
{
	std::map<int,MAP_MANAGER_INFO>::iterator it = this->m_MapManagerInfo.find(index);

	if(it != this->m_MapManagerInfo.end())
	{
		return it->second.Name;
	}

	return 0;
}