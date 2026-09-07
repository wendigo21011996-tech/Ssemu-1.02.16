// GoldenArcher.cpp: implementation of the CGoldenArcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GoldenArcher.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "ItemManager.h"
#include "ItemOptionRate.h"
#include "ReadFile.h"
#include "RandomManager.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGoldenArcher::CGoldenArcher() // OK
{
	#if(GAMESERVER_UPDATE<=603)

	this->m_GoldenArcherItemInfo.clear();

	#endif
}

CGoldenArcher::~CGoldenArcher() // OK
{

}

void CGoldenArcher::Load(char* path) // OK
{
	#if(GAMESERVER_UPDATE<=603)

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

	this->m_GoldenArcherItemInfo.clear();

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

			GOLDEN_ARCHER_ITEM_INFO info;

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.Group = lpReadFile->GetAsNumber();

			info.Option0 = lpReadFile->GetAsNumber();

			info.Option1 = lpReadFile->GetAsNumber();

			info.Option2 = lpReadFile->GetAsNumber();

			info.Option3 = lpReadFile->GetAsNumber();

			info.Option4 = lpReadFile->GetAsNumber();

			info.Option5 = lpReadFile->GetAsNumber();

			#if(GAMESERVER_UPDATE>=401)

			info.Option6 = lpReadFile->GetAsNumber();

			#endif

			info.Duration = lpReadFile->GetAsNumber();

			info.DropRate = lpReadFile->GetAsNumber();

			this->m_GoldenArcherItemInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	#endif
}

void CGoldenArcher::CGGoldenArcherCountRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->GoldenArcherTransaction[0] == 0)?(lpObj->GoldenArcherTransaction[0]++):lpObj->GoldenArcherTransaction[0]) != 0)
	{
		return;
	}

	SDHP_GOLDEN_ARCHER_COUNT_SEND pMsg;

	pMsg.header.set(0x94,0x00,sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CGoldenArcher::CGGoldenArcherRegisterRecv(PMSG_GOLDEN_ARCHER_COUNT_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(gItemManager->GetInventoryItemCount(lpObj,GET_ITEM(14,21),0) != 0)
	{
		gItemManager->DeleteInventoryItemCount(lpObj,GET_ITEM(14,21),0,1);

		this->GDGoldenArcherAddCountSaveSend(aIndex,1);

		this->CGGoldenArcherCountRecv(aIndex);
	}
	
	#endif
}

void CGoldenArcher::CGGoldenArcherRewardRecv(PMSG_GOLDEN_ARCHER_REWARD_RECV* lpMsg,int aIndex) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(((lpObj->GoldenArcherTransaction[1] == 0)?(lpObj->GoldenArcherTransaction[1]++):lpObj->GoldenArcherTransaction[1]) != 0)
	{
		return;
	}

	SDHP_GOLDEN_ARCHER_REWARD_SEND pMsg;

	pMsg.header.set(0x94,0x01,sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.type = lpMsg->type;

	pMsg.count = m_GoldenArcherAmount[pMsg.type];

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CGoldenArcher::CGGoldenArcherCloseRecv(int aIndex) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use == 0 || lpObj->Interface.type == INTERFACE_NONE || lpObj->Interface.type != INTERFACE_GOLDEN_ARCHER)
	{
		return;
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;

	#endif
}

void CGoldenArcher::DGGoldenArcherCountRecv(SDHP_GOLDEN_ARCHER_COUNT_RECV* lpMsg) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGGoldenArcherCountRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->GoldenArcherTransaction[0] = 0;

	PMSG_GOLDEN_ARCHER_COUNT_SEND pMsg;

	pMsg.header.set(0x94,sizeof(pMsg));

	pMsg.type = 0;

	pMsg.count = lpMsg->count;

	memset(pMsg.code,0,sizeof(pMsg.code));

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	#endif
}

