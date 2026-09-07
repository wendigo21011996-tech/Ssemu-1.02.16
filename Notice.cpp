// Notice.cpp: implementation of the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Notice.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotice::CNotice() // OK
{
	this->m_count = 0;

	this->m_NoticeValue = 0;

	this->m_NoticeTime = GetTickCount();
}

CNotice::~CNotice() // OK
{

}

void CNotice::Load(char* path) // OK
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

	this->m_count = 0;

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

			NOTICE_INFO info;

			memset(&info,0,sizeof(info));

			strcpy_s(info.Message,lpReadFile->GetString());

			info.Type = lpReadFile->GetAsNumber();

			info.Count = lpReadFile->GetAsNumber();

			info.Opacity = lpReadFile->GetAsNumber();

			info.Delay = lpReadFile->GetAsNumber();

			info.Color = 0;
			
			info.Color |= lpReadFile->GetAsNumber();
			
			info.Color |= (lpReadFile->GetAsNumber() << 8);
			
			info.Color |= (lpReadFile->GetAsNumber() << 16);
			
			info.Color |= (info.Opacity << 24);

			info.Speed = lpReadFile->GetAsNumber();

			info.RepeatTime = lpReadFile->GetAsNumber()*1000;

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CNotice::SetInfo(NOTICE_INFO info) // OK
{
	if(this->m_count < 0 || this->m_count >= MAX_NOTICE)
	{
		return;
	}

	this->m_NoticeInfo[this->m_count++] = info;
}

void CNotice::MainProc() // OK
{
	if(this->m_count == 0)
	{
		return;
	}

	NOTICE_INFO* lpInfo = &this->m_NoticeInfo[this->m_NoticeValue];

	if((GetTickCount()-this->m_NoticeTime) >= ((DWORD)lpInfo->RepeatTime))
	{
		this->m_NoticeValue = (((this->m_NoticeValue+1)>=this->m_count)?0:(this->m_NoticeValue+1));
		this->m_NoticeTime = GetTickCount();
		this->GCNoticeSendToAll(lpInfo->Type,lpInfo->Count,lpInfo->Opacity,lpInfo->Delay,lpInfo->Color,lpInfo->Speed,"%s",lpInfo->Message);
	}
}

void CNotice::GCNoticeSend(int aIndex,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);
}

void CNotice::GCNoticeSendToAll(BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE)?MAX_MESSAGE_SIZE:size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set(0x0D,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}
}