// ChaosBox.cpp: implementation of the CChaosBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChaosBox.h"
#include "BonusManager.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "CrywolfSync.h"
#include "CustomMix.h"
#include "DSProtocol.h"
#include "GuildManager.h"
#include "ItemBagManager.h"
#include "ItemOptionRate.h"
#include "JewelOfHarmonyOption.h"
#include "JewelOfHarmonyType.h"
#include "Log.h"
#include "Message.h"
#include "Notice.h"
#include "RandomManager.h"
#include "ServerInfo.h"
#include "SetItemType.h"
#include "Shop.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChaosBox::CChaosBox() // OK
{
	this->m_SeniorMixLimitDay = 0;

	this->m_SeniorMixLimitMonth = 0;

	this->m_SeniorMixLimitYear = 0;
}

CChaosBox::~CChaosBox() // OK
{

}

void CChaosBox::ChaosBoxInit(LPOBJ lpObj) // OK
{
	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		lpObj->ChaosBox[n].Clear();
		lpObj->ChaosBoxMap[n] = 0xFF;
	}
}

void CChaosBox::ChaosBoxItemDown(LPOBJ lpObj,int slot) // OK
{
	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(n != slot)
		{
			lpObj->ChaosBox[n].Clear();
			lpObj->ChaosBoxMap[n] = 0xFF;
			continue;
		}

		CItem* lpItem = &lpObj->ChaosBox[n];
		
		lpItem->m_Level = 0;

		float dur = (float)gItemManager->GetItemDurability(lpItem->m_Index,lpItem->m_Level,lpItem->IsExcItem(),lpItem->IsSetItem());

		lpItem->m_Durability = dur*(lpItem->m_Durability/lpItem->m_BaseDurability);

		lpItem->Convert(lpItem->m_Index,lpItem->m_Option1,lpItem->m_Option2,lpItem->m_Option3,lpItem->m_NewOption,lpItem->m_SetOption,lpItem->m_JewelOfHarmonyOption,lpItem->m_ItemOptionEx,lpItem->m_SocketOption,lpItem->m_SocketOptionBonus);
	}
}

void CChaosBox::ChaosBoxItemKeep(LPOBJ lpObj,int slot) // OK
{
	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(n != slot)
		{
			lpObj->ChaosBox[n].Clear();
			lpObj->ChaosBoxMap[n] = 0xFF;
			continue;
		}
	}
}

void CChaosBox::ChaosBoxItemSave(LPOBJ lpObj) // OK
{
	if(gObjInventoryCommit(lpObj->Index) != 0)
	{
		for(int n=0;n < CHAOS_BOX_SIZE;n++)
		{
			if(lpObj->ChaosBox[n].IsItem() != 0)
			{
				gItemManager->InventoryInsertItem(lpObj->Index,lpObj->ChaosBox[n]);
			}
		}
	}
}

bool CChaosBox::GetTalismanOfLuckRate(LPOBJ lpObj,int* rate) // OK
{
	int count = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,53)) // Talisman of Luck
		{
			count += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count>MAX_TALISMAN_OF_LUCK)?0:1);
}

bool CChaosBox::GetElementalTalismanOfLuckRate(LPOBJ lpObj,int* rate) // OK
{
	int count = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,189)) // Elemental Talisman of Luck
		{
			count += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count>MAX_TALISMAN_OF_LUCK)?0:1);
}

