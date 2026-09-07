// CastleSiege.h: interface for the CCastleSiege class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Protocol.h"

#define MAX_CASTLE_SIEGE_GUILD 3
#define MAX_CASTLE_SIEGE_CROWN_TIME 30000

#define CS_UPGRADE_RANGE(x) (((x)<1)?0:((x)>3)?0:1)

enum eCastleSiegeState
{
	CS_STATE_BLANK = -1,
	CS_STATE_IDLE_1 = 0,
	CS_STATE_SIGN1 = 1,
	CS_STATE_IDLE_2 = 2,
	CS_STATE_SIGN2 = 3,
	CS_STATE_IDLE_3 = 4,
	CS_STATE_NOTIFY = 5,
	CS_STATE_READY = 6,
	CS_STATE_START = 7,
	CS_STATE_END = 8,
	CS_STATE_END_CYCLE = 9,
};

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//

struct PMSG_CASTLE_SIEGE_GIVEUP_GUILD_RECV
{
	PSBMSG_HEAD header; // C1:B2:02
	BYTE GiveUp;
};

struct PMSG_CASTLE_SIEGE_NPC_BUY_RECV
{
	PSBMSG_HEAD header; // C1:B2:05
	int NpcClass;
	int NpcIndex;
};

struct PMSG_CASTLE_SIEGE_NPC_REPAIR_RECV
{
	PSBMSG_HEAD header; // C1:B2:06
	int NpcClass;
	int NpcIndex;
};

struct PMSG_CASTLE_SIEGE_NPC_UPGRADE_RECV
{
	PSBMSG_HEAD header; // C1:B2:07
	int NpcClass;
	int NpcIndex;
	int UpgradeType;
	int UpgradeValue;
};

struct PMSG_CS_TAX_RATE_CHANGE_RECV
{
	PSBMSG_HEAD header; // C1:B2:09
	BYTE TaxType;
	BYTE TaxRate[4];
};

struct PMSG_CASTLE_SIEGE_MONEY_OUT_RECV
{
	PSBMSG_HEAD header; // C1:B2:10
	BYTE money[4];
};

struct PMSG_CASTLE_SIEGE_GATE_OPERATE_RECV
{
	PSBMSG_HEAD header; // C1:B2:12
	BYTE operation;
	BYTE index[2];
};

struct PMSG_CASTLE_SIEGE_COMMAND_REQUEST_RECV
{
	PSBMSG_HEAD header; // C1:B2:1D
	BYTE team;
	BYTE x;
	BYTE y;
	BYTE command;
};

struct PMSG_CASTLE_SIEGE_HUNT_ZONE_ALLOW_RECV
{
	PSBMSG_HEAD header; // C1:B2:1F
	BYTE allow;
};

struct PMSG_CASTLE_SIEGE_NPC_LIST_RECV
{
	PBMSG_HEAD  header; // C1:B3
	BYTE code;
};

struct PMSG_CASTLE_SIEGE_LAND_OF_TRIAL_MOVE_RECV
{
	PSBMSG_HEAD header; // C1:B9:05
	int price;
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//

struct PMSG_CASTLE_SIEGE_STATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:00
	BYTE result;
	BYTE state;
	BYTE StartYear[2];
	BYTE StartMonth;
	BYTE StartDay;
	BYTE StartHour;
	BYTE StartMinute;
	BYTE EndYear[2];
	BYTE EndMonth;
	BYTE EndDay;
	BYTE EndHour;
	BYTE EndMinute;
	BYTE SiegeStartYear[2];
	BYTE SiegeStartMonth;
	BYTE SiegeStartDay;
	BYTE SiegeStartHour;
	BYTE SiegeStartMinute;
	char CastleOwner[8];
	char MasterName[10];
	BYTE RemainTime[4];
};

struct PMSG_CASTLE_SIEGE_REGISTER_GUILD_SEND
{
	PSBMSG_HEAD header; // C1:B2:01
	BYTE result;
	char GuildName[8];
};

struct PMSG_CASTLE_SIEGE_GIVEUP_GUILD_SEND
{
	PSBMSG_HEAD header; // C1:B2:02
	BYTE result;
	BYTE giveup;
	char GuildName[8];
};

struct PMSG_CASTLE_SIEGE_REGISTER_GUILD_INFO_SEND
{
	PSBMSG_HEAD header; // C1:B2:03
	BYTE result;
	char GuildName[8];
	BYTE MarkCount[4];
	BYTE giveup;
	BYTE position;
};

struct PMSG_CASTLE_SIEGE_REGISTER_MARK_SEND
{
	PSBMSG_HEAD header; // C1:B2:04
	BYTE result;
	char GuildName[8];
	BYTE MarkCount[4];
};

struct PMSG_CASTLE_SIEGE_NPC_BUY_SEND
{
	PSBMSG_HEAD header; // C1:B2:05
	BYTE result;
	int NpcClass;
	int NpcIndex;
};

