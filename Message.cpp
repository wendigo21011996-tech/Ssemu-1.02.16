// Message.cpp: implementation of the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Message.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessage::CMessage() // OK
{
	memset(this->m_DefaultMessage,0,sizeof(this->m_DefaultMessage));

	this->m_MessageInfo.clear();
}

CMessage::~CMessage() // OK
{

}

void CMessage::Load(char* path) // OK
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

	this->m_MessageInfo.clear();

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

			MESSAGE_INFO info;

			info.Index = lpReadFile->GetNumber();

			strcpy_s(info.Message,lpReadFile->GetAsString());

			this->m_MessageInfo.insert(std::pair<int,MESSAGE_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

char* CMessage::GetMessage(int index) // OK
{
	std::map<int,MESSAGE_INFO>::iterator it = this->m_MessageInfo.find(index);

	if(it == this->m_MessageInfo.end())
	{
		wsprintf(this->m_DefaultMessage,"Could not find message %d!",index);
		return this->m_DefaultMessage;
	}
	else
	{
		return it->second.Message;
	}
}