void CChaosBox::ChaosItemMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
		{
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if(ChaosCount == 0 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(gServerInfo->m_ChaosItemMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney/20000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_ChaosItemMixRate[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate*10000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		CItem item;

		if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHAOS_BOX,0,-1,lpObj,&item) == 0)
		{
			this->GCChaosMixSend(lpObj->Index,0,0);
			return;
		}

		GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

		gLog->Output(LOG_CHAOS_MIX,"[ChaosItemMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[ChaosItemMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::DevilSquareMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int EyeCount = 0;
	int EyeLevel = 0;
	int KeyCount = 0;
	int KeyLevel = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,17))
		{
			EyeCount++;
			EyeLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,18))
		{
			KeyCount++;
			KeyLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if(ChaosCount != 1 || EyeCount != 1 || KeyCount != 1 || EyeLevel != KeyLevel)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(EyeLevel > 7 || KeyLevel > 7)
	{
		this->GCChaosMixSend(lpObj->Index,8,0);
		return;
	}

	switch(EyeLevel)
	{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney = 100000;
			break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney = 200000;
			break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney = 400000;
			break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney = 700000;
			break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney = 1100000;
			break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney = 1600000;
			break;
		case 7:
			lpObj->ChaosSuccessRate = gServerInfo->m_DevilSquareMixRate7[lpObj->AccountLevel];
			lpObj->ChaosMoney = 2000000;
			break;
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	#if(GAMESERVER_UPDATE>=201)

	lpObj->ChaosSuccessRate += gCrywolfSync->GetPlusChaosRate();

	#endif

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(14,19),EyeLevel,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[DevilSquareMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[DevilSquareMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::PlusItemLevelMix(LPOBJ lpObj,int type) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;
	int ChaosAmulet = 0;
	int ElementalChaosAmulet = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13))
		{
			BlessCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
		{
			SoulCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,96))
		{
			ChaosAmulet++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,190))
		{
			ElementalChaosAmulet++;
		}
		else if(lpObj->ChaosBox[n].m_Level == (9+type))
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if(ChaosCount != 1 || SoulCount < (type+1) || BlessCount < (type+1) || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(lpObj->ChaosBox[ItemSlot].IsSocketItem() != 0)
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_PlusSocketItemLevelMixRate[type][lpObj->AccountLevel];
	}
	else if(lpObj->ChaosBox[ItemSlot].IsExcItem() != 0 || lpObj->ChaosBox[ItemSlot].IsSetItem() != 0)
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_PlusExcSetItemLevelMixRate[type][lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_PlusCommonItemLevelMixRate[type][lpObj->AccountLevel];
	}

	if(lpObj->ChaosBox[ItemSlot].m_Option2 != 0)
	{
		lpObj->ChaosSuccessRate += gServerInfo->m_AddLuckSuccessRate2[lpObj->AccountLevel];
	}

	if(lpObj->ChaosBox[ItemSlot].IsPentagramItem() == 0 && this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	if(lpObj->ChaosBox[ItemSlot].IsPentagramItem() != 0 && this->GetElementalTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 2000000*(type+1);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];

		item.m_Level++;

		item.m_Durability = (float)gItemManager->GetItemDurability(item.m_Index,item.m_Level,item.IsExcItem(),item.IsSetItem());

		item.m_Durability = (item.m_Durability*lpObj->ChaosBox[ItemSlot].m_Durability)/item.m_BaseDurability;

		this->ChaosBoxInit(lpObj);

		gItemManager->ChaosBoxAddItem(lpObj->Index,item,0);

		this->GCChaosMixSend(lpObj->Index,1,&item);

		gObjCustomLogPlusChaosMix(lpObj,type,item.m_Index);

		gLog->Output(LOG_CHAOS_MIX,"[PlusItemLevelMix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		if(ChaosAmulet == 0 && ElementalChaosAmulet == 0)
		{
			this->ChaosBoxInit(lpObj);

			this->GCChaosBoxSend(lpObj,0);

			this->GCChaosMixSend(lpObj->Index,0,0);

			gLog->Output(LOG_CHAOS_MIX,"[PlusItemLevelMix][Failure][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d,ChaosAmulet: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney,(ChaosAmulet+ElementalChaosAmulet));
		}
		else
		{
			this->ChaosBoxItemDown(lpObj,ItemSlot);

			this->GCChaosBoxSend(lpObj,0);

			this->GCChaosMixSend(lpObj->Index,0,0);

			gLog->Output(LOG_CHAOS_MIX,"[PlusItemLevelMix][Failure][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d,ChaosAmulet: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney,(ChaosAmulet+ElementalChaosAmulet));
		}
	}
}

void CChaosBox::DinorantMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int UniriaCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,2) && lpObj->ChaosBox[n].m_Durability == 255)
		{
			UniriaCount++;
		}
	}

	if(ChaosCount != 1 || UniriaCount != 10)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_DinorantMixRate[lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 500000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = GET_ITEM(13,3);
		BYTE ItemNewOption = 0;

		gItemOptionRate->GetItemOption4(3,&ItemNewOption);

		gItemOptionRate->MakeNewOption(ItemIndex,1,ItemNewOption,&ItemNewOption);

		GDCreateItemSend(lpObj->Index,0xFF,0,0,ItemIndex,0,255,0,0,0,-1,ItemNewOption,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[DinorantMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[DinorantMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::FruitMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int CreationCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
		{
			CreationCount++;
		}
	}

	if(ChaosCount != 1 || CreationCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_FruitMixRate[lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 3000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		BYTE ItemLevel = GetLargeRand()%5;

		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,15),ItemLevel,0,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[FruitMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[FruitMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::Wing2Mix(LPOBJ lpObj,int type) // OK
{
	int ChaosCount = 0;
	int FeatherCount = 0;
	int SleeveCount = 0;
	int WingItemCount = 0;
	int WingItemMoney = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int TalismanOfWingType = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,14) && lpObj->ChaosBox[n].m_Level == 0)
		{
			FeatherCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,14) && lpObj->ChaosBox[n].m_Level == 1)
		{
			SleeveCount++;
		}
		else if((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12,0) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12,2)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12,41))
		{
			WingItemCount++;
			WingItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if(lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(13,88) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(13,92))
		{
			TalismanOfWingType = lpObj->ChaosBox[n].m_Index-GET_ITEM(13,87);
		}
	}

	if(ChaosCount != 1 || (type == 0 && FeatherCount != 1) || (type == 1 && SleeveCount != 1) || WingItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(gServerInfo->m_Wing2MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (WingItemMoney/4000000)+(ItemMoney/40000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_Wing2MixRate[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	if(gServerInfo->m_Wing2MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>90)?90:lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 5000000;
	
	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		if(type == 0)
		{
			CItem item;

			if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHAOS_BOX,2,-1,lpObj,&item) == 0)
			{
				this->GCChaosMixSend(lpObj->Index,0,0);
				return;
			}

			switch(TalismanOfWingType)
			{
				case 1:
					item.m_Index = GET_ITEM(12,5);
					break;
				case 2:
					item.m_Index = GET_ITEM(12,4);
					break;
				case 3:
					item.m_Index = GET_ITEM(12,3);
					break;
				case 4:
					item.m_Index = GET_ITEM(12,42);
					break;
				case 5:
					item.m_Index = GET_ITEM(12,6);
					break;
			}

			GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

			gLog->Output(LOG_CHAOS_MIX,"[Wing2Mix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
		}
		else
		{
			CItem item;

			if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHAOS_BOX,3,-1,lpObj,&item) == 0)
			{
				this->GCChaosMixSend(lpObj->Index,0,0);
				return;
			}

			GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

			gLog->Output(LOG_CHAOS_MIX,"[Wing2Mix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
		}
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[Wing2Mix][Failure][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::BloodCastleMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int BoneCount = 0;
	int BoneLevel = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,16))
		{
			ScrollCount++;
			ScrollLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,17))
		{
			BoneCount++;
			BoneLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if(ChaosCount != 1 || ScrollCount != 1 || BoneCount != 1 || ScrollLevel != BoneLevel)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(ScrollLevel > 8 || BoneLevel > 8)
	{
		this->GCChaosMixSend(lpObj->Index,8,0);
		return;
	}

	switch(ScrollLevel)
	{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney = 50000;
			break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney = 80000;
			break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney = 150000;
			break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney = 250000;
			break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney = 400000;
			break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney = 600000;
			break;
		case 7:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate7[lpObj->AccountLevel];
			lpObj->ChaosMoney = 850000;
			break;
		case 8:
			lpObj->ChaosSuccessRate = gServerInfo->m_BloodCastleMixRate8[lpObj->AccountLevel];
			lpObj->ChaosMoney = 1050000;
			break;
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	#if(GAMESERVER_UPDATE>=201)

	lpObj->ChaosSuccessRate += gCrywolfSync->GetPlusChaosRate();

	#endif

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) <= lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,18),ScrollLevel,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[BloodCastleMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[BloodCastleMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::Wing1Mix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int ChaosItem = 0;
	int ItemCount = 0;
	int ItemMoney = 0;
	int TalismanOfWingType = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
		{
			ItemCount++;
			lpObj->ChaosBox[n].OldValue();
			ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
		}
		else if((lpObj->ChaosBox[n].m_Index == GET_ITEM(2,6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(4,6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(5,7)) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ChaosItem++;
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(13,83) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(13,86))
		{
			TalismanOfWingType = lpObj->ChaosBox[n].m_Index-GET_ITEM(13,82);
		}
	}

	if(ChaosCount == 0 || ChaosItem == 0)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(gServerInfo->m_Wing1MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney/20000);
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_Wing1MixRate[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate*10000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		CItem item;

		if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHAOS_BOX,1,-1,lpObj,&item) == 0)
		{
			this->GCChaosMixSend(lpObj->Index,0,0);
			return;
		}

		switch(TalismanOfWingType)
		{
			case 1:
				item.m_Index = GET_ITEM(12,2);
				break;
			case 2:
				item.m_Index = GET_ITEM(12,1);
				break;
			case 3:
				item.m_Index = GET_ITEM(12,0);
				break;
			case 4:
				item.m_Index = GET_ITEM(12,41);
				break;
		}

		GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

		gLog->Output(LOG_CHAOS_MIX,"[Wing1Mix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[Wing1Mix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::PetMix(LPOBJ lpObj,int type) // OK
{
	int ChaosCount = 0;
	int SoulOfDarkHorseCount = 0;
	int SoulOfDarkSpiritCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int CreationCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulOfDarkHorseCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,31) && lpObj->ChaosBox[n].m_Level == 1)
		{
			SoulOfDarkSpiritCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13))
		{
			BlessCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
		{
			SoulCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
		{
			CreationCount++;
		}
	}

	if(ChaosCount != 1 || (type == 0 && SoulOfDarkHorseCount != 1) || (type == 1 && SoulOfDarkSpiritCount != 1) || BlessCount != (5-(type*3)) || SoulCount != (5-(type*3)) || CreationCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_PetMixRate[lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 5000000-(4000000*type);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFE,0,0,(GET_ITEM(13,4)+type),0,0,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[PetMix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,1);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[PetMix][Failure][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
}

void CChaosBox::SiegePotionMix(LPOBJ lpObj,int type) // OK
{
	int BlessCount = 0;
	int SoulCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13))
		{
			BlessCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
		{
			SoulCount++;
		}
	}

	if((type == 0 && (BlessCount == 0 || BlessCount > 25)) || (type == 1 && (SoulCount == 0 || SoulCount > 25)))
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosMoney = 100000-(50000*type);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(14,7),type,((BlessCount+SoulCount)*10),0,0,0,-1,0,0,0,0,0,0xFF,0);

	gLog->Output(LOG_CHAOS_MIX,"[SiegePotionMix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
}

void CChaosBox::LifeStoneMix(LPOBJ lpObj) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int GuardianCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13))
		{
			BlessCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
		{
			SoulCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,31))
		{
			GuardianCount++;
		}
	}

	if(ChaosCount != 1 || BlessCount != 5 || SoulCount != 5 || GuardianCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosMoney = 5000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,11),1,0,0,0,0,-1,0,0,0,0,0,0xFF,0);

	gLog->Output(LOG_CHAOS_MIX,"[LifeStoneMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
}

void CChaosBox::SeniorMix(LPOBJ lpObj) // OK
{
	if(gCastleSiege->CheckGuildOwnCastle(lpObj->GuildName) == 0 || lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(743));
		this->GCChaosMixSend(lpObj->Index,0,0);
		return;
	}

	SYSTEMTIME SystemTime;

	GetSystemTime(&SystemTime);

	if(this->m_SeniorMixLimitDay == SystemTime.wDay && this->m_SeniorMixLimitMonth == SystemTime.wMonth && this->m_SeniorMixLimitYear == SystemTime.wYear)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(744));
		this->GCChaosMixSend(lpObj->Index,0,0);
		return;
	}

	int SoulPack10 = 0;
	int BlessPack10 = 0;
	int GuardianCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,30))
		{
			BlessPack10 += lpObj->ChaosBox[n].m_Level+1;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,31))
		{
			SoulPack10 += lpObj->ChaosBox[n].m_Level+1;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,31))
		{
			GuardianCount++;
		}
	}

	if(SoulPack10 != 3 || BlessPack10 != 3 || GuardianCount != 30)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosMoney = 1000000000;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	CItem item;

	if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_SENIOR_MIX,-1,-1,lpObj,&item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,0,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	this->m_SeniorMixLimitDay = SystemTime.wDay;

	this->m_SeniorMixLimitMonth = SystemTime.wMonth;

	this->m_SeniorMixLimitYear = SystemTime.wYear;

	GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

	gLog->Output(LOG_CHAOS_MIX,"[SeniorMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
}