struct PMSG_CASTLE_SIEGE_NPC_REPAIR_SEND
{
	PSBMSG_HEAD header; // C1:B2:06
	BYTE result;
	int NpcClass;
	int NpcIndex;
	int Life;
	int MaxLife;
};

struct PMSG_CASTLE_SIEGE_NPC_UPGRADE_SEND
{
	PSBMSG_HEAD header; // C1:B2:07
	BYTE result;
	int NpcClass;
	int NpcIndex;
	int UpgradeType;
	int UpgradeValue;
};

struct PMSG_CASTLE_SIEGE_TAX_MONEY_INFO_SEND
{
	PSBMSG_HEAD header; // C1:B2:08
	BYTE result;
	BYTE TaxRateChaos;
	BYTE TaxRateStore;
	BYTE TributeMoney[8];
};

struct PMSG_CASTLE_SIEGE_TAX_RATE_INFO_SEND
{
	PSBMSG_HEAD header; // C1:B2:09
	BYTE result;
	BYTE TaxType;
	BYTE TaxRate[4];
};

struct PMSG_CASTLE_SIEGE_MONEY_OUT_SEND
{
	PSBMSG_HEAD header; // C1:B2:10
	BYTE result;
	BYTE money[8];
};

struct PMSG_CASTLE_SIEGE_GATE_STATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:11
	BYTE result;
	BYTE index[2];
};

struct PMSG_CASTLE_SIEGE_GATE_OPERATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:12
	BYTE result;
	BYTE operation;
	BYTE index[2];
};

struct PMSG_CASTLE_SIEGE_GATE_OPERATE_STATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:13
	BYTE operation;
	BYTE index[2];
};

struct PMSG_CASTLE_SIEGE_SWITCH_STATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:14
	BYTE index[2];
	BYTE target[2];
	BYTE state;
};

struct PMSG_CASTLE_SIEGE_CROWN_ACCESS_SEND
{
	PSBMSG_HEAD header; // C1:B2:15
	BYTE state;
	int time;
};

struct PMSG_CASTLE_SIEGE_CROWN_STATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:16
	BYTE state;
};

struct PMSG_CASTLE_SIEGE_START_STATE_SEND
{
	PSBMSG_HEAD header; // C1:B2:17
	BYTE state;
};

struct PMSG_CASTLE_SIEGE_NOTIFY_PROGRESS_SEND
{
	PSBMSG_HEAD header; // C1:B2:18
	BYTE state;
	char GuildName[8];
};

struct PMSG_CASTLE_SIEGE_JOIN_SIDE_SEND
{
	PSBMSG_HEAD header; // C1:B2:19
	BYTE side;
};

struct PMSG_CASTLE_SIEGE_MINI_MAP_OPEN_SEND
{
	PSBMSG_HEAD header; // C1:B2:1B
	BYTE result;
};

struct PMSG_CASTLE_SIEGE_COMMAND_REQUEST_SEND
{
	PSBMSG_HEAD header; // C1:B2:1D
	BYTE team;
	BYTE x;
	BYTE y;
	BYTE command;
};

struct PMSG_CASTLE_SIEGE_REMAIN_TIME_SEND
{
	PSBMSG_HEAD header; // C1:B2:1E
	BYTE hour;
	BYTE minute;
};

struct PMSG_CASTLE_SIEGE_ENABLE_HUNT_SEND
{
	PSBMSG_HEAD header; // C1:B2:1F
	BYTE result;
	BYTE allow;
};

struct PMSG_CASTLE_SIEGE_SWITCH_INFO_SEND
{
	PSBMSG_HEAD header; // C1:B2:20
	BYTE index[2];
	BYTE State;
	BYTE JoinSide;
	char GuildName[8];
	char Name[10];
};

struct PMSG_CASTLE_SIEGE_NPC_LIST_SEND
{
	PWMSG_HEAD header; // C1:B3
	BYTE result;
	int count;
};

struct PMSG_CASTLE_SIEGE_NPC_LIST
{
	int NpcClass;
	int NpcIndex;
	int DefenseLevel;
	int RegenLevel;
	int LifeLevel;
	int Life;
	BYTE X;
	BYTE Y;
	BYTE Live;
};

struct PMSG_CASTLE_SIEGE_REGISTER_LIST_SEND
{
	PWMSG_HEAD header; // C1:B4
	BYTE result;
	int count;
};

struct PMSG_CASTLE_SIEGE_REGISTER_INFO
{
	char GuildName[8];
	BYTE MarkCount[4];
	BYTE giveup;
	BYTE position;
};

struct PMSG_CASTLE_SIEGE_GUILD_LIST_SEND
{
	PWMSG_HEAD header; // C3:B5
	BYTE result;
	int count;
};

struct PMSG_CASTLE_SIEGE_GUILD_LIST
{
	BYTE Side;
	BYTE Involved;
	char GuildName[8];
	int Score;
};

struct PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE_SEND
{
	PWMSG_HEAD header; // C1:B6
	int count;
};

struct PMSG_CASTLE_SIEGE_MINI_MAP_SINGLE
{
	BYTE x;
	BYTE y;
};

