// NpcTalk.cpp: implementation of the CNpcTalk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NpcTalk.h"
#include "BloodCastle.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "CastleSiegeWeapon.h"
#include "ChaosBox.h"
#include "CustomLukeHelper.h"
#include "DevilSquare.h"
#include "DSProtocol.h"
#include "Duel.h"
#include "EffectManager.h"
#include "GameMain.h"
#include "GuildManager.h"
#include "GoldenArcher.h"
#include "IllusionTemple.h"
#include "ItemBagManager.h"
#include "KanturuEntranceNPC.h"
#include "Map.h"
#include "ReadFile.h"
#include "Message.h"
#include "MonsterSchedule.h"
#include "Notice.h"
#include "Quest.h"
#include "ScriptLoader.h"
#include "ServerInfo.h"
#include "ShopManager.h"
#include "Trade.h"
#include "Util.h"
#include "Warehouse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpcTalk::CNpcTalk() // OK
{
	this->m_NpcTalkInfo.clear();
}

CNpcTalk::~CNpcTalk() // OK
{

}

void CNpcTalk::Load(char* path) // OK
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

	this->m_NpcTalkInfo.clear();

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

			NPC_TALK_INFO info;

			memset(&info,0,sizeof(info));

			info.MonsterClass = lpReadFile->GetNumber();

			info.Month = lpReadFile->GetAsNumber();

			info.Day = lpReadFile->GetAsNumber();

			info.DoW = lpReadFile->GetAsNumber();

			strcpy_s(info.Message,lpReadFile->GetAsString());

			this->m_NpcTalkInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CNpcTalk::NpcTalk(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Interface.use != 0)
	{
		return 1;
	}

	if(gQuest->NpcTalk(lpNpc,lpObj) != 0)
	{
		return 1;
	}

	for(std::vector<NPC_TALK_INFO>::iterator it = this->m_NpcTalkInfo.begin(); it != this->m_NpcTalkInfo.end(); it++)
	{
		SYSTEMTIME SystemTime;

		GetLocalTime(&SystemTime);

		if(it->MonsterClass != lpNpc->Class)
		{
			continue;
		}

		if(it->Day != -1 && it->Day != SystemTime.wDay)
		{
			continue;
		}

		if(it->Month != -1 && it->Month != SystemTime.wMonth)
		{
			continue;
		}

		if(it->DoW != -1 && it->DoW != (SystemTime.wDayOfWeek+1))
		{
			continue;
		}

		GCChatTargetSend(lpObj,lpNpc->Index,it->Message);
		
		break;
	}

	switch(lpNpc->Class)
	{
		case 205:
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 206:
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 207: 
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 208:
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 209: 
			this->NpcCrywolfAltar(lpNpc,lpObj);
			break;
		case 216:
			this->NpcCastleSiegeCrown(lpNpc,lpObj);
			break;
		case 217:
			this->NpcCastleSiegeCrownSwitch(lpNpc,lpObj);
			break;
		case 218:
			this->NpcCastleSiegeCrownSwitch(lpNpc,lpObj);
			break;
		case 219:
			this->NpcCastleSiegeGateSwitch(lpNpc,lpObj);
			break;
		case 220:
			this->NpcCastleSiegeGuard(lpNpc,lpObj);
			break;
		case 221:
			this->NpcCastleSiegeWeaponOffense(lpNpc,lpObj);
			break;
		case 222:
			this->NpcCastleSiegeWeaponDefense(lpNpc,lpObj);
			break;
		case 223:
			this->NpcCastleSiegeSenior(lpNpc,lpObj);
			break;
		case 224:
			this->NpcCastleSiegeGuardsman(lpNpc,lpObj);
			break;
		case 226:
			this->NpcTrainer(lpNpc,lpObj);
			break;
		case 232:
			this->NpcAngelKing(lpNpc,lpObj);
			break;
		case 233:
			this->NpcAngelMessenger(lpNpc,lpObj);
			break;
		case 234:
			this->NpcServerTransfer(lpNpc,lpObj);
			break;
		case 236:
			this->NpcGoldenArcher(lpNpc,lpObj);
			break;
		case 237:
			this->NpcCharon(lpNpc,lpObj);
			break;
		case 238:
			this->NpcChaosGoblin(lpNpc,lpObj);
			break;
		case 240:
			this->NpcWarehouse(lpNpc,lpObj);
			break;
		case 241:
			this->NpcGuildMaster(lpNpc,lpObj);
			break;
		case 256:
			this->NpcLehap(lpNpc,lpObj);
			break;
		case 257:
			this->NpcElfBuffer(lpNpc,lpObj);
			break;
		case 367:
			this->NpcKanturuGate(lpNpc,lpObj);
			break;
		case 368:
			this->NpcElpis(lpNpc,lpObj);
			break;
		case 369:
			this->NpcOsbourne(lpNpc,lpObj);
			break;
		case 370:
			this->NpcJerridon(lpNpc,lpObj);
			break;
		case 371:
			this->NpcLeoTheHelper(lpNpc,lpObj);
			break;
		case 375:
			this->NpcChaosCardMaster(lpNpc,lpObj);
			break;
		case 380:
			this->NpcStoneStatue(lpNpc,lpObj);
			break;
		case 383:
			this->NpcYellowStorage(lpNpc,lpObj);
			break;
		case 384:
			this->NpcBlueStorage(lpNpc,lpObj);
			break;
		case 385:
			this->NpcMirageTheMummy(lpNpc,lpObj);
			break;
		case 450:
			this->NpcGovernorBlossom(lpNpc,lpObj);
			break;
		case 452:
			this->NpcSeedMaster(lpNpc,lpObj);
			break;
		case 453:
			this->NpcSeedResearcher(lpNpc,lpObj);
			break;
		case 465:
			this->NpcSantaClaus(lpNpc,lpObj);
			break;
		case 467:
			this->NpcSnowman(lpNpc,lpObj);
			break;
		case 468:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 469:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 470:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 471:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 472:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 473:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 474:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 475:
			this->NpcChristmas(lpNpc,lpObj);
			break;
		case 478:
			this->NpcDelgado(lpNpc,lpObj);
			break;
		case 479:
			this->NpcTitus(lpNpc,lpObj);
			break;
		case 522:
			this->NpcJerryTheAdviser(lpNpc,lpObj);
			break;
		case 540:
			this->NpcSartiganTheAngel(lpNpc,lpObj);
			break;
		case 541:
			this->NpcSilverRewardChest(lpNpc,lpObj);
			break;
		case 542:
			this->NpcGoldenRewardChest(lpNpc,lpObj);
			break;
		case 543:
			this->NpcGensDuprianSteward(lpNpc,lpObj);
			break;
		case 544:
			this->NpcGensVarnertSteward(lpNpc,lpObj);
			break;
		case 546:
			this->NpcRaul(lpNpc,lpObj);
			break;
		case 547:
			this->NpcJulia(lpNpc,lpObj);
			break;
		case 566:
			this->NpcTercia(lpNpc,lpObj);
			break;
		case 567:
			this->NpcVeina(lpNpc,lpObj);
			break;
		case 568:
			this->NpcZyro(lpNpc,lpObj);
			break;
		case 579:
			this->NpcDavid(lpNpc,lpObj);
			break;
		case 580:
			this->NpcSloveCaptain(lpNpc,lpObj);
			break;
		case 581:
			this->NpcDeruvish(lpNpc,lpObj);
			break;
		case 582:
			this->NpcAdniel(lpNpc,lpObj);
			break;
		case 584:
			this->NpcLesnar(lpNpc,lpObj);
			break;
		case 651:
			this->NpcPersonalShopBoard(lpNpc,lpObj);
			break;
		default:
			return 0;
	}

	return 1;
}

