// CustomPotion.h: interface for the CCustomPotion class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

class CCustomPotion
{
	CCustomPotion();
	virtual ~CCustomPotion();
	SingletonInstance(CCustomPotion)
public:
	void ReadCustomPotionInfo(char* section,char* path);
	void MainProc(LPOBJ lpObj);
	void CGCustomPotionToggleRecv(int aIndex);
public:
	int m_CustomPotionSwitch;
	int m_CustomPotionPotionDelay;
	int m_CustomPotionPotionRate1;
	int m_CustomPotionPotionRate2;
	int m_CustomPotionPotionRate3;
};

#define gCustomPotion SingNull(CCustomPotion)