struct PMSG_CASTLE_SIEGE_OWNER_MARK_SEND
{
	PSBMSG_HEAD header;	// C1:B9:02
	BYTE Mark[32];
};

struct PMSG_CASTLE_SIEGE_LAND_OF_TRIALS_GUARD_SEND
{
	PSBMSG_HEAD header;	// C1:B9:03
	BYTE result;
	BYTE allowed;
	int CurrPrice;
	int MaxPrice;
	int UnitPrice;
};

struct PMSG_CASTLE_SIEGE_LAND_OF_TRIAL_MOVE_SEND
{
	PSBMSG_HEAD header;	// C1:B9:05
	BYTE result;
};

struct PMSG_CASTLE_SIEGE_MINI_MAP_SEND
{
	PWMSG_HEAD header; // C1:BB
	int count;
};

struct PMSG_CASTLE_SIEGE_MINI_MAP
{
	BYTE npc;
	BYTE x;
	BYTE y;
};

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_CASTLE_SIEGE_STATE_RECV
{
	PSBMSG_HEAD header; // C1:80:01
	int result;
	WORD MapServerGroup;
	int aIndex;
	char CastleOwner[9];
	char MasterName[10];
};

struct SDHP_CASTLE_SIEGE_NPC_BUY_RECV
{
	PSBMSG_HEAD header; // C1:80:03
	int result;
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int BuyCost;
};

struct SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV
{
	PSBMSG_HEAD header; // C1:80:04
	int result;
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int Life;
	int MaxLife;
	int RepairCost;
};

struct SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV
{
	PSBMSG_HEAD header; // C1:80:05
	int result;
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int UpgradeType;
	int UpgradeValue;
};

struct SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV
{
	PSBMSG_HEAD header; // C1:80:06
	int result;
	WORD MapServerGroup;
	int aIndex;
	QWORD TributeMoney;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
};

struct SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV
{
	PSBMSG_HEAD header; // C1:80:07
	int result;
	WORD MapServerGroup;
	int aIndex;
	int TaxType;
	int TaxRate;
};

struct SDHP_CASTLE_SIEGE_MONEY_OUT_RECV
{
	PSBMSG_HEAD header; // C1:80:08
	int result;
	WORD MapServerGroup;
	int aIndex;
	int Money;
	QWORD LastMoney;
};

struct SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV
{
	PSBMSG_HEAD header; // C1:80:0A
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int MarkCount;
	int Position;
	int GiveUp;
};

