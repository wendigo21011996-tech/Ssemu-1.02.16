// CustomMonsterKillCount.cpp: implementation of the CCustomMonsterKillCount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomMonsterKillCount.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Message.h"
#include "MonsterManager.h"
#include "Notice.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomMonsterKillCount::CCustomMonsterKillCount() // OK
{
	this->m_MonsterKillCountInfo.clear();
}

CCustomMonsterKillCount::~CCustomMonsterKillCount() // OK
{

}

void CCustomMonsterKillCount::Load(char* path) // OK
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

	this->m_MonsterKillCountInfo.clear();

	try
	{
		while (true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while (true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					MONSTER_KILL_COUNT_INFO info;

					memset(&info,0,sizeof(info));

					info.Index = lpReadFile->GetNumber();

					info.MonsterClass = lpReadFile->GetAsNumber();

					info.Enable[0] = lpReadFile->GetAsNumber();

					info.Enable[1] = lpReadFile->GetAsNumber();

					info.Enable[2] = lpReadFile->GetAsNumber();

					info.Enable[3] = lpReadFile->GetAsNumber();

					info.KillCount = lpReadFile->GetAsNumber();

					info.Money = lpReadFile->GetAsNumber();

					#if(GAMESERVER_SHOP>=1)

					info.Coin1 = lpReadFile->GetAsNumber();

					#endif

					#if(GAMESERVER_SHOP==3)

					info.Coin2 = lpReadFile->GetAsNumber();

					info.Coin3 = lpReadFile->GetAsNumber();

					#endif

					strcpy_s(info.Message,lpReadFile->GetAsString());

					this->SetInfo(info);
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					MONSTER_KILL_ITEM_LIST info;

					memset(&info,0,sizeof(info));

					info.Index = lpReadFile->GetNumber();

					info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(),lpReadFile->GetAsNumber()));

					info.Level = lpReadFile->GetAsNumber();

					info.Dur = lpReadFile->GetAsNumber();

					info.Option1 = lpReadFile->GetAsNumber();

					info.Option2 = lpReadFile->GetAsNumber();

					info.Option3 = lpReadFile->GetAsNumber();

					info.NewOption = lpReadFile->GetAsNumber();

					info.SetOption = lpReadFile->GetAsNumber();

					info.Duration = lpReadFile->GetAsNumber();

					this->SetItemInfo(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CCustomMonsterKillCount::SetInfo(MONSTER_KILL_COUNT_INFO info) // OK
{
	std::map<int,MONSTER_KILL_COUNT_INFO>::iterator it = this->m_MonsterKillCountInfo.find(info.Index);

	if(it != this->m_MonsterKillCountInfo.end())
	{
		return;
	}

	this->m_MonsterKillCountInfo.insert(std::pair<int,MONSTER_KILL_COUNT_INFO>(info.Index,info));
}

void CCustomMonsterKillCount::SetItemInfo(MONSTER_KILL_ITEM_LIST info) // OK
{
	std::map<int,MONSTER_KILL_COUNT_INFO>::iterator it = this->m_MonsterKillCountInfo.find(info.Index);

	if(it == this->m_MonsterKillCountInfo.end())
	{
		return;
	}

	it->second.ItemList.push_back(info);
}

void CCustomMonsterKillCount::CheckMonsterKill(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	for(std::map<int,MONSTER_KILL_COUNT_INFO>::iterator it=this->m_MonsterKillCountInfo.begin();it != this->m_MonsterKillCountInfo.end();it++)
	{
		if(it->second.MonsterClass != lpObj->Class)
		{
			continue;
		}

		if(it->second.Enable[lpTarget->AccountLevel] == 0)
		{
			continue;
		}

		SDHP_MONSTER_KILL_COUNT_SEND pMsg;

		pMsg.header.set(0x50,sizeof(pMsg));

		pMsg.Index = lpTarget->Index;

		memcpy(pMsg.Account,lpTarget->Account,sizeof(pMsg.Account));

		memcpy(pMsg.Name,lpTarget->Name,sizeof(pMsg.Name));

		pMsg.MonsterClass = lpObj->Class;

		gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

		return;
	}
}

void CCustomMonsterKillCount::DGMonsterKillCountRecv(SDHP_MONSTER_KILL_COUNT_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->Index,lpMsg->Account) == 0)
	{
		LogAdd(LOG_RED,"[DGMonsterKillCountRecv] Invalid Account [%d](%s)",lpMsg->Index,lpMsg->Account);
		CloseClient(lpMsg->Index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->Index];

	for(std::map<int,MONSTER_KILL_COUNT_INFO>::iterator it=this->m_MonsterKillCountInfo.begin();it != this->m_MonsterKillCountInfo.end();it++)
	{
		if(it->second.MonsterClass != lpMsg->MonsterClass)
		{
			continue;
		}

		if(lpMsg->Count > it->second.KillCount)
		{
			continue;
		}

		if(lpMsg->Count < it->second.KillCount)
		{
			gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,"%s %d/%d",gMonsterManager->GetName(lpMsg->MonsterClass),lpMsg->Count,it->second.KillCount);
			continue;
		}

		if(it->second.Money > 0)
		{
			if(gObjCheckMaxMoney(lpObj->Index,it->second.Money) == 0)
			{
				lpObj->Money = MAX_MONEY;
			}
			else
			{
				lpObj->Money += it->second.Money;
			}

			GCMoneySend(lpObj->Index,lpObj->Money);
		}

		if(it->second.Coin1 > 0 || it->second.Coin2 > 0 || it->second.Coin3 > 0)
		{
			gObjCoinAdd(lpObj->Index,it->second.Coin1,it->second.Coin2,it->second.Coin3);
	
			gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(183),it->second.Coin1,it->second.Coin2,it->second.Coin3);
		}

		int px = lpObj->X;
		int py = lpObj->Y;

		for(std::vector<MONSTER_KILL_ITEM_LIST>::iterator ir = it->second.ItemList.begin(); ir != it->second.ItemList.end(); ir++)
		{
			BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

			for(int u = 0; u < ir->SocketCount && ir->SocketCount <= MAX_SOCKET_OPTION; u++)
			{
				ItemSocketOption[u] = 0xFE;
			}

			if(gObjGetRandomFreeLocation(lpObj->Map,&px,&py,2,2,50) == 0)
			{
				px = lpObj->X;
				py = lpObj->Y;
			}

			GDCreateItemSend(lpObj->Index,lpObj->Map,px,py,ir->ItemIndex,ir->Level,ir->Dur,ir->Option1,ir->Option2,ir->Option3,lpObj->Index,ir->NewOption,ir->SetOption,0,0,ItemSocketOption,0xFF,((ir->Duration>0) ? ((DWORD)time(0)+ir->Duration) : 0));
		}

		GCServerCommandSend(lpObj->Index,0,lpObj->X,lpObj->Y,1);

		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,it->second.Message);

		return;
	}
}