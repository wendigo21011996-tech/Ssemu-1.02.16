// EventSpawnStage.h: interface for the CEventSpawnStage class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct EVENT_SPAWN_STAGE_INFO
{
	int Event;
	int Level;
	int Stage;
	int MonsterClass;
	int MaxRegenTime;
};

class CEventSpawnStage
{
	CEventSpawnStage();
	virtual ~CEventSpawnStage();
	SingletonInstance(CEventSpawnStage)
public:
	void Load(char* path);
public:
	std::vector<EVENT_SPAWN_STAGE_INFO> m_EventSpawnStageInfo;
};

#define gEventSpawnStage SingNull(CEventSpawnStage)