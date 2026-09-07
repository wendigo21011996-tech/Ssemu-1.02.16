// ConnectionManager.h: interface for the CConnectionManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_HARDWARE_ID_INFO_RECV
{
	PSBMSG_HEAD header; // C1:F3:09
	char HardwareId[45];
};

//**********************************************//
//**********************************************//
//**********************************************//

struct CONNECTION_TIME_INFO
{
	DWORD TickCount;
};

class CConnectionManager
{
	CConnectionManager();
	virtual ~CConnectionManager();
	SingletonInstance(CConnectionManager)
public:
	bool CheckMaxConnection(int aIndex);
	void CGHardwareIdRecv(PMSG_HARDWARE_ID_INFO_RECV* lpMsg, int aIndex);
private:
	std::map<std::string,CONNECTION_TIME_INFO> m_ConnectionTimeInfo;
};

#define gConnectionManager SingNull(CConnectionManager)