#include "stdafx.h"
#include "DSProtocol.h"
#include "BloodCastle.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "ChaosCastle.h"
#include "CommandManager.h"
#include "Console.h"
#include "Crywolf.h"
#include "CrywolfSync.h"
#include "CustomDailyReward.h"
#include "CustomGift.h"
#include "CustomMonsterKillCount.h"
#include "CustomPick.h"
#include "CustomStore.h"
#include "CustomWindowTime.h"
#include "CustomWing.h"
#include "DefaultClassFreebies.h"
#include "Filter.h"
#include "Friend.h"
#include "Fruit.h"
#include "GameMain.h"
#include "GameMaster.h"
#include "Guild.h"
#include "GuildManager.h"
#include "GoldenArcher.h"
#include "HackMoveSpeedCheck.h"
#include "IllusionTemple.h"
#include "InvasionManager.h"
#include "ItemBagManager.h"
#include "ItemStack.h"
#include "ItemValue.h"
#include "JSProtocol.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "MapServerManager.h"
#include "Message.h"
#include "MuCashShop.h"
#include "Notice.h"
#include "NpcTalk.h"
#include "ObjectManager.h"
#include "Party.h"
#include "PeriodicItem.h"
#include "PersonalShop.h"
#include "Quest.h"
#include "Reconnect.h"
#include "ScriptLoader.h"
#include "ServerInfo.h"
#include "Util.h"
#include "Viewport.h"
#include "Warehouse.h"

