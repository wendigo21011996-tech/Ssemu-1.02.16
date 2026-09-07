// CustomPotion.cpp: implementation of the CCustomPotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomPotion.h"
#include "ItemManager.h"
#include "Message.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomPotion::CCustomPotion() // OK
{
	
}

CCustomPotion::~CCustomPotion() // OK
{

}

void CCustomPotion::ReadCustomPotionInfo(char* section,char* path) // OK
{
	this->m_CustomPotionSwitch = GetPrivateProfileInt(section,"CustomPotionSwitch",0,path);

	this->m_CustomPotionPotionDelay = GetPrivateProfileInt(section,"CustomPotionPotionDelay",0,path);

	this->m_CustomPotionPotionRate1 = GetPrivateProfileInt(section,"CustomPotionPotionRate1",0,path);

	this->m_CustomPotionPotionRate2 = GetPrivateProfileInt(section,"CustomPotionPotionRate2",0,path);

	this->m_CustomPotionPotionRate3 = GetPrivateProfileInt(section,"CustomPotionPotionRate3",0,path);
}

void CCustomPotion::MainProc(LPOBJ lpObj) // OK
{
	if(this->m_CustomPotionSwitch == 0)
	{
		return;
	}

	if(lpObj->AutoPotion == 0)
	{
		return;
	}

	if((GetTickCount()-lpObj->AutoPotionTime) < (DWORD)this->m_CustomPotionPotionDelay)
	{
		return;
	}

	PMSG_ITEM_USE_RECV pMsg;

	pMsg.header.set(0x26,sizeof(pMsg));

	pMsg.SourceSlot = 0xFF;

	pMsg.TargetSlot = 0xFF;

	pMsg.type = 0;

	if(this->m_CustomPotionPotionRate1 > 0)
	{
		if(((lpObj->Life*100)/(lpObj->MaxLife+lpObj->AddLife)) < this->m_CustomPotionPotionRate1)
		{
			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,3),-1):pMsg.SourceSlot); // Large Healing Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,2),-1):pMsg.SourceSlot); // Healing Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,1),-1):pMsg.SourceSlot); // Small Healing Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,0),-1):pMsg.SourceSlot); // Apple

			if(INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
			{
				gItemManager->CGItemUseRecv(&pMsg,lpObj->Index);
			}
		}
	}

	if(this->m_CustomPotionPotionRate2 > 0)
	{
		if(((lpObj->Mana*100)/(lpObj->MaxMana+lpObj->AddMana)) < this->m_CustomPotionPotionRate2)
		{
			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,6),-1):pMsg.SourceSlot); // Large Mana Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,5),-1):pMsg.SourceSlot); // Mana Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,4),-1):pMsg.SourceSlot); // Small Mana Potion

			if(INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
			{
				gItemManager->CGItemUseRecv(&pMsg,lpObj->Index);
			}
		}
	}

	if(this->m_CustomPotionPotionRate3 > 0)
	{
		if(((lpObj->Shield*100)/(lpObj->MaxShield +lpObj->AddShield)) < this->m_CustomPotionPotionRate3)
		{
			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,37),-1):pMsg.SourceSlot); // Large SD Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,36),-1):pMsg.SourceSlot); // SD Potion

			pMsg.SourceSlot = ((pMsg.SourceSlot==0xFF)?gItemManager->GetInventoryItemSlot(lpObj,GET_ITEM(14,35),-1):pMsg.SourceSlot); // Small SD Potion

			if(INVENTORY_FULL_RANGE(pMsg.SourceSlot) != 0)
			{
				gItemManager->CGItemUseRecv(&pMsg,lpObj->Index);
			}
		}
	}

	lpObj->AutoPotionTime = GetTickCount();
}

void CCustomPotion::CGCustomPotionToggleRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(this->m_CustomPotionSwitch == 0)
	{
		return;
	}

	lpObj->AutoPotion ^= 1;

	GCNewMessageSend(lpObj->Index,1,1,gMessage->GetMessage(750+lpObj->AutoPotion));
}