struct SDHP_CASTLE_SIEGE_REGISTER_RECV
{
	PSBMSG_HEAD header; // C1:80:0D
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_RESET_STATE_RECV
{
	PSBMSG_HEAD header; // C1:80:0E
	int result;
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV
{
	PSBMSG_HEAD header; // C1:80:10
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int MarkCount;
};

struct SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV
{
	PSBMSG_HEAD header; // C1:80:12
	int result;
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int GiveUp;
	int MarkCount;
};

struct SDHP_CASTLE_SIEGE_SYNC_STATE_RECV
{
	PSBMSG_HEAD header; // C1:80:17
	WORD MapServerGroup;
	int State;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
	char CastleOwner[9];
};

struct SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV
{
	PSBMSG_HEAD header; // C1:80:18
	int result;
	WORD MapServerGroup;
	QWORD money;
};

struct SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV
{
	PSBMSG_HEAD header; // C1:80:19
	int result;
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_INIT_DATA_RECV
{
	PWMSG_HEAD header; // C1:81
	int result;
	WORD MapServerGroup;
	WORD StartYear;
	WORD StartMonth;
	WORD StartDay;
	WORD EndYear;
	WORD EndMonth;
	WORD EndDay;
	BYTE CastleGuildListLoad;
	BYTE CastleSiegeEnded;
	BYTE CastleOccupied;
	char CastleOwner[9];
	QWORD TributeMoney;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
	int FirstRun;
	int count;
};

struct SDHP_CASTLE_SIEGE_INIT_DATA
{
	int NpcClass;
	int NpcIndex;
	int DefenseLevel;
	int RegenLevel;
	int LifeLevel;
	int Life;
	BYTE X;
	BYTE Y;
	BYTE Dir;
};

struct SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV
{
	PWMSG_HEAD header; // C1:83
	int result;
	WORD MapServerGroup;
	int aIndex;
	int count;
};

struct SDHP_CASTLE_SIEGE_REGISTER_LIST
{
	char GuildName[9];
	int MarkCount;
	int GiveUp;
	int Position;
};

struct SDHP_CASTLE_SIEGE_NPC_CREATE_RECV
{
	PBMSG_HEAD header; // C1:84
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV
{
	PWMSG_HEAD header; // C1:85
	int result;
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST
{
	char GuildName[9];
	int MarkCount;
	int MemberCount;
	int MasterLevel;
	int Position;
};

struct SDHP_CASTLE_SIEGE_UNION_LIST_RECV
{
	PWMSG_HEAD header; // C1:86
	int result;
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_UNION_LIST
{
	char GuildName[9];
	int Side;
};

struct SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV
{
	PBMSG_HEAD header; // C1:87
	int result;
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV
{
	PWMSG_HEAD header; // C1:88
	int result;
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST
{
	char GuildName[9];
	int Side;
	int Involved;
	int Score;
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_CASTLE_SIEGE_STATE_SEND
{
	PSBMSG_HEAD header; // C1:80:01
	WORD MapServerGroup;
	int aIndex;
};

struct SDHP_CASTLE_SIEGE_NPC_BUY_SEND
{
	PSBMSG_HEAD header; // C1:80:03
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int DefenseLevel;
	int RegenLevel;
	int LifeLevel;
	int Life;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	int BuyCost;
};

struct SDHP_CASTLE_SIEGE_NPC_REPAIR_SEND
{
	PSBMSG_HEAD header; // C1:80:04
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int RepairCost;
};

struct SDHP_CASTLE_SIEGE_NPC_UPGRADE_SEND
{
	PSBMSG_HEAD header; // C1:80:05
	WORD MapServerGroup;
	int aIndex;
	int NpcClass;
	int NpcIndex;
	int UpgradeType;
	int UpgradeValue;
};

struct SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_SEND
{
	PSBMSG_HEAD header; // C1:80:06
	WORD MapServerGroup;
	int aIndex;
};

struct SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_SEND
{
	PSBMSG_HEAD header; // C1:80:07
	WORD MapServerGroup;
	int aIndex;
	int TaxType;
	int TaxRate;
};

struct SDHP_CASTLE_SIEGE_MONEY_OUT_SEND
{
	PSBMSG_HEAD header; // C1:80:08
	WORD MapServerGroup;
	int aIndex;
	int Money;
};

struct SDHP_CASTLE_SIEGE_DATE_CHANGE_SEND
{
	PSBMSG_HEAD header; // C1:80:09
	WORD MapServerGroup;
	WORD StartYear;
	WORD StartMonth;
	WORD StartDay;
	WORD EndYear;
	WORD EndMonth;
	WORD EndDay;
};

struct SDHP_CASTLE_SIEGE_REGISTER_INFO_SEND
{
	PSBMSG_HEAD header; // C1:80:0A
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_END_SEND
{
	PSBMSG_HEAD header; // C1:80:0B
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_OWNER_CHANGE_SEND
{
	PSBMSG_HEAD header; // C1:80:0C
	WORD MapServerGroup;
	BYTE CastleOccupied;
	char CastleOwner[9];
};

struct SDHP_CASTLE_SIEGE_REGISTER_SEND
{
	PSBMSG_HEAD header; // C1:80:0D
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_RESET_STATE_SEND
{
	PSBMSG_HEAD header; // C1:80:0E
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_REGISTER_MARK_SEND
{
	PSBMSG_HEAD header; // C1:80:10
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
};

struct SDHP_CASTLE_SIEGE_GIVEUP_GUILD_SEND
{
	PSBMSG_HEAD header; // C1:80:12
	WORD MapServerGroup;
	int aIndex;
	char GuildName[9];
	int GiveUp;
};

struct SDHP_CASTLE_SIEGE_NPC_REMOVE_SEND
{
	PSBMSG_HEAD header; // C1:80:16
	WORD MapServerGroup;
	int NpcClass;
	int NpcIndex;
};

struct SDHP_CASTLE_SIEGE_SYNC_STATE_SEND
{
	PSBMSG_HEAD header; // C1:80:17
	WORD MapServerGroup;
	int State;
	int TaxRateChaos;
	int TaxRateStore;
	int TaxRateHunt;
	char CastleOwner[9];
};

struct SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_SEND
{
	PSBMSG_HEAD header; // C1:80:18
	WORD MapServerGroup;
	int money;
};

struct SDHP_CASTLE_SIEGE_RESET_TAX_INFO_SEND
{
	PSBMSG_HEAD header; // C1:80:19
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_PARTICIPANT_CLEAR_SEND
{
	PSBMSG_HEAD header; // C1:80:1A
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_REGISTERED_CLEAR_SEND
{
	PSBMSG_HEAD header; // C1:80:1B
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_INIT_DATA_SEND
{
	PBMSG_HEAD header; // C1:81
	WORD MapServerGroup;
	int CastleCycle;
};

struct SDHP_CASTLE_SIEGE_REGISTER_LIST_SEND
{
	PBMSG_HEAD header; // C1:83
	WORD MapServerGroup;
	int aIndex;
};

struct SDHP_CASTLE_SIEGE_NPC_CREATE_SEND
{
	PWMSG_HEAD header; // C1:84
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_NPC_CREATE
{
	int NpcClass;
	int NpcIndex;
	int NpcDefense;
	int NpcRegen;
	int NpcMaxLife;
	int NpcLife;
	BYTE X;
	BYTE Y;
	BYTE Dir;
};

struct SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_SEND
{
	PBMSG_HEAD header; // C1:85
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_LOAD_UNION_LIST_SEND
{
	PWMSG_HEAD header; // C1:86
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_LOAD_UNION_LIST
{
	char GuildName[9];
	int Side;
};

struct SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_SEND
{
	PWMSG_HEAD  header; // C1:87
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST
{
	char GuildName[9];
	int Side;
	int Involved;
	int Score;
};

struct SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_SEND
{
	PBMSG_HEAD header; // C1:88
	WORD MapServerGroup;
};

struct SDHP_CASTLE_SIEGE_NPC_SAVE_SEND
{
	PWMSG_HEAD header; // C1:89
	WORD MapServerGroup;
	int count;
};

struct SDHP_CASTLE_SIEGE_NPC_SAVE
{
	int NpcClass;
	int NpcIndex;
	int NpcDefense;
	int NpcRegen;
	int NpcMaxLife;
	int NpcLife;
	BYTE X;
	BYTE Y;
	BYTE Dir;
};

//**********************************************//
//**********************************************//
//**********************************************//

struct CASTLE_STATE_TIME_DATA
{
	CASTLE_STATE_TIME_DATA() // OK
	{
		this->Clear();
	}

	void Clear() // OK
	{
		this->Active = false;
		this->State = CS_STATE_BLANK;
		this->Day = -1;
		this->Hour = -1;
		this->Minute = -1;
		this->Second = 0;
	}

	int Active;
	int State;
	int Day;
	int Hour;
	int Minute;
	int Second;
};

struct CASTLE_SIEGE_NPC_DATA
{
	CASTLE_SIEGE_NPC_DATA() // OK
	{
		this->Clear();
		this->SwitchIndex = -1;
	}

	void Clear() // OK
	{
		this->Class = -1;
		this->Index = -1;
		this->aIndex = -1;
		this->Live = 0;
		this->Side = 1;
		this->DbSave = 0;
		this->DefenseLevel = 0;
		this->RegenLevel = 0;
		this->LifeLevel = 0;
		this->Life = 0;
		this->X = -1;
		this->Y = -1;
		this->Dir = -1;
		this->DefaultDefenseLevel = 0;
		this->DefaultRegenLevel = 0;
		this->DefaultLifeLevel = 0;
		this->DefaultLife = 0;
		this->DefaultX = -1;
		this->DefaultY = -1;
		this->DefaultDir = -1;
	}

	void SetBaseValue() // OK
	{
		this->DefenseLevel = this->DefaultDefenseLevel;
		this->RegenLevel = this->DefaultRegenLevel;
		this->LifeLevel = this->DefaultLifeLevel;
		this->Life = this->DefaultLife;
		this->X = this->DefaultX;
		this->Y = this->DefaultY;
		this->Dir = this->DefaultDir;
	}

	int Class;
	int Index;
	int aIndex;
	int SwitchIndex;
	int Live;
	int Side;
	int DbSave;
	int DefenseLevel;
	int RegenLevel;
	int LifeLevel;
	int Life;
	int X;
	int Y;
	int Dir;
	int DefaultDefenseLevel;
	int DefaultRegenLevel;
	int DefaultLifeLevel;
	int DefaultLife;
	int DefaultX;
	int DefaultY;
	int DefaultDir;
};

struct CASTLE_SIEGE_GUILD_FINAL
{
	int Side;
	int Involved;
	int Score;
};

struct CASTLE_SIEGE_GUILD_DATA
{
	char GuildName[9];
	int MarkCount;
	int MemberCount;
	int MasterLevel;
	int Position;
	DWORD TotalScore;
};

struct CASTLE_SIEGE_MINI_MAP
{
	BYTE x;
	BYTE y;
};

struct CASTLE_SIEGE_MINI_MAP_DATA
{
	CASTLE_SIEGE_MINI_MAP_DATA() // OK
	{
		this->Clear();
	}

	void Clear()
	{
		this->GuildMasterIndex = -1;
		memset(this->GuildName,0,sizeof(this->GuildName));
		this->PointsCount = 0;

		for (int n = 0; n < 1000; n++)
		{
			this->MinimapPoints[n].x = 0;
			this->MinimapPoints[n].y = 0;
		}
	}

	int GuildMasterIndex;
	char GuildName[9];
	int PointsCount;
	CASTLE_SIEGE_MINI_MAP MinimapPoints[1000]; // 1C
};

class CCastleSiege
{
	CCastleSiege();
	virtual ~CCastleSiege();
	SingletonInstance(CCastleSiege)
public:
	void Init();
	void Load(char* path);
	void MainProc();
	void ProcState_BLANK();
	void ProcState_IDLE1();
	void ProcState_SIGN1();
	void ProcState_IDLE2();
	void ProcState_SIGN2();
	void ProcState_IDLE3();
	void ProcState_NOTIFY();
	void ProcState_READY();
	void ProcState_START();
	void ProcState_END();
	void ProcState_END_CYCLE();
	void SetState(int state,bool reset);
	void SetState_BLANK(bool reset);
	void SetState_IDLE1(bool reset);
	void SetState_SIGN1(bool reset);
	void SetState_IDLE2(bool reset);
	void SetState_SIGN2(bool reset);
	void SetState_IDLE3(bool reset);
	void SetState_NOTIFY(bool reset);
	void SetState_READY(bool reset);
	void SetState_START(bool reset);
	void SetState_END(bool reset);
	void SetState_END_CYCLE(bool reset);
	int GetState();
	int CheckSync();
	CASTLE_SIEGE_NPC_DATA* GetNpcData(int NpcClass,int NpcIndex,int aIndex);
	void GetNextDay(SYSTEMTIME*st,int day,int hour,int minute,int second);
	int GetNextRemainTime(int state);
	int GetLeftSiegeDate();
	bool GetCastleStateTerm(SYSTEMTIME* StartTime,SYSTEMTIME* EndTime);
	void ClearAllNpc();
	void ClearNpcNoDB();
	void ClearNpcLifeStone();
	void CreateNpcDB();
	void CreateAllGateSwitch();
	int CreateNpcGateSwitch(int x,int y);
	void CheckCastleNPCLive();
	void MonsterDie(LPOBJ lpObj,bool remove);
	void AdjustDbNpcLevel();
	void UpgradeDefenseLevel(int NpcClass,int NpcIndex,int UpgradeValue);
	void UpgradeRegenLevel(int NpcClass,int NpcIndex,int UpgradeValue);
	void UpgradeLifeLevel(int NpcClass,int NpcIndex,int UpgradeValue);
	void SetCrownIndex(int aIndex);
	bool GetRegCrownAvailable();
	void SetRegCrownAvailable(bool state);
	int CheckCastleTowerEnable();
	bool CheckCsGateAlive(int aIndex);
	bool CheckCastleHasMoney(int money);
	QWORD GetCastleMoney();
	bool CheckGuildOwnCastle(char* GuildName);
	void SetGateBlockState(int x,int y,BYTE state);
	bool GetHuntZoneEnter();
	void ClearAndLoadGuildList();
	void SetPlayerJoinSide(int aIndex);
	void ResetPlayerJoinSide();
	void SetCastleJoinSide();
	bool CheckCastleOwnerMember(int aIndex);
	bool CheckCastleOwnerUnionMember(int aIndex);
	bool CheckGuardianStatueExist();
	void ChangeWinnerGuild(int side);
	void CheckMiddleWinnerGuild();
	bool CheckCastleSiegeResult();
	bool CheckUnionGuildMaster(int aIndex);
	int GetCrownUserIndex();
	void SetCrownUserIndex(int aIndex);
	bool CheckCrownAccessPosition(short X,short Y);
	void SetCrownAccessPosition(BYTE X, BYTE Y);
	int GetCrownSwitchUserIndex(int MonsterClass);
	void SetCrownSwitchUserIndex(int MonsterClass,int aIndex);
	void ResetCrownSwitchUserIndex(int MonsterClass);
	DWORD GetCrownAccessTickCount();
	void ResetCrownAccessTickCount();
	void SetCrownAccessTickCount();
	void CheckTeleportMagicAxisY(int x,int y,BYTE* ty);
	void CheckReviveNonDbNPC();
	void CheckReviveGuardianStatue();
	void RespawnPlayers(bool defense);
	bool GetRespawnCoord(short* x,short* y,BYTE side);
	void ClearCastleTowerBarrier();
	void CloseAllCastleGates();
	void OperateMiniMapWork();
	static bool ScheduleStateCompFunc(CASTLE_STATE_TIME_DATA & A,CASTLE_STATE_TIME_DATA & B);
	static bool RegGuildScoreCompFunc(CASTLE_SIEGE_GUILD_DATA & A,CASTLE_SIEGE_GUILD_DATA & B);
	void DataSendToAll(BYTE* lpMsg,int size);
	void CGCastleSiegeStateRecv(int aIndex);
	void CGCastleSiegeRegisterRecv(int aIndex);
	void CGCastleSiegeGiveUpGuildRecv(PMSG_CASTLE_SIEGE_GIVEUP_GUILD_RECV* lpMsg,int aIndex);
	void CGCastleSiegeRegisterGuildInfoRecv(int aIndex);
	void CGCastleSiegeRegisterMarkRecv(int aIndex);
	void CGCastleSiegeNpcBuyRecv(PMSG_CASTLE_SIEGE_NPC_BUY_RECV* lpMsg,int aIndex);
	void CGCastleSiegeNpcRepairRecv(PMSG_CASTLE_SIEGE_NPC_REPAIR_RECV* lpMsg,int aIndex);
	void CGCastleSiegeNpcUpgradeRecv(PMSG_CASTLE_SIEGE_NPC_UPGRADE_RECV* lpMsg,int aIndex);
	void CGCastleSiegeTaxMoneyInfoRecv(int aIndex);
	void CGCastleSiegeTaxRateChangeRecv(PMSG_CS_TAX_RATE_CHANGE_RECV* lpMsg,int aIndex);
	void CGCastleSiegeMoneyOutRecv(PMSG_CASTLE_SIEGE_MONEY_OUT_RECV* lpMsg,int aIndex);
	void CGCastleSiegeGateOperateRecv(PMSG_CASTLE_SIEGE_GATE_OPERATE_RECV* lpMsg,int aIndex);
	void CGCastleSiegeMiniMapRequestRecv(int aIndex);
	void CGCastleSiegeMiniMapCloseRecv(int aIndex);
	void CGCastleSiegeCommandRequestRecv(PMSG_CASTLE_SIEGE_COMMAND_REQUEST_RECV* lpMsg,int aIndex);
	void CGCastleSiegeHuntZoneAllowRecv(PMSG_CASTLE_SIEGE_HUNT_ZONE_ALLOW_RECV* lpMsg,int aIndex);
	void CGCastleSiegeNpcListRecv(PMSG_CASTLE_SIEGE_NPC_LIST_RECV* lpMsg,int aIndex);
	void CGCastleSiegeGuildRegisterListRecv(int aIndex);
	void CGCastleSiegeGuildListRecv(int aIndex);
	void CGCastleSiegeCastleOwnerMarkRecv(int aIndex);
	void CGCastleSiegeLandOfTrialsMoveRecv(PMSG_CASTLE_SIEGE_LAND_OF_TRIAL_MOVE_RECV* lpMsg,int aIndex);
	void GCCastleSiegeRegisterGuildResultSend(int aIndex,int result,char* GuildName);
	void GCCastleSiegeGiveupGuildResultSend(int aIndex,int result,int giveup,char* GuildName);
	void GCCastleSiegeNpcBuyResultSend(int aIndex,int result,int NpcClass,int NpcIndex);
	void GCCastleSiegeNpcRepairResultSend(int aIndex,int result,int NpcClass,int NpcIndex,int life,int maxlife);
	void GCCastleSiegeNpcUpgradeResultSend(int aIndex,int result,int NpcClass,int NpcIndex,int UpgradeType,int UpgradeValue);
	void GCCastleSiegeTaxRateRequestSend(int aIndex,int result,int type,int rate);
	void GCCastleSiegeMoneyOutResultSend(int aIndex,int result,QWORD money);
	void GCCastleSiegeGateStateSend(int aIndex,int result,int bIndex);
	void GCCastleSiegeGateOperateSend(int aIndex,int result,int index,int operation);
	void GCCastleSiegeGateOperateStateSend(int aIndex,int operation);
	void GCCastleSiegeSwitchStateSend(int aIndex,int bIndex,int Target,int state);
	void GCCastleSiegeCrownAccessStateSend(int aIndex,int state);
	void GCCastleSiegeCrownStateSend(int state);
	void GCCastleSiegeStartStateSend(int aIndex,int state);
	void GCCastleSiegeStartStateSendToAll(int state);
	void GCCastleSiegeNotifyProgressSendToAll(int state,char* GuildName);
	void GCCastleSiegeJoinSideSend(int aIndex);
	void GCCastleSiegeMiniMapRequestSend(int aIndex,int result);
	void GCCastleSiegeLeftTimeSend(int aIndex);
	void GCCastleSiegeLeftTimeSendToAll();
	void GCCastleSiegeSwitchInfoSend(int aIndex);
	void DGCastleSiegeStateRecv(SDHP_CASTLE_SIEGE_STATE_RECV* lpMsg);
	void DGCastleSiegeNpcBuyRecv(SDHP_CASTLE_SIEGE_NPC_BUY_RECV* lpMsg);
	void DGCastleSiegeNpcRepairRecv(SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV* lpMsg);
	void DGCastleSiegeNpcUpgradeRecv(SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV* lpMsg);
	void DGCastleSiegeTaxMoneyInfoRecv(SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV* lpMsg);
	void DGCastleSiegeTaxRateChangeRecv(SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV* lpMsg);
	void DGCastleSiegeMoneyOutRecv(SDHP_CASTLE_SIEGE_MONEY_OUT_RECV* lpMsg);
	void DGCastleSiegeRegisterInfoRecv(SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV* lpMsg);
	void DGCastleSiegeRegisterRecv(SDHP_CASTLE_SIEGE_REGISTER_RECV* lpMsg);
	void DGCastleSiegeResetStateRecv(SDHP_CASTLE_SIEGE_RESET_STATE_RECV* lpMsg);
	void DGCastleSiegeRegisterMarkRecv(SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV* lpMsg);
	void DGCastleSiegeGiveUpGuildRecv(SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV* lpMsg);
	void DGCastleSiegeSyncStateRecv(SDHP_CASTLE_SIEGE_SYNC_STATE_RECV* lpMsg);
	void DGCastleSiegeAddTributeMoneyRecv(SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV* lpMsg);
	void DGCastleSiegeResetTaxInfoRecv(SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV* lpMsg);
	void DGCastleSiegeInitDataRecv(SDHP_CASTLE_SIEGE_INIT_DATA_RECV* lpMsg);
	void DGCastleSiegeRegisterListRecv(SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV* lpMsg);
	void DGCastleSiegeNpcCreateRecv(SDHP_CASTLE_SIEGE_NPC_CREATE_RECV* lpMsg);
	void DGCastleSiegeBuildGuildListRecv(SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV* lpMsg);
	void DGCastleSiegeUnionListRecv(SDHP_CASTLE_SIEGE_UNION_LIST_RECV* lpMsg);
	void DGCastleSiegeSaveListRecv(SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV* lpMsg);
	void DGCastleSiegeLoadGuildListRecv(SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV* lpMsg);
	void GDCastleSiegeDateChangeSend();
	void GDCastleSiegeEndSend();
	void GDCastleSiegeOwnerChangeSend();
	void GDCastleSiegeResetStateSend();
	void GDCastleSiegeSyncStateSend();
	void GDCastleSiegeAddTributeMoneySend(int money);
	void GDCastleSiegeResetTaxInfoSend();
	void GDCastleSiegeParticipantClearSend();
	void GDCastleSiegeRegisteredClearSend();
	void GDCastleSiegeNpcCreateSend();
	void GDCastleSiegeBuildGuildListSend();
	void GDCastleSiegeLoadUnionListSend();
	void GDCastleSiegeSaveGuildListSend();
	void GDCastleSiegeLoadGuildListSend();
	void GDCastleSiegeNpcSaveSend();
public:
	int m_CastleSiegeSwitch;
	bool m_CastleSiegeLoadFile;
	bool m_CastleSiegeLoadDatabase;
	bool m_CastleSiegeDoRun;
	int m_CastleSiegeDataLoad;
	DWORD m_CastleSiegeDataLoadTime;
	int m_Cycle;
	int m_State;
	int m_RemainTime;
	int m_TargetTime;
	int m_TickCount;
	BYTE m_CastleOccupied;
	char m_CastleOwner[9];
	char m_MiddleOwner[9];
	QWORD m_TributeMoney;
	int m_TaxRateChaos;
	int m_TaxRateStore;
	int m_TaxRateHunt;
	bool m_HuntEnabled;
	DWORD m_SaveNpcTime;
	DWORD m_CalcLeftTime;
	DWORD m_MessageTime;
	DWORD m_CheckGateTime;
	DWORD m_UpdateSideTime;
	DWORD m_MiniMapTime;
	DWORD m_LeftTime;
	DWORD m_LeftStartTime;
	bool m_CastleTowerEnable;
	bool m_CrownAvailable;
	int m_CrownIndex;
	int m_CrownAccessUser;
	BYTE m_CrownAccessUserX;
	BYTE m_CrownAccessUserY;
	int m_CrownSwitchUser1;
	int m_CrownSwitchUser2;
	DWORD m_CrownAccessTime;
	bool m_CastleNpcCreated;
	bool m_BasicGuildDataLoad;
	bool m_SiegeGuildDataLoad;
	BYTE m_CastleGuildListLoad;
	BYTE m_CastleSiegeEnded;
	std::map<std::string,CASTLE_SIEGE_GUILD_FINAL> m_BasicGuildDataMap;
	std::map<std::string,CASTLE_SIEGE_GUILD_FINAL> m_SiegeGuildDataMap;
	std::map<int,CASTLE_SIEGE_MINI_MAP_DATA> m_MiniMapDataMap;
	std::vector<int> m_MiniMapRequestList;
	std::vector<CASTLE_SIEGE_NPC_DATA> m_NpcDataList;
	std::vector<CASTLE_STATE_TIME_DATA> m_StateTimeData;
	int m_bFixCastleCycleStartDate;
	int m_iFixCastleSpecificState;
	int m_bFixCastleStateStartDate;
	SYSTEMTIME m_SiegeEndSchedule;
	SYSTEMTIME m_FixCastleCycleStartDate;
	SYSTEMTIME m_FixCastleStateStartDate;
	SYSTEMTIME m_StateStartDate;
	SYSTEMTIME m_LeftCastleSiegeDate;
	SYSTEMTIME m_StartDate;
	SYSTEMTIME m_EndDate;
};

#define gCastleSiege SingNull(CCastleSiege)

static int NpcDefense[2][4] = {{100,180,300,520},{80,180,340,550}};

static int NpcUpgradeGateDefense[3][3] = {{1,2,3000000},{2,3,3000000},{3,4,3000000} };
static int NpcUpgradeGateLife[3][3] = {{2500000,2,1000000},{3500000,3,1000000},{5200000,4,1000000}};

static int NpcUpgradeStatueDefense[3][3] = {{1,3,3000000},{2,5,3000000},{3,7,3000000}};
static int NpcUpgradeStatueLife[3][3] = {{2200000,3,1000000},{3400000,5,1000000},{5000000,7,1000000}};
static int NpcUpgradeStatueRegen[3][3] = {{1,3,5000000},{2,5,5000000},{3,7,5000000}};