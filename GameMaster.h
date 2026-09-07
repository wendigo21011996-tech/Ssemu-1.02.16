// GameMaster.h: interface for the CGameMaster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct GAME_MASTER_INFO
{
	char Account[11];
	char Name[11];
	int Level;
};

class CGameMaster
{
	CGameMaster();
	virtual ~CGameMaster();
	SingletonInstance(CGameMaster)
public:
	void Load(char* path);
	int GetGameMasterLevel(LPOBJ lpObj);
private:
	std::vector<GAME_MASTER_INFO> m_GameMasterInfo;
};

#define gGameMaster SingNull(CGameMaster)