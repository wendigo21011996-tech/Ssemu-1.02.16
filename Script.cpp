// Script.cpp: implementation of the CScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Script.h"
#include "Log.h"
#include "LuaFunction.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScript::CScript() // OK
{
	this->m_luaState = luaL_newstate();

	luaL_openlibs(this->m_luaState);
	lua_gc(this->m_luaState,LUA_GCCOLLECT,0);

	InitLuaFunction(this->m_luaState);
}

CScript::~CScript() // OK
{
	if(this->m_luaState != NULL)
	{
		lua_close(this->m_luaState);
		this->m_luaState = NULL;
	}
}

void CScript::Load(char* path) // OK
{
	this->m_critical.lock();

	if(luaL_loadfile(this->m_luaState,path) != 0)
    {
		LogAdd(LOG_RED,"[Script] Could not load '%s'. %s",path,lua_tostring(this->m_luaState,-1));
		gLog->Output(LOG_SCRIPT,"[Script] Could not load '%s'. %s",path,lua_tostring(this->m_luaState,-1));
		lua_close(this->m_luaState);
		this->m_critical.unlock();
		return;
    }

	if(lua_pcall(this->m_luaState,0,LUA_MULTRET,0) != 0)
    {
		LogAdd(LOG_RED,"[Script] Error in Lua-file. %s",lua_tostring(this->m_luaState,-1));
		gLog->Output(LOG_SCRIPT,"[Script] Error in Lua-file. %s",lua_tostring(this->m_luaState,-1));
		lua_close(this->m_luaState);
		this->m_critical.unlock();
		return;
    }

	this->m_critical.unlock();
}

void CScript::Execute(char* function,char* param,...) // OK
{
	this->m_critical.lock();

	va_list	arg;
	va_start(arg,param);

	lua_getglobal(this->m_luaState,function);

	int InCount = 0;

	while(true)
	{
		if(*param)
		{
			switch(*param++)
			{
				case '>':
					goto OUTPUT_JUMP;
				case 'i':
					lua_pushnumber(this->m_luaState,va_arg(arg,int));
					InCount++;
					continue;
				case 's':
					lua_pushstring(this->m_luaState,va_arg(arg,char*));
					InCount++;
					continue;
			}
		}
	}

	OUTPUT_JUMP:

	int OutCount = strlen(param);

	if(lua_pcall(this->m_luaState,InCount,OutCount,0) != 0)
	{
		LogAdd(LOG_RED,"[Script] Error running function `%s': %s",function,lua_tolstring(this->m_luaState,-1,0));
		gLog->Output(LOG_SCRIPT,"[Script] Error running function `%s': %s", lua_tolstring(this->m_luaState,-1,0));
		this->m_critical.unlock();
		return;
	}
			
	int pos = -OutCount;

	while(*param)
	{
		switch(*param++)
		{
			case 'i':
				if(lua_isnumber(this->m_luaState,pos) == 0)
				{
					LogAdd(LOG_RED,"[Script] function `%s': wrong result type [%c][%d]",function,*(param-1),pos);
					gLog->Output(LOG_SCRIPT,"[Script] function `%s': wrong result type [%c][%d]",function,*(param-1),pos);
					this->m_critical.unlock();
					return;
				}
				*va_arg(arg,int*) = lua_tointeger(this->m_luaState,pos);
				break;
			case 's':
				if(lua_isstring(this->m_luaState,pos) == 0)
				{
					LogAdd(LOG_RED,"[Script] function `%s': wrong result type [%c][%d]",function,*(param-1),pos);
					gLog->Output(LOG_SCRIPT,"[Script] function `%s': wrong result type [%c][%d]",function,*(param-1),pos);
					this->m_critical.unlock();
					return;
				}
				*va_arg(arg,const char**) = lua_tostring(this->m_luaState,pos);
				break;
		}

		pos++;
	}

	if(OutCount != 0)
	{
		lua_settop(this->m_luaState,-OutCount-1);
	}

	this->m_critical.unlock();
}