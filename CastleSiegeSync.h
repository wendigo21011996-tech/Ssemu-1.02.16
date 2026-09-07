// CastleSiegeSync.h: interface for the CCastleSiegeSync class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CCastleSiegeSync
{
	CCastleSiegeSync();
	virtual ~CCastleSiegeSync();
	SingletonInstance(CCastleSiegeSync)
public:
	void MainProc();
	void Clear();
	int GetState();
	int GetTaxRateChaos(int aIndex);
	int GetTaxRateStore(int aIndex);
	int GetTaxHuntZone(int aIndex,bool CheckOwnerGuild);
	char* GetCastleOwnerGuild();
	bool CheckCastleOwnerMember(int aIndex);
	bool CheckCastleOwnerUnionMember(int aIndex);
	void AddTributeMoney(int money);
	void SetCastleState(int state);
	void SetTaxRateChaos(int rate);
	void SetTaxRateStore(int rate);
	void SetTaxHuntZone(int rate);
	void SetCastleOwnerGuild(char* GuildName);
private:
	int m_CurCastleState;
	int m_CurTaxRateChaos;
	int m_CurTaxRateStore;
	int m_CurTaxHuntZone;
	int m_TributeMoney;
	DWORD m_TributeSaveTime;
	char m_CastleOwnerGuild[8];
};

#define gCastleSiegeSync SingNull(CCastleSiegeSync)