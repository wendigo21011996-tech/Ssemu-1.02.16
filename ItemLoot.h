// ItemLoot.h: interface for the CItemLoot class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct ITEM_LOOT_INFO
{
	int ItemIndex;
	int Level;
	int AllowLoot;
};

class CItemLoot
{
	CItemLoot();
	virtual ~CItemLoot();
	SingletonInstance(CItemLoot)
public:
	void Load(char* path);
	bool CheckItemAllowLoot(int index,int level);
private:
	std::vector<ITEM_LOOT_INFO> m_ItemLootInfo;
};

#define gItemLoot SingNull(CItemLoot)