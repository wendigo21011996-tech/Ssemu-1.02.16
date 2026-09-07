// KanturuMonsterMng.cpp: implementation of the CKanturuMonsterMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KanturuMonsterMng.h"
#include "Map.h"
#include "ReadFile.h"
#include "Monster.h"
#include "User.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuMonsterMng::CKanturuMonsterMng() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->ResetLoadData();

	this->ResetRegenMonsterObjData();
	
	#endif
}

CKanturuMonsterMng::~CKanturuMonsterMng() // OK
{

}

void CKanturuMonsterMng::ResetLoadData() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_MaxMonsterCount = 0;

	for(int n=0;n < MAX_KANTURU_MONSTER;n++)
	{
		memset(&this->m_SetBaseInfo[n],-1,sizeof(this->m_SetBaseInfo[n]));
	}

	this->m_MayaObjIndex = -1;

	
	#endif
}

void CKanturuMonsterMng::Load(char* path,int MapNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CReadFile* lpReadFile = new CReadFile;

	if(lpReadFile == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadFile->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
		delete lpReadFile;
		return;
	}

	this->ResetLoadData();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 5)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					KANTURU_MONSTER_SETBASE_INFO info;

					memset(&info,0,sizeof(info));

					info.Group = lpReadFile->GetNumber();

					info.MonsterClass = lpReadFile->GetAsNumber();

					info.Map = MapNumber;

					info.Dis = lpReadFile->GetAsNumber();

					info.X = lpReadFile->GetAsNumber();

					info.Y = lpReadFile->GetAsNumber();

					info.Dir = lpReadFile->GetAsNumber();

					this->SetMonsterSetBaseInfo(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	
	#endif
}

void CKanturuMonsterMng::SetMonsterSetBaseInfo(KANTURU_MONSTER_SETBASE_INFO info) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->m_MaxMonsterCount < 0 || this->m_MaxMonsterCount >= MAX_KANTURU_MONSTER)
	{
		return;
	}

	this->m_SetBaseInfo[this->m_MaxMonsterCount++] = info;

	
	#endif
}

void CKanturuMonsterMng::MonsterDie(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < this->m_KanturuMonster.GetCount();n++)
	{
		if(this->m_KanturuMonster.m_ObjIndex[n] == aIndex)
		{
			this->m_AliveMonsterCount--;
			return;
		}
	}
	
	#endif
}

void CKanturuMonsterMng::ResetRegenMonsterObjData() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(OBJECT_RANGE(this->m_MayaObjIndex) != 0)
	{
		gObjDel(this->m_MayaObjIndex);
		this->m_MayaObjIndex = -1;
	}

	for(int n=0;n < this->m_KanturuMonster.GetCount();n++)
	{
		if(OBJECT_RANGE(this->m_KanturuMonster.m_ObjIndex[n]) != 0)
		{
			gObjDel(this->m_KanturuMonster.m_ObjIndex[n]);
		}
	}

	this->m_KanturuMonster.Reset();

	this->m_MaxRegenMonsterCount = 0;

	this->m_AliveMonsterCount = 0;
	
	#endif
}

int CKanturuMonsterMng::SetKanturuMonster(int GroupNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int RegenMonsterCount = 0;

	for(int n=0;n < MAX_KANTURU_MONSTER;n++)
	{
		if(this->m_SetBaseInfo[n].Group != GroupNumber || this->m_SetBaseInfo[n].MonsterClass == 0)
		{
			continue;
		}

		int index = gObjAddMonster(this->m_SetBaseInfo[n].Map);

		if(OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[index];

		lpObj->PosNum = -1;
		lpObj->X = this->m_SetBaseInfo[n].X;
		lpObj->Y = this->m_SetBaseInfo[n].Y;
		lpObj->Map = this->m_SetBaseInfo[n].Map;

		if(this->GetPosition(n,lpObj->Map,&lpObj->X,&lpObj->Y) == 0)
		{
			gObjDel(index);
			continue;
		}

		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->OldX = lpObj->X;
		lpObj->OldY = lpObj->Y;
		lpObj->StartX = (BYTE)lpObj->X;
		lpObj->StartY = (BYTE)lpObj->Y;
		lpObj->DieRegen = 0;
		lpObj->MoveRange = this->m_SetBaseInfo[n].Dis;

		if(this->m_SetBaseInfo[n].Dir == 0xFF)
		{
			lpObj->Dir = GetLargeRand()%8;
		}
		else
		{
			lpObj->Dir = this->m_SetBaseInfo[n].Dir;
		}

		if(gObjSetMonster(index,this->m_SetBaseInfo[n].MonsterClass) == 0)
		{
			gObjDel(index);
			continue;
		}

		if(this->m_SetBaseInfo[n].MonsterClass == 364)
		{
			this->m_MayaObjIndex = index;
			continue;
		}
		else
		{
			RegenMonsterCount++;
			this->m_KanturuMonster.AddObj(index);
			this->m_MaxMonsterCount++;
			this->m_AliveMonsterCount++;
			continue;
		}
	}

	return RegenMonsterCount;
	
	#else

	return 0;

	#endif
}

int CKanturuMonsterMng::GetPosition(int index,short map,short* x,short* y) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(index < 0 || index >= MAX_KANTURU_MONSTER)
	{
		return 0;
	}

	if(gMap[map].CheckAttr(this->m_SetBaseInfo[index].X,this->m_SetBaseInfo[index].Y,1) == 0 && gMap[map].CheckAttr(this->m_SetBaseInfo[index].X,this->m_SetBaseInfo[index].Y,4) == 0 && gMap[map].CheckAttr(this->m_SetBaseInfo[index].X,this->m_SetBaseInfo[index].Y,8) == 0)
	{
		(*x) = this->m_SetBaseInfo[index].X;
		(*y) = this->m_SetBaseInfo[index].Y;
		return 1;
	}

	for(int n=0;n < 100;n++)
	{
		int px = (this->m_SetBaseInfo[index].X-3)+(GetLargeRand()%7);
		int py = (this->m_SetBaseInfo[index].Y-3)+(GetLargeRand()%7);

		if(gMap[map].CheckAttr(px,py,1) == 0 && gMap[map].CheckAttr(px,py,4) == 0 && gMap[map].CheckAttr(px,py,8) == 0)
		{
			(*x) = px;
			(*y) = py;
			return 1;
		}
	}
	
	#endif

	return 0;
}

int CKanturuMonsterMng::GetMayaObjIndex() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_MayaObjIndex;
	
	#else

	return 0;

	#endif
}

int CKanturuMonsterMng::GetAliveMonsterCount() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_AliveMonsterCount;
	
	#else

	return 0;

	#endif
}

int CKanturuMonsterMng::IsExistAliveMonster() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return ((this->m_AliveMonsterCount<=0)?0:1);

	#else

	return 0;

	#endif
}