void CChaosBox::PieceOfHornMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int ChaosCount = 0;
	int SplinterOfArmorCount = 0;
	int BlessOfGuardianCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,32))
		{
			SplinterOfArmorCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,33))
		{
			BlessOfGuardianCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if(ChaosCount != 1 || SplinterOfArmorCount != 20 || BlessOfGuardianCount != 20)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_PieceOfHornMixRate[lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,35),0,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[PieceOfHornMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[PieceOfHornMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::BrokenHornMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int ChaosCount = 0;
	int ClawOfBeastCount = 0;
	int PieceOfHornCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,34))
		{
			ClawOfBeastCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,35))
		{
			PieceOfHornCount++;
		}
	}

	if(ChaosCount != 1 || ClawOfBeastCount != 10 || PieceOfHornCount != 5)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_BrokenHornMixRate[lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,36),0,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[BrokenHornMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[BrokenHornMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::HornOfFenrirMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int ChaosCount = 0;
	int BrokenHornCount = 0;
	int LifeCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,36))
		{
			BrokenHornCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,16))
		{
			LifeCount++;
		}
	}

	if(ChaosCount != 1 || BrokenHornCount != 1 || LifeCount != 3)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_HornOfFenrirMixRate[lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,37),0,255,1,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[HornOfFenrirMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[HornOfFenrirMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::HornOfFenrirUpgradeMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int ChaosCount = 0;
	int HornOfFenrirCount = 0;
	int LifeCount = 0;
	int WeaponCount = 0;
	int WeaponMoney = 0;
	int ArmorCount = 0;
	int ArmorMoney = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,37))
		{
			HornOfFenrirCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,16))
		{
			LifeCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(0,0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(6,0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			WeaponCount++;
			WeaponMoney = lpObj->ChaosBox[n].m_BuyMoney;
		}
		else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(6,0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(12,0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ArmorCount++;
			ArmorMoney = lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if(ChaosCount != 1 || HornOfFenrirCount != 1 || LifeCount != 5 || (WeaponCount == 0 && ArmorCount == 0) || (WeaponCount != 0 && ArmorCount != 0))
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(gServerInfo->m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (WeaponMoney+ArmorMoney)/10000;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	if(gServerInfo->m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>80)?80:lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		BYTE ItemNewOption = ((WeaponCount==0)?((ArmorCount==0)?0:2):1);

		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,37),0,255,1,0,0,-1,ItemNewOption,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[HornOfFenrirUpgradeMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[HornOfFenrirUpgradeMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::ShieldPotionMix(LPOBJ lpObj,int type) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int LargeHealingPotionCount = 0;
	int SmallCompoundPotionCount = 0;
	int MediumCompoundPotionCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,3))
		{
			LargeHealingPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,38))
		{
			SmallCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,39))
		{
			MediumCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if((type == 0 && LargeHealingPotionCount != 3) || (type == 1 && SmallCompoundPotionCount != 3) || (type == 2 && MediumCompoundPotionCount != 3))
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_ShieldPotionMixRate[type][lpObj->AccountLevel];

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = ((type==0)?100000:(500000*type));

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,(GET_ITEM(14,35)+type),0,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[ShieldPotionMix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[ShieldPotionMix][Failure][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::JewelOfHarmonyItemPurityMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int GemStoneCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,41))
		{
			GemStoneCount++;
		}
	}

	if(GemStoneCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_JewelOfHarmonyItemPurityMixRate[lpObj->AccountLevel];

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(14,42),0,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[JewelOfHarmonyItemPurityMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[JewelOfHarmonyItemPurityMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::JewelOfHarmonyItemSmeltMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int ItemCount = 0;
	int ItemSlot = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(4,7) || lpObj->ChaosBox[n].m_Index == GET_ITEM(4,15))
		{
			continue;
		}

		if(gJewelOfHarmonyType->CheckJewelOfHarmonyItemType(&lpObj->ChaosBox[n]) != 0 && lpObj->ChaosBox[n].IsSetItem() == 0 && lpObj->ChaosBox[n].IsJewelOfHarmonyItem() == 0)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if(ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(lpObj->ChaosBox[ItemSlot].IsExcItem() == 0)
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_JewelOfHarmonyItemSmeltMixRate1[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_JewelOfHarmonyItemSmeltMixRate2[lpObj->AccountLevel];
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		WORD ItemIndex = ((lpObj->ChaosBox[ItemSlot].IsExcItem()==0)?GET_ITEM(14,43):GET_ITEM(14,44));

		GDCreateItemSend(lpObj->Index,0xFF,0,0,ItemIndex,0,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[JewelOfHarmonyItemSmeltMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[JewelOfHarmonyItemSmeltMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::JewelOfHarmonyItemRestoreMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int ItemCount = 0;
	int ItemSlot = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].IsJewelOfHarmonyItem() != 0)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if(ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	lpObj->ChaosMoney = gJewelOfHarmonyOption->GetJewelOfHarmonyItemRestoreMoney(&lpObj->ChaosBox[ItemSlot]);

	if(lpObj->ChaosMoney == -1)
	{
		this->GCChaosMixSend(lpObj->Index,0,0);
		return;
	}

	lpObj->ChaosSuccessRate = gServerInfo->m_JewelOfHarmonyItemRestoreMixRate[lpObj->AccountLevel];

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];

		item.m_JewelOfHarmonyOption = JEWEL_OF_HARMONY_OPTION_NONE;

		this->ChaosBoxInit(lpObj);

		gItemManager->ChaosBoxAddItem(lpObj->Index,item,0);

		this->GCChaosMixSend(lpObj->Index,1,&item);

		gLog->Output(LOG_CHAOS_MIX,"[JewelOfHarmonyItemRestoreMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[JewelOfHarmonyItemRestoreMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::Item380Mix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	int GuardianCount = 0;
	int HarmonyCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,31))
		{
			GuardianCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,42))
		{
			HarmonyCount++;
		}
		else if(lpObj->ChaosBox[n].Is380Item() == 0 && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	if(GuardianCount != 1 || HarmonyCount != 1 || ItemCount != 1)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(lpObj->ChaosBox[ItemSlot].m_Level >= 7)
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_Item380MixRate2[lpObj->AccountLevel];
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_Item380MixRate1[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}
	
	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	lpObj->ChaosMoney = 10000000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->ChaosBox[ItemSlot];

		item.m_ItemOptionEx |= 0x80;

		this->ChaosBoxInit(lpObj);

		gItemManager->ChaosBoxAddItem(lpObj->Index,item,0);

		this->GCChaosMixSend(lpObj->Index,1,&item);

		gLog->Output(LOG_CHAOS_MIX,"[Item380Mix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxItemKeep(lpObj,ItemSlot);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[Item380Mix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::IllusionTempleMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int PotionCount = 0;
	int PotionLevel = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,49))
		{
			ScrollCount++;
			ScrollLevel = lpObj->ChaosBox[n].m_Level;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,50))
		{
			PotionCount++;
			PotionLevel = lpObj->ChaosBox[n].m_Level;
		}
	}

	if(ChaosCount != 1 || ScrollCount != 1 || PotionCount != 1 || ScrollLevel != PotionLevel)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(ScrollLevel > 6 || PotionLevel > 6)
	{
		this->GCChaosMixSend(lpObj->Index,8,0);
		return;
	}

	switch(ScrollLevel)
	{
		case 1:
			lpObj->ChaosSuccessRate = gServerInfo->m_IllusionTempleMixRate1[lpObj->AccountLevel];
			lpObj->ChaosMoney = 3000000;
			break;
		case 2:
			lpObj->ChaosSuccessRate = gServerInfo->m_IllusionTempleMixRate2[lpObj->AccountLevel];
			lpObj->ChaosMoney = 5000000;
			break;
		case 3:
			lpObj->ChaosSuccessRate = gServerInfo->m_IllusionTempleMixRate3[lpObj->AccountLevel];
			lpObj->ChaosMoney = 7000000;
			break;
		case 4:
			lpObj->ChaosSuccessRate = gServerInfo->m_IllusionTempleMixRate4[lpObj->AccountLevel];
			lpObj->ChaosMoney = 9000000;
			break;
		case 5:
			lpObj->ChaosSuccessRate = gServerInfo->m_IllusionTempleMixRate5[lpObj->AccountLevel];
			lpObj->ChaosMoney = 11000000;
			break;
		case 6:
			lpObj->ChaosSuccessRate = gServerInfo->m_IllusionTempleMixRate6[lpObj->AccountLevel];
			lpObj->ChaosMoney = 13000000;
			break;
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	lpObj->ChaosSuccessRate += gCrywolfSync->GetPlusChaosRate();

	lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,51),ScrollLevel,1,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[IllusionTempleMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[IllusionTempleMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::FeatherOfCondorMix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulPack10 = 0;
	int WingCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
		{
			CreationCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if(((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12,3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12,6)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12,42) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12,49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13,30)) && (lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1))
		{
			WingCount++;
		}
		else if(lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 7 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if(ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || WingCount != 1 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(gServerInfo->m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney/300000)+1;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_FeatherOfCondorMixRate[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	if(gServerInfo->m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>60)?60:lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate*200000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		GDCreateItemSend(lpObj->Index,0xFF,0,0,GET_ITEM(13,53),0,0,0,0,0,-1,0,0,0,0,0,0xFF,0);

		gLog->Output(LOG_CHAOS_MIX,"[FeatherOfCondorMix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[FeatherOfCondorMix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::Wing3Mix(LPOBJ lpObj) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulPack10 = 0;
	int BlessPack10 = 0;
	int FlameCount = 0;
	int FeatherCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
		{
			ChaosCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
		{
			CreationCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,31) && lpObj->ChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,30) && lpObj->ChaosBox[n].m_Level == 0)
		{
			BlessPack10++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,52))
		{
			FlameCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,53))
		{
			FeatherCount++;
		}
		else if(lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
		}
	}

	if(ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || BlessPack10 != 1 || FlameCount != 1 || FeatherCount != 1 || ItemCount == 0)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	if(gServerInfo->m_Wing3MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = (ItemMoney/3000000)+1;
	}
	else
	{
		lpObj->ChaosSuccessRate = gServerInfo->m_Wing3MixRate[lpObj->AccountLevel];
	}

	if(this->GetTalismanOfLuckRate(lpObj,&lpObj->ChaosSuccessRate) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,240,0);
		return;
	}

	lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

	if(gServerInfo->m_Wing3MixRate[lpObj->AccountLevel] == -1)
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>40)?40:lpObj->ChaosSuccessRate);
	}
	else
	{
		lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100)?100:lpObj->ChaosSuccessRate);
	}

	lpObj->ChaosMoney = lpObj->ChaosSuccessRate*200000;

	int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

	lpObj->ChaosMoney += TaxMoney;

	if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		this->GCChaosMixSend(lpObj->Index,2,0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index,lpObj->Money);

	gCastleSiegeSync->AddTributeMoney(TaxMoney);

	if((GetLargeRand()%100) < lpObj->ChaosSuccessRate)
	{
		CItem item;

		if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHAOS_BOX,4,-1,lpObj,&item) == 0)
		{
			this->GCChaosMixSend(lpObj->Index,0,0);
			return;
		}

		GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

		gLog->Output(LOG_CHAOS_MIX,"[Wing3Mix][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		this->GCChaosBoxSend(lpObj,0);

		this->GCChaosMixSend(lpObj->Index,0,0);

		gLog->Output(LOG_CHAOS_MIX,"[Wing3Mix][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
	}

	#endif
}