void DataServerProtocolCore(BYTE head,BYTE* lpMsg,int size) // OK
{
	PROTECT_START

	switch(head)
	{
		case 0x01:
			DGCharacterListRecv((SDHP_CHARACTER_LIST_RECV*)lpMsg);
			break;
		case 0x02:
			DGCharacterCreateRecv((SDHP_CHARACTER_CREATE_RECV*)lpMsg);
			break;
		case 0x03:
			DGCharacterDeleteRecv((SDHP_CHARACTER_DELETE_RECV*)lpMsg);
			break;
		case 0x04:
			DGCharacterInfoRecv((SDHP_CHARACTER_INFO_RECV*)lpMsg);
			break;
		case 0x05:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gWarehouse->DGWarehouseItemRecv((SDHP_WAREHOUSE_ITEM_RECV*)lpMsg);
					break;
				case 0x01:
					gWarehouse->DGWarehouseFreeRecv((SDHP_WAREHOUSE_FREE_RECV*)lpMsg);
					break;
			}
			break;
		case 0x06:
			gItemManager->DGGetItemSerialRecv((SDHP_GET_ITEM_SERIAL_RECV*)lpMsg);
			break;
		case 0x07:
			DGCreateItemRecv((SDHP_CREATE_ITEM_RECV*)lpMsg);
			break;
		case 0x08:
			DGOptionDataRecv((SDHP_OPTION_DATA_RECV*)lpMsg);
			break;
		case 0x09:
			DGPetItemInfoRecv((SDHP_PET_ITEM_INFO_RECV*)lpMsg);
			break;
		case 0x0C:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gQuest->DGQuestKillCountRecv((SDHP_QUEST_KILL_COUNT_RECV*)lpMsg);
					break;
			}
			break;
		case 0x0E:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gNpcTalk->DGNpcLeoTheHelperRecv((SDHP_NPC_LEO_THE_HELPER_RECV*)lpMsg);
					break;
			}
			break;
		case 0x0F:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gCommandManager->DGCommandResetRecv((SDHP_COMMAND_RESET_RECV*)lpMsg);
					break;
				case 0x01:
					gCommandManager->DGCommandMasterResetRecv((SDHP_COMMAND_MASTER_RESET_RECV*)lpMsg);
					break;
				case 0x03:
					gCustomGift->DGCommandGiftRecv((SDHP_COMMAND_GIFT_RECV*)lpMsg);
					break;
				case 0x04:
					gCommandManager->DGCommandRenameRecv((SDHP_COMMAND_RENAME_RECV*)lpMsg);
					break;
				case 0x05:
					gCommandManager->DGCommandBanAccountRecv((SDHP_COMMAND_BAN_ACCOUNT_RECV*)lpMsg);
					break;
				case 0x06:
					gCommandManager->DGCommandBanCharacterRecv((SDHP_COMMAND_BAN_CHARACTER_RECV*)lpMsg);
					break;
			}
			break;
		case 0x13:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gCustomPick->DGCustomPickRecv((SDHP_CUSTOM_PICK_RECV*)lpMsg);
					break;
			}
			break;
		case 0x16:
			gCustomDailyReward->DGDailyRewardCheckRecv((SDHP_DAILY_REWARD_INFO_RECV*)lpMsg);
			break;
		case 0x1C:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gMuCashShop->DGMuCashShopPointRecv((SDHP_MU_CASH_SHOP_POINT_RECV*)lpMsg);
					break;
				case 0x01:
					gMuCashShop->DGMuCashShopItemBuyRecv((SDHP_MU_CASH_SHOP_ITEM_BUY_RECV*)lpMsg);
					break;
				case 0x02:
					gMuCashShop->DGMuCashShopRecievePointRecv((SDHP_MU_CASH_SHOP_RECIEVE_POINT_RECV*)lpMsg);
					break;
			}
			break;
		case 0x1E:
			DGCrywolfSyncRecv((SDHP_CRYWOLF_SYNC_RECV*)lpMsg);
			break;
		case 0x1F:
			DGCrywolfInfoRecv((SDHP_CRYWOLF_INFO_RECV*)lpMsg);
			break;
		case 0x20:
			DGGlobalPostRecv((SDHP_GLOBAL_POST_RECV*)lpMsg);
			break;
		case 0x21:
			DGGlobalNoticeRecv((SDHP_GLOBAL_NOTICE_RECV*)lpMsg);
			break;
		case 0x2A:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gPeriodicItem->DGPeriodicItemRecv((SDHP_PERIODIC_ITEM_RECV*)lpMsg);
					break;
			}
			break;
		case 0x50:
			gCustomMonsterKillCount->DGMonsterKillCountRecv((SDHP_MONSTER_KILL_COUNT_RECV*)lpMsg);
			break;
		case 0x72:
			DGGlobalWhisperRecv((SDHP_GLOBAL_WHISPER_RECV*)lpMsg);
			break;
		case 0x73:
			DGGlobalWhisperEchoRecv((SDHP_GLOBAL_WHISPER_ECHO_RECV*)lpMsg);
			break;
		case 0x80:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x01:
					gCastleSiege->DGCastleSiegeStateRecv((SDHP_CASTLE_SIEGE_STATE_RECV*)lpMsg);
					break;
				case 0x03:
					gCastleSiege->DGCastleSiegeNpcBuyRecv((SDHP_CASTLE_SIEGE_NPC_BUY_RECV*)lpMsg);
					break;
				case 0x04:
					gCastleSiege->DGCastleSiegeNpcRepairRecv((SDHP_CASTLE_SIEGE_NPC_REPAIR_RECV*)lpMsg);
					break;
				case 0x05:
					gCastleSiege->DGCastleSiegeNpcUpgradeRecv((SDHP_CASTLE_SIEGE_NPC_UPGRADE_RECV*)lpMsg);
					break;
				case 0x06:
					gCastleSiege->DGCastleSiegeTaxMoneyInfoRecv((SDHP_CASTLE_SIEGE_TAX_MONEY_INFO_RECV*)lpMsg);
					break;
				case 0x07:
					gCastleSiege->DGCastleSiegeTaxRateChangeRecv((SDHP_CASTLE_SIEGE_TAX_RATE_CHANGE_RECV*)lpMsg);
					break;
				case 0x08:
					gCastleSiege->DGCastleSiegeMoneyOutRecv((SDHP_CASTLE_SIEGE_MONEY_OUT_RECV*)lpMsg);
					break;
				case 0x0A:
					gCastleSiege->DGCastleSiegeRegisterInfoRecv((SDHP_CASTLE_SIEGE_REGISTER_INFO_RECV*)lpMsg);
					break;
				case 0x0D:
					gCastleSiege->DGCastleSiegeRegisterRecv((SDHP_CASTLE_SIEGE_REGISTER_RECV*)lpMsg);
					break;
				case 0x0E:
					gCastleSiege->DGCastleSiegeResetStateRecv((SDHP_CASTLE_SIEGE_RESET_STATE_RECV*)lpMsg);
					break;
				case 0x10:
					gCastleSiege->DGCastleSiegeRegisterMarkRecv((SDHP_CASTLE_SIEGE_REGISTER_MARK_RECV*)lpMsg);
					break;
				case 0x12:
					gCastleSiege->DGCastleSiegeGiveUpGuildRecv((SDHP_CASTLE_SIEGE_GIVEUP_GUILD_RECV*)lpMsg);
					break;
				case 0x17:
					gCastleSiege->DGCastleSiegeSyncStateRecv((SDHP_CASTLE_SIEGE_SYNC_STATE_RECV*)lpMsg);
					break;
				case 0x18:
					gCastleSiege->DGCastleSiegeAddTributeMoneyRecv((SDHP_CASTLE_SIEGE_ADD_TRIBUTE_MONEY_RECV*)lpMsg);
					break;
				case 0x19:
					gCastleSiege->DGCastleSiegeResetTaxInfoRecv((SDHP_CASTLE_SIEGE_RESET_TAX_INFO_RECV*)lpMsg);
					break;
			}
			break;
		case 0x81:
			gCastleSiege->DGCastleSiegeInitDataRecv((SDHP_CASTLE_SIEGE_INIT_DATA_RECV*)lpMsg);
			break;
		case 0x83:
			gCastleSiege->DGCastleSiegeRegisterListRecv((SDHP_CASTLE_SIEGE_REGISTER_LIST_RECV*)lpMsg);
			break;
		case 0x84:
			gCastleSiege->DGCastleSiegeNpcCreateRecv((SDHP_CASTLE_SIEGE_NPC_CREATE_RECV*)lpMsg);
			break;
		case 0x85:
			gCastleSiege->DGCastleSiegeBuildGuildListRecv((SDHP_CASTLE_SIEGE_BUILD_GUILD_LIST_RECV*)lpMsg);
			break;
		case 0x86:
			gCastleSiege->DGCastleSiegeUnionListRecv((SDHP_CASTLE_SIEGE_UNION_LIST_RECV*)lpMsg);
			break;
		case 0x87:
			gCastleSiege->DGCastleSiegeSaveListRecv((SDHP_CASTLE_SIEGE_SAVE_GUILD_LIST_RECV*)lpMsg);
			break;
		case 0x88:
			gCastleSiege->DGCastleSiegeLoadGuildListRecv((SDHP_CASTLE_SIEGE_LOAD_GUILD_LIST_RECV*)lpMsg);
			break;
		case 0x94:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gGoldenArcher->DGGoldenArcherCountRecv((SDHP_GOLDEN_ARCHER_COUNT_RECV*)lpMsg);
					break;
				case 0x01:
					gGoldenArcher->DGGoldenArcherRewardRecv((SDHP_GOLDEN_ARCHER_REWARD_RECV*)lpMsg);
					break;
			}
			break;
		case 0xA0:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gGuild->DGGuildCreateRecv((SDHP_GUILD_CREATE_RECV*)lpMsg);
					break;
				case 0x01:
					gGuild->DGGuildDeleteRecv((SDHP_GUILD_DELETE_RECV*)lpMsg);
					break;
				case 0x02:
					gGuild->DGGuildMemberAddRecv((SDHP_GUILD_MEMBER_ADD_RECV*)lpMsg);
					break;
				case 0x03:
					gGuild->DGGuildMemberDelRecv((SDHP_GUILD_MEMBER_DEL_RECV*)lpMsg);
					break;
				case 0x04:
					gGuild->DGGuildMemberUpdateRecv((SDHP_GUILD_MEMBER_UPDATE_RECV*)lpMsg);
					break;
				case 0x05:
					gGuild->DGGuildAssignStatusRecv((SDHP_GUILD_ASSIGN_STATUS_RECV*)lpMsg);
					break;
				case 0x06:
					gGuild->DGGuildAssignTypeRecv((SDHP_GUILD_ASSIGN_TYPE_RECV*)lpMsg);
					break;
				case 0x07:
					gGuild->DGGuildNoticeRecv((SDHP_GUILD_NOTICE_RECV*)lpMsg);
					break;
				case 0x08:
					gGuild->DGGuildPostRecv((SDHP_GUILD_POST_RECV*)lpMsg);
					break;
				case 0x09:
					gGuild->DGGuildScoreRecv((SDHP_GUILD_SCORE_RECV*)lpMsg);
					break;
				case 0x0A:
					gGuild->DGGuildInfoRecv((SDHP_GUILD_INFO_RECV*)lpMsg);
					break;
				case 0x0B:
					gGuild->DGUnionInfoRecv((SDHP_UNION_INFO_RECV*)lpMsg);
					break;
				case 0x0C:
					gGuild->DGUnionClearRecv((SDHP_UNION_CLEAR_RECV*)lpMsg);
					break;
				case 0x0D:
					gGuild->DGGuildUnionInfoRecv((SDHP_GUILD_UNION_INFO_RECV*)lpMsg);
					break;
				case 0x0E:
					gGuild->DGGuildUnionJoinRecv((SDHP_GUILD_UNION_JOIN_RECV*)lpMsg);
					break;				
				case 0x0F:
					gGuild->DGGuildUnionBreakRecv((SDHP_GUILD_UNION_BREAK_RECV*)lpMsg);
					break;
				case 0x10:
					gGuild->DGGuildAllianceKickRecv((SDHP_GUILD_ALLIANCE_KICK_RECV*)lpMsg);
					break;
				}
			break;
		case 0xB0:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gFriend->DGFriendListRecv((SDHP_FRIEND_LIST_RECV*)lpMsg);
					break;
				case 0x01:
					gFriend->DGFriendResultRecv((SDHP_FRIEND_RESULT_RECV*)lpMsg);
					break;
				case 0x02:
					gFriend->DGFriendStateRecv((SDHP_FRIEND_STATE_RECV*)lpMsg);
					break;
				case 0x03:
					gFriend->DGFriendRequestRecv((SDHP_FRIEND_REQUEST_RECV*)lpMsg);
					break;
				case 0x04:
					gFriend->DGFriendDeleteRecv((SDHP_FRIEND_DELETE_RECV*)lpMsg);
					break;
				case 0x05:
					gFriend->DGFriendMailResultRecv((SDHP_FRIEND_MAIL_RESULT_RECV*)lpMsg);
					break;
				case 0x06:
					gFriend->DGFriendMailListRecv((SDHP_FRIEND_MAIL_LIST_RECV*)lpMsg);
					break;
				case 0x07:
					gFriend->DGFriendMailReadRecv((SDHP_FRIEND_MAIL_READ_RECV*)lpMsg);
					break;
				case 0x08:
					gFriend->DGFriendMailDeleteRecv((SDHP_FRIEND_MAIL_DELETE_RECV*)lpMsg);
					break;
			}
			break;
		case 0xC0:
			switch(((lpMsg[0]==0xC1)?lpMsg[3]:lpMsg[4]))
			{
				case 0x00:
					gReconnect->DGReconnectInfoInsertRecv((SDHP_RECONNECT_INFO_INSERT_RECV*)lpMsg);
					break;
			}
			break;
	}

	PROTECT_FINAL
}

