// Tick.cpp: implementation of the CTick class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Tick.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTick::CTick() // OK
{
	this->TickCount = GetTickCount();
}

CTick::~CTick() // OK
{

}

void CTick::Set(DWORD time) // OK
{
	this->TickCount = GetTickCount();

	while(true)
	{
		if((GetTickCount()-this->TickCount) > time)
		{
			break;
		}
	}
}