// ItemBag.h: interface for the CItemBag class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Item.h"
#include "ItemBagEx.h"
#include "User.h"

struct ITEM_BAG_INFO
{
	int ItemIndex;
	int MinLevel;
	int MaxLevel;
	int Option1;
	int Option2;
	int Option3;
	int NewOption;
	int SetOption;
	int SocketOption;
	int Duration;
};

class CItemBag
{
public:
	CItemBag();
	virtual ~CItemBag();
	void Init();
	void Load(char* path);
	bool GetItem(LPOBJ lpObj,CItem* lpItem);
	bool DropItem(LPOBJ lpObj,int map,int x,int y);
private:
	char m_EventName[32];
	int m_DropZen;
	int m_ItemDropRate;
	int m_ItemDropCount;
	int m_SetItemDropRate;
	int m_RandomOption;
	int m_BagUseEffect;
	std::vector<ITEM_BAG_INFO> m_ItemBagInfo;
	CItemBagEx m_ItemBagEx;
};