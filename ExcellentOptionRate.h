// ExcellentOptionRate.h: interface for the CExcellentOptionRate class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct EXCELLENT_ITEM_OPTION_INFO
{
	int Index;
	int ItemMinIndex;
	int ItemMaxIndex;
	char Name[32];
	int Rate;
};

class CExcellentOptionRate
{
	CExcellentOptionRate();
	virtual ~CExcellentOptionRate();
	SingletonInstance(CExcellentOptionRate)
public:
	void Load(char* path);
	int GetNewOption(int ItemIndex);
private:
	std::vector<EXCELLENT_ITEM_OPTION_INFO> m_ExcellentItemOptionInfo;
};

#define gExcellentOptionRate SingNull(CExcellentOptionRate)