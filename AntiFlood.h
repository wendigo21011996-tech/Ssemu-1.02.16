// AntiFlood.h: interface for the CAntiFlood class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct ANTI_FLOOD_INFO
{
	DWORD Time;
	int Count;
	bool Blocked;
	DWORD BlockTime;
};

class CAntiFlood
{
	CAntiFlood();
	virtual ~CAntiFlood();
	SingletonInstance(CAntiFlood)
public:
	bool ConnectionCheck(char* IpAddress);
private:
	std::map<std::string,ANTI_FLOOD_INFO> m_AntiFloodInfo;
};

#define gAntiFlood SingNull(CAntiFlood)