void CNpcTalk::NpcCrywolfAltar(LPOBJ lpNpc,LPOBJ lpObj) // OK
{

}

void CNpcTalk::NpcCastleSiegeCrown(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(gCastleSiegeSync->GetState() != CS_STATE_START)
	{
		return;
	}
	
	if(lpObj->CastleJoinSide < 2 || lpObj->CastleInvolved == 0)
	{
		return;
	}

	if(gCastleSiege->CheckUnionGuildMaster(lpObj->Index) == false)
	{
		return;
	}

	if((abs(lpObj->Y-lpNpc->Y)) > 3 || (abs(lpObj->X-lpNpc->X)) > 3)
	{
		return;
	}

	if(gCastleSiege->GetRegCrownAvailable() == false)
	{
		return;
	}

	int CrownIndex = gCastleSiege->GetCrownUserIndex();

	if(gObjIsConnected(CrownIndex) == 0)
	{
		int aIndex = gCastleSiege->GetCrownSwitchUserIndex(217);
		int bIndex = gCastleSiege->GetCrownSwitchUserIndex(218);

		if(gObjIsConnected(aIndex) == 0 || gObjIsConnected(bIndex) == 0)
		{
			gCastleSiege->GCCastleSiegeCrownAccessStateSend(lpObj->Index,4);
			return;
		}

		if(lpObj->CastleJoinSide != gObj[aIndex].CastleJoinSide || lpObj->CastleJoinSide != gObj[bIndex].CastleJoinSide)
		{
			gCastleSiege->GCCastleSiegeCrownAccessStateSend(lpObj->Index,4);
		}
		else
		{
			gCastleSiege->GCCastleSiegeCrownAccessStateSend(lpObj->Index,0);
			gCastleSiege->SetCrownUserIndex(lpObj->Index);
			gCastleSiege->SetCrownAccessPosition((BYTE)lpObj->X,(BYTE)lpObj->Y);
			gCastleSiege->SetCrownAccessTickCount();
			gCastleSiege->GCCastleSiegeNotifyProgressSendToAll(0,lpObj->GuildName);
		}
	}
	else if(lpObj->Index != CrownIndex)
	{
		gCastleSiege->GCCastleSiegeCrownAccessStateSend(lpObj->Index,3);
	}
}

