// ScriptLoader.h: interface for the CScriptLoader class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CriticalSection.h"
#include "LuaQueue.h"

class CScriptLoader
{
	CScriptLoader();
	virtual ~CScriptLoader();
	SingletonInstance(CScriptLoader)
public:
	void Load(char* path);
	void SetInfo(LUA_QUEUE_INFO* lpInfo);
	void OnReadScript();
	void OnShutScript();
	void OnTimerThread();
	int OnCommandManager(int aIndex,int code,char* arg);
	void OnCommandDone(int aIndex,int code);
	void OnCharacterEntry(int aIndex);
	void OnCharacterClose(int aIndex);
	int OnNpcTalk(int aIndex,int bIndex);
	void OnMonsterDie(int aIndex,int bIndex);
	void OnUserDie(int aIndex,int bIndex);
	void OnUserMove(int aIndex,int MapIndex);
	void OnUserRespawn(int aIndex,int KillerType);
	int OnCheckUserTarget(int aIndex,int bIndex);
	int OnCheckUserKiller(int aIndex,int bIndex);
	int OnUserItemPick(int aIndex,int slot);
	int OnUserItemDrop(int aIndex,int slot,int x,int y);
	int OnUserItemMove(int aIndex,int aFlag,int aSlot,int bFlag,int bSlot);
	void OnSQLAsyncResult();
private:
	CCriticalSection m_critical;
	CLuaQueue m_luaQueue;
	lua_State* m_luaState;
};

#define gScriptLoader SingNull(CScriptLoader)