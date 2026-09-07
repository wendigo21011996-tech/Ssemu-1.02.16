// MonsterMove.cpp: implementation of the CMonsterMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterMove.h"
#include "MapManager.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterMove::CMonsterMove() // OK
{
	this->m_MonsterMoveInfo.clear();
}

CMonsterMove::~CMonsterMove() // OK
{

}

void CMonsterMove::Load(char* path) // OK
{
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

	this->m_MonsterMoveInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			MONSTER_MOVE_SPAWN info;

			info.Index = lpReadFile->GetNumber();

			info.MapN = lpReadFile->GetAsNumber();

			info.MapX = lpReadFile->GetAsNumber();

			info.MapY = lpReadFile->GetAsNumber();

			info.Dir = lpReadFile->GetAsNumber();

			info.Time = lpReadFile->GetAsNumber();

			this->SetInfo(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CMonsterMove::SetInfo(MONSTER_MOVE_SPAWN info) // OK
{
	std::map<int,MONSTER_MOVE_INFO>::iterator it = this->m_MonsterMoveInfo.find(info.Index);

	if(it == this->m_MonsterMoveInfo.end())
	{
		MONSTER_MOVE_INFO lpMonsterMoveInfo;

		lpMonsterMoveInfo.MonsterClass = info.Index;
		
		lpMonsterMoveInfo.MonsterIndex = -1;

		lpMonsterMoveInfo.RemainTime = 0;

		lpMonsterMoveInfo.TargetTime = 0;

		lpMonsterMoveInfo.TickCount = GetTickCount();

		lpMonsterMoveInfo.SpawnInfo.push_back(info);

		this->m_MonsterMoveInfo.insert(std::pair<int,MONSTER_MOVE_INFO>(info.Index,lpMonsterMoveInfo));
	}
	else
	{
		it->second.SpawnInfo.push_back(info);
	}
}

void CMonsterMove::MainProc() // OK
{
	for(std::map<int,MONSTER_MOVE_INFO>::iterator it=this->m_MonsterMoveInfo.begin();it != this->m_MonsterMoveInfo.end();it++)
	{
		if(it->second.SpawnInfo.size() == 0)
		{
			return;
		}

		if((GetTickCount()-it->second.TickCount) >= 1000)
		{
			it->second.TickCount = GetTickCount();

			it->second.RemainTime = (int)difftime(it->second.TargetTime,time(0));

			if(it->second.RemainTime <= 0)
			{
				gObjDel(it->second.MonsterIndex);

				MONSTER_MOVE_SPAWN* lpInfo = &it->second.SpawnInfo[GetLargeRand()%it->second.SpawnInfo.size()];

				int index = gObjAddMonster(lpInfo->MapN);

				if(OBJECT_RANGE(index) == 0)
				{
					return;
				}

				LPOBJ lpObj = &gObj[index];

				lpObj->PosNum = -1;
				lpObj->X = lpInfo->MapX;
				lpObj->Y = lpInfo->MapY;
				lpObj->TX = lpInfo->MapX;
				lpObj->TY = lpInfo->MapY;
				lpObj->OldX = lpInfo->MapX;
				lpObj->OldY = lpInfo->MapY;
				lpObj->StartX = (BYTE)lpInfo->MapX;
				lpObj->StartY = (BYTE)lpInfo->MapY;
				lpObj->Dir = lpInfo->Dir;
				lpObj->Map = lpInfo->MapN;

				if(gObjSetMonster(index,lpInfo->Index) == 0)
				{
					gObjDel(index);
					return;
				}

				it->second.MonsterIndex = index;

				it->second.TargetTime = (int)(time(0)+lpInfo->Time);

				LogAdd(LOG_BLACK,"[Monster Move] Spawn Monster [%d] Map:%d X:%d Y:%d",lpInfo->Index,lpInfo->MapN,lpInfo->MapX,lpInfo->MapY);
			}
		}
	}
}