void CNpcTalk::NpcCastleSiegeCrownSwitch(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(gCastleSiegeSync->GetState() != CS_STATE_START)
	{
		return;
	}
	
	if(lpObj->CastleJoinSide < 2)
	{
		return;
	}

	if((abs(lpObj->Y-lpNpc->Y)) > 3 || (abs(lpObj->X-lpNpc->X)) > 3)
	{
		return;
	}

	if(gCastleSiege->CheckGuardianStatueExist() == true)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(121));
		return;
	}

	int CrownUserIndex = gCastleSiege->GetCrownSwitchUserIndex(lpNpc->Class);

	if(gObjIsConnected(CrownUserIndex) == 0)
	{
		gCastleSiege->GCCastleSiegeSwitchStateSend(lpObj->Index,lpNpc->Index,-1,1);
		gCastleSiege->SetCrownSwitchUserIndex(lpNpc->Class,lpObj->Index);
	}
	else if(lpObj->Index != CrownUserIndex)
	{
		gCastleSiege->GCCastleSiegeSwitchStateSend(lpObj->Index,lpNpc->Index,CrownUserIndex,2);
	}
}

void CNpcTalk::NpcCastleSiegeGateSwitch(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	BOOL bControlEnable = FALSE;
	BYTE btResult = 0;
	int iGateIndex = -1;

	if(gCastleSiegeSync->GetState() == CS_STATE_START)
	{
		if(lpObj->CastleJoinSide != 1)
		{
			btResult = 4;
			bControlEnable = FALSE;
		}
		else
		{
			bControlEnable = TRUE;
		}
	}
	else
	{
		if(gCastleSiege->CheckCastleOwnerMember(lpObj->Index) == FALSE && gCastleSiege->CheckCastleOwnerUnionMember(lpObj->Index) == FALSE)
		{
			btResult = 4;
			bControlEnable = FALSE;
		}
		else
		{
			bControlEnable = TRUE;
		}
	}

	if(bControlEnable != FALSE)
	{
		if(gCastleSiege->CheckCsGateAlive(lpNpc->CastleSwitchIndex) == TRUE)
		{
			btResult = 1;
			iGateIndex = lpNpc->CastleSwitchIndex;
		}
		else
		{
			btResult = 2;
		}
	}

	gCastleSiege->GCCastleSiegeGateStateSend(lpObj->Index,btResult,iGateIndex);

	if(btResult != 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = 12;
}

void CNpcTalk::NpcCastleSiegeGuard(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	PMSG_CASTLE_SIEGE_LAND_OF_TRIALS_GUARD_SEND pMsg;

	pMsg.header.set(0xB9,0x03,sizeof(pMsg));

	pMsg.result = 1;
	
	pMsg.MaxPrice = 300000;
	
	pMsg.UnitPrice = 10000;
	
	pMsg.allowed = gCastleSiege->GetHuntZoneEnter();
	
	pMsg.CurrPrice = gCastleSiegeSync->GetTaxHuntZone(lpObj->Index,0);

	if(lpObj->Guild != 0)
	{
		if(gCastleSiege->CheckCastleOwnerMember(lpObj->Index) != 0)
		{
			if(lpObj->GuildRank == GUILD_STATUS_MASTER)
			{
				pMsg.result = 3;
			}
			else
			{
				pMsg.result = 2;
				pMsg.allowed = 0;
			}
		}
		else if(gCastleSiege->CheckCastleOwnerUnionMember(lpObj->Index) != 0)
		{
			pMsg.result = 2;
			pMsg.allowed = 0;
		}
	}

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcCastleSiegeWeaponOffense(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiegeWeapon->NpcCastleSiegeWeaponOffense(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeWeaponDefense(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gCastleSiegeWeapon->NpcCastleSiegeWeaponDefense(lpNpc,lpObj);
}

void CNpcTalk::NpcCastleSiegeSenior(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	if(gCastleSiegeSync->GetState() == CS_STATE_START)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(241));
		return;
	}

	if(gCastleSiege->CheckGuildOwnCastle(lpObj->GuildName) == 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(242));
		return;
	}

	if(lpObj->GuildRank != GUILD_STATUS_OFFICE_MASTER && lpObj->GuildRank != GUILD_STATUS_MASTER)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(243));
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 12;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = false;

	lpObj->CastleInvolved = 0;

	lpObj->CastleSeniorMixOpen = true;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcCastleSiegeGuardsman(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(gCastleSiegeSync->GetState() == CS_STATE_START)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetMessage(244));
		return;
	}

	if(lpObj->Guild == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,"You need a guild");
		return;
	}
	
	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_COMMON;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 13;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcTrainer(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRAINER;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 7;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcAngelKing(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gBloodCastle->NpcAngelKing(lpNpc,lpObj);
}

void CNpcTalk::NpcAngelMessenger(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gBloodCastle->NpcAngelMessenger(lpNpc,lpObj);
}

void CNpcTalk::NpcServerTransfer(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Guild != 0)
	{
		GCServerCommandSend(lpObj->Index,6,0,0,0);
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRANSFER;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 5;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcGoldenArcher(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_GOLDEN_ARCHER;
	lpObj->Interface.state = 0;

	gGoldenArcher->CGGoldenArcherCountRecv(lpObj->Index);
}

void CNpcTalk::NpcCharon(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gDevilSquare->NpcCharon(lpNpc,lpObj);
}

void CNpcTalk::NpcChaosGoblin(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 3;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcWarehouse(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Interface.type == INTERFACE_TRADE || lpObj->Interface.type == INTERFACE_CHAOS_BOX || lpObj->Interface.type == INTERFACE_PERSONAL_SHOP)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_WAREHOUSE;
	lpObj->Interface.state = 0;

	gWarehouse->GDWarehouseItemSend(lpObj->Index,lpObj->Account);
}

void CNpcTalk::NpcGuildMaster(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Guild != 0)
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(245));
		return;
	}

	if(lpObj->Level < gServerInfo->m_GuildCreateMinLevel[lpObj->AccountLevel])
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(246));
		return;
	}

	if(lpObj->Reset < gServerInfo->m_GuildCreateMinReset[lpObj->AccountLevel])
	{
		gNotice->GCNoticeSend(lpObj->Index,1,0,0,0,0,0,gMessage->GetMessage(247));
		return;
	}

	GCGuildMasterQuestionSend(lpObj->Index);
}

