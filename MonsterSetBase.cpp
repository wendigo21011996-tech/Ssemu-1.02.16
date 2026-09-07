// MonsterSetBase.cpp: implementation of the CMonsterSetBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterSetBase.h"
#include "KanturuMonsterMng.h"
#include "MapManager.h"
#include "MapServerManager.h"
#include "ReadFile.h"
#include "Path.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterSetBase::CMonsterSetBase() // OK
{
	this->m_count = 0;
}

CMonsterSetBase::~CMonsterSetBase() // OK
{

}

void CMonsterSetBase::LoadSpawn() // OK
{
	this->m_count = 0;

	char wildcard_path[MAX_PATH];

	wsprintf(wildcard_path,"%s*",gPath->GetFullPath("Monster\\Spawn\\"));

	WIN32_FIND_DATA data;

	HANDLE file = FindFirstFile(wildcard_path,&data);

	if(file == INVALID_HANDLE_VALUE)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,wildcard_path);
		return;
	}

	do
	{
		if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if(isdigit(data.cFileName[0]) != 0 && isdigit(data.cFileName[1]) != 0 && isdigit(data.cFileName[2]) != 0)
			{
				if(data.cFileName[3] == ' ' && data.cFileName[4] == '-' && data.cFileName[5] == ' ')
				{
					char path[MAX_PATH];

					wsprintf(path,"Monster\\Spawn\\%s",data.cFileName);

					if(gMapManager->IsValidMap(atoi(data.cFileName)) != 0)
					{
						this->Load(gPath->GetFullPath(path),atoi(data.cFileName));
					}
				}
			}
		}
	} while (FindNextFile(file,&data) != 0);
}

void CMonsterSetBase::Load(char* path,int MapNumber) // OK
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
				if(strcmp("end",lpReadFile->GetAsString()) == 0)
				{
					break;
				}

				if(section == 5)
				{
					#if(GAMESERVER_UPDATE>=201)
					gKanturuMonsterMng->Load(path,MapNumber);
					#endif
				}
				else
				{
					MONSTER_SET_BASE_INFO info;

					memset(&info,0,sizeof(info));

					info.Type = section;

					info.MonsterClass = lpReadFile->GetNumber();

					info.Map = MapNumber;

					info.Dis = lpReadFile->GetAsNumber();

					info.X = lpReadFile->GetAsNumber();

					info.Y = lpReadFile->GetAsNumber();

					if(section == 1)
					{
						info.TX = lpReadFile->GetAsNumber();
						info.TY = lpReadFile->GetAsNumber();
					}
					else if(section == 2)
					{
						info.X = (info.X-3)+GetLargeRand()%7;
						info.Y = (info.Y-3)+GetLargeRand()%7;
					}

					info.Dir = lpReadFile->GetAsNumber();

					if(section == 1)
					{
						int count = lpReadFile->GetAsNumber();

						for(int n=0;n < count;n++)
						{
							this->SetInfo(info);
						}
					}
					else
					{
						this->SetInfo(info);
					}
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CMonsterSetBase::SetInfo(MONSTER_SET_BASE_INFO info) // OK
{
	if(this->m_count < 0 || this->m_count >= MAX_MSB_MONSTER)
	{
		return;
	}
	
	if(gMapServerManager->CheckMapServer(info.Map) == 0)
	{
		return;
	}

	info.Dir = ((info.Dir==-1)?(GetLargeRand()%8):info.Dir);

	this->m_MonsterSetBaseInfo[this->m_count++] = info;
}

bool CMonsterSetBase::GetPosition(int index,short map,short* ox,short* oy) // OK
{
	if(index < 0 || index >= MAX_MSB_MONSTER)
	{
		return 0;
	}

	MONSTER_SET_BASE_INFO* lpInfo = &this->m_MonsterSetBaseInfo[index];

	if(lpInfo->Type == 0 || lpInfo->Type == 4)
	{
		(*ox) = lpInfo->X;
		(*oy) = lpInfo->Y;
		return 1;
	}
	else if(lpInfo->Type == 1 || lpInfo->Type == 3)
	{
		return this->GetBoxPosition(map,lpInfo->X,lpInfo->Y,lpInfo->TX,lpInfo->TY,ox,oy);
	}
	else if(lpInfo->Type == 2)
	{
		return this->GetBoxPosition(map,(lpInfo->X - 3),(lpInfo->Y - 3),(lpInfo->X+3),(lpInfo->Y+3),ox,oy);
	}

	return 0;
}

bool CMonsterSetBase::GetBoxPosition(int map,int x,int y,int tx,int ty,short* ox,short* oy) // OK
{
	for(int n = 0; n < 100; n++)
	{
		int subx = tx - x;
		int suby = ty - y;

		subx = ((subx<1) ? 1 : subx);
		suby = ((suby<1) ? 1 : suby);

		subx = x+(GetLargeRand() % subx);
		suby = y+(GetLargeRand() % suby);

		if(gMap[map].CheckAttr(subx,suby,1) == 0 && gMap[map].CheckAttr(subx,suby,4) == 0 && gMap[map].CheckAttr(subx,suby,8) == 0)
		{
			(*ox) = subx;
			(*oy) = suby;
			return 1;
		}
	}

	return 0;
}