void DGCharacterListRecv(SDHP_CHARACTER_LIST_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCharacterListRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	BYTE send[256];

	PMSG_CHARACTER_LIST_SEND pMsg;

	pMsg.header.setE(0xF3,0x00,0);

	int size = sizeof(pMsg);

	pMsg.MoveCnt = lpMsg->MoveCnt;

	pMsg.count = 0;

	PMSG_CHARACTER_LIST info;

	WORD CharacterCreationLevel = 0;

	for(int n=0;n < lpMsg->count;n++)
	{
		SDHP_CHARACTER_LIST* lpInfo = (SDHP_CHARACTER_LIST*)(((BYTE*)lpMsg)+sizeof(SDHP_CHARACTER_LIST_RECV)+(sizeof(SDHP_CHARACTER_LIST)*n));

		info.slot = lpInfo->slot;

		memcpy(info.Name,lpInfo->name,sizeof(info.Name));

		info.Level = lpInfo->level;

		CharacterCreationLevel = ((info.Level>CharacterCreationLevel)?info.Level:CharacterCreationLevel);

		info.CtlCode = lpInfo->CtlCode;

		#pragma region CHAR_SET_CALC

		memset(info.CharSet,0,sizeof(info.CharSet));

		info.CharSet[0] = (lpInfo->Class%16)*16;
		info.CharSet[0] -= (info.CharSet[0]/32);
		info.CharSet[0] += (lpInfo->Class/16)*32;

		WORD TempInventory[9];

		for(int i=0;i < 9;i++)
		{
			if(i == 0 || i == 1)
			{
				if(lpInfo->Inventory[0+(i*5)] == 0xFF && (lpInfo->Inventory[2+(i*5)] & 0x80) == 0x80 && (lpInfo->Inventory[4+(i*5)] & 0xF0) == 0xF0)
				{
					TempInventory[i] = 0xFFFF;
				}
				else
				{
					TempInventory[i] = (lpInfo->Inventory[0+(i*5)]+((lpInfo->Inventory[2+(i*5)] & 0x80)*2)+((lpInfo->Inventory[4+(i*5)] & 0xF0)*32));
				}
			}
			else
			{
				if(lpInfo->Inventory[0+(i*5)] == 0xFF && (lpInfo->Inventory[2+(i*5)] & 0x80) == 0x80 && (lpInfo->Inventory[4+(i*5)] & 0xF0) == 0xF0)
				{
					TempInventory[i] = (MAX_ITEM_TYPE-1);
				}
				else
				{
					TempInventory[i] = (lpInfo->Inventory[0+(i*5)]+((lpInfo->Inventory[2+(i*5)] & 0x80)*2)+((lpInfo->Inventory[4+(i*5)] & 0xF0)*32))%MAX_ITEM_TYPE;
				}
			}
		}
		
		info.CharSet[1] = TempInventory[INVENTORY_SLOT_WEAPON1]%256;
		info.CharSet[12] |= (TempInventory[INVENTORY_SLOT_WEAPON1]/16) & 0xF0;

		info.CharSet[2] = TempInventory[INVENTORY_SLOT_WEAPON2]%256;
		info.CharSet[13] |= (TempInventory[INVENTORY_SLOT_WEAPON2]/16) & 0xF0;

		info.CharSet[3] |= (TempInventory[INVENTORY_SLOT_HELM] & 0x0F) << 4;
		info.CharSet[9] |= (TempInventory[INVENTORY_SLOT_HELM] & 0x10) << 3;
		info.CharSet[13] |= (TempInventory[INVENTORY_SLOT_HELM] & 0x1E0) >> 5;

		info.CharSet[3] |= (TempInventory[INVENTORY_SLOT_ARMOR] & 0x0F);
		info.CharSet[9] |= (TempInventory[INVENTORY_SLOT_ARMOR] & 0x10) << 2;
		info.CharSet[14] |= (TempInventory[INVENTORY_SLOT_ARMOR] & 0x1E0) >> 1;

		info.CharSet[4] |= (TempInventory[INVENTORY_SLOT_PANTS] & 0x0F) << 4;
		info.CharSet[9] |= (TempInventory[INVENTORY_SLOT_PANTS] & 0x10) << 1;
		info.CharSet[14] |= (TempInventory[INVENTORY_SLOT_PANTS] & 0x1E0) >> 5;

		info.CharSet[4] |= (TempInventory[INVENTORY_SLOT_GLOVES] & 0x0F);
		info.CharSet[9] |= (TempInventory[INVENTORY_SLOT_GLOVES] & 0x10);
		info.CharSet[15] |= (TempInventory[INVENTORY_SLOT_GLOVES] & 0x1E0) >> 1;

		info.CharSet[5] |= (TempInventory[INVENTORY_SLOT_BOOTS] & 0x0F) << 4;
		info.CharSet[9] |= (TempInventory[INVENTORY_SLOT_BOOTS] & 0x10) >> 1;
		info.CharSet[15] |= (TempInventory[INVENTORY_SLOT_BOOTS] & 0x1E0) >> 5;

		int level = 0;

		BYTE table[7] = {1,0,6,5,4,3,2};

		for(int i=0;i < 7;i++)
		{
			if(TempInventory[i] != (MAX_ITEM_TYPE-1) && TempInventory[i] != 0xFFFF)
			{
				level |= ((((lpInfo->Inventory[1+(i*5)]/8) & 0x0F)-1)/2) << (i*3);

				info.CharSet[10] |= ((lpInfo->Inventory[2+(i*5)] & 0x3F)?2:0) << table[i];
				info.CharSet[11] |= ((lpInfo->Inventory[3+(i*5)] & 0x03)?2:0) << table[i];
			}
		}

		info.CharSet[6] = level >> 16;
		info.CharSet[7] = level >> 8;
		info.CharSet[8] = level;

		if(TempInventory[INVENTORY_SLOT_WING] == (MAX_ITEM_TYPE-1))
		{
			info.CharSet[5] |= 12;
		}
		else if(TempInventory[INVENTORY_SLOT_WING] >= 0 && TempInventory[INVENTORY_SLOT_WING] <= 2)
		{
			info.CharSet[5] |= TempInventory[INVENTORY_SLOT_WING] << 2;
		}
		else if(TempInventory[INVENTORY_SLOT_WING] >= 3 && TempInventory[INVENTORY_SLOT_WING] <= 6)
		{
			info.CharSet[5] |= 12;
			info.CharSet[9] |= TempInventory[INVENTORY_SLOT_WING]-2;
		}
		else if(TempInventory[INVENTORY_SLOT_WING] == 30)
		{
			info.CharSet[5] |= 12;
			info.CharSet[9] |= 5;
		}
		else if(TempInventory[INVENTORY_SLOT_WING] >= 36 && TempInventory[INVENTORY_SLOT_WING] <= 40)
		{
			info.CharSet[5] |= 12;
			info.CharSet[16] |= (TempInventory[INVENTORY_SLOT_WING]-35) << 2;
		}
		else if(gCustomWing->CheckCustomWingByItem(GET_ITEM(12,TempInventory[INVENTORY_SLOT_WING])) != 0)
		{
			info.CharSet[5] |= 12;
			info.CharSet[17] |= (gCustomWing->GetCustomWingIndex(GET_ITEM(12,TempInventory[INVENTORY_SLOT_WING]))+1) << 1;
		}

		if(TempInventory[INVENTORY_SLOT_HELPER] == (MAX_ITEM_TYPE-1))
		{
			info.CharSet[5] |= 3;
		}
		else if(TempInventory[INVENTORY_SLOT_HELPER] >= 0 && TempInventory[INVENTORY_SLOT_HELPER] <= 2)
		{
			info.CharSet[5] |= TempInventory[INVENTORY_SLOT_HELPER];
		}
		else if(TempInventory[INVENTORY_SLOT_HELPER] == 3)
		{
			info.CharSet[5] |= 3;
			info.CharSet[10] |= 1;
		}
		else if(TempInventory[INVENTORY_SLOT_HELPER] == 4)
		{
			info.CharSet[5] |= 3;
			info.CharSet[12] |= 1;
		}
		else if(TempInventory[INVENTORY_SLOT_HELPER] == 37)
		{
			info.CharSet[5] |= 3;
			info.CharSet[10] &= 0xFE;
			info.CharSet[12] &= 0xFE;
			info.CharSet[12] |= 4;

			if((lpInfo->Inventory[42] & 1) != 0)
			{
				info.CharSet[16] |= 1;
			}
			else if((lpInfo->Inventory[42] & 2) != 0)
			{
				info.CharSet[16] |= 2;
			}
			else if((lpInfo->Inventory[42] & 4) != 0)
			{
				info.CharSet[17] |= 1;
			}
		}

		#pragma endregion

		info.GuildStatus = lpInfo->GuildStatus;

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.ClassCode = 0;

	if((gServerInfo->m_MGCreateType == 0 && CharacterCreationLevel >= gServerInfo->m_MGCreateLevel[gObj[lpMsg->index].AccountLevel]) || (gServerInfo->m_MGCreateType != 0 && (lpMsg->ExtClass & CARD_CODE_MG) != 0))
	{
		pMsg.ClassCode |= CARD_CODE_MG;
	}

	if((gServerInfo->m_DLCreateType == 0 && CharacterCreationLevel >= gServerInfo->m_DLCreateLevel[gObj[lpMsg->index].AccountLevel]) || (gServerInfo->m_DLCreateType != 0 && (lpMsg->ExtClass & CARD_CODE_DL) != 0))
	{
		pMsg.ClassCode |= CARD_CODE_DL;
	}

	if((gServerInfo->m_SUCreateType == 0 && CharacterCreationLevel >= gServerInfo->m_SUCreateLevel[gObj[lpMsg->index].AccountLevel]) || (gServerInfo->m_SUCreateType != 0 && (lpMsg->ExtClass & CARD_CODE_SU) != 0))
	{
		pMsg.ClassCode |= CARD_CODE_SU;
	}

	if((gServerInfo->m_RFCreateType == 0 && CharacterCreationLevel >= gServerInfo->m_RFCreateLevel[gObj[lpMsg->index].AccountLevel]) || (gServerInfo->m_RFCreateType != 0 && (lpMsg->ExtClass & CARD_CODE_RF) != 0))
	{
		pMsg.ClassCode |= CARD_CODE_RF;
	}

	gObj[lpMsg->index].ClassCode = pMsg.ClassCode;

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpMsg->index,send,size);

	GCCharacterCreationEnableSend(lpMsg->index,0,gObj[lpMsg->index].ClassCode);
}

