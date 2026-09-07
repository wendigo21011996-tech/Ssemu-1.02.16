// KanturuObjInfo.cpp: implementation of the CKanturuObjInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuObjInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuObjInfo::CKanturuObjInfo() // OK
{

}

CKanturuObjInfo::~CKanturuObjInfo() // OK
{

}

void CKanturuObjInfo::Reset() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_ObjCount = 0;

	for(int n=0;n < MAX_KANTURU_OBJ_INDEX;n++)
	{
		this->m_ObjIndex[n] = -1;
	}
	
	#endif
}

bool CKanturuObjInfo::AddObj(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->m_ObjCount < 0 || this->m_ObjCount >= MAX_KANTURU_OBJ_INDEX)
	{
		return 0;
	}

	this->m_ObjIndex[this->m_ObjCount++] = aIndex;
	return 1;
	
	#else

	return 0;
	
	#endif
}

int CKanturuObjInfo::GetCount() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_ObjCount;

	#else

	return 0;
	
	#endif
}
