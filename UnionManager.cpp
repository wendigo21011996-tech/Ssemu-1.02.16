// UnionManager.cpp: implementation of the CUnionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UnionManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnionManager::CUnionManager() // OK
{
	this->m_UnionInfoList.clear();
}

CUnionManager::~CUnionManager() // OK
{
	for(std::map<int,UNION_INFO*>::iterator it = this->m_UnionInfoList.begin(); it != this->m_UnionInfoList.end(); ++it)
    {
        delete it->second;
    }

    this->m_UnionInfoList.clear();
}

void CUnionManager::AddUnion(int index,char* name) // OK
{
	if(this->GetUnion(index) != 0)
	{
		return;
	}

	UNION_INFO* lpUnion = new UNION_INFO;

	lpUnion->Index = index;

	memcpy(lpUnion->Name,name,sizeof(lpUnion->Name));
		
	this->m_UnionInfoList[index] = lpUnion;
}

void CUnionManager::DelUnion(int index) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion != 0)
	{
		this->m_UnionInfoList.erase(index);
		delete lpUnion;
	}
}

UNION_INFO* CUnionManager::GetUnion(int index) // OK
{
	std::map<int,UNION_INFO*>::iterator it = this->m_UnionInfoList.find(index);

	if(it != this->m_UnionInfoList.end())
	{
		return it->second;
	}

	return 0;
}

int CUnionManager::GetGuildRelationShip(int index,int bindex) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion != 0)
	{
		if((std::find(lpUnion->UnionMemberList.begin(), lpUnion->UnionMemberList.end(),bindex) != lpUnion->UnionMemberList.end()) != 0)
		{
			return GUILD_RELATION_UNION;
		}

		if((std::find(lpUnion->RivalMemberList.begin(), lpUnion->RivalMemberList.end(),bindex) != lpUnion->RivalMemberList.end()) != 0)
		{
			return GUILD_RELATION_RIVAL;
		}
	}
	
	return GUILD_RELATION_NONE;
}

int CUnionManager::GetGuildRelationShipCount(int index,int type) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion == 0)
	{
		if(type == GUILD_RELATION_UNION)
		{
			return (int)lpUnion->UnionMemberList.size();
		}
		else if(type == GUILD_RELATION_RIVAL)
		{
			return (int)lpUnion->RivalMemberList.size();
		}
	}

	return GUILD_RELATION_NONE;
}

bool CUnionManager::GetUnionName(int index,char* name) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion == 0)
	{
		return 0;
	}

	memcpy(name,lpUnion->Name,sizeof(lpUnion->Name));
	return 1;
}

bool CUnionManager::GetGuildUnionMemberList(int index,int& count,int* list) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion == 0)
	{
		return 0;
	}

	for(std::vector<int>::iterator it = lpUnion->UnionMemberList.begin(); it != lpUnion->UnionMemberList.end(); it++)
	{
		list[count++] = *it;
	}

	return 1;
}

void CUnionManager::SetGuildUnionMemberList(int index,int count,int const*list) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion != 0)
	{
		lpUnion->UnionMemberList.clear();

		for( int n=0;n< count;n++)
		{
			lpUnion->UnionMemberList.push_back(list[n]);
		}
	}
}

void CUnionManager::SetGuildRivalMemberList(int index,int count,int const*list) // OK
{
	UNION_INFO* lpUnion = this->GetUnion(index);

	if(lpUnion != 0)
	{
		lpUnion->RivalMemberList.clear();

		for( int n=0;n< count;n++)
		{
			lpUnion->RivalMemberList.push_back(list[n]);
		}
	}
}