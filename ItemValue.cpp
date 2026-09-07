// ItemValue.cpp: implementation of the CItemValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemValue.h"
#include "ItemManager.h"
#include "ItemStack.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemValue::CItemValue() // OK
{
	this->m_ItemValueInfo.clear();
}

CItemValue::~CItemValue() // OK
{

}

void CItemValue::Load(char* path) // OK
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

	this->m_ItemValueInfo.clear();

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

			ITEM_VALUE_INFO info;

			info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetNumber(),lpReadFile->GetAsNumber()));

			info.Level = lpReadFile->GetAsNumber();

			info.Grade = lpReadFile->GetAsNumber();

			info.Value = lpReadFile->GetAsNumber();

			this->m_ItemValueInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;

	this->GCItemListValueSend();
}

bool CItemValue::GetItemValue(CItem* lpItem,int* value) // OK
{
	for(std::vector<ITEM_VALUE_INFO>::iterator it = this->m_ItemValueInfo.begin(); it != this->m_ItemValueInfo.end(); it++)
	{
		if(it->ItemIndex == lpItem->m_Index)
		{
			if(it->Level == -1 || it->Level == lpItem->m_Level)
			{
				if(it->Grade == -1 || it->Grade == lpItem->m_NewOption)
				{
					if(gItemStack->GetItemMaxStack(it->ItemIndex,it->Level) == 0 || it->ItemIndex == GET_ITEM(4,7) || it->ItemIndex == GET_ITEM(4,15))
					{
						(*value) = it->Value;
						return 1;
					}
					else
					{
						(*value) = (int)(it->Value*lpItem->m_Durability);
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

void CItemValue::GCItemListValueSend(int aIndex) // OK
{
	BYTE send[8192];

	PMSG_ITEM_LIST_VALUE_SEND pMsg;

	pMsg.header.set(0xF3,0xED,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	PMSG_ITEM_VALUE_INFO info;

	for (std::vector<ITEM_VALUE_INFO>::iterator it = this->m_ItemValueInfo.begin(); it != this->m_ItemValueInfo.end(); it++)
	{
		info.Index = it->ItemIndex;

		info.Level = it->Level;

		info.Grade = it->Grade;

		info.Value = it->Value;

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