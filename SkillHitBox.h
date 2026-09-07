// SkillHitBox.h: interface for the CSkillHitBox class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CSkillHitBox
{
	CSkillHitBox();
	virtual ~CSkillHitBox();
	SingletonInstance(CSkillHitBox)
public:
	void Init();
	void Load(char* path);
	int Check(int dir,int x,int y);
	int HitCheck(int dir,int x,int y,int tx,int ty);
private:
	BYTE m_table[36][441];
};

#define gSkillHitBox SingNull(CSkillHitBox)