// MonsterAIMovePath.cpp: implementation of the CMonsterAIMovePath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIMovePath.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIMovePath::CMonsterAIMovePath() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->DelAllAIMonsterMovePath();

	#endif
}

CMonsterAIMovePath::~CMonsterAIMovePath() // OK
{

}

void CMonsterAIMovePath::LoadData(char* path) // OK
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

	this->DelAllAIMonsterMovePath();

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
				if(section == 2)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					CMonsterAIMovePathInfo info;

					info.m_Type = lpReadFile->GetNumber();

					info.m_MapNumber = lpReadFile->GetAsNumber();

					info.m_PathX = lpReadFile->GetAsNumber();

					info.m_PathY = lpReadFile->GetAsNumber();

					if(this->m_MonsterAIMovePathInfoCount >= 0 && this->m_MonsterAIMovePathInfoCount < MAX_MONSTER_AI_MOVE_PATH_INFO)
					{
						this->m_MonsterAIMovePathInfoArray[this->m_MonsterAIMovePathInfoCount++] = info;
					}
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

	this->m_DataLoad = 1;

	delete lpReadFile;

	#endif
}

void CMonsterAIMovePath::DelAllAIMonsterMovePath() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_AI_MOVE_PATH_INFO;n++)
	{
		this->m_MonsterAIMovePathInfoArray[n].Reset();
	}

	this->m_MonsterAIMovePathInfoCount = 0;

	#endif
}

void CMonsterAIMovePath::GetMonsterNearestPath(int sx,int sy,int tx,int ty,int* ox,int* oy,int NearestDistance) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < this->m_MonsterAIMovePathInfoCount;n++)
	{
		if(sqrt(pow(((float)sx-(float)this->m_MonsterAIMovePathInfoArray[n].m_PathX),2)+pow(((float)sy-(float)this->m_MonsterAIMovePathInfoArray[n].m_PathY),2)) < 10)
		{
			if(sqrt(pow(((float)tx-(float)this->m_MonsterAIMovePathInfoArray[n].m_PathX),2)+pow(((float)ty-(float)this->m_MonsterAIMovePathInfoArray[n].m_PathY),2)) < NearestDistance)
			{
				NearestDistance = (int)sqrt(pow(((float)tx-(float)this->m_MonsterAIMovePathInfoArray[n].m_PathX),2)+pow(((float)ty-(float)this->m_MonsterAIMovePathInfoArray[n].m_PathY),2));
				(*ox) = this->m_MonsterAIMovePathInfoArray[n].m_PathX;
				(*oy) = this->m_MonsterAIMovePathInfoArray[n].m_PathY;
			}
		}
	}

	#endif
}
