// ItemDrop.h: interface for the CItemDrop class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

struct ITEM_DROP_INFO
{
	int ItemIndex;
	int Level;
	int Durability;
	int Option0;
	int Option1;
	int Option2;
	int Option3;
	int Option4;
	int Option5;
	#if(GAMESERVER_UPDATE>=401)
	int Option6;
	#endif
	int Duration;
	int MapNumber;
	int MonsterClass;
	int MonsterLevelMin;
	int MonsterLevelMax;
	int AccountLevel;
	int DropRate;
};

class CItemDrop
{
	CItemDrop();
	virtual ~CItemDrop();
	SingletonInstance(CItemDrop)
public:
	void Load(char* path);
	int DropItem(LPOBJ lpObj,LPOBJ lpTarget);
	int GetItemDropRate(LPOBJ lpObj,LPOBJ lpTarget,int ItemIndex,int DropRate);
private:
	std::vector<ITEM_DROP_INFO> m_ItemDropInfo;
};

#define gItemDrop SingNull(CItemDrop)