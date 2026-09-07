// ItemLevel.h: interface for the CItemLevel class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct ITEM_LEVEL_INFO
{
	int ItemIndex;
	int Level;
	char Name[50];
};

class CItemLevel
{
	CItemLevel();
	virtual ~CItemLevel();
	SingletonInstance(CItemLevel)
public:
	void Load(char* path);
	bool CheckItemlevel(int index,int level);
public:
	std::vector<ITEM_LEVEL_INFO> m_ItemLevelInfo;
};

#define gItemLevel SingNull(CItemLevel)