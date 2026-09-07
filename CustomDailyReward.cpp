// CustomDailyReward.cpp: implementation of the CCustomDailyReward class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomDailyReward.h"
#include "GameMain.h"
#include "ItemBagManager.h"
#include "Message.h"
#include "Notice.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomDailyReward::CCustomDailyReward() // OK
{
	this->m_CustomDailyReward.clear();
}

CCustomDailyReward::~CCustomDailyReward() // OK
{

}

void CCustomDailyReward::Load(char* path) // OK
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

	this->m_CustomDailyReward.clear();

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

			DAILY_REWARD_INFO info;

			memset(&info,0,sizeof(info));

			info.Index = lpReadFile->GetNumber();
			
			info.Month = lpReadFile->GetAsNumber();
			
			info.Day = lpReadFile->GetAsNumber();
			
			info.MinOnlineTime = lpReadFile->GetAsNumber();
			
			info.MinLevel = lpReadFile->GetAsNumber();
			
			info.MaxLevel = lpReadFile->GetAsNumber();
			
			info.MinReset = lpReadFile->GetAsNumber();
			
			info.MaxReset = lpReadFile->GetAsNumber();
			
			info.AccountLevel = lpReadFile->GetAsNumber();
			
			info.MessageReward = lpReadFile->GetAsNumber();
			
			info.BagSpecial = lpReadFile->GetAsNumber();

			this->m_CustomDailyReward.insert(std::pair<int,DAILY_REWARD_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CCustomDailyReward::DGDailyRewardCheckRecv(SDHP_DAILY_REWARD_INFO_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->aIndex,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGDailyRewardCheckRecv] Invalid Account [%d](%s)",lpMsg->aIndex,lpMsg->account);
		CloseClient(lpMsg->aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	if(lpMsg->result != 0)
	{
		std::map<int,DAILY_REWARD_INFO>::iterator it = this->m_CustomDailyReward.find(lpMsg->index);

		if(it != this->m_CustomDailyReward.end())
		{
			gItemBagManager->DropItemBySpecialValue(it->second.BagSpecial,-1,-1,lpObj,lpObj->Map,lpObj->X,lpObj->Y);

			if(it->second.MessageReward != -1)
			{
				gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(it->second.MessageReward),lpObj->Name);
			}

			GCServerCommandSend(lpObj->Index,0,lpObj->X,lpObj->Y,0);
		}
	}
}

void CCustomDailyReward::GDDailyRewardCheckSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	_SYSTEMTIME tmToDay;

	GetLocalTime(&tmToDay);

	for(std::map<int,DAILY_REWARD_INFO>::iterator it = this->m_CustomDailyReward.begin(); it != this->m_CustomDailyReward.end(); it++)
	{
		if(it->second.Month != -1 && it->second.Month != tmToDay.wMonth)
		{
			continue;
		}

		if(it->second.Day != -1 && it->second.Day != tmToDay.wDay)
		{
			continue;
		}

		if(it->second.MinLevel != -1 && it->second.MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->second.MaxLevel != -1 && it->second.MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->second.MinReset != -1 && it->second.MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->second.MaxReset != -1 && it->second.MaxReset < lpObj->Reset)
		{
			continue;
		}

		if(it->second.AccountLevel != -1 && it->second.AccountLevel > lpObj->AccountLevel)
		{
			continue;
		}

		SDHP_DAILY_REWARD_INFO_SEND pMsg;

		pMsg.header.set(0x16,sizeof(pMsg));

		pMsg.aIndex = aIndex;

		memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

		memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

		pMsg.index = it->second.Index;

		pMsg.MinOnlineTime = it->second.MinOnlineTime;

		gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
	}
}