void CGoldenArcher::DGGoldenArcherRewardRecv(SDHP_GOLDEN_ARCHER_REWARD_RECV* lpMsg) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGGoldenArcherRewardRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->GoldenArcherTransaction[1] = 0;

	if(lpMsg->result != 0)
	{
		CRandomManager RandomManager;

		for(std::vector<GOLDEN_ARCHER_ITEM_INFO>::iterator it=this->m_GoldenArcherItemInfo.begin();it != this->m_GoldenArcherItemInfo.end();it++)
		{
			if(it->Group == lpMsg->type)
			{
				RandomManager.AddElement((int)(&(*it)),it->DropRate);
			}
		}

		GOLDEN_ARCHER_ITEM_INFO* lpGoldenArcherItemInfo = 0;

		if(RandomManager.GetRandomElement((int*)&lpGoldenArcherItemInfo) != 0)
		{
			WORD ItemIndex = lpGoldenArcherItemInfo->ItemIndex;
			BYTE ItemLevel = 0;
			BYTE ItemOption1 = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;
			BYTE ItemSetOption = 0;
			BYTE ItemSocketOption[MAX_SOCKET_OPTION] = {0xFF,0xFF,0xFF,0xFF,0xFF};

			gItemOptionRate->GetItemOption0(lpGoldenArcherItemInfo->Option0,&ItemLevel);

			gItemOptionRate->GetItemOption1(lpGoldenArcherItemInfo->Option1,&ItemOption1);

			gItemOptionRate->GetItemOption2(lpGoldenArcherItemInfo->Option2,&ItemOption2);

			gItemOptionRate->GetItemOption3(lpGoldenArcherItemInfo->Option3,&ItemOption3);

			gItemOptionRate->GetItemOption4(lpGoldenArcherItemInfo->Option4,&ItemNewOption);

			gItemOptionRate->GetItemOption5(lpGoldenArcherItemInfo->Option5,&ItemSetOption);

			gItemOptionRate->GetItemOption6(lpGoldenArcherItemInfo->Option6,&ItemSocketOption[0]);

			gItemOptionRate->MakeNewOption(ItemIndex,1,ItemNewOption,&ItemNewOption);

			gItemOptionRate->MakeSetOption(ItemIndex,ItemSetOption,&ItemSetOption);

			gItemOptionRate->MakeSocketOption(ItemIndex,ItemSocketOption[0],&ItemSocketOption[0]);

			if(gItemManager->CheckItemInventorySpace(lpObj,ItemIndex) != 0)
			{
				GDCreateItemSend(lpObj->Index,0xEB,0,0,ItemIndex,ItemLevel,0,ItemOption1,ItemOption2,ItemOption3,-1,ItemNewOption,ItemSetOption,0,0,ItemSocketOption,0xFF,((lpGoldenArcherItemInfo->Duration>0)?((DWORD)time(0)+lpGoldenArcherItemInfo->Duration):0));
			}
			else
			{
				GDCreateItemSend(lpObj->Index,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,ItemIndex,ItemLevel,0,ItemOption1,ItemOption2,ItemOption3,lpObj->Index,ItemNewOption,ItemSetOption,0,0,ItemSocketOption,0xFF,((lpGoldenArcherItemInfo->Duration>0)?((DWORD)time(0)+lpGoldenArcherItemInfo->Duration):0));
			}

			GCServerCommandSend(lpObj->Index,0,lpObj->X,lpObj->Y,1);
		}

		this->CGGoldenArcherCountRecv(lpMsg->index);
	}

	#endif
}

void CGoldenArcher::GDGoldenArcherAddCountSaveSend(int aIndex, DWORD count) // OK
{
	#if(GAMESERVER_UPDATE<=603)

	SDHP_GOLDEN_ARCHER_ADD_COUNT_SAVE_SEND pMsg;

	pMsg.header.set(0x94,0x30,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	pMsg.count = count;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));

	#endif
}