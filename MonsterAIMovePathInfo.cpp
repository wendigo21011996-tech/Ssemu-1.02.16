// MonsterAIMovePathInfo.cpp: implementation of the CMonsterAIMovePathInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIMovePathInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIMovePathInfo::CMonsterAIMovePathInfo() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->Reset();

	#endif
}

void CMonsterAIMovePathInfo::Reset() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_Type = -1;
	this->m_MapNumber = -1;
	this->m_PathX = -1;
	this->m_PathY = -1;

	#endif
}