void CNpcTalk::NpcLehap(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_COMMON;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 9;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::NpcElfBuffer(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->Level > gServerInfo->m_ElfBufferMaxLevel[lpObj->AccountLevel])
	{
		GCServerCommandSend(lpObj->Index,13,0,0,0);
		return;
	}

	if(lpObj->Reset > gServerInfo->m_ElfBufferMaxReset[lpObj->AccountLevel])
	{
		GCServerCommandSend(lpObj->Index,13,0,0,0);
		return;
	}

	gEffectManager->AddEffect(lpObj,0,EFFECT_ELF_BUFFER,1800,(gServerInfo->m_ElfBufferDamageConstA+(lpObj->Level/gServerInfo->m_ElfBufferDamageConstB)),(gServerInfo->m_ElfBufferDefenseConstA+(lpObj->Level/gServerInfo->m_ElfBufferDefenseConstB)),0,0);
}

void CNpcTalk::NpcKanturuGate(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gKanturuEntranceNPC->CGKanturuEnterInfoRecv(lpObj->Index);
}

void CNpcTalk::NpcElpis(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 17;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcOsbourne(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 18;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcJerridon(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 19;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcLeoTheHelper(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	GCServerCommandSend(lpObj->Index,15,1,0,0);
}

void CNpcTalk::NpcChaosCardMaster(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 21;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcStoneStatue(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gIllusionTemple->NpcStoneStatue(lpNpc,lpObj);
}

void CNpcTalk::NpcYellowStorage(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gIllusionTemple->NpcYellowStorage(lpNpc,lpObj);
}

void CNpcTalk::NpcBlueStorage(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gIllusionTemple->NpcBlueStorage(lpNpc,lpObj);
}

void CNpcTalk::NpcMirageTheMummy(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	gIllusionTemple->NpcMirageTheMummy(lpNpc,lpObj);
}

void CNpcTalk::NpcGovernorBlossom(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	if(lpObj->PShopOpen != 0)
	{
		return;
	}

	if(lpObj->Transaction == 1)
	{
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_CHAOS_BOX;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 22;

	DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);

	GCTaxInfoSend(lpObj->Index,1,gCastleSiegeSync->GetTaxRateChaos(lpObj->Index));

	lpObj->IsChaosMixCompleted = 0;

	gObjInventoryTransaction(lpObj->Index);
}

void CNpcTalk::NpcSeedMaster(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcSeedResearcher(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcSantaClaus(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcSnowman(LPOBJ lpNpc,LPOBJ lpObj) // OK
{

}

void CNpcTalk::NpcChristmas(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcDelgado(LPOBJ lpNpc,LPOBJ lpObj) // OK
{

}

void CNpcTalk::NpcTitus(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcJerryTheAdviser(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcSartiganTheAngel(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcSilverRewardChest(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcGoldenRewardChest(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcGensDuprianSteward(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcGensVarnertSteward(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcRaul(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcJulia(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcTercia(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcVeina(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcZyro(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcDavid(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcSloveCaptain(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcDeruvish(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcAdniel(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcLesnar(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::NpcPersonalShopBoard(LPOBJ lpNpc,LPOBJ lpObj) // OK
{
	
}

void CNpcTalk::CGNpcTalkRecv(PMSG_NPC_TALK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use != 0)
	{
		return;
	}

	if(lpObj->Live == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpNpc = &gObj[bIndex];

	if(lpNpc->Type != OBJECT_NPC || lpObj->Map != lpNpc->Map)
	{
		return;
	}

	if(gObjCalcDistance(lpObj,lpNpc) > 5)
	{
		return;
	}

	if(gMonsterSchedule->CheckEnableNpc(lpObj,lpNpc) != 0)
	{
		return;
	}

	if(gScriptLoader->OnNpcTalk(bIndex,aIndex) != 0)
	{
		return;
	}

	if(gCustomLukeHelper->NpcLukeHelper(lpNpc,lpObj) != 0)
	{
		return;
	}

	if(this->NpcTalk(lpNpc,lpObj) != 0)
	{
		return;
	}

	if(SHOP_RANGE(lpNpc->ShopNumber) == 0)
	{
		return;
	}

	if(gServerInfo->m_PKLimitFree == 0 && lpObj->PKLevel > 4 && gServerInfo->m_PKLimitShop != 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetMessage(184));
		return;
	}

	if(gShopManager->GetItemCountByIndex(lpNpc->ShopNumber) == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetMessage(180));
		return;
	}

	if(gShopManager->CheckShopGameMasterLevel(lpNpc,lpObj) == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetMessage(181));
		return;
	}

	if(gShopManager->CheckShopAccountLevel(lpNpc,lpObj) == 0)
	{
		GCChatTargetSend(lpObj,lpNpc->Index,gMessage->GetMessage(182));
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_SHOP;
	lpObj->Interface.state = 0;

	PMSG_NPC_TALK_SEND pMsg;

	pMsg.header.setE(0x30,sizeof(pMsg));

	pMsg.result = 0;

	DataSend(aIndex,(BYTE*)&pMsg,pMsg.header.size);

	lpObj->TargetShopNumber = lpNpc->ShopNumber;

	gShopManager->GCShopItemListSendByIndex(lpNpc->ShopNumber,aIndex);

	gShopManager->GCShopItemPriceSendByIndex(lpNpc->ShopNumber,aIndex);

	GCTaxInfoSend(aIndex,2,gCastleSiegeSync->GetTaxRateStore(aIndex));
}

void CNpcTalk::CGNpcTalkCloseRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Interface.use == 0 || lpObj->Interface.type == INTERFACE_NONE || (lpObj->Interface.type == INTERFACE_CHAOS_BOX && lpObj->CastleSeniorMixOpen == 0))
	{
		return;
	}

	switch(lpObj->Interface.type)
	{
		case INTERFACE_TRADE:
			gTrade->CGTradeCancelButtonRecv(aIndex);
			break;
		case INTERFACE_SHOP:
			if(((lpObj->Interface.state==1)?(lpObj->Interface.state=0):1)==0){return;}
			lpObj->TargetShopNumber = -1;
			break;
		case INTERFACE_WAREHOUSE:
			gWarehouse->CGWarehouseClose(aIndex);
			break;
		case INTERFACE_TRAINER:
			gChaosBox->ChaosBoxInit(lpObj);
			gObjInventoryCommit(aIndex);
			break;
		case INTERFACE_CHAOS_BOX:
			lpObj->CastleSeniorMixOpen = 0;
			break;
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
}

void CNpcTalk::CGNpcLeoTheHelperRecv(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	this->GDNpcLeoTheHelperSend(aIndex);
}

void CNpcTalk::CGNpcSnowmanRecv(int aIndex) // OK
{
	
}

void CNpcTalk::CGNpcSantaClausRecv(int aIndex) // OK
{
	
}

void CNpcTalk::CGNpcJuliaRecv(int aIndex) // OK
{
	
}

void CNpcTalk::DGNpcLeoTheHelperRecv(SDHP_NPC_LEO_THE_HELPER_RECV* lpMsg) // OK
{
	if(gObjIsAccountValid(lpMsg->index,lpMsg->account) == 0)
	{
		LogAdd(LOG_RED,"[DGNpcLeoTheHelperRecv] Invalid Account [%d](%s)",lpMsg->index,lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if(lpMsg->status != 0)
	{
		GCServerCommandSend(lpObj->Index,15,0,0,0);
		return;
	}

	gItemBagManager->DropItemBySpecialValue(ITEM_BAG_SPECIAL_LEO_HELPER,-1,-1,lpObj,lpObj->Map,lpObj->X,lpObj->Y);

	this->GDNpcLeoTheHelperSaveSend(lpObj->Index,1);
}

void CNpcTalk::DGNpcSantaClausRecv(SDHP_NPC_SANTA_CLAUS_RECV* lpMsg) // OK
{
	
}

void CNpcTalk::GDNpcLeoTheHelperSend(int aIndex) // OK
{
	if(gObjIsAccountValid(aIndex,gObj[aIndex].Account) == 0)
	{
		return;
	}

	SDHP_NPC_LEO_THE_HELPER_SEND pMsg;

	pMsg.header.set(0x0E,0x00,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,gObj[aIndex].Account,sizeof(pMsg.account));

	memcpy(pMsg.name,gObj[aIndex].Name,sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::GDNpcSantaClausSend(int aIndex) // OK
{
	
}

void CNpcTalk::GDNpcLeoTheHelperSaveSend(int aIndex,BYTE status) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	SDHP_NPC_LEO_THE_HELPER_SAVE_SEND pMsg;

	pMsg.header.set(0x0E,0x30,sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account,lpObj->Account,sizeof(pMsg.account));

	memcpy(pMsg.name,lpObj->Name,sizeof(pMsg.name));

	pMsg.status = status;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}

void CNpcTalk::GDNpcSantaClausSaveSend(int aIndex,BYTE status) // OK
{
}