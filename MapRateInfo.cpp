// MapRateInfo.cpp: implementation of the CMapRateInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapRateInfo.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapRateInfo::CMapRateInfo() // OK
{
	this->m_MapRateInfo.clear();
}

CMapRateInfo::~CMapRateInfo() // OK
{

}

void CMapRateInfo::Load(char* path) // OK
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

	this->m_MapRateInfo.clear();

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

			MAP_RATE_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.DropType = lpReadFile->GetAsNumber();

			info.Option0 = lpReadFile->GetAsNumber();

			info.Option1 = lpReadFile->GetAsNumber();

			info.Option2 = lpReadFile->GetAsNumber();

			info.Option3 = lpReadFile->GetAsNumber();

			info.Option4 = lpReadFile->GetAsNumber();

			info.Option5 = lpReadFile->GetAsNumber();

			#if(GAMESERVER_UPDATE>=401)

			info.Option6 = lpReadFile->GetAsNumber();

			#endif

			this->m_MapRateInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CMapRateInfo::GetDropOption0(int index,int type) // OK
{
	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option0;
	}

	return 0;
}

int CMapRateInfo::GetDropOption1(int index,int type) // OK
{
	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option1;
	}

	return 0;
}

int CMapRateInfo::GetDropOption2(int index,int type) // OK
{
	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option2;
	}

	return 0;
}

int CMapRateInfo::GetDropOption3(int index,int type) // OK
{
	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option3;
	}

	return 0;
}

int CMapRateInfo::GetDropOption4(int index,int type) // OK
{
	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option4;
	}

	return 0;
}

int CMapRateInfo::GetDropOption5(int index,int type) // OK
{
	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option5;
	}

	return 0;
}

int CMapRateInfo::GetDropOption6(int index,int type) // OK
{
	#if(GAMESERVER_UPDATE>=401)

	for(std::vector<MAP_RATE_INFO>::iterator it=this->m_MapRateInfo.begin();it != this->m_MapRateInfo.end();it++)
	{
		if(it->DropType != type)
		{
			continue;
		}

		if(it->Index != -1 && it->Index != index)
		{
			continue;
		}

		return it->Option6;
	}

	#endif

	return 0;
}