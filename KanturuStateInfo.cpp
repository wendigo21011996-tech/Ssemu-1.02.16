// KanturuStateInfo.cpp: implementation of the CKanturuStateInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuStateInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuStateInfo::CKanturuStateInfo() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->ResetTimeInfo();
	
	#endif
}

CKanturuStateInfo::~CKanturuStateInfo() // OK
{

}

void CKanturuStateInfo::ResetTimeInfo() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->SetStateInfo(0);

	this->SetCondition(0);

	this->SetValue(0);
	
	#endif
}

void CKanturuStateInfo::SetStateInfo(int state) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_State = state;
	
	#endif
}

void CKanturuStateInfo::SetCondition(int condition) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_Condition = condition;
	
	#endif
}

void CKanturuStateInfo::SetValue(int value) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_Value = value;
	
	#endif
}

void CKanturuStateInfo::SetAppliedTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_AppliedTickCount = GetTickCount();
	
	#endif
}

void CKanturuStateInfo::SetConditionAppliedTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_AppliedTickCount = ((this->GetCondition()==0)?this->m_AppliedTickCount:GetTickCount());
	
	#endif
}

int CKanturuStateInfo::GetCondition() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_Condition;
	
	#else

	return 0;

	#endif
}

int CKanturuStateInfo::GetValue() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_Value;
	
	#else

	return 0;

	#endif
}

int CKanturuStateInfo::GetAppliedTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_AppliedTickCount;
	
	#else

	return 0;

	#endif
}

int CKanturuStateInfo::GetElapsedTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return (GetTickCount()-this->GetAppliedTime());
	
	#else

	return 0;

	#endif
}

int CKanturuStateInfo::GetRemainTime() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return ((this->GetCondition()==0)?0:(((this->m_Value-this->GetElapsedTime())<=0)?0:((this->m_Value-this->GetElapsedTime())/1000)));
	
	#else

	return 0;

	#endif
}

int CKanturuStateInfo::IsTimeOut() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return ((this->GetRemainTime()>0)?0:1);

	#else

	return 0;

	#endif
}
