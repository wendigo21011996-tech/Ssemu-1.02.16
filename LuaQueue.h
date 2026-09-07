// LuaQueue.h: interface for the CLuaQueue class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CriticalSection.h"

#define MAX_LUA_QUEUE_SIZE 1024

struct LUA_QUEUE_INFO
{
	char label[256];
	char query[256];
	char param[256];
};

class CLuaQueue
{
public:
	CLuaQueue();
	virtual ~CLuaQueue();
	void ClearQueue();
	DWORD GetQueueSize();
	bool AddToQueue(LUA_QUEUE_INFO* lpInfo);
	bool GetFromQueue(LUA_QUEUE_INFO* lpInfo);
private:
	CCriticalSection m_critical;
	std::queue<LUA_QUEUE_INFO> m_QueueInfo;
};
