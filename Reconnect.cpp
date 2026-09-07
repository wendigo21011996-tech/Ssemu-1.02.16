// Reconnect.cpp: implementation of the CReconnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reconnect.h"
#include "CustomAttack.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "JSProtocol.h"
#include "Log.h"
#include "Party.h"
#include "ServerInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReconnect::CReconnect() // OK
{

}

CReconnect::~CReconnect() // OK
{

}

void CReconnect::MainProc() // OK
{
	for(std::map<std::string,RECONNECT_INFO>::iterator it=this->m_ReconnectInfo.begin();it != this->m_ReconnectInfo.end();)
	{
		if((--it->second.ReconnectTime) < 0)
		{
			this->GDReconnectInfoRemoveSend(it->second.Name);

			it = this->m_ReconnectInfo.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CReconnect::SetReconnectInfo(LPOBJ lpObj) // OK
{
	if(gServerInfo->m_ReconnectOfflineSwitch != 0)
	{
		if(lpObj->Attack.Offline != 0 || lpObj->Helper.Offline != 0)
		{
			if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
			{
				return;
			}

			GDCharacterInfoSaveSend(lpObj->Index);
		}
	}

	RECONNECT_INFO info;

	memcpy(info.Account,lpObj->Account,sizeof(info.Account));

	memcpy(info.Password,lpObj->Password,sizeof(info.Password));

	memcpy(info.Name,lpObj->Name,sizeof(info.Name));

	memcpy(info.IpAddr,lpObj->IpAddr,sizeof(info.IpAddr));

	memcpy(info.HardwareId,lpObj->HardwareId,sizeof(info.HardwareId));

	info.ReconnectTime = gServerInfo->m_ReconnectTime;

	info.PartyNumber = lpObj->PartyNumber;

	info.AutoAddPointCount = lpObj->AutoAddPointCount;

	memcpy(info.AutoAddPointStats,lpObj->AutoAddPointStats,sizeof(info.AutoAddPointStats));

	info.AutoResetEnable = lpObj->AutoResetEnable;

	memcpy(info.AutoResetStats,lpObj->AutoResetStats,sizeof(info.AutoResetStats));

	info.RequestOption = lpObj->Option;

	memcpy(info.AutoPartyPassword,lpObj->AutoPartyPassword,sizeof(info.AutoPartyPassword));

	info.AttackStarted = lpObj->Attack.Started;

	info.AttackOffline = lpObj->Attack.Offline;

	info.AttackAutoBuff = lpObj->Attack.AutoBuff;

	info.AttackSkillIndex = lpObj->Attack.SkillIndex;

	info.AttackStartMap = lpObj->Attack.StartMap;

	info.AttackStartX = lpObj->Attack.StartX;

	info.AttackStartY = lpObj->Attack.StartY;

	info.PickStarted = lpObj->Pick.Started;

	info.HelperStarted = lpObj->Helper.Started;

	info.HelperOffline = lpObj->Helper.Offline;

	std::map<std::string,RECONNECT_INFO>::iterator it = this->m_ReconnectInfo.find(std::string(lpObj->Name));

	if(it == this->m_ReconnectInfo.end())
	{
		this->m_ReconnectInfo.insert(std::pair<std::string,RECONNECT_INFO>(std::string(lpObj->Name),info));
	}
	else
	{
		it->second = info;
	}

	this->GDReconnectInfoInsertSend(&info);
}

void CReconnect::GetReconnectInfo(LPOBJ lpObj) // OK
{
	std::map<std::string,RECONNECT_INFO>::iterator it = this->m_ReconnectInfo.find(std::string(lpObj->Name));

	if(it != this->m_ReconnectInfo.end())
	{
		this->ResumeParty(lpObj,&it->second);

		this->ResumeCommand(lpObj,&it->second);

		this->ResumeAttack(lpObj,&it->second);

		this->ResumePick(lpObj,&it->second);

		this->ResumeHelper(lpObj, &it->second);

		this->GDReconnectInfoRemoveSend(it->second.Name);

		this->m_ReconnectInfo.erase(it);
	}
}

bool CReconnect::CheckResumeParty(RECONNECT_INFO* lpInfo) // OK
{
	int count = 0;

	if(OBJECT_RANGE(lpInfo->PartyNumber) != 0)
	{
		for(std::map<std::string,RECONNECT_INFO>::iterator it=this->m_ReconnectInfo.begin();it != this->m_ReconnectInfo.end();it++)
		{
			if(it->second.PartyNumber == lpInfo->PartyNumber)
			{
				count++;
			}
		}
	}

	return ((count>1)?1:gParty->IsParty(lpInfo->PartyNumber));
}

void CReconnect::ResumeParty(LPOBJ lpObj,RECONNECT_INFO* lpInfo) // OK
{
	if(this->CheckResumeParty(lpInfo) != 0)
	{
		if(gParty->IsParty(lpInfo->PartyNumber) == 0)
		{
			lpObj->PartyNumber = lpInfo->PartyNumber;
			gParty->m_PartyInfo[lpInfo->PartyNumber].Count = 1;
			gParty->m_PartyInfo[lpInfo->PartyNumber].Index[0] = lpObj->Index;
			gParty->m_PartyInfo[lpInfo->PartyNumber].Index[1] = -1;
			gParty->m_PartyInfo[lpInfo->PartyNumber].Index[2] = -1;
			gParty->m_PartyInfo[lpInfo->PartyNumber].Index[3] = -1;
			gParty->m_PartyInfo[lpInfo->PartyNumber].Index[4] = -1;
			gParty->GCPartyListSend(lpInfo->PartyNumber);
		}
		else
		{
			gParty->AddMember(lpInfo->PartyNumber,lpObj->Index);
		}
	}
}

void CReconnect::ResumeCommand(LPOBJ lpObj,RECONNECT_INFO* lpInfo) // OK
{
	lpObj->AutoAddPointCount = lpInfo->AutoAddPointCount;

	memcpy(lpObj->AutoAddPointStats,lpInfo->AutoAddPointStats,sizeof(lpObj->AutoAddPointStats));

	lpObj->AutoResetEnable = lpInfo->AutoResetEnable;

	memcpy(lpObj->AutoResetStats,lpInfo->AutoResetStats,sizeof(lpObj->AutoResetStats));

	lpObj->Option = lpInfo->RequestOption;

	memcpy(lpObj->AutoPartyPassword,lpInfo->AutoPartyPassword,sizeof(lpObj->AutoPartyPassword));
}

void CReconnect::ResumeAttack(LPOBJ lpObj,RECONNECT_INFO* lpInfo) // OK
{
	if(lpInfo->AttackStarted != 0)
	{
		lpObj->Attack.Started = lpInfo->AttackStarted;

		lpObj->Attack.Offline = (lpObj->Socket==INVALID_SOCKET)?lpInfo->AttackOffline:0;

		lpObj->Attack.AutoBuff = lpInfo->AttackAutoBuff;
	
		lpObj->Attack.SkillIndex = lpInfo->AttackSkillIndex;
	
		lpObj->Attack.StartMap = lpInfo->AttackStartMap;
	
		lpObj->Attack.StartX = lpInfo->AttackStartX;
	
		lpObj->Attack.StartY = lpInfo->AttackStartY;
	
		lpObj->Attack.AttackTime = GetTickCount();

		lpObj->Attack.PotionTime = GetTickCount();

		lpObj->Attack.OnlineTime = ((DWORD)time(0));
	}

	gCustomAttack->CustomAttackSend(lpObj->Index);
}

void CReconnect::ResumePick(LPOBJ lpObj,RECONNECT_INFO* lpInfo) // OK
{
	if(lpInfo->PickStarted != 0)
	{
		lpObj->Pick.Started = lpInfo->PickStarted;

		lpObj->Pick.OnlineTime = ((DWORD)time(0));
	}
}

void CReconnect::ResumeHelper(LPOBJ lpObj,RECONNECT_INFO* lpInfo) // OK
{
	
}

void CReconnect::ResumeOffline(RECONNECT_INFO* lpInfo) // OK
{
	if(gServerInfo->m_ReconnectOfflineSwitch == 0)
	{
		return;
	}

	if(gCustomAttack->m_CustomAttackOfflineSwitch == 0 && gServerInfo->m_HelperOfflineSwitch == 0)
	{
		return;
	}

	if(lpInfo->AttackOffline == 0 && lpInfo->HelperOffline == 0)
	{
		return;
	}

	if(strlen(lpInfo->Account) == 0 || strlen(lpInfo->Password) == 0 || strlen(lpInfo->Name) == 0 || strlen(lpInfo->IpAddr) == 0 || strlen(lpInfo->HardwareId) == 0)
	{
		return;
	}

	int index = gObjAddSearch(INVALID_SOCKET,lpInfo->IpAddr);

	if(OBJECT_RANGE(index) == 0)
	{
		return;
	}

	if(gObjAdd(INVALID_SOCKET,lpInfo->IpAddr,index) == -1)
	{
		return;
	}

	gObj[index].LoginMessageSend++;
	gObj[index].LoginMessageCount++;
	gObj[index].ConnectTickCount = GetTickCount();
	gObj[index].ClientTickCount = GetTickCount();
	gObj[index].ServerTickCount = GetTickCount();
	gObj[index].ClientVerify = 1;

	memcpy(gObj[index].Account,lpInfo->Account,sizeof(gObj[index].Account));

	memcpy(gObj[index].Password, lpInfo->Password, sizeof(gObj[index].Password));

	memcpy(gObj[index].HardwareId,lpInfo->HardwareId,sizeof(gObj[index].HardwareId));

	GJConnectAccountSend(index,lpInfo->Account,lpInfo->Password,lpInfo->IpAddr,lpInfo->HardwareId);

	GDCharacterInfoSend(index,lpInfo->Name);

	gLog->Output(LOG_CONNECT,"[ObjectManager][%d] Offline Reconnect [%s][%s] [%s][%s]",index,lpInfo->Account,lpInfo->Name,lpInfo->IpAddr,lpInfo->HardwareId);
}

void CReconnect::DGReconnectInfoInsertRecv(SDHP_RECONNECT_INFO_INSERT_RECV* lpMsg) // OK
{
	RECONNECT_INFO info = *(RECONNECT_INFO*)lpMsg->data;

	std::map<std::string,RECONNECT_INFO>::iterator it = this->m_ReconnectInfo.find(std::string(lpMsg->name));

	if(it == this->m_ReconnectInfo.end())
	{
		this->m_ReconnectInfo.insert(std::pair<std::string,RECONNECT_INFO>(std::string(lpMsg->name),info));
	}
	else
	{
		it->second = info;
	}

	this->ResumeOffline(&info);
}

void CReconnect::GDReconnectInfoInsertSend(RECONNECT_INFO* lpInfo) // OK
{
	SDHP_RECONNECT_INFO_INSERT_SEND pMsg;

	pMsg.header.set(0xC0,0x00,sizeof(pMsg));

	memcpy(pMsg.name,lpInfo->Name,sizeof(pMsg.name));
	
	pMsg.ServerCode = gServerInfo->m_ServerCode;

	pMsg.ReconnectTime = lpInfo->ReconnectTime;

	memcpy(pMsg.data,lpInfo,sizeof(pMsg.data));

	gDataServerConnection.DataSend((BYTE*)&pMsg,sizeof(pMsg));
}

void CReconnect::GDReconnectInfoRemoveSend(char* Name) // OK
{
	SDHP_RECONNECT_INFO_DELETE_SEND pMsg;

	pMsg.header.set(0xC0,0x01,sizeof(pMsg));

	memcpy(pMsg.name,Name,sizeof(pMsg.name));

	pMsg.ServerCode = gServerInfo->m_ServerCode;

	gDataServerConnection.DataSend((BYTE*)&pMsg,pMsg.header.size);
}