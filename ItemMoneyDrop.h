// ItemMoneyDrop.h: interface for the CItemMoneyDrop class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct ITEM_MONEY_DROP_INFO
{
	int MapNumber;
	int MonsterClass;
	int MonsterLevelMin;
	int MonsterLevelMax;
	int AccountLevel;
	int MoneyAmount;
	int MoneyDropRate;
};

class CItemMoneyDrop
{
	CItemMoneyDrop();
	virtual ~CItemMoneyDrop();
	SingletonInstance(CItemMoneyDrop)
public:
	void Load(char* path);
	int DropMoney(LPOBJ lpObj,LPOBJ lpTarget);
private:
	std::vector<ITEM_MONEY_DROP_INFO> m_ItemMoneyDropInfo;
};

#define gItemMoneyDrop SingNull(CItemMoneyDrop)