// HackPacketCheck.cpp: implementation of the CHackPacketCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HackPacketCheck.h"
#include "Log.h"
#include "ReadFile.h"
#include "SerialCheck.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHackPacketCheck::CHackPacketCheck() // OK
{
	this->Init();
}

CHackPacketCheck::~CHackPacketCheck() // OK
{

}

void CHackPacketCheck::Init() // OK
{
	for(int n=0;n < MAX_HACK_PACKET_INFO;n++)
	{
		this->m_HackPacketInfo[n].ResetIndex();
		this->m_HackPacketInfo[n].ResetValue();
	}
}

void CHackPacketCheck::Load(char* path) // OK
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

	this->Init();

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

			HACK_PACKET_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.Value = lpReadFile->GetAsNumber();

			info.Encrypt = lpReadFile->GetAsNumber();

			info.MaxDelay = lpReadFile->GetAsNumber();

			info.MinCount = lpReadFile->GetAsNumber();

			info.MaxCount = lpReadFile->GetAsNumber();

			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CHackPacketCheck::SetInfo(HACK_PACKET_INFO info) // OK
{
	if(info.Index < 0 || info.Index >= MAX_HACK_PACKET_INFO)
	{
		return;
	}

	this->m_HackPacketInfo[info.Index].IndexInfo = info;

	if(info.Value != -1){this->m_HackPacketInfo[info.Index].ValueInfo[info.Value] = info;}
}

HACK_PACKET_INFO* CHackPacketCheck::GetInfo(int index,int value) // OK
{
	if(index < 0 || index >= MAX_HACK_PACKET_INFO)
	{
		return 0;
	}

	if(value < 0 || value >= MAX_HACK_PACKET_INFO)
	{
		return 0;
	}

	if(this->m_HackPacketInfo[index].IndexInfo.Index != index)
	{
		return 0;
	}

	if(this->m_HackPacketInfo[index].IndexInfo.Value != -1 && this->m_HackPacketInfo[index].ValueInfo[value].Value != value)
	{
		return 0;
	}

	return ((this->m_HackPacketInfo[index].IndexInfo.Value==-1)?&this->m_HackPacketInfo[index].IndexInfo:&this->m_HackPacketInfo[index].ValueInfo[value]);
}

bool CHackPacketCheck::CheckPacketHack(int aIndex,int index,int value,int encrypt,int serial) // OK
{
	HACK_PACKET_INFO* lpInfo = this->GetInfo(index,value);

	if(lpInfo == 0)
	{
		gLog->Output(LOG_HACK,"[HackPacketCheck][%s][%s] Packet unknown error (Index: %d,Value: %d)",gObj[aIndex].Account,gObj[aIndex].Name,index,value);
		CloseClient(aIndex);return 0;
	}

	if(lpInfo->Encrypt >= 0 && lpInfo->Encrypt != encrypt)
	{
		gLog->Output(LOG_HACK,"[HackPacketCheck][%s][%s] Packet encryption error (Index: %d,Value: %d,Encrypt: [%d][%d])",gObj[aIndex].Account,gObj[aIndex].Name,index,lpInfo->Value,encrypt,lpInfo->Encrypt);
		CloseClient(aIndex);return 0;
	}

	if(serial >= 0 && gSerialCheck[aIndex].CheckSerial(serial) == 0)
	{
		gLog->Output(LOG_HACK,"[HackPacketCheck][%s][%s] Packet serial error (Index: %d,Value: %d,Serial: [%d][%d])",gObj[aIndex].Account,gObj[aIndex].Name,index,lpInfo->Value,serial,gSerialCheck[aIndex].GetRecvSerial());
		CloseClient(aIndex);return 0;
	}

	if(this->CheckPacketHackMaxDelay(aIndex,index,lpInfo->MaxDelay) == 0)
	{
		gObj[aIndex].HackPacketDelay[index] = GetTickCount();
		gObj[aIndex].HackPacketCount[index] = 0;
	}

	if(this->CheckPacketHackMinCount(aIndex,index,lpInfo->MinCount) == 0)
	{
		gLog->Output(LOG_HACK,"[HackPacketCheck][%s][%s] Packet count error (Index: %d,Value: %d,Count: [%d][%d])",gObj[aIndex].Account,gObj[aIndex].Name,index,lpInfo->Value,gObj[aIndex].HackPacketCount[index],lpInfo->MaxCount);
		if(this->CheckPacketHackMaxCount(aIndex,index,lpInfo->MaxCount) == 0){CloseClient(aIndex);}return 0;
	}

	return 1;
}

bool CHackPacketCheck::CheckPacketHackMaxDelay(int aIndex,int index,int MaxDelay) // OK
{
	return ((MaxDelay==0)?1:(((GetTickCount()-gObj[aIndex].HackPacketDelay[index])>=((DWORD)MaxDelay))?0:1));
}

bool CHackPacketCheck::CheckPacketHackMinCount(int aIndex,int index,int MinCount) // OK
{
	return ((MinCount==0)?1:(((++gObj[aIndex].HackPacketCount[index])>((DWORD)MinCount))?0:1));
}

bool CHackPacketCheck::CheckPacketHackMaxCount(int aIndex,int index,int MaxCount) // OK
{
	return ((MaxCount==0)?1:((gObj[aIndex].HackPacketCount[index]>((DWORD)MaxCount))?0:1));
}
