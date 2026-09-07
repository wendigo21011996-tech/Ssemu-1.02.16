// BlackList.cpp: implementation of the CBlackList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlackList.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlackList::CBlackList() // OK
{
	this->m_BlackListIpInfo.clear();

	this->m_BlackListHidInfo.clear();
}

CBlackList::~CBlackList() // OK
{

}

void CBlackList::Load(char* path) // OK
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

	this->m_BlackListIpInfo.clear();

	this->m_BlackListHidInfo.clear();

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
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					BLACKLIST_IP_INFO info;

					memset(&info,0,sizeof(info));

					strcpy_s(info.IpAddress,lpReadFile->GetString());

					this->m_BlackListIpInfo.insert(std::pair<std::string,BLACKLIST_IP_INFO>(std::string(info.IpAddress),info));
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					BLACKLIST_HID_INFO info;

					memset(&info,0,sizeof(info));

					strcpy_s(info.HardwareId,lpReadFile->GetString());

					this->m_BlackListHidInfo.insert(std::pair<std::string,BLACKLIST_HID_INFO>(std::string(info.HardwareId),info));
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
}

bool CBlackList::CheckIpAddress(char* IpAddress) // OK
{
	std::map<std::string,BLACKLIST_IP_INFO>::iterator it = this->m_BlackListIpInfo.find(std::string(IpAddress));

	if(it != this->m_BlackListIpInfo.end())
	{
		return 0;
	}

	return 1;
}

bool CBlackList::CheckHardwareId(char* HardwareId) // OK
{
	std::map<std::string,BLACKLIST_HID_INFO>::iterator it = this->m_BlackListHidInfo.find(std::string(HardwareId));

	if(it != this->m_BlackListHidInfo.end())
	{
		return 0;
	}

	return 1;
}