void CChaosBox::ChaosCardMix(LPOBJ lpObj,int type) // OK
{
	int ChaosCardCount = 0;
	int ChaosCardGoldCount = 0;
	int ChaosCardRareCount = 0;
	int ChaosCardMiniCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,54))
		{
			type = 1;
			ChaosCardCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,92))
		{
			type = 2;
			ChaosCardGoldCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,93))
		{
			type = 3;
			ChaosCardRareCount++;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,95))
		{
			type = 4;
			ChaosCardMiniCount++;
		}
	}

	if(type == 0 || (type == 1 && ChaosCardCount != 1) || (type == 2 && ChaosCardGoldCount != 1) || (type == 3 && ChaosCardRareCount != 1) || (type == 4 && ChaosCardMiniCount != 1))
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	CItem item;

	if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHAOS_CARD_MIX,(type-1),-1,lpObj,&item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,0,0);
		return;
	}

	GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

	gLog->Output(LOG_CHAOS_MIX,"[ChaosCardMix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
}

void CChaosBox::CherryBlossomMix(LPOBJ lpObj,int type) // OK
{
	int WhiteCherryBlossomCount = 0;
	int RedCherryBlossomCount = 0;
	int GoldenCherryBlossomCount = 0;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,88))
		{
			type = 1;
			WhiteCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,89))
		{
			type = 2;
			RedCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
		else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,90))
		{
			type = 3;
			GoldenCherryBlossomCount += (int)lpObj->ChaosBox[n].m_Durability;
		}
	}

	if(type == 0 || (type == 1 && WhiteCherryBlossomCount != 10) || (type == 2 && RedCherryBlossomCount != 30) || (type == 3 && GoldenCherryBlossomCount != 255))
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	CItem item;

	if(gItemBagManager->GetItemBySpecialValue(ITEM_BAG_SPECIAL_CHERRY_BLOSSOM_MIX,(type-1),-1,lpObj,&item) == 0)
	{
		this->GCChaosMixSend(lpObj->Index,0,0);
		return;
	}

	GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

	gLog->Output(LOG_CHAOS_MIX,"[CherryBlossomMix][Success][%s][%s] - (Type: %d,ChaosSuccessRate: %d,ChaosMoney: %d)",lpObj->Account,lpObj->Name,type,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);
}

void CChaosBox::SocketItemCreateSeedMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::SocketItemCreateSeedSphereMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::SocketItemMountSeedSphereMix(LPOBJ lpObj,BYTE info) // OK
{
	
}

void CChaosBox::SocketItemUnMountSeedSphereMix(LPOBJ lpObj,BYTE info) // OK
{
	
}

void CChaosBox::ImperialGuardianMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::ChestMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::SummonScrollMix(LPOBJ lpObj,int type) // OK
{
	
}

