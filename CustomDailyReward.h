// CustomDailyReward.h: interface for the CCustomDailyReward class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_DAILY_REWARD_INFO_RECV
{
	PBMSG_HEAD header; // C1:16
	WORD aIndex;
	char account[11];
	char name[11];
	int index;
	BYTE result;
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_DAILY_REWARD_INFO_SEND
{
	PBMSG_HEAD header; // C1:16
	WORD aIndex;
	char account[11];
	char name[11];
	int index;
	int MinOnlineTime;
};

//**********************************************//
//**********************************************//
//**********************************************//

struct DAILY_REWARD_INFO
{
	int Index;
	int Month;
	int Day;
	int MinOnlineTime;
	int MinLevel;
	int MaxLevel;
	int MinReset;
	int MaxReset;
	int AccountLevel;
	int MessageReward;
	int BagSpecial;
};

class CCustomDailyReward
{
	CCustomDailyReward();
	virtual ~CCustomDailyReward();
	SingletonInstance(CCustomDailyReward)
public:
	void Load(char* path);
	void DGDailyRewardCheckRecv(SDHP_DAILY_REWARD_INFO_RECV* lpMsg);
	void GDDailyRewardCheckSend(int aIndex);
private:
	std::map<int,DAILY_REWARD_INFO> m_CustomDailyReward;
};

#define gCustomDailyReward SingNull(CCustomDailyReward)