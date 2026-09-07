// ConnectMember.cpp: implementation of the CConnectMember class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConnectMember.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectMember::CConnectMember() // OK
{
	this->m_ConnectMemberInfo.clear();
}

CConnectMember::~CConnectMember() // OK
{

}

void CConnectMember::Load(char* path) // OK
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

	this->m_ConnectMemberInfo.clear();

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

			CONNECT_MEMBER_INFO info;

			memset(&info,0,sizeof(info));

			strcpy_s(info.Account,lpReadFile->GetString());

			this->m_ConnectMemberInfo.insert(std::pair<std::string,CONNECT_MEMBER_INFO>(info.Account,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CConnectMember::CheckAccount(char* Account) // OK
{
	std::map<std::string,CONNECT_MEMBER_INFO>::iterator it = this->m_ConnectMemberInfo.find(Account);

	if(it == this->m_ConnectMemberInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}