// ResetLimit.cpp: implementation of the CResetLimit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResetLimit.h"
#include "ReadFile.h"
#include "ServerInfo.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResetLimit::CResetLimit() // OK
{
	this->m_ResetLimitInfo.clear();
}

CResetLimit::~CResetLimit() // OK
{

}

void CResetLimit::Load(char* path) // OK
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

	this->m_ResetLimitInfo.clear();

	try
	{
		while (true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpReadFile->GetString()) == 0)
			{
				break;
			}

			RESET_LIMIT_INFO info;

			memset(&info,0,sizeof(info));

			info.MinReset = lpReadFile->GetNumber();

			info.MaxReset = lpReadFile->GetAsNumber();

			info.LimitDay[0] = lpReadFile->GetAsNumber();

			info.LimitDay[1] = lpReadFile->GetAsNumber();

			info.LimitDay[2] = lpReadFile->GetAsNumber();

			info.LimitDay[3] = lpReadFile->GetAsNumber();

			info.LimitWek[0] = lpReadFile->GetAsNumber();

			info.LimitWek[1] = lpReadFile->GetAsNumber();

			info.LimitWek[2] = lpReadFile->GetAsNumber();

			info.LimitWek[3] = lpReadFile->GetAsNumber();

			info.LimitMon[0] = lpReadFile->GetAsNumber();

			info.LimitMon[1] = lpReadFile->GetAsNumber();

			info.LimitMon[2] = lpReadFile->GetAsNumber();

			info.LimitMon[3] = lpReadFile->GetAsNumber();

			this->m_ResetLimitInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

DWORD CResetLimit::GetLimitDay(LPOBJ lpObj) // OK
{
	for(std::vector<RESET_LIMIT_INFO>::iterator it = this->m_ResetLimitInfo.begin(); it != this->m_ResetLimitInfo.end(); it++)
	{
		if((lpObj->Reset+1) >= it->MinReset && (lpObj->Reset+1) <= it->MaxReset)
		{
			if(it->LimitDay[lpObj->AccountLevel] == -1)
			{
				return gServerInfo->m_CommandResetLimitDay[lpObj->AccountLevel];
			}
			else
			{
				return it->LimitDay[lpObj->AccountLevel];
			}
		}
	}

	return gServerInfo->m_CommandResetLimitDay[lpObj->AccountLevel];
}

DWORD CResetLimit::GetLimitWek(LPOBJ lpObj) // OK
{
	for(std::vector<RESET_LIMIT_INFO>::iterator it = this->m_ResetLimitInfo.begin(); it != this->m_ResetLimitInfo.end(); it++)
	{
		if((lpObj->Reset+1) >= it->MinReset && (lpObj->Reset+1) <= it->MaxReset)
		{
			if(it->LimitWek[lpObj->AccountLevel] == -1)
			{
				return gServerInfo->m_CommandResetLimitWek[lpObj->AccountLevel];
			}
			else
			{
				return it->LimitWek[lpObj->AccountLevel];
			}
		}
	}

	return gServerInfo->m_CommandResetLimitWek[lpObj->AccountLevel];
}

DWORD CResetLimit::GetLimitMon(LPOBJ lpObj) // OK
{
	for(std::vector<RESET_LIMIT_INFO>::iterator it = this->m_ResetLimitInfo.begin(); it != this->m_ResetLimitInfo.end(); it++)
	{
		if((lpObj->Reset+1) >= it->MinReset && (lpObj->Reset+1) <= it->MaxReset)
		{
			if(it->LimitMon[lpObj->AccountLevel] == -1)
			{
				return gServerInfo->m_CommandResetLimitMon[lpObj->AccountLevel];
			}
			else
			{
				return it->LimitMon[lpObj->AccountLevel];
			}
		}
	}

	return gServerInfo->m_CommandResetLimitMon[lpObj->AccountLevel];
}