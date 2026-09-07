// Script.h: interface for the CScript class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CriticalSection.h"

class CScript
{
public:
	CScript();
	virtual ~CScript();
	void Load(char* path);
	void Execute(char* func,char* sig,...);
private:
	lua_State* m_luaState;
	CCriticalSection m_critical;
};