void DGCharacterCreateRecv(SDHP_CHARACTER_CREATE_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCharacterCreateRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	PMSG_CHARACTER_CREATE_SEND pMsg;

	pMsg.header.set(0xF3,0x01,sizeof(pMsg));

	pMsg.result = lpMsg->result;

	memcpy(pMsg.name,lpMsg->name,sizeof(pMsg.name));

	pMsg.slot = lpMsg->slot;

	pMsg.level = lpMsg->level;

	pMsg.Class = (lpMsg->Class%16)*16;
	pMsg.Class -= (pMsg.Class/32);
	pMsg.Class += (lpMsg->Class/16)*32;

	memcpy(pMsg.equipment,lpMsg->equipment,sizeof(pMsg.equipment));

	DataSend(lpMsg->index,(BYTE*)&pMsg,pMsg.header.size);
}

void DGCharacterDeleteRecv(SDHP_CHARACTER_DELETE_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCharacterDeleteRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	PMSG_CHARACTER_DELETE_SEND pMsg;

	pMsg.header.set(0xF3,0x02,sizeof(pMsg));

	pMsg.result = lpMsg->result;
	
	DataSend(lpMsg->index,(BYTE*)&pMsg,pMsg.header.size);
}

void DGCharacterInfoRecv(SDHP_CHARACTER_INFO_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCharacterInfoRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	if(lpMsg->result == 0)
	{
		CloseClient(lpMsg->index);
		return;
	}

	if((lpMsg->CtlCode & 1) != 0)
	{
		CloseClient(lpMsg->index);
		return;
	}

	if(gObjectManager->CharacterInfoSet((BYTE*)lpMsg,lpMsg->index) == 0)
	{
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(lpObj->MapServerMoveRequest == 0)
	{
		if((lpObj->NextServerCode=gMapServerManager->CheckMapServerMove(lpObj->Index,lpObj->Map,lpObj->LastServerCode)) != gServerInfo->m_ServerCode)
		{
			if(lpObj->NextServerCode == -1)
			{
				CloseClient(lpObj->Index);
				return;
			}
			else
			{
				GJMapServerMoveSend(lpObj->Index,lpObj->NextServerCode,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y);
				return;
			}
		}
	}

	PMSG_CHARACTER_INFO_SEND pMsg;

	pMsg.header.setE(0xF3,0x03,sizeof(pMsg));

	pMsg.X = (BYTE)lpObj->X;
	pMsg.Y = (BYTE)lpObj->Y;
	pMsg.Map = lpObj->Map;
	pMsg.Dir = lpObj->Dir;

	
	
	pMsg.Experience = lpObj->Experience;
	pMsg.NextExperience = lpObj->NextExperience;
	pMsg.LevelUpPoint = lpObj->LevelUpPoint;
	pMsg.Strength = GET_MAX_WORD_VALUE(lpObj->Strength);
	pMsg.Dexterity = GET_MAX_WORD_VALUE(lpObj->Dexterity);
	pMsg.Vitality = GET_MAX_WORD_VALUE(lpObj->Vitality);
	pMsg.Energy = GET_MAX_WORD_VALUE(lpObj->Energy);
	pMsg.Life = GET_MAX_WORD_VALUE(lpObj->Life);
	pMsg.MaxLife = GET_MAX_WORD_VALUE((lpObj->MaxLife+lpObj->AddLife));
	pMsg.Mana = GET_MAX_WORD_VALUE(lpObj->Mana);
	pMsg.MaxMana = GET_MAX_WORD_VALUE((lpObj->MaxMana+lpObj->AddMana));
	pMsg.Shield = GET_MAX_WORD_VALUE(lpObj->Shield);
	pMsg.MaxShield = GET_MAX_WORD_VALUE((lpObj->MaxShield+lpObj->AddShield));
	pMsg.BP = GET_MAX_WORD_VALUE(lpObj->BP);
	pMsg.MaxBP = GET_MAX_WORD_VALUE((lpObj->MaxBP+lpObj->AddBP));
	pMsg.Money = lpObj->Money;
	pMsg.PKLevel = lpObj->PKLevel;
	pMsg.CtlCode = lpMsg->CtlCode;
	pMsg.FruitAddPoint = lpObj->FruitAddPoint;
	pMsg.MaxFruitAddPoint = gFruit->GetMaxFruitPoint(lpObj);
	pMsg.Leadership = GET_MAX_WORD_VALUE(lpObj->Leadership);
	pMsg.FruitSubPoint = lpObj->FruitSubPoint;
	pMsg.MaxFruitSubPoint = gFruit->GetMaxFruitPoint(lpObj);

	#if(GAMESERVER_EXTRA==1)
	pMsg.ViewReset = (DWORD)(lpObj->Reset);
	pMsg.ViewPoint = (DWORD)(lpObj->LevelUpPoint);
	pMsg.ViewCurHP = (DWORD)(lpObj->Life);
	pMsg.ViewMaxHP = (DWORD)(lpObj->MaxLife+lpObj->AddLife);
	pMsg.ViewCurMP = (DWORD)(lpObj->Mana);
	pMsg.ViewMaxMP = (DWORD)(lpObj->MaxMana+lpObj->AddMana);
	pMsg.ViewCurBP = (DWORD)(lpObj->BP);
	pMsg.ViewMaxBP = (DWORD)(lpObj->MaxBP+lpObj->AddBP);
	pMsg.ViewCurSD = (DWORD)(lpObj->Shield);
	pMsg.ViewMaxSD = (DWORD)(lpObj->MaxShield+lpObj->AddShield);
	pMsg.ViewStrength = lpObj->Strength;
	pMsg.ViewDexterity = lpObj->Dexterity;
	pMsg.ViewVitality = lpObj->Vitality;
	pMsg.ViewEnergy = lpObj->Energy;
	pMsg.ViewLeadership = lpObj->Leadership;
	pMsg.Class = lpObj->Class;
	#endif

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GDConnectCharacterSend(lpObj->Index);

	GDOptionDataSend(lpObj->Index);

	gFriend->CGFriendListRecv(lpObj->Index);

	GDPetItemInfoSend(lpObj->Index,0);

	gQuest->GDQuestKillCountSend(lpObj->Index);

	gItemManager->GCItemListSend(lpObj->Index);

	gSkillManager->GCSkillListSend(lpObj,0);

	gQuest->GCQuestInfoSend(lpObj->Index);

	gPeriodicItem->GCPeriodicItemInit(lpObj->Index);

	gPeriodicItem->GDPeriodicItemListSend(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);

	GCNewGensBattleInfoSend(lpObj);

	gReconnect->GetReconnectInfo(lpObj);

	gSkillManager->SkillChangeUse(lpObj->Index);

	gObjViewportListProtocolCreate(lpObj);

	gObjectManager->CharacterUpdateMapEffect(lpObj);

	gDefaultClassFreebies->GetCharacterFreebies(lpObj,lpMsg->IsNewChar);
	
	gScriptLoader->OnCharacterEntry(lpObj->Index);

	lpObj->MapServerMoveRequest = 0;

	gCustomPick->GDCustomPickSend(lpObj->Index);

	gItemStack->GCItemListStackSend(lpObj->Index);

	gItemValue->GCItemListValueSend(lpObj->Index);

	for(int n=0;n < MAX_EFFECT_LIST;n++)
	{
		if(lpObj->Effect[n].IsEffect() != 0)
		{
			gEffectManager->GCPeriodicEffectSend(lpObj,lpObj->Effect[n].m_group,0,0,lpObj->Effect[n].m_count,lpObj->Effect[n].m_index,true);
		}
	}

	gHackMoveSpeedCheck[lpObj->Index].Set(lpObj->Index);

	gCustomDailyReward->GDDailyRewardCheckSend(lpObj->Index);

	gLog->Output(LOG_CONNECT,"[ObjectManager][%d] AddCharacterInfo [%s] [%s][%s]",lpObj->Index,lpObj->Name,lpObj->IpAddr,lpObj->HardwareId);
}

void DGCreateItemRecv(SDHP_CREATE_ITEM_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGCreateItemRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(lpMsg->Map != 0xFF && lpMsg->Map != 0xFE && lpMsg->Map != 0xED && lpMsg->Map != 0xEC && lpMsg->Map != 0xEB && gMapManager->IsValidMap(lpMsg->Map) == 0)
	{
		return;
	}

	if(lpMsg->Map == 0xFE || lpMsg->Map == 0xFF)
	{
		if((lpMsg->Map == 0xFE && lpObj->Interface.type != INTERFACE_TRAINER) || (lpMsg->Map == 0xFF && lpObj->Interface.type != INTERFACE_CHAOS_BOX))
		{
			return;
		}

		CItem item;

		item.m_Level = lpMsg->Level;

		item.m_Serial = lpMsg->Serial;

		if(lpMsg->ItemIndex == GET_ITEM(13,18) || lpMsg->ItemIndex == GET_ITEM(13,37) || lpMsg->ItemIndex == GET_ITEM(13,51) || lpMsg->ItemIndex == GET_ITEM(14,7) || lpMsg->ItemIndex == GET_ITEM(14,19) || lpMsg->ItemIndex == GET_ITEM(14,109))
		{
			item.m_Durability = (float)lpMsg->Dur;
		}
		else
		{
			item.m_Durability = (float)gItemManager->GetItemDurability(lpMsg->ItemIndex,lpMsg->Level,lpMsg->NewOption,lpMsg->SetOption);
		}

		item.Convert(lpMsg->ItemIndex,lpMsg->Option1,lpMsg->Option2,lpMsg->Option3,lpMsg->NewOption,lpMsg->SetOption,lpMsg->JewelOfHarmonyOption,lpMsg->ItemOptionEx,lpMsg->SocketOption,lpMsg->SocketOptionBonus);

		if(lpMsg->ItemIndex == GET_ITEM(13,4) || lpMsg->ItemIndex == GET_ITEM(13,5))
		{
			item.SetPetItemInfo(1,0);
		}

		item.m_IsPeriodicItem = ((lpMsg->Duration>0)?1:0);

		item.m_LoadPeriodicItem = ((lpMsg->Duration>0)?1:0);

		item.m_PeriodicItemTime = ((lpMsg->Duration>0)?lpMsg->Duration:0);

		gChaosBox->ChaosBoxInit(lpObj);

		gItemManager->ChaosBoxAddItem(lpObj->Index,item,0);

		gChaosBox->GCChaosMixSend(lpObj->Index,((lpMsg->Map==0xFF)?1:100),&item);
	}
	else if(lpMsg->Map == 0xEB)
	{
		CItem item;

		item.m_Level = lpMsg->Level;

		item.m_Serial = lpMsg->Serial;

		item.m_Durability = lpMsg->Dur;

		item.Convert(lpMsg->ItemIndex,lpMsg->Option1,lpMsg->Option2,lpMsg->Option3,lpMsg->NewOption,lpMsg->SetOption,lpMsg->JewelOfHarmonyOption,lpMsg->ItemOptionEx,lpMsg->SocketOption,lpMsg->SocketOptionBonus);

		if(lpMsg->ItemIndex == GET_ITEM(13,4) || lpMsg->ItemIndex == GET_ITEM(13,5))
		{
			item.SetPetItemInfo(1,0);
		}

		item.m_IsPeriodicItem = ((lpMsg->Duration>0)?1:0);

		item.m_LoadPeriodicItem = ((lpMsg->Duration>0)?1:0);

		item.m_PeriodicItemTime = ((lpMsg->Duration>0)?lpMsg->Duration:0);

		BYTE slot = gItemManager->InventoryInsertItem(lpObj->Index,item);

		if(slot != 0xFF)
		{
			gItemManager->GCItemModifySend(lpObj->Index,slot);

			if(lpMsg->Duration > 0)
			{
				gPeriodicItem->GCPeriodicItemSend(lpObj->Index,-1,slot,item.m_PeriodicItemTime);
			}
		}
	}
	else
	{
		if(gMap[lpMsg->Map].MonsterItemDrop(lpMsg->ItemIndex,lpMsg->Level,lpMsg->Dur,lpMsg->X,lpMsg->Y,lpMsg->Option1,lpMsg->Option2,lpMsg->Option3,lpMsg->NewOption,lpMsg->SetOption,lpMsg->LootIndex,lpMsg->Serial,lpMsg->JewelOfHarmonyOption,lpMsg->ItemOptionEx,lpMsg->SocketOption,lpMsg->SocketOptionBonus,lpMsg->Duration) != 0)
		{
			if(BC_MAP_RANGE(lpMsg->Map) != 0)
			{
				gBloodCastle->SetEventItemSerial(lpMsg->Map,lpMsg->ItemIndex,lpMsg->Serial);
			}

			if(IT_MAP_RANGE(lpMsg->Map) != 0)
			{
				gIllusionTemple->SetEventItemSerial(lpMsg->Map,lpMsg->ItemIndex,lpMsg->Serial);
			}
		}
	}
}

void DGPetItemInfoRecv(SDHP_PET_ITEM_INFO_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGPetItemInfoRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(lpMsg->type == 0)
	{
		for(int n=0;n < lpMsg->count;n++)
		{
			SDHP_PET_ITEM_INFO1* lpInfo = (SDHP_PET_ITEM_INFO1*)(((BYTE*)lpMsg)+sizeof(SDHP_PET_ITEM_INFO_RECV)+(sizeof(SDHP_PET_ITEM_INFO1)*n));

			if(INVENTORY_RANGE(lpInfo->slot) == 0)
			{
				continue;
			}

			if(lpObj->Inventory[lpInfo->slot].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Inventory[lpInfo->slot].m_Serial == lpInfo->serial)
			{
				lpObj->Inventory[lpInfo->slot].SetPetItemInfo(lpInfo->level,lpInfo->experience);

				if(INVENTORY_WEAR_RANGE(lpInfo->slot) != 0)
				{
					gObjectManager->CharacterCalcAttribute(lpObj->Index);
				}
			}
		}
	}
	
	if(lpMsg->type == 1)
	{
		for(int n=0;n < lpMsg->count;n++)
		{
			SDHP_PET_ITEM_INFO1* lpInfo = (SDHP_PET_ITEM_INFO1*)(((BYTE*)lpMsg)+sizeof(SDHP_PET_ITEM_INFO_RECV)+(sizeof(SDHP_PET_ITEM_INFO1)*n));

			if(WAREHOUSE_RANGE(lpInfo->slot) == 0)
			{
				continue;
			}

			if(lpObj->Warehouse[lpInfo->slot].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Warehouse[lpInfo->slot].m_Serial == lpInfo->serial)
			{
				lpObj->Warehouse[lpInfo->slot].SetPetItemInfo(lpInfo->level,lpInfo->experience);
			}
		}
	}
}

void DGOptionDataRecv(SDHP_OPTION_DATA_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGOptionDataRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	PMSG_OPTION_DATA_SEND pMsg;

	pMsg.header.set(0xF3,0x30,sizeof(pMsg));

	memcpy(pMsg.SkillKey,lpMsg->SkillKey,sizeof(pMsg.SkillKey));

	pMsg.GameOption = lpMsg->GameOption;

	pMsg.QKey = lpMsg->QKey;

	pMsg.WKey = lpMsg->WKey;

	pMsg.EKey = lpMsg->EKey;

	pMsg.ChatWindow = lpMsg->ChatWindow;

	DataSend(lpMsg->index,(BYTE*)&pMsg,pMsg.header.size);
}

void DGCharacterNameCheckRecv(SDHP_CHARACTER_NAME_CHECK_RECV* lpMsg) // OK
{
}

void DGCharacterNameChangeRecv(SDHP_CHARACTER_NAME_CHANGE_RECV* lpMsg) // OK
{
}

void DGCrywolfSyncRecv(SDHP_CRYWOLF_SYNC_RECV* lpMsg) // OK
{
	if(gMapServerManager->GetMapServerGroup() != lpMsg->MapServerGroup)
	{
		return;
	}

	if(gCrywolfSync->GetCrywolfState() == lpMsg->CrywolfState && gCrywolfSync->GetOccupationState() == lpMsg->OccupationState)
	{
		return;
	}

	gCrywolfSync->SetCrywolfState(lpMsg->CrywolfState);

	gCrywolfSync->SetOccupationState(lpMsg->OccupationState);

	LogAdd(LOG_BLACK,"[Crywolf] Sync Occupation: (%d) State: (%d)",lpMsg->OccupationState,lpMsg->CrywolfState);
}

void DGCrywolfInfoRecv(SDHP_CRYWOLF_INFO_RECV* lpMsg) // OK
{
	if(gMapServerManager->GetMapServerGroup() != lpMsg->MapServerGroup)
	{
		return;
	}

	if(lpMsg->OccupationState == 0 || lpMsg->OccupationState == 1 || lpMsg->OccupationState == 2)
	{
		gCrywolf->ApplyCrywolfDBInfo(lpMsg->OccupationState);
	}
	else
	{
		LogAdd(LOG_RED,"[Crywolf] Incorrect DB OccupationState!!!");
	}
}

void DGGlobalPostRecv(SDHP_GLOBAL_POST_RECV* lpMsg) // OK
{
	if(gMapServerManager->GetMapServerGroup() != lpMsg->MapServerGroup)
	{
		return;
	}

	switch(lpMsg->type)
	{
		case 0:
			PostMessage1(lpMsg->name,gMessage->GetMessage(lpMsg->code),lpMsg->message);
			break;
		case 1:
			PostMessage2(lpMsg->name,gMessage->GetMessage(lpMsg->code),lpMsg->message);
			break;
		case 2:
			PostMessage3(lpMsg->name,gMessage->GetMessage(lpMsg->code),lpMsg->message);
			break;
		case 3:
			PostMessage4(lpMsg->name,gMessage->GetMessage(lpMsg->code),lpMsg->message);
			break;
		case 4:
			GCNewMessageSendToAll(2,0,"%s: %s",lpMsg->name,lpMsg->message);
			break;
		case 5:
			GCNewMessageSendToAll(2,1,"%s: %s",lpMsg->name,lpMsg->message);
			break;
		case 6:
			GCNewMessageSendToAll(2,2,"%s: %s",lpMsg->name,lpMsg->message);
			break;
		case 7:
			GCNewMessageSendToAll(2,3,"%s: %s",lpMsg->name,lpMsg->message);
			break;
		case 8:
			GCNewMessageSendToAll(2,4,"%s: %s",lpMsg->name,lpMsg->message);
			break;
		case 9:
			GCNewMessageSendToAll(2,5,"%s: %s",lpMsg->name,lpMsg->message);
			break;
		default:
			GCNewMessageSendToAll(2,6,"%s: %s",lpMsg->name,lpMsg->message);
			break;
	}
}

void DGGlobalNoticeRecv(SDHP_GLOBAL_NOTICE_RECV* lpMsg) // OK
{
	if(gMapServerManager->GetMapServerGroup() != lpMsg->MapServerGroup)
	{
		return;
	}

	gNotice->GCNoticeSendToAll(lpMsg->type,lpMsg->count,lpMsg->opacity,lpMsg->delay,lpMsg->color,lpMsg->speed,"%s",lpMsg->message);
}

void DGSNSDataRecv(SDHP_SNS_DATA_RECV* lpMsg) // OK
{
}

void DGGlobalWhisperRecv(SDHP_GLOBAL_WHISPER_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGGlobalWhisperRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	if(lpMsg->result == 0)
	{
		GCServerMsgSend(lpMsg->index,0);
		return;
	}

	lpMsg->message[(sizeof(lpMsg->message)-1)] = ((strlen(lpMsg->message)>(sizeof(lpMsg->message)-1))?0:lpMsg->message[(sizeof(lpMsg->message)-1)]);

	gLog->Output(LOG_CHAT,"[Whisper][%s][%s] - (Name: %s,Message: %s)",lpMsg->account,lpMsg->name,lpMsg->TargetName,lpMsg->message);
}

void DGGlobalWhisperEchoRecv(SDHP_GLOBAL_WHISPER_ECHO_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGGlobalWhisperEchoRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	lpMsg->message[(sizeof(lpMsg->message)-1)] = ((strlen(lpMsg->message)>(sizeof(lpMsg->message)-1))?0:lpMsg->message[(sizeof(lpMsg->message)-1)]);

	GCChatWhisperSend(lpMsg->index,lpMsg->SourceName,lpMsg->message);
}

void GDServerInfoSend() // OK
{
	SDHP_DATA_SERVER_INFO_SEND pMsg;

	pMsg.header.set(0x00,sizeof(pMsg));

	memcpy(pMsg.ServerName,gServerInfo->m_ServerName,sizeof(pMsg.ServerName));

	pMsg.ServerPort = (WORD)gServerInfo->m_ServerPort;

	pMsg.ServerCode = (WORD)gServerInfo->m_ServerCode;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDCharacterListSend(int aIndex) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_CHARACTER_LIST_SEND pMsg;

	pMsg.header.set(0x01,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDCharacterCreateSend(int aIndex,char* name,BYTE Class) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_CHARACTER_CREATE_SEND pMsg;

	pMsg.header.set(0x02,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	pMsg.Class = Class;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDCharacterDeleteSend(int aIndex,char* name) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_CHARACTER_DELETE_SEND pMsg;

	pMsg.header.set(0x03,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	pMsg.guild = 0;

	if(gObj[aIndex].Guild != 0)
	{
		if(strcmp(gObj[aIndex].Name,gObj[aIndex].Guild->Master) == 0)
		{
			pMsg.guild = 1; // Guild Master
		}
		else
		{
			pMsg.guild = 2; // Guild Member
		}

		memcpy(pMsg.GuildName,gObj[aIndex].Guild->Name,sizeof(pMsg.GuildName));
	}

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDCharacterInfoSend(int aIndex,char* name) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_CHARACTER_INFO_SEND pMsg;

	pMsg.header.set(0x04,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDCreateItemSend(int aIndex,BYTE map,BYTE x,BYTE y,int index,BYTE level,BYTE dur,BYTE Option1,BYTE Option2,BYTE Option3,int LootIndex,BYTE NewOption,BYTE SetOption,BYTE JewelOfHarmonyOption,BYTE ItemOptionEx,BYTE SocketOption[MAX_SOCKET_OPTION],BYTE SocketOptionBonus,DWORD duration) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	ITEM_INFO ItemInfo;

	if(gItemManager->GetInfo(index,&ItemInfo) == 0)
	{
		Console(1, "[CreateItem] ItemIndex does not exist [%02d,%02d]", index / MAX_ITEM_TYPE, index % MAX_ITEM_TYPE);
		return;
	}

	dur = ((dur==0)?gItemManager->GetItemDurability(index,level,NewOption,SetOption):dur);

	Option1 = ((gItemManager->GetItemSkill(index)==0)?0:Option1);

	if(ItemInfo.HaveSerial == 0 && MAP_RANGE(map) != 0)
	{
		BYTE m_Level = level;
		BYTE m_SocketOption[MAX_SOCKET_OPTION];
		BYTE m_SocketOptionBonus = SocketOptionBonus;

		if(SocketOption == 0)
		{
			memset(m_SocketOption,0xFF,MAX_SOCKET_OPTION);
		}
		else
		{
			memcpy(m_SocketOption,SocketOption,MAX_SOCKET_OPTION);
		}

		gMap[map].MonsterItemDrop(index,m_Level,dur,x,y,Option1,Option2,Option3,NewOption,SetOption,LootIndex,0,JewelOfHarmonyOption,ItemOptionEx,m_SocketOption,m_SocketOptionBonus,duration);
		return;
	}

	if(map == 0xEB || map == 0xEC || map == 0xED || map == 0xFE || map == 0xFF || index == GET_ITEM(13,19) || index == GET_ITEM(14,64) || index == GET_ITEM(14,223))
	{
		SDHP_CREATE_ITEM_SEND pMsg;

		pMsg.header.set(0x07,sizeof(pMsg));

		pMsg.index = aIndex;

		memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

		pMsg.X = x;
		pMsg.Y = y;
		pMsg.Map = map;
		pMsg.ItemIndex = index;
		pMsg.Level = level;
		pMsg.Dur = dur;
		pMsg.Option1 = Option1;
		pMsg.Option2 = Option2;
		pMsg.Option3 = Option3;
		pMsg.NewOption = NewOption;
		pMsg.LootIndex = LootIndex;
		pMsg.SetOption = SetOption;
		pMsg.JewelOfHarmonyOption = JewelOfHarmonyOption;
		pMsg.ItemOptionEx = ItemOptionEx;
		pMsg.SocketOptionBonus = SocketOptionBonus;
		pMsg.Duration = duration;

		if(SocketOption == 0)
		{
			memset(pMsg.SocketOption,0xFF,MAX_SOCKET_OPTION);
		}
		else
		{
			memcpy(pMsg.SocketOption,SocketOption,MAX_SOCKET_OPTION);
		}

		gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
	}
	else
	{
		BYTE m_Level = level;
		BYTE m_SocketOption[MAX_SOCKET_OPTION];
		BYTE m_SocketOptionBonus = SocketOptionBonus;

		if(SocketOption == 0)
		{
			memset(m_SocketOption,0xFF,MAX_SOCKET_OPTION);
		}
		else
		{
			memcpy(m_SocketOption,SocketOption,MAX_SOCKET_OPTION);
		}

		gMap[map].MonsterItemDrop(index,m_Level,dur,x,y,Option1,Option2,Option3,NewOption,SetOption,LootIndex,-1,JewelOfHarmonyOption,ItemOptionEx,m_SocketOption,m_SocketOptionBonus,duration);
	}
}

void GDOptionDataSend(int aIndex) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_OPTION_DATA_SEND pMsg;

	pMsg.header.set(0x08,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDPetItemInfoSend(int aIndex,int type) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	SDHP_PET_ITEM_INFO_SEND pMsg;

	pMsg.header.set(0x09,0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.type = type;

	pMsg.count = 0;

	SDHP_PET_ITEM_INFO2 info;

	if(type == 0)
	{
		for(int n=0;n < INVENTORY_SIZE;n++)
		{
			if(lpObj->Inventory[n].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Inventory[n].m_Index == GET_ITEM(13,4) || lpObj->Inventory[n].m_Index == GET_ITEM(13,5)) // Dark Horse,Dark Spirit
			{
				info.slot = n;

				info.serial = lpObj->Inventory[n].m_Serial;

				memcpy(&send[size],&info,sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}
		}
	}
	else if(type == 1)
	{
		for(int n=0;n < WAREHOUSE_SIZE;n++)
		{
			if(lpObj->Warehouse[n].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Warehouse[n].m_Index == GET_ITEM(13,4) || lpObj->Warehouse[n].m_Index == GET_ITEM(13,5)) // Dark Horse,Dark Spirit
			{
				info.slot = n;

				info.serial = lpObj->Warehouse[n].m_Serial;

				memcpy(&send[size],&info,sizeof(info));
				size += sizeof(info);

				pMsg.count++;
			}
		}
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		gDataServerConnection.DataSend(send,size);
	}
}

void GDCharacterNameCheckSend(int aIndex,char* name) // OK
{
}

void GDCharacterNameChangeSend(int aIndex,char* OldName,char* NewName) // OK
{
}

void GDCrywolfSyncSend(int MapServerGroup,int CrywolfState,int OccupationState) // OK
{
	if(gServerInfo->m_ServerType != 1)
	{
		return;
	}

	SDHP_CRYWOLF_SYNC_SEND pMsg;

	pMsg.header.set(0x1E,sizeof(pMsg));

	pMsg.MapServerGroup = MapServerGroup;

	pMsg.CrywolfState = CrywolfState;

	pMsg.OccupationState = OccupationState;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDCrywolfInfoSend(int MapServerGroup) // OK
{
	if(gServerInfo->m_ServerType == 1)
	{
		SDHP_CRYWOLF_INFO_SEND pMsg;

		pMsg.header.set(0x1F,sizeof(pMsg));

		pMsg.MapServerGroup = MapServerGroup;

		gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
	}
}

void GDGlobalPostSend(int MapServerGroup,BYTE type,WORD code,char* name,char* message) // OK
{
	SDHP_GLOBAL_POST_SEND pMsg;

	pMsg.header.set(0x20,sizeof(pMsg));

	pMsg.MapServerGroup = MapServerGroup;

	pMsg.type = type;

	pMsg.code = code;

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,message,sizeof(pMsg.message));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDGlobalNoticeSend(int MapServerGroup,BYTE type,BYTE count,BYTE opacity,WORD delay,DWORD color,BYTE speed,char* message,...) // OK
{
	char buff[256] = {0};

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	SDHP_GLOBAL_NOTICE_SEND pMsg;

	pMsg.header.set(0x21,sizeof(pMsg));

	pMsg.MapServerGroup = MapServerGroup;

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message,buff,sizeof(pMsg.message));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDSNSDataSend(int aIndex) // OK
{
}

void GDCharacterInfoSaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->MapServerMoveQuit != 0)
	{
		return;
	}

	SDHP_CHARACTER_INFO_SAVE_SEND pMsg;

	pMsg.header.set(0x30,sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.Level = lpObj->Level;
	pMsg.Class = lpObj->DBClass;
	pMsg.LevelUpPoint = lpObj->LevelUpPoint;
	pMsg.Experience = lpObj->Experience;
	pMsg.Money = lpObj->Money;
	pMsg.Strength = lpObj->Strength;
	pMsg.Dexterity = lpObj->Dexterity;
	pMsg.Vitality = lpObj->Vitality;
	pMsg.Energy = lpObj->Energy;
	pMsg.Leadership = lpObj->Leadership;
	pMsg.Life = (DWORD)lpObj->Life;
	pMsg.MaxLife = (DWORD)lpObj->MaxLife;
	pMsg.Mana = (DWORD)lpObj->Mana;
	pMsg.MaxMana = (DWORD)lpObj->MaxMana;
	pMsg.BP = (DWORD)lpObj->BP;
	pMsg.MaxBP = (DWORD)lpObj->MaxBP;
	pMsg.Shield = (DWORD)lpObj->Shield;
	pMsg.MaxShield = (DWORD)lpObj->MaxShield;

	for(int n=0;n < INVENTORY_SIZE;n++){gItemManager->DBItemByteConvert(pMsg.Inventory[n],&lpObj->Inventory[n]);}

	for(int n=0;n < MAX_SKILL_LIST;n++){gSkillManager->SkillByteConvert(pMsg.Skill[n],&lpObj->Skill[n]);}

	pMsg.Map = lpObj->Map;
	pMsg.X = (BYTE)lpObj->X;
	pMsg.Y = (BYTE)lpObj->Y;
	pMsg.Dir = lpObj->Dir;
	pMsg.PKCount = lpObj->PKCount;
	pMsg.PKLevel = lpObj->PKLevel;
	pMsg.PKTime = lpObj->PKTime;

	memcpy(pMsg.Quest,lpObj->Quest,sizeof(pMsg.Quest));

	pMsg.ChatLimitTime = lpObj->ChatLimitTime;

	pMsg.FruitAddPoint = lpObj->FruitAddPoint;
	pMsg.FruitSubPoint = lpObj->FruitSubPoint;

	for(int n=0;n < MAX_EFFECT_LIST;n++){gEffectManager->EffectByteConvert(pMsg.Effect[n],&lpObj->Effect[n]);}

	pMsg.Married = lpObj->Married;
	memcpy(pMsg.MarryName,lpObj->MarryName,sizeof(pMsg.MarryName));

	pMsg.BCCount = lpObj->BCCount;
	pMsg.CCCount = lpObj->CCCount;
	pMsg.DSCount = lpObj->DSCount;
	pMsg.DGCount = lpObj->DGCount;
	pMsg.ITCount = lpObj->ITCount;
	pMsg.IGCount = lpObj->IGCount;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));

	GDPetItemInfoSaveSend(aIndex,0);

	gWarehouse->GDWarehouseItemSaveSend(aIndex);

	gQuest->GDQuestKillCountSaveSend(aIndex);
	
	gPeriodicItem->GDPeriodicItemSaveSend(aIndex);

	gCustomPick->GDCustomPickSaveSend(aIndex);
}

void GDInventoryItemSaveSend(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_INVENTORY_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x31,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	for(int n=0;n < INVENTORY_SIZE;n++){gItemManager->DBItemByteConvert(pMsg.Inventory[n],&lpObj->Inventory[n]);}

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDOptionDataSaveSend(int aIndex,BYTE* SkillKey,BYTE GameOption,BYTE QKey,BYTE WKey,BYTE EKey,BYTE ChatWindow,BYTE RKey,DWORD QWERLevel) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_OPTION_DATA_SAVE_SEND pMsg;

	pMsg.header.set(0x33,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	memcpy(pMsg.SkillKey,SkillKey,sizeof(pMsg.SkillKey));

	pMsg.GameOption = GameOption;

	pMsg.QKey = QKey;

	pMsg.WKey = WKey;

	pMsg.EKey = EKey;

	pMsg.ChatWindow = ChatWindow;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDPetItemInfoSaveSend(int aIndex,int type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[4096];

	SDHP_PET_ITEM_INFO_SAVE_SEND pMsg;

	pMsg.header.set(0x34,0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	pMsg.count = 0;

	SDHP_PET_ITEM_INFO_SAVE info;

	if(type == 0)
	{
		for(int n=0;n < INVENTORY_SIZE;n++)
		{
			if(lpObj->Inventory[n].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Inventory[n].m_Index == GET_ITEM(13,4) || lpObj->Inventory[n].m_Index == GET_ITEM(13,5)) // Dark Horse,Dark Spirit
			{
				if(lpObj->Inventory[n].m_LoadPetItemInfo != 0)
				{
					info.serial = lpObj->Inventory[n].m_Serial;

					info.level = lpObj->Inventory[n].m_PetItemLevel;

					info.experience = lpObj->Inventory[n].m_PetItemExp;

					memcpy(&send[size],&info,sizeof(info));
					size += sizeof(info);

					pMsg.count++;
				}
			}
		}
	}
	else if(type == 1)
	{
		for(int n=0;n < WAREHOUSE_SIZE;n++)
		{
			if(lpObj->Warehouse[n].IsItem() == 0)
			{
				continue;
			}

			if(lpObj->Warehouse[n].m_Index == GET_ITEM(13,4) || lpObj->Warehouse[n].m_Index == GET_ITEM(13,5)) // Dark Horse,Dark Spirit
			{
				if(lpObj->Warehouse[n].m_LoadPetItemInfo != 0)
				{
					info.serial = lpObj->Warehouse[n].m_Serial;

					info.level = lpObj->Warehouse[n].m_PetItemLevel;

					info.experience = lpObj->Warehouse[n].m_PetItemExp;

					memcpy(&send[size],&info,sizeof(info));
					size += sizeof(info);

					pMsg.count++;
				}
			}
		}
	}

	if(pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send,&pMsg,sizeof(pMsg));

		gDataServerConnection.DataSend(send,size);
	}
}

void GDResetInfoSaveSend(int aIndex,int ResetDay,int ResetWek,int ResetMon) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RESET_INFO_SAVE_SEND pMsg;

	pMsg.header.set(0x39,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.Reset = lpObj->Reset;

	pMsg.ResetDay = ResetDay;

	pMsg.ResetWek = ResetWek;

	pMsg.ResetMon = ResetMon;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDMasterResetInfoSaveSend(int aIndex,int MasterResetDay,int MasterResetWek,int MasterResetMon) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_MASTER_RESET_INFO_SAVE_SEND pMsg;

	pMsg.header.set(0x3A,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.Reset = lpObj->Reset;

	pMsg.MasterReset = lpObj->MasterReset;

	pMsg.MasterResetDay = MasterResetDay;

	pMsg.MasterResetWek = MasterResetWek;

	pMsg.MasterResetMon = MasterResetMon;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDRankingDuelSaveSend(int aIndex,DWORD WinScore,DWORD LoseScore) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RANKING_DUEL_SAVE_SEND pMsg;

	pMsg.header.set(0x3C,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.WinScore = WinScore;

	pMsg.LoseScore = LoseScore;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDRankingBloodCastleSaveSend(int aIndex,DWORD Score) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RANKING_BLOOD_CASTLE_SAVE_SEND pMsg;

	pMsg.header.set(0x3D,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.score = Score;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDRankingChaosCastleSaveSend(int aIndex,DWORD Score) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RANKING_CHAOS_CASTLE_SAVE_SEND pMsg;

	pMsg.header.set(0x3E,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.score = Score;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDRankingDevilSquareSaveSend(int aIndex,DWORD Score) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RANKING_DEVIL_SQUARE_SAVE_SEND pMsg;

	pMsg.header.set(0x3F,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.score = Score;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDRankingIllusionTempleSaveSend(int aIndex,DWORD Score) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_RANKING_ILLUSION_TEMPLE_SAVE_SEND pMsg;

	pMsg.header.set(0x40,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.score = Score;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void GDCreationCardSaveSend(int aIndex,BYTE ExtClass) // OK
{
	SDHP_CREATION_CARD_SAVE_SEND pMsg;

	pMsg.header.set(0x42,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	pMsg.ExtClass = ExtClass;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDCrywolfInfoSaveSend(int MapServerGroup,int CrywolfState,int OccupationState) // OK
{
	SDHP_CRYWOLF_INFO_SAVE_SEND pMsg;

	pMsg.header.set(0x49,sizeof(pMsg));

	pMsg.MapServerGroup = MapServerGroup;

	pMsg.CrywolfState = CrywolfState;

	pMsg.OccupationState = OccupationState;

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDSNSDataSaveSend(int aIndex,BYTE* data) // OK
{
}

void GDConnectCharacterSend(int aIndex) // OK
{
	SDHP_CONNECT_CHARACTER_SEND pMsg;

	pMsg.header.set(0x70,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDDisconnectCharacterSend(int aIndex) // OK
{
	SDHP_DISCONNECT_CHARACTER_SEND pMsg;

	pMsg.header.set(0x71,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void GDGlobalWhisperSend(int aIndex,char* TargetName,char* message) // OK
{
	SDHP_GLOBAL_WHISPER_SEND pMsg;

	pMsg.header.set(0x72,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	memcpy(pMsg.TargetName,TargetName,sizeof(pMsg.TargetName));

	memcpy(pMsg.message,message,sizeof(pMsg.message));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}