void CChaosBox::LuckyItemCreateMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::LuckyItemRefineMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::ArcaBattleTrophiesMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::ArcaBattleRegisterMark(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::MonsterWingMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::SocketWeaponMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::PentagramMithrilMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::PentagramElixirMix(LPOBJ lpObj) // OK
{
	
}

void CChaosBox::PentagramJewelMix(LPOBJ lpObj) // OK
{

}

void CChaosBox::PentagramDecompositeMix(LPOBJ lpObj,int type) // OK
{
	
}

void CChaosBox::PentagramJewelUpgradeLevelMix(LPOBJ lpObj,BYTE info) // OK
{
	
}

void CChaosBox::PentagramJewelUpgradeRankMix(LPOBJ lpObj,BYTE info) // OK
{
	
}

void CChaosBox::CustomMix(LPOBJ lpObj,int Type) // OK
{
	#if(GAMESERVER_UPDATE>=301)

	CUSTOM_MIX_INFO* lpInfo = gCustomMix->GetInfoMix(Type);

	if(lpInfo == 0)
	{
		this->GCChaosMixSend(lpObj->Index,7,0);
		return;
	}

	for(std::map<int,CUSTOM_MIX_GROUP_INFO>::iterator it = lpInfo->MixGroup.begin(); it != lpInfo->MixGroup.end(); it++)
	{
		if(gCustomMix->CheckGroup(lpObj,&it->second) != 0)
		{
			lpObj->ChaosSuccessRate = lpInfo->MixRate[lpObj->AccountLevel];

			lpObj->ChaosSuccessRate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,lpObj->ChaosSuccessRate);

			lpObj->ChaosSuccessRate = ((lpObj->ChaosSuccessRate>100) ? 100 : lpObj->ChaosSuccessRate);

			lpObj->ChaosMoney = lpInfo->MixMoney;

			int TaxMoney = (lpObj->ChaosMoney*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;

			lpObj->ChaosMoney += TaxMoney;

			if(lpObj->Money < ((DWORD)lpObj->ChaosMoney))
			{
				this->GCChaosMixSend(lpObj->Index,2,0);
				return;
			}

			lpObj->Money -= lpObj->ChaosMoney;

			GCMoneySend(lpObj->Index,lpObj->Money);

			gCastleSiegeSync->AddTributeMoney(TaxMoney);

			if((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
			{
				CItem item;

				if(gItemBagManager->GetItemBySpecialValue(it->second.BagSpecial,-1,-1,lpObj,&item) == 0)
				{
					this->GCChaosMixSend(lpObj->Index,0,0);
					return;
				}

				GDCreateItemSend(lpObj->Index,0xFF,0,0,item.m_Index,(BYTE)item.m_Level,(BYTE)item.m_Durability,item.m_Option1,item.m_Option2,item.m_Option3,-1,item.m_NewOption,item.m_SetOption,item.m_JewelOfHarmonyOption,item.m_ItemOptionEx,item.m_SocketOption,item.m_SocketOptionBonus,0);

				gLog->Output(LOG_CHAOS_MIX,"[CustomMix][%s][Success][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpInfo->Name,lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);

				return;
			}
			else
			{
				this->ChaosBoxInit(lpObj);

				this->GCChaosBoxSend(lpObj,0);

				this->GCChaosMixSend(lpObj->Index,0,0);

				gLog->Output(LOG_CHAOS_MIX,"[CustomMix][%s][Failure][%s][%s] - (ChaosSuccessRate: %d,ChaosMoney: %d)",lpInfo->Name,lpObj->Account,lpObj->Name,lpObj->ChaosSuccessRate,lpObj->ChaosMoney);

				return;
			}
		}
	}

	this->GCChaosMixSend(lpObj->Index,0,0);

	#endif
}

void CChaosBox::CGChaosMixRecv(PMSG_CHAOS_MIX_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->ChaosLock != 0)
	{
		return;
	}

	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Permission[9] != 0)
	{
		return;
	}

	lpObj->ChaosLock = 1;

	lpObj->ChaosMoney = 0;

	lpObj->ChaosSuccessRate = 0;

	lpObj->IsChaosMixCompleted = 1;

	switch(lpMsg->type)
	{
		case CHAOS_MIX_CHAOS_ITEM:
			this->ChaosItemMix(lpObj);
			break;
		case CHAOS_MIX_DEVIL_SQUARE:
			this->DevilSquareMix(lpObj);
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL1:
			this->PlusItemLevelMix(lpObj,0);
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL2:
			this->PlusItemLevelMix(lpObj,1);
			break;
		case CHAOS_MIX_DINORANT:
			this->DinorantMix(lpObj);
			break;
		case CHAOS_MIX_FRUIT:
			this->FruitMix(lpObj);
			break;
		case CHAOS_MIX_WING1:
			this->Wing2Mix(lpObj,0);
			break;
		case CHAOS_MIX_BLOOD_CASTLE:
			this->BloodCastleMix(lpObj);
			break;
		case CHAOS_MIX_WING2:
			this->Wing1Mix(lpObj);
			break;
		case CHAOS_MIX_PET1:
			this->PetMix(lpObj,0);
			break;
		case CHAOS_MIX_PET2:
			this->PetMix(lpObj,1);
			break;
		case CHAOS_MIX_SIEGE_POTION1:
			this->SiegePotionMix(lpObj,0);
			break;
		case CHAOS_MIX_SIEGE_POTION2:
			this->SiegePotionMix(lpObj,1);
			break;
		case CHAOS_MIX_LIFE_STONE:
			this->LifeStoneMix(lpObj);
			break;
		case CHAOS_MIX_SENIOR:
			this->SeniorMix(lpObj);
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL3:
			this->PlusItemLevelMix(lpObj,2);
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL4:
			this->PlusItemLevelMix(lpObj,3);
			break;
		case CHAOS_MIX_WING3:
			this->Wing2Mix(lpObj,1);
			break;
		case CHAOS_MIX_PIECE_OF_HORN:
			this->PieceOfHornMix(lpObj);
			break;
		case CHAOS_MIX_BROKEN_HORN:
			this->BrokenHornMix(lpObj);
			break;
		case CHAOS_MIX_HORN_OF_FENRIR:
			this->HornOfFenrirMix(lpObj);
			break;
		case CHAOS_MIX_HORN_OF_FENRIR_UPGRADE:
			this->HornOfFenrirUpgradeMix(lpObj);
			break;
		case CHAOS_MIX_SHIELD_POTION1:
			this->ShieldPotionMix(lpObj,0);
			break;
		case CHAOS_MIX_SHIELD_POTION2:
			this->ShieldPotionMix(lpObj,1);
			break;
		case CHAOS_MIX_SHIELD_POTION3:
			this->ShieldPotionMix(lpObj,2);
			break;
		case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY:
			this->JewelOfHarmonyItemPurityMix(lpObj);
			break;
		case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT:
			this->JewelOfHarmonyItemSmeltMix(lpObj);
			break;
		case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_RESTORE:
			this->JewelOfHarmonyItemRestoreMix(lpObj);
			break;
		case CHAOS_MIX_ITEM_380:
			this->Item380Mix(lpObj);
			break;
		case CHAOS_MIX_ILLUSION_TEMPLE:
			this->IllusionTempleMix(lpObj);
			break;
		case CHAOS_MIX_FEATHER_OF_CONDOR:
			this->FeatherOfCondorMix(lpObj);
			break;
		case CHAOS_MIX_WING4:
			this->Wing3Mix(lpObj);
			break;
		case CHAOS_MIX_CHAOS_CARD:
			this->ChaosCardMix(lpObj,0);
			break;
		case CHAOS_MIX_CHERRY_BLOSSOM:
			this->CherryBlossomMix(lpObj,0);
			break;
		case CHAOS_MIX_SOCKET_ITEM_CREATE_SEED:
			this->SocketItemCreateSeedMix(lpObj);
			break;
		case CHAOS_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE:
			this->SocketItemCreateSeedSphereMix(lpObj);
			break;
		case CHAOS_MIX_SOCKET_ITEM_MOUNT_SEED_SPHERE:
			this->SocketItemMountSeedSphereMix(lpObj,lpMsg->info);
			break;
		case CHAOS_MIX_SOCKET_ITEM_UN_MOUNT_SEED_SPHERE:
			this->SocketItemUnMountSeedSphereMix(lpObj,lpMsg->info);
			break;
		case CHAOS_MIX_IMPERIAL_GUARDIAN:
			this->ImperialGuardianMix(lpObj);
			break;
		case CHAOS_MIX_CHEST:
			this->ChestMix(lpObj);
			break;
		case CHAOS_MIX_SUMMON_SCROLL:
			this->SummonScrollMix(lpObj,0);
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL5:
			this->PlusItemLevelMix(lpObj,4);
			break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL6:
			this->PlusItemLevelMix(lpObj,5);
			break;
		case CHAOS_MIX_LUCKY_ITEM_CREATE:
			this->LuckyItemCreateMix(lpObj);
			break;
		case CHAOS_MIX_LUCKY_ITEM_REFINE:
			this->LuckyItemRefineMix(lpObj);
			break;
		case CHAOS_MIX_ARCA_BATTLE_TROPHIES_MIX:
			this->ArcaBattleTrophiesMix(lpObj);
			break;
		case CHAOS_MIX_ARCA_BATTLE_REGISTER_MARK:
			this->ArcaBattleRegisterMark(lpObj);
			break;
		case CHAOS_MIX_MONSTER_WING:
			this->MonsterWingMix(lpObj);
			break;
		case CHAOS_MIX_SOCKET_WEAPON:
			this->SocketWeaponMix(lpObj);
			break;
		default:
			this->CustomMix(lpObj,lpMsg->type);
			break;
	}
}

void CChaosBox::CGChaosMixRateRecv(PMSG_CHAOS_MIX_RATE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use == 0 || (lpObj->Interface.type != INTERFACE_CHAOS_BOX && lpObj->Interface.type != INTERFACE_TRAINER))
	{
		return;
	}

	if(lpObj->Permission[9] != 0)
	{
		return;
	}

	PMSG_CHAOS_MIX_RATE_SEND pMsg;

	pMsg.header.set(0x88,sizeof(pMsg));

	pMsg.rate = 0;

	pMsg.money = 0;

	pMsg.tax = 0;

	switch(lpMsg->type)
	{
		case CHAOS_MIX_CHAOS_ITEM:
		{
			int ChaosCount = 0;
			int ItemCount = 0;
			int ItemMoney = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
					lpObj->ChaosBox[n].OldValue();
					ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
				{
					lpObj->ChaosBox[n].OldValue();
					ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ItemCount++;
					ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
			}

			if(ChaosCount == 0 || ItemCount == 0)
			{
				return;
			}

			if(gServerInfo->m_ChaosItemMixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = (ItemMoney/20000);
			}
			else
			{
				pMsg.rate = gServerInfo->m_ChaosItemMixRate[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.rate = ((pMsg.rate>100)?100:pMsg.rate);

			pMsg.money = pMsg.rate*10000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_DEVIL_SQUARE:
		{
			int ChaosCount = 0;
			int EyeCount = 0;
			int EyeLevel = 0;
			int KeyCount = 0;
			int KeyLevel = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,17))
				{
					EyeCount++;
					EyeLevel = lpObj->ChaosBox[n].m_Level;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,18))
				{
					KeyCount++;
					KeyLevel = lpObj->ChaosBox[n].m_Level;
				}
			}

			if(ChaosCount != 1 || EyeCount != 1 || KeyCount != 1 || EyeLevel != KeyLevel)
			{
				return;
			}

			if(EyeLevel > 7 || KeyLevel > 7)
			{
				return;
			}

			switch(EyeLevel)
			{
				case 1:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate1[lpObj->AccountLevel];
					pMsg.money = 100000;
					break;
				case 2:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate2[lpObj->AccountLevel];
					pMsg.money = 200000;
					break;
				case 3:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate3[lpObj->AccountLevel];
					pMsg.money = 400000;
					break;
				case 4:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate4[lpObj->AccountLevel];
					pMsg.money = 700000;
					break;
				case 5:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate5[lpObj->AccountLevel];
					pMsg.money = 1100000;
					break;
				case 6:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate6[lpObj->AccountLevel];
					pMsg.money = 1600000;
					break;
				case 7:
					pMsg.rate = gServerInfo->m_DevilSquareMixRate7[lpObj->AccountLevel];
					pMsg.money = 2000000;
					break;
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			#if(GAMESERVER_UPDATE>=201)

			pMsg.rate += gCrywolfSync->GetPlusChaosRate();

			#endif

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_PLUS_ITEM_LEVEL1:
		case CHAOS_MIX_PLUS_ITEM_LEVEL2:
		case CHAOS_MIX_PLUS_ITEM_LEVEL3:
		case CHAOS_MIX_PLUS_ITEM_LEVEL4:
		{
			int ChaosCount = 0;
			int BlessCount = 0;
			int SoulCount = 0;
			int ItemCount = 0;
			int ItemSlot = 0;
			int type = 0;

			switch(lpMsg->type)
			{
				case CHAOS_MIX_PLUS_ITEM_LEVEL1:
					type = 0;
					break;
				case CHAOS_MIX_PLUS_ITEM_LEVEL2:
					type = 1;
					break;
				case CHAOS_MIX_PLUS_ITEM_LEVEL3:
					type = 2;
					break;
				case CHAOS_MIX_PLUS_ITEM_LEVEL4:
					type = 3;
					break;
				case CHAOS_MIX_PLUS_ITEM_LEVEL5:
					type = 4;
					break;
				case CHAOS_MIX_PLUS_ITEM_LEVEL6:
					type = 5;
					break;
			}

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13))
				{
					BlessCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
				{
					SoulCount++;
				}
				else if(lpObj->ChaosBox[n].m_Level == (9+type))
				{
					ItemCount++;
					ItemSlot = n;
				}
			}

			if(ChaosCount != 1 || SoulCount < (type+1) || BlessCount < (type+1) || ItemCount != 1)
			{
				return;
			}

			if(lpObj->ChaosBox[ItemSlot].IsSocketItem() != 0)
			{
				pMsg.rate = gServerInfo->m_PlusSocketItemLevelMixRate[type][lpObj->AccountLevel];
			}
			else if(lpObj->ChaosBox[ItemSlot].IsExcItem() != 0 || lpObj->ChaosBox[ItemSlot].IsSetItem() != 0)
			{
				pMsg.rate = gServerInfo->m_PlusExcSetItemLevelMixRate[type][lpObj->AccountLevel];
			}
			else
			{
				pMsg.rate = gServerInfo->m_PlusCommonItemLevelMixRate[type][lpObj->AccountLevel];
			}

			if(lpObj->ChaosBox[ItemSlot].m_Option2 != 0)
			{
				pMsg.rate += gServerInfo->m_AddLuckSuccessRate2[lpObj->AccountLevel];
			}

			if(lpObj->ChaosBox[ItemSlot].IsPentagramItem() == 0 && this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			if(lpObj->ChaosBox[ItemSlot].IsPentagramItem() != 0 && this->GetElementalTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = 2000000*(type+1);

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_DINORANT:
		{
			int ChaosCount = 0;
			int UniriaCount = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,2) && lpObj->ChaosBox[n].m_Durability == 255)
				{
					UniriaCount++;
				}
			}

			if(ChaosCount != 1 || UniriaCount != 10)
			{
				return;
			}

			pMsg.rate = gServerInfo->m_DinorantMixRate[lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = 500000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_FRUIT:
		{
			int ChaosCount = 0;
			int CreationCount = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
				{
					CreationCount++;
				}
			}

			if(ChaosCount != 1 || CreationCount != 1)
			{
				return;
			}

			pMsg.rate = gServerInfo->m_FruitMixRate[lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = 3000000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_WING1:
		case CHAOS_MIX_WING3:
		{
			int ChaosCount = 0;
			int FeatherCount = 0;
			int SleeveCount = 0;
			int WingItemCount = 0;
			int WingItemMoney = 0;
			int ItemCount = 0;
			int ItemMoney = 0;
			int TalismanOfWingType = 0;
			int type = 0;

			switch(lpMsg->type)
			{
				case CHAOS_MIX_WING1:
					type = 0;
					break;
				case CHAOS_MIX_WING3:
					type = 1;
					break;
			}

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,14) && lpObj->ChaosBox[n].m_Level == 0)
				{
					FeatherCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,14) && lpObj->ChaosBox[n].m_Level == 1)
				{
					SleeveCount++;
				}
				else if((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12,0) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12,2)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12,41))
				{
					WingItemCount++;
					WingItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
				else if(lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 4)
				{
					ItemCount++;
					ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(13,88) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(13,92))
				{
					TalismanOfWingType = lpObj->ChaosBox[n].m_Index-GET_ITEM(13,87);
				}
			}

			if(ChaosCount != 1 || (type == 0 && FeatherCount != 1) || (type == 1 && SleeveCount != 1) || WingItemCount != 1)
			{
				return;
			}

			if(gServerInfo->m_Wing2MixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = (WingItemMoney/4000000)+(ItemMoney/40000);
			}
			else
			{
				pMsg.rate = gServerInfo->m_Wing2MixRate[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			if(gServerInfo->m_Wing2MixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = ((pMsg.rate>90)?90:pMsg.rate);
			}

			pMsg.money = 5000000;
	
			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_BLOOD_CASTLE:
		{
			int ChaosCount = 0;
			int ScrollCount = 0;
			int ScrollLevel = 0;
			int BoneCount = 0;
			int BoneLevel = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,16))
				{
					ScrollCount++;
					ScrollLevel = lpObj->ChaosBox[n].m_Level;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,17))
				{
					BoneCount++;
					BoneLevel = lpObj->ChaosBox[n].m_Level;
				}
			}

			if(ChaosCount != 1 || ScrollCount != 1 || BoneCount != 1 || ScrollLevel != BoneLevel)
			{
				return;
			}

			if(ScrollLevel > 8 || BoneLevel > 8)
			{
				return;
			}

			switch(ScrollLevel)
			{
				case 1:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate1[lpObj->AccountLevel];
					pMsg.money = 50000;
					break;
				case 2:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate2[lpObj->AccountLevel];
					pMsg.money = 80000;
					break;
				case 3:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate3[lpObj->AccountLevel];
					pMsg.money = 150000;
					break;
				case 4:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate4[lpObj->AccountLevel];
					pMsg.money = 250000;
					break;
				case 5:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate5[lpObj->AccountLevel];
					pMsg.money = 400000;
					break;
				case 6:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate6[lpObj->AccountLevel];
					pMsg.money = 600000;
					break;
				case 7:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate7[lpObj->AccountLevel];
					pMsg.money = 850000;
					break;
				case 8:
					pMsg.rate = gServerInfo->m_BloodCastleMixRate8[lpObj->AccountLevel];
					pMsg.money = 1050000;
					break;
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			#if(GAMESERVER_UPDATE>=201)

			pMsg.rate += gCrywolfSync->GetPlusChaosRate();

			#endif

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_WING2:
		{
			int ChaosCount = 0;
			int ChaosItem = 0;
			int ItemCount = 0;
			int ItemMoney = 0;
			int TalismanOfWingType = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
					lpObj->ChaosBox[n].OldValue();
					ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13) || lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
				{
					ItemCount++;
					lpObj->ChaosBox[n].OldValue();
					ItemMoney += lpObj->ChaosBox[n].m_OldBuyMoney;
				}
				else if((lpObj->ChaosBox[n].m_Index == GET_ITEM(2,6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(4,6) || lpObj->ChaosBox[n].m_Index == GET_ITEM(5,7)) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ChaosItem++;
					ItemCount++;
					ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ItemCount++;
					ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(13,83) && lpObj->ChaosBox[n].m_Index >= GET_ITEM(13,86))
				{
					TalismanOfWingType = lpObj->ChaosBox[n].m_Index-GET_ITEM(13,82);
				}
			}

			if(ChaosCount == 0 || ChaosItem == 0)
			{
				return;
			}

			if(gServerInfo->m_Wing1MixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = (ItemMoney/20000);
			}
			else
			{
				pMsg.rate = gServerInfo->m_Wing1MixRate[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.rate = ((pMsg.rate>100)?100:pMsg.rate);

			pMsg.money = pMsg.rate*10000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_PET1:
		case CHAOS_MIX_PET2:
		{
			int ChaosCount = 0;
			int SoulOfDarkHorseCount = 0;
			int SoulOfDarkSpiritCount = 0;
			int BlessCount = 0;
			int SoulCount = 0;
			int CreationCount = 0;
			int type = 0;

			switch(lpMsg->type)
			{
				case CHAOS_MIX_PET1:
					type = 0;
					break;
				case CHAOS_MIX_PET2:
					type = 1;
					break;
			}

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,31) && lpObj->ChaosBox[n].m_Level == 0)
				{
					SoulOfDarkHorseCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,31) && lpObj->ChaosBox[n].m_Level == 1)
				{
					SoulOfDarkSpiritCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,13))
				{
					BlessCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,14))
				{
					SoulCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
				{
					CreationCount++;
				}
			}

			if(ChaosCount != 1 || (type == 0 && SoulOfDarkHorseCount != 1) || (type == 1 && SoulOfDarkSpiritCount != 1) || BlessCount != (5-(type*3)) || SoulCount != (5-(type*3)) || CreationCount != 1)
			{
				return;
			}

			pMsg.rate = gServerInfo->m_PetMixRate[lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = 5000000-(4000000*type);

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		#if(GAMESERVER_UPDATE>=201)
		case CHAOS_MIX_PIECE_OF_HORN:
		{
			int ChaosCount = 0;
			int SplinterOfArmorCount = 0;
			int BlessOfGuardianCount = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,32))
				{
					SplinterOfArmorCount += (int)lpObj->ChaosBox[n].m_Durability;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,33))
				{
					BlessOfGuardianCount += (int)lpObj->ChaosBox[n].m_Durability;
				}
			}

			if(ChaosCount != 1 || SplinterOfArmorCount != 20 || BlessOfGuardianCount != 20)
			{
				return;
			}

			pMsg.rate = gServerInfo->m_PieceOfHornMixRate[lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);
		} break;
		case CHAOS_MIX_BROKEN_HORN:
		{
			int ChaosCount = 0;
			int ClawOfBeastCount = 0;
			int PieceOfHornCount = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,34))
				{
					ClawOfBeastCount += (int)lpObj->ChaosBox[n].m_Durability;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,35))
				{
					PieceOfHornCount++;
				}
			}

			if(ChaosCount != 1 || ClawOfBeastCount != 10 || PieceOfHornCount != 5)
			{
				return;
			}

			pMsg.rate = gServerInfo->m_BrokenHornMixRate[lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);
		} break;
		case CHAOS_MIX_HORN_OF_FENRIR:
		{
			int ChaosCount = 0;
			int BrokenHornCount = 0;
			int LifeCount = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,36))
				{
					BrokenHornCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,16))
				{
					LifeCount++;
				}
			}

			if(ChaosCount != 1 || BrokenHornCount != 1 || LifeCount != 3)
			{
				return;
			}

			pMsg.rate = gServerInfo->m_HornOfFenrirMixRate[lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = 10000000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_HORN_OF_FENRIR_UPGRADE:
		{
			int ChaosCount = 0;
			int HornOfFenrirCount = 0;
			int LifeCount = 0;
			int WeaponCount = 0;
			int WeaponMoney = 0;
			int ArmorCount = 0;
			int ArmorMoney = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,37))
				{
					HornOfFenrirCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,16))
				{
					LifeCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(0,0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(6,0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					WeaponCount++;
					WeaponMoney = lpObj->ChaosBox[n].m_BuyMoney;
				}
				else if(lpObj->ChaosBox[n].m_Index >= GET_ITEM(6,0) && lpObj->ChaosBox[n].m_Index < GET_ITEM(12,0) && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ArmorCount++;
					ArmorMoney = lpObj->ChaosBox[n].m_BuyMoney;
				}
			}

			if(ChaosCount != 1 || HornOfFenrirCount != 1 || LifeCount != 5 || (WeaponCount == 0 && ArmorCount == 0) || (WeaponCount != 0 && ArmorCount != 0))
			{
				return;
			}

			if(gServerInfo->m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = (WeaponMoney+ArmorMoney)/10000;
			}
			else
			{
				pMsg.rate = gServerInfo->m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			if(gServerInfo->m_HornOfFenrirUpgradeMixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = ((pMsg.rate>80)?80:pMsg.rate);
			}

			pMsg.money = 10000000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_SHIELD_POTION1:
		case CHAOS_MIX_SHIELD_POTION2:
		case CHAOS_MIX_SHIELD_POTION3:
		{
			int LargeHealingPotionCount = 0;
			int SmallCompoundPotionCount = 0;
			int MediumCompoundPotionCount = 0;
			int type = 0;

			switch(lpMsg->type)
			{
				case CHAOS_MIX_SHIELD_POTION1:
					type = 0;
					break;
				case CHAOS_MIX_SHIELD_POTION2:
					type = 1;
					break;
				case CHAOS_MIX_SHIELD_POTION3:
					type = 2;
					break;
			}

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,3))
				{
					LargeHealingPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,38))
				{
					SmallCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,39))
				{
					MediumCompoundPotionCount += (int)lpObj->ChaosBox[n].m_Durability;
				}
			}

			if((type == 0 && LargeHealingPotionCount != 3) || (type == 1 && SmallCompoundPotionCount != 3) || (type == 2 && MediumCompoundPotionCount != 3))
			{
				return;
			}

			pMsg.rate = gServerInfo->m_ShieldPotionMixRate[type][lpObj->AccountLevel];

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = ((type==0)?100000:(500000*type));

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY:
		case CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT:
		{
			int GemStoneCount = 0;
			int ItemCount = 0;
			int ItemSlot = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,41))
				{
					GemStoneCount++;
				}

				if(gJewelOfHarmonyType->CheckJewelOfHarmonyItemType(&lpObj->ChaosBox[n]) != 0 && lpObj->ChaosBox[n].IsSetItem() == 0 && lpObj->ChaosBox[n].IsJewelOfHarmonyItem() == 0)
				{
					ItemCount++;
					ItemSlot = n;
				}
			}

			if(GemStoneCount == 1)
			{
				pMsg.rate = gServerInfo->m_JewelOfHarmonyItemPurityMixRate[lpObj->AccountLevel];

				pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);
			}
			else if(ItemCount == 1)
			{
				if(lpObj->ChaosBox[ItemSlot].IsExcItem() == 0)
				{
					pMsg.rate = gServerInfo->m_JewelOfHarmonyItemSmeltMixRate1[lpObj->AccountLevel];
				}
				else
				{
					pMsg.rate = gServerInfo->m_JewelOfHarmonyItemSmeltMixRate2[lpObj->AccountLevel];
				}

				pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);
			}
		} break;
		case CHAOS_MIX_ITEM_380:
		{
			int GuardianCount = 0;
			int HarmonyCount = 0;
			int ItemCount = 0;
			int ItemSlot = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,31))
				{
					GuardianCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,42))
				{
					HarmonyCount++;
				}
				else if(lpObj->ChaosBox[n].Is380Item() == 0 && lpObj->ChaosBox[n].m_Level >= 4 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ItemCount++;
					ItemSlot = n;
				}
			}

			if(GuardianCount != 1 || HarmonyCount != 1 || ItemCount != 1)
			{
				return;
			}

			if(lpObj->ChaosBox[ItemSlot].m_Level >= 7)
			{
				pMsg.rate = gServerInfo->m_Item380MixRate2[lpObj->AccountLevel];
			}
			else
			{
				pMsg.rate = gServerInfo->m_Item380MixRate1[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}
	
			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.money = 10000000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		#endif
		#if(GAMESERVER_UPDATE>=301)
		case CHAOS_MIX_ILLUSION_TEMPLE:
		{
			int ChaosCount = 0;
			int ScrollCount = 0;
			int ScrollLevel = 0;
			int PotionCount = 0;
			int PotionLevel = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,49))
				{
					ScrollCount++;
					ScrollLevel = lpObj->ChaosBox[n].m_Level;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,50))
				{
					PotionCount++;
					PotionLevel = lpObj->ChaosBox[n].m_Level;
				}
			}

			if(ChaosCount != 1 || ScrollCount != 1 || PotionCount != 1 || ScrollLevel != PotionLevel)
			{
				return;
			}

			if(ScrollLevel > 6 || PotionLevel > 6)
			{
				return;
			}

			switch(ScrollLevel)
			{
				case 1:
					pMsg.rate = gServerInfo->m_IllusionTempleMixRate1[lpObj->AccountLevel];
					pMsg.money = 3000000;
					break;
				case 2:
					pMsg.rate = gServerInfo->m_IllusionTempleMixRate2[lpObj->AccountLevel];
					pMsg.money = 5000000;
					break;
				case 3:
					pMsg.rate = gServerInfo->m_IllusionTempleMixRate3[lpObj->AccountLevel];
					pMsg.money = 7000000;
					break;
				case 4:
					pMsg.rate = gServerInfo->m_IllusionTempleMixRate4[lpObj->AccountLevel];
					pMsg.money = 9000000;
					break;
				case 5:
					pMsg.rate = gServerInfo->m_IllusionTempleMixRate5[lpObj->AccountLevel];
					pMsg.money = 11000000;
					break;
				case 6:
					pMsg.rate = gServerInfo->m_IllusionTempleMixRate6[lpObj->AccountLevel];
					pMsg.money = 13000000;
					break;
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			pMsg.rate += gCrywolfSync->GetPlusChaosRate();

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_FEATHER_OF_CONDOR:
		{
			int ChaosCount = 0;
			int CreationCount = 0;
			int SoulPack10 = 0;
			int WingCount = 0;
			int ItemCount = 0;
			int ItemMoney = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
				{
					CreationCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,31) && lpObj->ChaosBox[n].m_Level == 0)
				{
					SoulPack10++;
				}
				else if(((lpObj->ChaosBox[n].m_Index >= GET_ITEM(12,3) && lpObj->ChaosBox[n].m_Index <= GET_ITEM(12,6)) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12,42) || lpObj->ChaosBox[n].m_Index == GET_ITEM(12,49) || lpObj->ChaosBox[n].m_Index == GET_ITEM(13,30)) && (lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1))
				{
					WingCount++;
				}
				else if(lpObj->ChaosBox[n].IsSetItem() != 0 && lpObj->ChaosBox[n].m_Level >= 7 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ItemCount++;
					ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
			}

			if(ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || WingCount != 1 || ItemCount == 0)
			{
				return;
			}

			if(gServerInfo->m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = (ItemMoney/300000)+1;
			}
			else
			{
				pMsg.rate = gServerInfo->m_FeatherOfCondorMixRate[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			if(gServerInfo->m_FeatherOfCondorMixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = ((pMsg.rate>60)?60:pMsg.rate);
			}
			else
			{
				pMsg.rate = ((pMsg.rate>100)?100:pMsg.rate);
			}

			pMsg.money = pMsg.rate*200000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		case CHAOS_MIX_WING4:
		{
			int ChaosCount = 0;
			int CreationCount = 0;
			int SoulPack10 = 0;
			int BlessPack10 = 0;
			int FlameCount = 0;
			int FeatherCount = 0;
			int ItemCount = 0;
			int ItemMoney = 0;

			for(int n=0;n < CHAOS_BOX_SIZE;n++)
			{
				if(lpObj->ChaosBox[n].IsItem() == 0)
				{
					continue;
				}

				if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,15))
				{
					ChaosCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(14,22))
				{
					CreationCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,31) && lpObj->ChaosBox[n].m_Level == 0)
				{
					SoulPack10++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(12,30) && lpObj->ChaosBox[n].m_Level == 0)
				{
					BlessPack10++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,52))
				{
					FlameCount++;
				}
				else if(lpObj->ChaosBox[n].m_Index == GET_ITEM(13,53))
				{
					FeatherCount++;
				}
				else if(lpObj->ChaosBox[n].IsExcItem() != 0 && lpObj->ChaosBox[n].m_Level >= 9 && lpObj->ChaosBox[n].m_Option3 >= 1)
				{
					ItemCount++;
					ItemMoney += lpObj->ChaosBox[n].m_BuyMoney;
				}
			}

			if(ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || BlessPack10 != 1 || FlameCount != 1 || FeatherCount != 1 || ItemCount == 0)
			{
				return;
			}

			if(gServerInfo->m_Wing3MixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = (ItemMoney/3000000)+1;
			}
			else
			{
				pMsg.rate = gServerInfo->m_Wing3MixRate[lpObj->AccountLevel];
			}

			if(this->GetTalismanOfLuckRate(lpObj,&pMsg.rate) == 0)
			{
				return;
			}

			pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

			if(gServerInfo->m_Wing3MixRate[lpObj->AccountLevel] == -1)
			{
				pMsg.rate = ((pMsg.rate>40)?40:pMsg.rate);
			}
			else
			{
				pMsg.rate = ((pMsg.rate>100)?100:pMsg.rate);
			}

			pMsg.money = pMsg.rate*200000;

			pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
		} break;
		#endif
		#if(GAMESERVER_UPDATE>=301)
		default:
		{
			CUSTOM_MIX_INFO* lpInfo = gCustomMix->GetInfoMix(lpMsg->type);

			if(lpInfo == 0)
			{
				return;
			}

			for(std::map<int,CUSTOM_MIX_GROUP_INFO>::iterator it=lpInfo->MixGroup.begin();it != lpInfo->MixGroup.end();it++)
			{
				if(gCustomMix->CheckGroup(lpObj,&it->second) != 0)
				{
					pMsg.rate = lpInfo->MixRate[lpObj->AccountLevel];

					pMsg.rate = gBonusManager->GetBonusValue(lpObj,BONUS_INDEX_CHAOSMIX_RATE,pMsg.rate);

					pMsg.money = lpInfo->MixMoney;

					pMsg.tax = (pMsg.money*gCastleSiegeSync->GetTaxRateChaos(lpObj->Index))/100;
				}
			}
		}break;
		#endif
	}

	pMsg.rate = ((pMsg.rate>100)?100:((pMsg.rate<0)?0:pMsg.rate));

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CChaosBox::CGChaosMixCloseRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_CHAOS_BOX)
	{
		return;
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;

	this->ChaosBoxInit(lpObj);

	gObjInventoryCommit(aIndex);

	lpObj->ChaosLock = 0;

	lpObj->IsChaosMixCompleted = 0;

	lpObj->CastleSeniorMixUpgrade = 0;

	PBMSG_HEAD pMsg;

	pMsg.set(0x87,sizeof(pMsg));

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.size);
}

