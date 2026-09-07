// CustomWindowTime.h: interface for the CCustomWindowTime class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"

struct PMSG_EVENT_TIME_LIST_SEND
{
	PSWMSG_HEAD header; // C2:F3:EF
	BYTE count;
};

struct EVENT_TIME_INFO
{
	char name[32];
	BYTE status;
	DWORD time;
};

enum eEventTimeState
{
	EVENT_STATE_BLANK = 0,
	EVENT_STATE_STAND = 1,
	EVENT_STATE_OPEN = 2,
	EVENT_STATE_START = 3,
};

class CCustomWindowTime
{
	CCustomWindowTime();
	virtual ~CCustomWindowTime();
	SingletonInstance(CCustomWindowTime)
public:
	void GCEventTimeSend(int aIndex);
};

#define gCustomWindowTime SingNull(CCustomWindowTime)