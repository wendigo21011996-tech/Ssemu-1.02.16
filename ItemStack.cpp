// ItemStack.cpp: implementation of the CItemStack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemStack.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemStack::CItemStack() // OK
{
	this->m_ItemStackInfo.clear();
}

CItemStack::~CItemStack() // OK
{

}

void CItemStack::Load(char* path) // OK
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

	this->m_ItemStackInfo.clear();

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

					ITEM_STACK_INFO info;

					info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

					info.Level = lpReadFile->GetAsNumber();

					info.MaxStack = lpReadFile->GetAsNumber();

					info.CreateItemIndex = -1;

					this->m_ItemStackInfo.push_back(info);
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					ITEM_STACK_INFO info;

					info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

					info.Level = -1;

					info.MaxStack = lpReadFile->GetAsNumber();

					info.CreateItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(),lpReadFile->GetAsNumber()));

					this->m_ItemStackInfo.push_back(info);
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

	this->GCItemListStackSend();
}

bool CItemStack::CheckItemStack(int index,int level) // OK
{
	for (std::vector<ITEM_STACK_INFO>::iterator it = this->m_ItemStackInfo.begin(); it != this->m_ItemStackInfo.end(); it++)
	{
		if(it->ItemIndex != index)
		{
			continue;
		}

		if(it->Level != -1 && it->Level != level)
		{
			continue;
		}

		return 1;
	}

	return 0;
}

int CItemStack::GetItemMaxStack(int index,int level) // OK
{
	for (std::vector<ITEM_STACK_INFO>::iterator it = this->m_ItemStackInfo.begin(); it != this->m_ItemStackInfo.end(); it++)
	{
		if(it->ItemIndex != index)
		{
			continue;
		}

		if(it->Level != -1 && it->Level != level)
		{
			continue;
		}

		return it->MaxStack;
	}
	
	return 0;
}

int CItemStack::GetCreateItemIndex(int index) // OK
{
	for (std::vector<ITEM_STACK_INFO>::iterator it = this->m_ItemStackInfo.begin(); it != this->m_ItemStackInfo.end(); it++)
	{
		if(it->ItemIndex != index)
		{
			continue;
		}

		return it->CreateItemIndex;
	}

	return -1;
}

void CItemStack::GCItemListStackSend(int aIndex) // OK
{
	BYTE send[8192];

	PMSG_ITEM_LIST_STACK_SEND pMsg;

	pMsg.header.set(0xF3,0xEC,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_ITEM_STACK_INFO info;

	for (std::vector<ITEM_STACK_INFO>::iterator it = this->m_ItemStackInfo.begin(); it != this->m_ItemStackInfo.end(); it++)
	{
		info.Index = it->ItemIndex;

		info.Level = it->Level;

		info.MaxStack = it->MaxStack;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	if(aIndex == -1)
	{
		for(int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
		{
			if(gObjIsConnectedGP(n) != 0)
			{
				DataSend(n,send,size);
			}
		}
	}
	else
	{
		DataSend(aIndex,send,size);
	}
}