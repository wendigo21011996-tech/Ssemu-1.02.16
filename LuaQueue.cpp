// LuaQueue.cpp: implementation of the CLuaQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LuaQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLuaQueue::CLuaQueue() // OK
{

}

CLuaQueue::~CLuaQueue() // OK
{
	this->ClearQueue();
}

void CLuaQueue::ClearQueue() // OK
{
	this->m_critical.lock();

	this->m_QueueInfo.swap(std::queue<LUA_QUEUE_INFO>());

	this->m_critical.unlock();
}

DWORD CLuaQueue::GetQueueSize() // OK
{
	DWORD size = 0;

	this->m_critical.lock();

	size = this->m_QueueInfo.size();

	this->m_critical.unlock();

	return size;
}

bool CLuaQueue::AddToQueue(LUA_QUEUE_INFO* lpInfo) // OK
{
	bool result = 0;

	this->m_critical.lock();

	if(this->m_QueueInfo.size() < MAX_LUA_QUEUE_SIZE)
	{
		this->m_QueueInfo.push((*lpInfo));

		result = 1;
	}

	this->m_critical.unlock();

	return result;
}

bool CLuaQueue::GetFromQueue(LUA_QUEUE_INFO* lpInfo) // OK
{
	bool result = 0;

	this->m_critical.lock();

	if(this->m_QueueInfo.empty() == 0)
	{
		(*lpInfo) = this->m_QueueInfo.front();

		this->m_QueueInfo.pop();

		result = 1;
	}

	this->m_critical.unlock();

	return result;
}