void CChaosBox::GCChaosBoxSend(LPOBJ lpObj,BYTE type) // OK
{
	if((type == 0 && (lpObj->Interface.type != INTERFACE_CHAOS_BOX || lpObj->Interface.state == 1)) || (type != 0 && (lpObj->Interface.type != INTERFACE_TRAINER || lpObj->Interface.state == 1)))
	{
		return;
	}

	BYTE send[2048];

	PMSG_SHOP_ITEM_LIST_SEND pMsg;

	pMsg.header.set(0x31,0);

	int size = sizeof(pMsg);

	pMsg.type = ((type==0)?3:5);

	pMsg.count = 0;

	PMSG_SHOP_ITEM_LIST info;

	for(int n=0;n < CHAOS_BOX_SIZE;n++)
	{
		if(lpObj->ChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		info.slot = n;

		gItemManager->ItemByteConvert(info.ItemInfo,lpObj->ChaosBox[n]);

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);
}

void CChaosBox::GCChaosMixSend(int aIndex,BYTE result,CItem* lpItem) // OK
{
	PMSG_CHAOS_MIX_SEND pMsg;

	pMsg.header.set(0x86,sizeof(pMsg));

	pMsg.result = result;

	if(lpItem == 0)
	{
		memset(pMsg.ItemInfo,0xFF,sizeof(pMsg.ItemInfo));
	}
	else
	{
		gItemManager->ItemByteConvert(pMsg.ItemInfo,(*lpItem));
	}

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	gObj[aIndex].ChaosLock = 0;
}