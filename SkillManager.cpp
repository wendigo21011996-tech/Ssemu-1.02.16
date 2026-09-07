// SkillManager.cpp: implementation of the CSkillManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillManager.h"
#include "Math.h"
#include "Attack.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "Console.h"
#include "CustomArena.h"
#include "CustomAttack.h"
#include "Duel.h"
#include "EffectManager.h"
#include "Fruit.h"
#include "Guild.h"
#include "HackSkillSpeedCheck.h"
#include "IllusionTemple.h"
#include "ItemManager.h"
#include "Map.h"
#include "ReadFile.h"
#include "Message.h"
#include "Monster.h"
#include "Move.h"
#include "MoveSummon.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Party.h"
#include "Quest.h"
#include "ServerInfo.h"
#include "SkillHitBox.h"
#include "SkillUseArea.h"
#include "Util.h"
#include "Viewport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillManager::CSkillManager() // OK
{
	this->m_SkillInfo.clear();
}

CSkillManager::~CSkillManager() // OK
{

}

void CSkillManager::Load(char* path) // OK
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

	this->m_SkillInfo.clear();

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

			SKILL_INFO info;

			info.Index = lpReadFile->GetNumber();

			strcpy_s(info.Name,lpReadFile->GetAsString());

			info.Damage = lpReadFile->GetAsNumber();

			info.Mana = lpReadFile->GetAsNumber();

			info.BP = lpReadFile->GetAsNumber();

			info.Range = lpReadFile->GetAsNumber();

			info.Radio = lpReadFile->GetAsNumber();

			info.Delay = lpReadFile->GetAsNumber();

			info.Type = lpReadFile->GetAsNumber();

			info.Effect = lpReadFile->GetAsNumber();

			info.RequireLevel = lpReadFile->GetAsNumber();

			info.RequireEnergy = lpReadFile->GetAsNumber();

			info.RequireLeadership = lpReadFile->GetAsNumber();

			info.RequireKillCount = lpReadFile->GetAsNumber();

			info.RequireGuildStatus = lpReadFile->GetAsNumber();

			for(int n=0;n < MAX_CLASS;n++){info.RequireClass[n] = lpReadFile->GetAsNumber();}

			this->m_SkillInfo.insert(std::pair<int,SKILL_INFO>(info.Index,info));
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CSkillManager::GetInfo(int index,SKILL_INFO* lpInfo) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

int CSkillManager::GetSkillDamage(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Damage;
	}
}

int CSkillManager::GetSkillMana(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Mana;
	}
}

int CSkillManager::GetSkillBP(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.BP;
	}
}

int CSkillManager::GetSkillRange(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return 0;
	}
	else
	{
		return it->second.Range;
	}
}

int CSkillManager::GetSkillDelay(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return 0;
	}
	else
	{
		return it->second.Delay;
	}
}

int CSkillManager::GetSkillType(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Type;
	}
}

int CSkillManager::GetSkillEffect(int index) // OK
{
	std::map<int,SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);

	if(it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Effect;
	}
}

int CSkillManager::GetSkillAngle(int x,int y,int tx,int ty) // OK
{
	float dx = (float)(x-tx);
	float dy = (float)(y-ty);

	float rad = atan2(dy,dx);

	int angle = (int)(((rad*180)/3.141592741012573)+90);

	return ((angle<0)?(angle+360):angle);
}

int CSkillManager::GetSkillFrustrum(int* SkillFrustrumX,int* SkillFrustrumY,BYTE angle,int x,int y,float sx,float sy,float tx,float ty) // OK
{
	vec3_t p[4];

	Vector(-sx,sy,0.0f,p[0]);
	Vector(sx,sy,0.0f,p[1]);
	Vector(tx,ty,0.f,p[2]);
	Vector(-tx,ty,0.f,p[3]);

	vec3_t Angle;

	Vector(0.0f,0.0f,(vec_t)((angle*360)/255),Angle);

	float Matrix[3][4];

	AngleMatrix(Angle,Matrix);

	vec3_t vFrustrum[4];

	for(int n=0;n < 4;n++)
	{
		VectorRotate(p[n],Matrix,vFrustrum[n]);

		SkillFrustrumX[n] = (int)vFrustrum[n][0]+x;

		SkillFrustrumY[n] = (int)vFrustrum[n][1]+y;
	}

	return 1;
}

int CSkillManager::GetSkillNumber(int index,int level) // OK
{
	if(index >= GET_ITEM(15,0) && index <= GET_ITEM(15,15))
	{
		return ((index%MAX_ITEM_TYPE)+1);
	}

	switch(index)
	{
		case GET_ITEM(12,7):
			return SKILL_TWISTING_SLASH;
		case GET_ITEM(12,8):
			return SKILL_HEAL;
		case GET_ITEM(12,9):
			return SKILL_GREATER_DEFENSE;
		case GET_ITEM(12,10):
			return SKILL_GREATER_DAMAGE;
		case GET_ITEM(12,11):
			return (SKILL_SUMMON1+level);
		case GET_ITEM(12,12):
			return SKILL_RAGEFUL_BLOW;
		case GET_ITEM(12,13):
			return SKILL_IMPALE;
		case GET_ITEM(12,14):
			return SKILL_GREATER_LIFE;
		case GET_ITEM(12,16):
			return SKILL_FIRE_SLASH;
		case GET_ITEM(12,17):
			return SKILL_PENETRATION;
		case GET_ITEM(12,18):
			return SKILL_ICE_ARROW;
		case GET_ITEM(12,19):
			return SKILL_DEATH_STAB;
		case GET_ITEM(12,21):
			return SKILL_FIRE_BURST;
		case GET_ITEM(12,22):
			return SKILL_SUMMON_PARTY;
		case GET_ITEM(12,23):
			return SKILL_GREATER_CRITICAL_DAMAGE;
		case GET_ITEM(12,24):
			return SKILL_ELECTRIC_SPARK;
		case GET_ITEM(12,35):
			return SKILL_FIRE_SCREAM;
		case GET_ITEM(12,44):
			return SKILL_FROZEN_STAB;
		case GET_ITEM(12,45):
			return SKILL_FIVE_SHOT;
		case GET_ITEM(12,46):
			return SKILL_SHIELD_RECOVER;
		case GET_ITEM(12,47):
			return SKILL_SWORD_SLASH;
		case GET_ITEM(12,48):
			return SKILL_BIRDS;
		case GET_ITEM(15,16):
			return SKILL_DECAY;
		case GET_ITEM(15,17):
			return SKILL_ICE_STORM;
		case GET_ITEM(15,18):
			return SKILL_NOVA;
		case GET_ITEM(15,19):
			return SKILL_CHAIN_LIGHTNING;
		case GET_ITEM(15,20):
			return SKILL_DRAIN_LIFE;
		case GET_ITEM(15,21):
			return SKILL_RED_STORM;
		case GET_ITEM(15,22):
			return SKILL_DAMAGE_REFLECT;
		case GET_ITEM(15,23):
			return SKILL_SWORD_POWER;
		case GET_ITEM(15,24):
			return SKILL_SLEEP;
		case GET_ITEM(15,26):
			return SKILL_LESSER_DAMAGE;
		case GET_ITEM(15,27):
			return SKILL_LESSER_DEFENSE;
		case GET_ITEM(15,28):
			return SKILL_MAGIC_CIRCLE;
		case GET_ITEM(15,29):
			return SKILL_LIGHTNING_STORM;
		case GET_ITEM(15,30):
			return SKILL_CHAIN_DRIVER;
		case GET_ITEM(15,31):
			return SKILL_DARK_SIDE;
		case GET_ITEM(15,32):
			return SKILL_DRAGON_LORE;
		case GET_ITEM(15,33):
			return SKILL_DRAGON_SLAYER;
		case GET_ITEM(15,34):
			return SKILL_GREATER_IGNORE_DEFENSE_RATE;
		case GET_ITEM(15,35):
			return SKILL_FITNESS;
		case GET_ITEM(15,36):
			return SKILL_GREATER_DEFENSE_SUCCESS_RATE;
	}

	return -1;
}

bool CSkillManager::CheckSkillMana(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(lpObj->Mana < ((SkillInfo.Mana*lpObj->MPConsumptionRate)/100))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CSkillManager::CheckSkillBP(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(lpObj->BP < ((SkillInfo.BP*lpObj->BPConsumptionRate)/100))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

bool CSkillManager::CheckSkillRange(LPOBJ lpObj,int index,int x,int y,int tx,int ty) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(SkillInfo.Range == 0)
	{
		return 0;
	}

	if(lpObj->Class == CLASS_DL && lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,4))
	{
		if(sqrt(pow(((float)x-(float)tx),2)+pow(((float)y-(float)ty),2)) <= (SkillInfo.Range+2))
		{
			return 1;
		}
	}
	else
	{
		if(sqrt(pow(((float)x-(float)tx),2)+pow(((float)y-(float)ty),2)) <= SkillInfo.Range)
		{
			return 1;
		}
	}
	
	return 0;
}

bool CSkillManager::CheckSkillRadio(int index,int x,int y,int tx,int ty) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(SkillInfo.Radio == 0)
	{
		return 0;
	}

	if(sqrt(pow(((float)x-(float)tx),2)+pow(((float)y-(float)ty),2)) <= SkillInfo.Radio)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillFrustrum(int* SkillFrustrumX,int* SkillFrustrumY,int x,int y) // OK
{
	for(int n=0,i=3;n < 4;i=n,n++)
	{
		if((((SkillFrustrumX[n]-x)*(SkillFrustrumY[i]-y))-((SkillFrustrumX[i]-x)*(SkillFrustrumY[n]-y))) < 0)
		{
			return 0;
		}
	}

	return 1;
}

bool CSkillManager::CheckSkillDelay(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(lpObj->RageFighterSkillIndex == index && lpObj->RageFighterSkillCount > 0)
	{
		return 1;
	}

	if((GetTickCount()-lpObj->SkillDelay[index]) < ((DWORD)SkillInfo.Delay))
	{
		return 0;
	}

	lpObj->SkillDelay[index] = GetTickCount();
	return 1;
}

bool CSkillManager::CheckSkillTarget(LPOBJ lpObj,int aIndex,int bIndex,int type) // OK
{
	if(OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}

	if(type == OBJECT_NPC)
	{
		return 0;
	}

	if(gObj[aIndex].Live == 0 || gObj[aIndex].State != OBJECT_PLAYING || gObj[aIndex].Teleport != 0)
	{
		return 0;
	}

	if(lpObj->Type == OBJECT_MONSTER && type == OBJECT_USER)
	{
		return 1;
	}

	if(aIndex == bIndex)
	{
		return 1;
	}

	int SummonIndex = aIndex;

	if(type == OBJECT_MONSTER)
	{
		if((gObj[aIndex].Class >= 100 && gObj[aIndex].Class <= 110) || gObj[aIndex].Class == 523) // Trap
		{
			return 0;
		}

		if(OBJECT_RANGE(gObj[aIndex].SummonIndex) == 0)
		{
			return 1;
		}
		else
		{
			SummonIndex = gObj[aIndex].SummonIndex;
		}
	}

	if(gDuel->CheckDuel(lpObj,&gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	if(gGuild->CheckWar(lpObj,&gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	if(gCustomArena->CheckPlayerTarget(lpObj,&gObj[SummonIndex]) != 0)
	{
		return 1;
	}

	if(gObj[SummonIndex].Map == MAP_CASTLE_SIEGE && gCastleSiegeSync->GetState() == CS_STATE_START)
	{
		if(lpObj->Type == OBJECT_USER && gObj[SummonIndex].Type == OBJECT_USER)
		{
			if(gServerInfo->m_CastleSiegeDamageRate2 != 0 || lpObj->CastleJoinSide == 0 || gObj[SummonIndex].CastleJoinSide == 0 || lpObj->CastleJoinSide != gObj[SummonIndex].CastleJoinSide)
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

bool CSkillManager::CheckSkillRequireLevel(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(lpObj->Level >= SkillInfo.RequireLevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireEnergy(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if((lpObj->Energy+lpObj->AddEnergy) >= SkillInfo.RequireEnergy)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireLeadership(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if((lpObj->Leadership+lpObj->AddLeadership) >= SkillInfo.RequireLeadership)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireKillPoint(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(lpObj->KillCount >= SkillInfo.RequireKillCount)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireGuildStatus(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(SkillInfo.RequireGuildStatus == 0 || (SkillInfo.RequireGuildStatus & 0x20) == lpObj->GuildRank || (SkillInfo.RequireGuildStatus & 0x40) == lpObj->GuildRank || (SkillInfo.RequireGuildStatus & 0x80) == lpObj->GuildRank)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireClass(LPOBJ lpObj,int index) // OK
{
	SKILL_INFO SkillInfo;

	if(this->GetInfo(index,&SkillInfo) == 0)
	{
		return 0;
	}

	if(CHECK_RANGE(lpObj->Class,MAX_CLASS) == 0)
	{
		return 0;
	}

	if(SkillInfo.RequireClass[lpObj->Class] == 0)
	{
		return 0;
	}

	if((lpObj->ChangeUp+1) >= SkillInfo.RequireClass[lpObj->Class])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CSkillManager::CheckSkillRequireWeapon(LPOBJ lpObj,int index) // OK
{
	if(index != SKILL_DEFENSE && index != SKILL_FALLING_SLASH && index != SKILL_LUNGE && index != SKILL_UPPERCUT && index != SKILL_CYCLONE && index != SKILL_SLASH && index != SKILL_TRIPLE_SHOT && index != SKILL_FIRE_BREATH && index != SKILL_POWER_SLASH && index != SKILL_EARTHQUAKE && index != SKILL_PLASMA_STORM && index != SKILL_SAHAMUTT && index != SKILL_NEIL && index != SKILL_GHOST_PHANTOM && index != SKILL_LARGE_RING_BLOWER && index != SKILL_UPPER_BEAST && index != SKILL_PHOENIX_SHOT)
	{
		return 1;
	}

	for(int n=0;n < INVENTORY_WEAR_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() != 0)
		{
			if(lpObj->Inventory[n].m_Option1 != 0)
			{
				if(lpObj->Inventory[n].m_SpecialIndex[SPECIAL_OPTION1] == index)
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

int CSkillManager::AddSkillWeapon(LPOBJ lpObj,int index,int level) // OK
{
	int slot=0,skill=0;

	if((slot=this->AddSkill(lpObj,(skill=index),level)) >= 0)
	{
		this->GCSkillAddSend(lpObj->Index,slot,skill,(BYTE)level,0);return 1;
	}
	else
	{
		return 0;
	}
}

int CSkillManager::DelSkillWeapon(LPOBJ lpObj,int index,int level) // OK
{
	int slot=0,skill=0,count=0;

	for(int n=0;n < INVENTORY_WEAR_SIZE;n++)
	{
		if(lpObj->Inventory[n].IsItem() != 0)
		{
			if(lpObj->Inventory[n].m_Option1 != 0)
			{
				if(lpObj->Inventory[n].m_SpecialIndex[SPECIAL_OPTION1] == index)
				{
					count++;
				}
			}
		}
	}

	if(count == 1 && (slot=this->DelSkill(lpObj,(skill=index))) >= 0)
	{
		this->GCSkillDelSend(lpObj->Index,slot,skill,(BYTE)level,0);return 1;
	}
	else
	{
		return 0;
	}
}

int CSkillManager::AddSkill(LPOBJ lpObj,int index,int level) // OK
{
	if(lpObj->Type == OBJECT_USER && (this->CheckSkillRequireLevel(lpObj,index) == 0 || this->CheckSkillRequireEnergy(lpObj,index) == 0 || this->CheckSkillRequireLeadership(lpObj,index) == 0 || this->CheckSkillRequireClass(lpObj,index) == 0))
	{
		return -1;
	}

	if(this->GetSkill(lpObj,index) != 0)
	{
		return -1;
	}

	for(int n=0;n < MAX_SKILL_LIST;n++)
	{
		if(lpObj->Skill[n].IsSkill() == 0)
		{
			return ((lpObj->Skill[n].Set(index,level)==0)?-1:n);
		}
	}

	return -1;
}

int CSkillManager::DelSkill(LPOBJ lpObj,int index) // OK
{
	for(int n=0;n < MAX_SKILL_LIST;n++)
	{
		if(lpObj->Skill[n].IsSkill() != 0)
		{
			if(lpObj->Skill[n].m_index == index)
			{
				lpObj->Skill[n].Clear();
				return n;
			}
		}
	}

	return -1;
}

CSkill* CSkillManager::GetSkill(LPOBJ lpObj,int index) // OK
{
	index = ((index==SKILL_NOVA_START)?SKILL_NOVA:index);

	for(int n=0;n < MAX_SKILL_LIST;n++)
	{
		if(lpObj->Skill[n].IsSkill() != 0)
		{
			if(lpObj->Skill[n].m_index == index || lpObj->Skill[n].m_skill == index)
			{
				return &lpObj->Skill[n];
			}
		}
	}

	return 0;
}

int CSkillManager::AddMasterSkill(LPOBJ lpObj,int index,int level) // OK
{
	return -1;
}

int CSkillManager::DelMasterSkill(LPOBJ lpObj,int index) // OK
{
	return -1;
}

CSkill* CSkillManager::GetMasterSkill(LPOBJ lpObj,int index) // OK
{
	return 0;
}

bool CSkillManager::ConvertSkillByte(CSkill* lpSkill,BYTE* lpMsg) // OK
{
	WORD index = MAKE_NUMBERW(((lpMsg[0]==0xFF&&lpMsg[2]==0x00)?0xFF:lpMsg[2]),lpMsg[0]);

	if(index == 0xFFFF)
	{
		return 0;
	}

	lpSkill->m_level = lpMsg[1];
	lpSkill->m_skill = index;
	lpSkill->m_index = index;
	return 1;
}

void CSkillManager::SkillByteConvert(BYTE* lpMsg,CSkill* lpSkill) // OK
{
	lpMsg[0] = SET_NUMBERLB(lpSkill->m_index);
	lpMsg[1] = lpSkill->m_level;
	lpMsg[2] = SET_NUMBERHB(lpSkill->m_index);
}

void CSkillManager::UseAttackSkill(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type == OBJECT_USER)
	{
		if(this->CheckSkillRequireWeapon(lpObj,lpSkill->m_skill) == 0)
		{
			return;
		}

		if(lpSkill->m_skill == SKILL_TWISTING_SLASH || lpSkill->m_skill == SKILL_RAGEFUL_BLOW || lpSkill->m_skill == SKILL_DEATH_STAB)
		{
			if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].IsItem() == 0)
			{
				return;
			}
		}
	}

	bool combo = 0;

	if(lpObj->Type == OBJECT_USER && (lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF || gQuest->CheckQuestListState(lpObj,((lpObj->Class==CLASS_DK)?3:2),QUEST_FINISH) != 0))
	{
		if(lpObj->ComboSkill.CheckCombo(lpObj->Class,lpSkill->m_skill) != 0)
		{
			if(gServerInfo->m_CheckAutoComboHack != 0)
			{
				DWORD ComboTime = (GetTickCount()-lpObj->ComboTime);

				if(ComboTime < (DWORD)gServerInfo->m_CheckAutoComboHackTolerance)
				{
					Console(1,"[HackAutoComboCheck][%s] Time [%d][%d]",lpObj->Name,ComboTime,gServerInfo->m_CheckAutoComboHackTolerance);

					if(gServerInfo->m_CheckAutoComboHackAction == 1)
					{
						gObjUserKill(lpObj->Index);
					}
					else if(gServerInfo->m_CheckAutoComboHackAction == 2)
					{
						GCCloseClientSend(lpObj->Index,0);
					}

					return;
				}
			}

			combo = 1;
			lpObj->ComboTime = GetTickCount();
		}
	}

	if(lpObj->SkillNovaState != 0 && lpSkill->m_skill != SKILL_NOVA && this->GetSkill(lpObj,SKILL_NOVA) != 0)
	{
		this->RunningSkill(aIndex,0,this->GetSkill(lpObj,SKILL_NOVA),(BYTE)lpObj->X,(BYTE)lpObj->Y,0,0);
		return;
	}

	if(lpSkill->m_skill == SKILL_NOVA && bIndex != SKILL_NOVA_START)
	{
		this->RunningSkill(aIndex,bIndex,lpSkill,(BYTE)lpObj->X,(BYTE)lpObj->Y,0,combo);
		return;
	}

	if((lpSkill->m_skill == SKILL_UPPER_BEAST || lpSkill->m_skill == SKILL_CHAIN_DRIVER || lpSkill->m_skill == SKILL_DARK_SIDE) && lpObj->RageFighterSkillIndex == lpSkill->m_index && (lpObj->RageFighterSkillCount++) > 0)
	{
		this->RunningSkill(aIndex,bIndex,lpSkill,(BYTE)lpObj->X,(BYTE)lpObj->Y,0,combo);
		return;
	}

	if(lpObj->Type != OBJECT_USER || (this->CheckSkillMana(lpObj,lpSkill->m_index) != 0 && this->CheckSkillBP(lpObj,lpSkill->m_index) != 0))
	{
		if(this->RunningSkill(aIndex,bIndex,lpSkill,(BYTE)lpObj->X,(BYTE)lpObj->Y,0,combo) != 0 && lpObj->Type == OBJECT_USER)
		{
			lpObj->Mana -= ((this->GetSkillMana(lpSkill->m_index)*lpObj->MPConsumptionRate)/100);
			lpObj->BP -= ((this->GetSkillBP(lpSkill->m_index)*lpObj->BPConsumptionRate)/100);
			GCManaSend(aIndex,0xFF,(int)lpObj->Mana,lpObj->BP);
		}
	}
}

void CSkillManager::UseDurationSkillAttack(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,BYTE dir,BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type == OBJECT_USER)
	{
		if(this->CheckSkillRequireWeapon(lpObj,lpSkill->m_skill) == 0)
		{
			return;
		}

		if(lpSkill->m_skill == SKILL_TWISTING_SLASH || lpSkill->m_skill == SKILL_RAGEFUL_BLOW || lpSkill->m_skill == SKILL_DEATH_STAB)
		{
			if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].IsItem() == 0)
			{
				return;
			}
		}
	}

	bool combo = 0;

	if(lpObj->Type == OBJECT_USER && (lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF || gQuest->CheckQuestListState(lpObj,((lpObj->Class==CLASS_DK)?3:2),QUEST_FINISH) != 0))
	{
		if(lpObj->ComboSkill.CheckCombo(lpObj->Class,lpSkill->m_skill) != 0)
		{
			if(gServerInfo->m_CheckAutoComboHack != 0)
			{
				DWORD ComboTime = (GetTickCount()-lpObj->ComboTime);

				if(ComboTime < (DWORD)gServerInfo->m_CheckAutoComboHackTolerance)
				{
					Console(1,"[HackAutoComboCheck][%s] Time [%d][%d]",lpObj->Name,ComboTime,gServerInfo->m_CheckAutoComboHackTolerance);

					if(gServerInfo->m_CheckAutoComboHackAction == 1)
					{
						gObjUserKill(lpObj->Index);
					}
					else if(gServerInfo->m_CheckAutoComboHackAction == 2)
					{
						GCCloseClientSend(lpObj->Index,0);
					}

					return;
				}
			}

			combo = 1;
			lpObj->ComboTime = GetTickCount();
		}
	}

	if(lpObj->SkillNovaState != 0 && lpSkill->m_skill != SKILL_NOVA && this->GetSkill(lpObj,SKILL_NOVA) != 0)
	{
		this->RunningSkill(aIndex,0,this->GetSkill(lpObj,SKILL_NOVA),x,y,0,0);
		return;
	}

	this->GCDurationSkillAttackSend(lpObj,lpSkill->m_index,x,y,dir);

	if(lpObj->Type != OBJECT_USER || (this->CheckSkillMana(lpObj,lpSkill->m_index) != 0 && this->CheckSkillBP(lpObj,lpSkill->m_index) != 0))
	{
		if(this->RunningSkill(aIndex,bIndex,lpSkill,x,y,angle,combo) != 0 && lpObj->Type == OBJECT_USER)
		{
			lpObj->Mana -= ((this->GetSkillMana(lpSkill->m_index)*lpObj->MPConsumptionRate)/100);
			lpObj->BP -= ((this->GetSkillBP(lpSkill->m_index)*lpObj->BPConsumptionRate)/100);
			GCManaSend(aIndex,0xFF,(int)lpObj->Mana,lpObj->BP);
		}
	}
}

bool CSkillManager::RunningSkill(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	lpObj->ShieldDamageReductionTime = 0;

	if(lpObj->Authority == AUTHORITY_USER)
	{
		gEffectManager->DelEffect(lpObj,EFFECT_INVISIBILITY);
	}

	switch(lpSkill->m_skill)
	{
		case SKILL_FLAME:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_TWISTER:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_EVIL_SPIRIT:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_HELL_FIRE:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_AQUA_BEAM:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_BLAST:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_INFERNO:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_MANA_SHIELD:
			return this->SkillManaShield(aIndex,bIndex,lpSkill);
		case SKILL_DEFENSE:
			return this->SkillDefense(aIndex,bIndex,lpSkill);
		case SKILL_TRIPLE_SHOT:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_HEAL:
			return this->SkillHeal(aIndex,bIndex,lpSkill);
		case SKILL_GREATER_DEFENSE:
			return this->SkillGreaterDefense(aIndex,bIndex,lpSkill);
		case SKILL_GREATER_DAMAGE:
			return this->SkillGreaterDamage(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON1:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON2:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON3:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON4:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON5:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON6:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON7:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_DECAY:
			return this->SkillDecay(aIndex,bIndex,lpSkill,x,y,combo);
		case SKILL_ICE_STORM:
			return this->SkillIceStorm(aIndex,bIndex,lpSkill,x,y,combo);
		case SKILL_NOVA:
			return this->SkillNova(aIndex,bIndex,lpSkill,combo);
		case SKILL_TWISTING_SLASH:
			return this->SkillTwistingSlash(aIndex,bIndex,lpSkill,combo);
		case SKILL_RAGEFUL_BLOW:
			return this->SkillRagefulBlow(aIndex,bIndex,lpSkill,combo);
		case SKILL_DEATH_STAB:
			return this->SkillDeathStab(aIndex,bIndex,lpSkill,angle,combo);
		case SKILL_CRESCENT_MOON_SLASH:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_MANA_GLAIVE:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_STAR_FALL:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_IMPALE:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_GREATER_LIFE:
			return this->SkillGreaterLife(aIndex,bIndex,lpSkill);
		case SKILL_MONSTER_AREA_ATTACK:
			return this->SkillMonsterAreaAttack(aIndex,bIndex,lpSkill);
		case SKILL_PENETRATION:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_FIRE_SLASH:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_POWER_SLASH:
			return this->SkillPowerSlash(aIndex,bIndex,lpSkill,angle,combo);
		case SKILL_SPIRAL_SLASH:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_FORCE:
			return this->SkillForce(aIndex,bIndex,lpSkill,combo);
		case SKILL_FIRE_BURST:
			return this->SkillFireBurst(aIndex,bIndex,lpSkill,combo);
		case SKILL_EARTHQUAKE:
			return this->SkillEarthquake(aIndex,bIndex,lpSkill,combo);
		case SKILL_SUMMON_PARTY:
			return this->SkillSummonParty(aIndex,bIndex,lpSkill);
		case SKILL_GREATER_CRITICAL_DAMAGE:
			return this->SkillGreaterCriticalDamage(aIndex,bIndex,lpSkill);
		case SKILL_ELECTRIC_SPARK:
			return this->SkillElectricSpark(aIndex,bIndex,lpSkill,angle,combo);
		case SKILL_STERN:
			return this->SkillStern(aIndex,bIndex,lpSkill,angle);
		case SKILL_REMOVE_STERN:
			return this->SkillRemoveStern(aIndex,bIndex,lpSkill);
		case SKILL_GREATER_MANA:
			return this->SkillGreaterMana(aIndex,bIndex,lpSkill);
		case SKILL_INVISIBILITY:
			return this->SkillInvisibility(aIndex,bIndex,lpSkill);
		case SKILL_REMOVE_INVISIBILITY:
			return this->SkillRemoveInvisibility(aIndex,bIndex,lpSkill);
		case SKILL_REMOVE_ALL_EFFECT:
			return this->SkillRemoveAllEffect(aIndex,bIndex,lpSkill);
		case SKILL_MANA_RAYS:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_FIRE_BLAST:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_BRAND:
			return this->SkillBrand(aIndex,bIndex,lpSkill);
		case SKILL_PLASMA_STORM:
			return this->SkillPlasmaStorm(aIndex,bIndex,lpSkill);
		case SKILL_INFINITY_ARROW:
			return this->SkillInfinityArrow(aIndex,bIndex,lpSkill);
		case SKILL_FIRE_SCREAM:
			return this->MultiSkillAttack(aIndex,bIndex,lpSkill,combo);
		case SKILL_MONSTER_SUMMON:
			return this->SkillMonsterSummon(aIndex,bIndex,lpSkill);
		case SKILL_MAGIC_DAMAGE_IMMUNITY:
			return this->SkillMagicDamageImmunity(aIndex,bIndex,lpSkill);
		case SKILL_PHYSI_DAMAGE_IMMUNITY:
			return this->SkillPhysiDamageImmunity(aIndex,bIndex,lpSkill);
		case SKILL_CHAIN_LIGHTNING:
			return this->SkillChainLightning(aIndex,bIndex,lpSkill,combo);
		case SKILL_DAMAGE_REFLECT:
			return this->SkillDamageReflect(aIndex,bIndex,lpSkill,combo);
		case SKILL_SWORD_POWER:
			return this->SkillSwordPower(aIndex,bIndex,lpSkill,combo);
		case SKILL_SLEEP:
			return this->SkillSleep(aIndex,bIndex,lpSkill);
		case SKILL_LESSER_DEFENSE:
			return this->SkillLesserDefense(aIndex,bIndex,lpSkill);
		case SKILL_LESSER_DAMAGE:
			return this->SkillLesserDamage(aIndex,bIndex,lpSkill);
		case SKILL_SAHAMUTT:
			return this->SkillSahamutt(aIndex,bIndex,lpSkill,x,y,combo);
		case SKILL_NEIL:
			return this->SkillNeil(aIndex,bIndex,lpSkill,x,y);
		case SKILL_GHOST_PHANTOM:
			return this->SkillGhostPhantom(aIndex,bIndex,lpSkill,x,y);
		case SKILL_RED_STORM:
			return this->SkillRedStorm(aIndex,bIndex,lpSkill,combo);
		case SKILL_FROZEN_STAB:
			return this->SkillFrozenStab(aIndex,bIndex,lpSkill,x,y,combo);
		case SKILL_MAGIC_CIRCLE:
			return this->SkillMagicCircle(aIndex,bIndex,lpSkill);
		case SKILL_SHIELD_RECOVER:
			return this->SkillShieldRecover(aIndex,bIndex,lpSkill);
		case SKILL_FIVE_SHOT:
			return this->SkillFiveShot(aIndex,bIndex,lpSkill,angle,combo);
		case SKILL_SWORD_SLASH:
			return this->SkillSwordSlash(aIndex,bIndex,lpSkill,angle,combo);
		case SKILL_LIGHTNING_STORM:
			return this->SkillLightningStorm(aIndex,bIndex,lpSkill,combo);
		case SKILL_BIRDS:
			return this->SkillBirds(aIndex,bIndex,lpSkill,angle,combo);
		case SKILL_SELF_EXPLOSION:
			return this->SkillSelfExplosion(aIndex,bIndex,lpSkill);
		case SKILL_LARGE_RING_BLOWER:
			return this->SkillLargeRingBlower(aIndex,bIndex,lpSkill,combo);
		case SKILL_UPPER_BEAST:
			return this->SkillUpperBeast(aIndex,bIndex,lpSkill,combo);
		case SKILL_CHAIN_DRIVER:
			return this->SkillChainDriver(aIndex,bIndex,lpSkill,combo);
		case SKILL_DARK_SIDE:
			return this->SkillDarkSide(aIndex,bIndex,lpSkill,combo);
		case SKILL_DRAGON_LORE:
			return this->SkillDragonLore(aIndex,bIndex,lpSkill,combo);
		case SKILL_GREATER_IGNORE_DEFENSE_RATE:
			return this->SkillGreaterIgnoreDefenseRate(aIndex,bIndex,lpSkill);
		case SKILL_FITNESS:
			return this->SkillFitness(aIndex,bIndex,lpSkill);
		case SKILL_GREATER_DEFENSE_SUCCESS_RATE:
			return this->SkillGreaterDefenseSuccessRate(aIndex,bIndex,lpSkill);
		case SKILL_CHARGE:
			return this->SkillCastleSiege(aIndex,bIndex,lpSkill,combo);
		case SKILL_PHOENIX_SHOT:
			return this->SkillPhoenixShot(aIndex,bIndex,lpSkill,combo);
		case SKILL_BLOOD_STORM:
			return this->SkillBloodStorm(aIndex,bIndex,lpSkill,x,y,combo);
		case SKILL_CURE:
			return this->SkillCure(aIndex,bIndex,lpSkill);
		case SKILL_PARTY_HEAL:
			return this->SkillPartyHeal(aIndex,bIndex,lpSkill);
		case SKILL_POISON_ARROW:
			return this->SkillPoisonArrow(aIndex,bIndex,lpSkill,combo);
		case SKILL_BLESS:
			return this->SkillBless(aIndex,bIndex,lpSkill);
		case SKILL_SUMMON_SATYROS:
			return this->SkillSummon(aIndex,bIndex,lpSkill);
		case SKILL_BLIND:
			return this->SkillBlind(aIndex,bIndex,lpSkill);
		case SKILL_EARTH_PRISON:
			return this->SkillEarthPrison(aIndex,bIndex,lpSkill,combo);
		case SKILL_IRON_DEFENSE:
			return this->SkillIronDefense(aIndex,bIndex,lpSkill);
		case SKILL_BLOOD_HOWLING:
			return this->SkillBloodHowling(aIndex,bIndex,lpSkill);
		default:
			return this->BasicSkillAttack(aIndex,bIndex,lpSkill,combo);
	}

	return 0;
}

bool CSkillManager::BasicSkillAttack(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}

	if(lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,gObj[bIndex].X,gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gAttack->Attack(lpObj,&gObj[bIndex],lpSkill,1,0,0,0,combo);

	if(combo != 0)
	{
		this->GCSkillAttackSend(lpObj,SKILL_COMBO,bIndex,1);
	}

	return 1;
}

bool CSkillManager::MultiSkillAttack(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type == OBJECT_USER)
	{
		lpObj->MultiSkillIndex = lpSkill->m_index;

		lpObj->MultiSkillCount = 0;

		switch(lpSkill->m_skill)
		{
			case SKILL_FLAME:
				this->BasicSkillAttack(aIndex,bIndex,lpSkill,combo);
				break;
			case SKILL_BLAST:
				this->BasicSkillAttack(aIndex,bIndex,lpSkill,combo);
				break;
			case SKILL_IMPALE:
				this->BasicSkillAttack(aIndex,bIndex,lpSkill,combo);
				break;
			case SKILL_FIRE_SCREAM:
				this->BasicSkillAttack(aIndex,bIndex,lpSkill,combo);
				break;
		}
	}
	else
	{
		switch(lpSkill->m_skill)
		{
			case SKILL_EVIL_SPIRIT:
				this->SkillEvilSpirit(aIndex,bIndex,lpSkill,combo);
				break;
		}
	}

	return 1;
}

bool CSkillManager::SkillChangeUse(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int change = -1;

	for(int n=0;n < 2;n++)
	{
		CItem* Ring = &lpObj->Inventory[10+n];

		if(Ring->IsItem() == 0 || Ring->m_Durability == 0 || change != -1)
		{
			continue;
		}

		if(Ring->m_Index == GET_ITEM(13,10)) // Transformation Ring
		{
			change = Ring->m_Level;
		}
		else if(Ring->m_Index == GET_ITEM(13,39)) // Ring 1
		{
			change = 372;
		}
		else if(Ring->m_Index == GET_ITEM(13,40)) // Halloween Ring
		{
			change = 373;
		}
		else if(Ring->m_Index == GET_ITEM(13,41)) // Christmas Ring
		{
			change = 374;
		}
		else if(Ring->m_Index == GET_ITEM(13,42) && lpObj->Authority >= AUTHORITY_GAME_MASTER) // GM Ring
		{
			change = 378;
		}
		else if(Ring->m_Index == GET_ITEM(13,68)) // Snowman Ring
		{
			change = 477;
		}
		else if(Ring->m_Index == GET_ITEM(13,76)) // Panda Ring
		{
			change = 503;
		}
		else if(Ring->m_Index == GET_ITEM(13,77)) // Brown Panda Ring
		{
			change = 616;
		}
		else if(Ring->m_Index == GET_ITEM(13,78)) // Pink Panda Ring
		{
			change = 617;
		}
		else if(Ring->m_Index == GET_ITEM(13,122)) // Skeleton Ring
		{
			change = 548;
		}
		else if(Ring->m_Index == GET_ITEM(13,163)) // Robot Knight Ring
		{
			change = 625;
		}
		else if(Ring->m_Index == GET_ITEM(13,164)) // Mini Robot Ring
		{
			change = 626;
		}
		else if(Ring->m_Index == GET_ITEM(13,165)) // Great Heavenly Mage Ring
		{
			change = 642;
		}
	}

	switch(change)
	{
		case 0:
			change = gServerInfo->m_TransformationRing1;
			break;
		case 1:
			change = gServerInfo->m_TransformationRing2;
			break;
		case 2:
			change = gServerInfo->m_TransformationRing3;
			break;
		case 3:
			change = gServerInfo->m_TransformationRing4;
			break;
		case 4:
			change = gServerInfo->m_TransformationRing5;
			break;
		case 5:
			change = gServerInfo->m_TransformationRing6;
			break;
	}

	#if(GAMESERVER_UPDATE>=301)

	if(IT_MAP_RANGE(lpObj->Map) != 0 && (gIllusionTemple->GetState(GET_IT_LEVEL(lpObj->Map)) == IT_STATE_STAND || gIllusionTemple->GetState(GET_IT_LEVEL(lpObj->Map)) == IT_STATE_START || gIllusionTemple->GetState(GET_IT_LEVEL(lpObj->Map)) == IT_STATE_CLEAN))
	{
		switch(gIllusionTemple->GetUserJoinTeam(lpObj))
		{
			case IT_TEAM_Y:
				change = 404;
				break;
			case IT_TEAM_B:
				change = 405;
				break;
		}
	}

	#endif

	if(lpObj->Change == change)
	{
		return 0;
	}
	else
	{
		lpObj->Change = change;
		return 1;
	}
}

bool CSkillManager::SkillEvilSpirit(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillManaShield(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	int value1 = gServerInfo->m_ManaShieldConstA+((lpObj->Dexterity+lpObj->AddDexterity)/gServerInfo->m_ManaShieldConstB)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_ManaShieldConstC);

	value1 = ((value1>gServerInfo->m_ManaShieldMaxRate)?gServerInfo->m_ManaShieldMaxRate:value1);

	value1 = (value1*((lpTarget->Type==OBJECT_USER)?gServerInfo->m_ManaShieldRate[lpTarget->Class]:100))/100;

	int value2 = 0;

	int count = gServerInfo->m_ManaShieldTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_ManaShieldTimeConstB);

	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);
	return 1;
}

bool CSkillManager::SkillDefense(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].IsItem() != 0)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index >= GET_ITEM(6,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index < GET_ITEM(7,0))
		{
			if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Option1 != 0)
			{
				lpObj->ShieldDamageReductionTime = GetTickCount();
				GCActionSend(lpObj,SKILL_DEFENSE,lpObj->Index,bIndex);
				return 1;
			}
		}
	}

	return 0;
}

bool CSkillManager::SkillHeal(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = gServerInfo->m_HealConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_HealConstB);

	if((lpTarget->Life+value) > (lpTarget->MaxLife+lpTarget->AddLife))
	{
		lpTarget->Life = lpTarget->MaxLife+lpTarget->AddLife;
	}
	else
	{
		lpTarget->Life += value;
	}

	if(lpTarget->Type == OBJECT_USER)
	{
		GCLifeSend(bIndex,0xFF,(int)lpTarget->Life,lpTarget->Shield);
	}

	if(lpTarget->Type == OBJECT_MONSTER && OBJECT_RANGE(lpTarget->SummonIndex) != 0)
	{
		GCSummonLifeSend(lpTarget->SummonIndex,(int)lpTarget->Life,(int)lpTarget->MaxLife);
	}

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index,(value/40));
		}
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);
	return 1;
}

bool CSkillManager::SkillGreaterDefense(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = gServerInfo->m_GreaterDefenseConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_GreaterDefenseConstB);

	value = (value*((lpTarget->Type==OBJECT_USER)?gServerInfo->m_GreaterDefenseRate[lpTarget->Class]:100))/100;

	int count = gServerInfo->m_GreaterDefenseTimeConstA;

	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),count,(value*2),0,0,0);

	#if(GAMESERVER_UPDATE>=201)
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index,(value/10));
		}
	}
	#endif

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);
	return 1;
}

bool CSkillManager::SkillGreaterDamage(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = gServerInfo->m_GreaterDamageConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_GreaterDamageConstB);

	value = (value*((lpTarget->Type==OBJECT_USER)?gServerInfo->m_GreaterDamageRate[lpTarget->Class]:100))/100;

	int count = gServerInfo->m_GreaterDamageTimeConstA;

	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),count,value,0,0,0);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index,(value/10));
		}
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);
	return 1;
}

bool CSkillManager::SkillSummon(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map == MAP_ICARUS || CC_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}

	int monster = 0;

	switch(lpSkill->m_index)
	{
		case SKILL_SUMMON1:
			monster = gServerInfo->m_SummonMonster1;
			break;
		case SKILL_SUMMON2:
			monster = gServerInfo->m_SummonMonster2;
			break;
		case SKILL_SUMMON3:
			monster = gServerInfo->m_SummonMonster3;
			break;
		case SKILL_SUMMON4:
			monster = gServerInfo->m_SummonMonster4;
			break;
		case SKILL_SUMMON5:
			monster = gServerInfo->m_SummonMonster5;
			break;
		case SKILL_SUMMON6:
			monster = gServerInfo->m_SummonMonster6;
			break;
		case SKILL_SUMMON7:
			monster = gServerInfo->m_SummonMonster7;
			break;
		case SKILL_SUMMON_SATYROS:
			monster = gServerInfo->m_SummonMonster8;
			break;
		default:
			return 0;
	}

	if(OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonKill(aIndex);
		GCSummonLifeSend(aIndex,0,1);
		return 0;
	}

	int index = gObjAddSummon();

	if(OBJECT_RANGE(index) == 0)
	{
		return 0;
	}

	lpObj->SummonIndex = index;

	LPOBJ lpSummon = &gObj[index];

	lpSummon->X = lpObj->X-1;
	lpSummon->Y = lpObj->Y+1;
	lpSummon->MTX = lpSummon->X;
	lpSummon->MTY = lpSummon->Y;
	lpSummon->Dir = 2;
	lpSummon->Map = lpObj->Map;

	gObjSetMonster(index,monster);

	lpSummon->SummonIndex = aIndex;
	lpSummon->Attribute = ATTRIBUTE_SUMMONED;
	lpSummon->TargetNumber = -1;
	lpSummon->ActionState.Reset();
	lpSummon->PathCount = 0;
	lpSummon->MoveRange = 15;

	GCSummonLifeSend(lpSummon->SummonIndex,(int)lpSummon->Life,(int)lpSummon->MaxLife);
	return 1;
}

bool CSkillManager::SkillDecay(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,x,y) == 0)
	{
		return 0;
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,x,y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillIceStorm(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,x,y) == 0)
	{
		return 0;
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;
		
		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,x,y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillNova(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(bIndex != SKILL_NOVA_START)
	{
		if(lpObj->SkillNovaState == 0)
		{
			return 0;
		}
	}
	else
	{
		if(lpObj->SkillNovaState != 0)
		{
			return 0;
		}

		lpObj->SkillNovaState = 1;
		lpObj->SkillNovaCount = 0;
		lpObj->SkillNovaTime = GetTickCount();
		this->GCSkillAttackSend(lpObj,bIndex,aIndex,1);
		return 1;
	}

	lpObj->SkillNovaState = 0;

	lpObj->SkillNovaTime = 0;

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,600,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);

	return 1;
}

bool CSkillManager::SkillTwistingSlash(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillRagefulBlow(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,500,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillDeathStab(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	gAttack->Attack(lpObj,lpTarget,lpSkill,0,1,0,0,combo);

	if(combo != 0)
	{
		this->GCSkillAttackSend(lpObj,SKILL_COMBO,bIndex,1);
	}

	int SkillFrustrumX[4],SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX,SkillFrustrumY,angle,lpObj->X,lpObj->Y,6.0f,6.0f,1.0f,0.0f);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(this->CheckSkillFrustrum(SkillFrustrumX,SkillFrustrumY,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,1,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillGreaterLife(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int value1 = gServerInfo->m_GreaterLifeConstA+((lpObj->Vitality+lpObj->AddVitality)/gServerInfo->m_GreaterLifeConstB)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_GreaterLifeConstC);

	value1 = ((value1>gServerInfo->m_GreaterLifeMaxRate)?gServerInfo->m_GreaterLifeMaxRate:value1);

	int value2 = 0;

	int value3 = 0;

	int count = gServerInfo->m_GreaterLifeTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_GreaterLifeTimeConstB);

	if(OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		value1 = (value1*((lpObj->Type==OBJECT_USER)?gServerInfo->m_GreaterLifeRate[lpObj->Class]:100))/100;

		gEffectManager->AddEffect(lpObj,0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,value3,0);

		this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty->m_PartyInfo[lpObj->PartyNumber];

		for(int n=0;n < MAX_PARTY_USER;n++)
		{
			int index = lpParty->Index[n];

			if(OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
			{
				continue;
			}

			value1 = (value1*((gObj[index].Type==OBJECT_USER)?gServerInfo->m_GreaterLifeRate[gObj[index].Class]:100))/100;

			gEffectManager->AddEffect(&gObj[index],0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,value3,0);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillMonsterAreaAttack(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,0);

		if(CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFireSlash(int aIndex, int bIndex, CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,aIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,1,0,0,0,combo);

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillPowerSlash(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4],SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX,SkillFrustrumY,angle,lpObj->X,lpObj->Y,6.0f,6.0f,1.0f,0.0f);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(this->CheckSkillFrustrum(SkillFrustrumX,SkillFrustrumY,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillForce(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	if(lpObj->SkillLongSpearChange == 0)
	{
		gAttack->Attack(lpObj,lpTarget,lpSkill,0,0,0,0,0);
		return 1;
	}

	int angle = this->GetSkillAngle(lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(gSkillHitBox->HitCheck(angle,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFireBurst(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	gAttack->Attack(lpObj,lpTarget,lpSkill,0,0,0,0,combo);

	if(combo != 0)
	{
		this->GCSkillAttackSend(lpObj,SKILL_COMBO,bIndex,1);
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		if(lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpTarget->X,lpTarget->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillEarthquake(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,500,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillSummonParty(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	if(OBJECT_RANGE(lpObj->PartyNumber) != 0)
	{
		PARTY_INFO* lpParty = &gParty->m_PartyInfo[lpObj->PartyNumber];

		for(int n=0;n < MAX_PARTY_USER;n++)
		{
			int index = lpParty->Index[n];

			if(OBJECT_RANGE(index) == 0 || aIndex == index)
			{
				continue;
			}

			if(gMoveSummon->CheckMoveSummon(&gObj[index],lpObj->Map,lpObj->X,lpObj->Y) == 0)
			{
				gNotice->GCNoticeSend(index,1,0,0,0,0,0,gMessage->GetMessage(272));
				continue;
			}

			if(lpObj->Map == MAP_CASTLE_SIEGE && gCastleSiegeSync->GetState() == CS_STATE_START && lpObj->CastleJoinSide != gObj[index].CastleJoinSide)
			{
				gNotice->GCNoticeSend(index,1,0,0,0,0,0,gMessage->GetMessage(272));
				continue;
			}

			int px = lpObj->X;
			int py = lpObj->Y;

			if(gObjGetRandomFreeLocation(lpObj->Map,&px,&py,4,4,10) == 0)
			{
				px = lpObj->X;
				py = lpObj->Y;
			}

			gObj[index].SkillSummonPartyTime = 5;

			gObj[index].SkillSummonPartyMap = lpObj->Map;

			gObj[index].SkillSummonPartyX = (BYTE)px;

			gObj[index].SkillSummonPartyY = (BYTE)py;

			gNotice->GCNoticeSend(index,1,0,0,0,0,0,gMessage->GetMessage(273),gObj[index].SkillSummonPartyTime);
		}
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);
	return 1;
}

bool CSkillManager::SkillGreaterCriticalDamage(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER)
	{
		return 0;
	}

	int value1 = ((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_GreaterCriticalDamageConstA)+((lpObj->Leadership+lpObj->AddLeadership)/gServerInfo->m_GreaterCriticalDamageConstB);


	int value2 = 0;

	int value3 = 0;

	int count = gServerInfo->m_GreaterCriticalDamageTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_GreaterCriticalDamageTimeConstB);

	if(OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager->AddEffect(lpObj,0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,value3,0);

		this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty->m_PartyInfo[lpObj->PartyNumber];

		for(int n=0;n < MAX_PARTY_USER;n++)
		{
			int index = lpParty->Index[n];

			if(OBJECT_RANGE(index) == 0)
			{
				continue;
			}

			if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
			{
				continue;
			}

			gEffectManager->AddEffect(&gObj[index],0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,value3,0);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillElectricSpark(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int x = lpObj->X-(8-(angle & 15));
	int y = lpObj->Y-(8-((angle & 240)/16));

	int nangle = this->GetSkillAngle(lpObj->X,lpObj->Y,x,y);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(gSkillHitBox->HitCheck(nangle,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,1000,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillCastleSiege(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,gObj[bIndex].X,gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	if(lpSkill->m_skill == SKILL_CRESCENT_MOON_SLASH || lpSkill->m_skill == SKILL_SPIRAL_SLASH || lpSkill->m_skill == SKILL_MANA_RAYS)
	{
		gAttack->Attack(lpObj,&gObj[bIndex],lpSkill,0,0,0,0,combo);
	}
	else if(lpSkill->m_skill == SKILL_MANA_GLAIVE || lpSkill->m_skill == SKILL_FIRE_BLAST)
	{
		gObjAddAttackProcMsgSendDelay(lpObj,50,bIndex,500,lpSkill->m_index,combo);
	}
	else if(lpSkill->m_skill == SKILL_MANA_GLAIVE || lpSkill->m_skill == SKILL_STAR_FALL)
	{
		gObjAddAttackProcMsgSendDelay(lpObj,50,bIndex,500,lpSkill->m_index,combo);
	}
	else if(lpSkill->m_skill == SKILL_SPIRAL_SLASH)
	{
		gObjAddAttackProcMsgSendDelay(lpObj,50,bIndex,500,lpSkill->m_index,combo);
	}
	else if(lpSkill->m_skill == SKILL_CHARGE)
	{
		gAttack->Attack(lpObj,&gObj[bIndex],lpSkill,0,0,0,0,combo);
	}

	if(combo != 0)
	{
		this->GCSkillAttackSend(lpObj,SKILL_COMBO,bIndex,1);
	}

	return 1;
}

bool CSkillManager::SkillStern(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	
	int x = lpObj->X-(8-(angle & 15));
	int y = lpObj->Y-(8-((angle & 240)/16));

	int nangle = this->GetSkillAngle(lpObj->X,lpObj->Y,x,y);

	gObjSetKillCount(aIndex,0);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(gSkillHitBox->HitCheck(nangle,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) != 0)
		{
			if(gObj[index].Teleport == 0 && gEffectManager->CheckEffect(&gObj[index],EFFECT_IRON_DEFENSE) == 0 && gEffectManager->CheckEffect(&gObj[index],EFFECT_IRON_DEFENSE_IMPROVED) == 0)
			{
				gEffectManager->AddEffect(&gObj[index],0,EFFECT_STERN,gServerInfo->m_SternTimeConstA,0,0,0,0);

				gObjSetPosition(index,gObj[index].X,gObj[index].Y);

				this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
			}
		}
	}

	return 1;
}

bool CSkillManager::SkillRemoveStern(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) != 0)
		{
			gEffectManager->DelEffect(&gObj[index],EFFECT_STERN);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillGreaterMana(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex,0);

	gEffectManager->AddEffect(lpObj,0,this->GetSkillEffect(lpSkill->m_index),gServerInfo->m_GreaterManaTimeConstA,gServerInfo->m_GreaterManaConstA,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if(lpObj->GuildIndex == 0 || gObj[index].GuildIndex == 0)
		{
			continue;
		}

		if(lpObj->GuildIndex != gObj[index].GuildIndex)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) != 0)
		{
			gEffectManager->AddEffect(&gObj[index],0,this->GetSkillEffect(lpSkill->m_index),gServerInfo->m_GreaterManaTimeConstA,gServerInfo->m_GreaterManaConstA,0,0,0);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillInvisibility(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	gObjSetKillCount(aIndex,0);

	gEffectManager->AddEffect(lpTarget,0,EFFECT_INVISIBILITY,gServerInfo->m_InvisibilityTimeConstA,0,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);
	return 1;
}

bool CSkillManager::SkillRemoveInvisibility(int aIndex, int bIndex, CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex, 0);

	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			if (gEffectManager->CheckEffect(&gObj[index], EFFECT_INVISIBILITY) != 0)
			{
				gEffectManager->DelEffect(&gObj[index], EFFECT_INVISIBILITY);

				gObjViewportListProtocolCreate(&gObj[index]);
			}

			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}

	return 1;
}

bool CSkillManager::SkillRemoveAllEffect(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) != 0)
		{
			gEffectManager->ClearAllEffect(&gObj[index]);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillBrand(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}

	gObjSetKillCount(aIndex,0);

	gEffectManager->AddEffect(lpObj,0,EFFECT_GUILD_STATE5,gServerInfo->m_BrandTimeConstA,0,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) != 0)
		{
			gEffectManager->AddEffect(&gObj[index],0,EFFECT_GUILD_STATE5,gServerInfo->m_BrandTimeConstA,0,0,0,0);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillPlasmaStorm(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,53,index,500,lpSkill->m_index,0);

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillInfinityArrow(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value = 0;

	gEffectManager->AddEffect(lpObj,0,this->GetSkillEffect(lpSkill->m_index),gServerInfo->m_InfinityArrowTimeConstA,value,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);
	return 1;
}

bool CSkillManager::SkillMonsterSummon(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if((GetLargeRand()%10) == 0)
	{
		return 0;
	}

	int MonsterClass = 0;

	switch(lpObj->Class)
	{
		case 161:
			MonsterClass = 147+GetLargeRand()%2;
			break;
		case 181:
			MonsterClass = 177+GetLargeRand()%2;
	 		break;
		case 189:
			MonsterClass = 185+GetLargeRand()%2;
			break;
		case 197:
			MonsterClass = 193+GetLargeRand()%2;
			break;
		case 267:
			MonsterClass = 263+GetLargeRand()%2;
			break;
		case 275:
			MonsterClass = 271+GetLargeRand()%2;
			break;
		default:
			return 0;
	}

	bool success = 0;

	int x = lpObj->X;
	int y = lpObj->Y;

	if(gObjGetRandomFreeLocation(lpObj->Map,&x,&y,2,2,10) != 0)
	{
		int index = gObjAddMonster(lpObj->Map);

		if(OBJECT_RANGE(index) != 0)
		{
			LPOBJ lpSummon = &gObj[index];

			lpSummon->PosNum = -1;
			lpSummon->X = x;
			lpSummon->Y = y;
			lpSummon->TX = x;
			lpSummon->TY = y;
			lpSummon->OldX = x;
			lpSummon->OldY = y;
			lpSummon->StartX = x;
			lpSummon->StartY = y;
			lpSummon->Dir = GetLargeRand()%8;
			lpSummon->Map = lpObj->Map;

			gObjSetMonster(index,MonsterClass);

			lpSummon->Attribute = ATTRIBUTE_SUMMON;
			lpSummon->MaxRegenTime = 0;

			success = 1;
		}
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,((y*256)+x),success);
	return success;
}

bool CSkillManager::SkillMagicDamageImmunity(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	gEffectManager->AddEffect(lpObj,0,EFFECT_MAGIC_DAMAGE_IMMUNITY,gServerInfo->m_MagicDamageImmunityTimeConstA,0,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,lpObj->Index,1);
	return 1;
}

bool CSkillManager::SkillPhysiDamageImmunity(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	gEffectManager->AddEffect(lpObj,0,EFFECT_PHYSI_DAMAGE_IMMUNITY,gServerInfo->m_PhysiDamageImmunityTimeConstA,0,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,lpObj->Index,1);
	return 1;
}

bool CSkillManager::SkillOrderOfProtection(int aIndex,int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	gEffectManager->AddEffect(lpObj,0,EFFECT_ORDER_OF_PROTECTION,15,SKILL_ORDER_OF_PROTECTION,0,0,0);

	return 1;
}

bool CSkillManager::SkillOrderOfRestraint(int aIndex,int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,SKILL_ORDER_OF_RESTRAINT,lpObj->X,lpObj->Y,gObj[bIndex].X,gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	gEffectManager->AddEffect(&gObj[bIndex],0,EFFECT_ORDER_OF_RESTRAINT,15,SKILL_ORDER_OF_RESTRAINT,0,0,0);

	gObjSetPosition(bIndex,gObj[bIndex].X,gObj[bIndex].Y);

	return 1;
}

bool CSkillManager::SkillOrderOfTracking(int aIndex,int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0 || aIndex == bIndex)
	{
		return 0;
	}

	if(gEffectManager->CheckStunEffect(lpObj) != 0)
	{
		return 0;
	}

	int sx = gObj[bIndex].X;
	int sy = gObj[bIndex].Y;

	for(int n=0;n < 50;n++)
	{
		int tx = gObj[bIndex].X+((GetLargeRand()%3)-1);
		int ty = gObj[bIndex].Y+((GetLargeRand()%3)-1);

		if(gObjCheckTeleportArea(aIndex,tx,ty) != 0)
		{
			sx = tx;
			sy = ty;
			break;
		}
	}

	gObjTeleportMagicUse(aIndex,sx,sy);
	return 1;
}

bool CSkillManager::SkillOrderOfWeaken(int aIndex,int bIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,SKILL_ORDER_OF_WEAKEN,lpObj->X,lpObj->Y,gObj[bIndex].X,gObj[bIndex].Y) == 0)
	{
		return 0;
	}

	int ShieldDamage = (gObj[bIndex].Shield*50)/100;

	if(gObj[bIndex].Shield < ShieldDamage)
	{
		ShieldDamage = gObj[bIndex].Shield;
		gObj[bIndex].Shield = 0;
	}
	else
	{
		ShieldDamage = ShieldDamage;
		gObj[bIndex].Shield -= ShieldDamage;
	}

	GCDamageSend(aIndex,bIndex,0,0,0,ShieldDamage);

	return 1;
}

bool CSkillManager::SkillChainLightning(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	if(lpObj->Type == OBJECT_USER && gHackSkillSpeedCheck->CheckSkillSpeed(lpObj->Index,lpSkill->m_index) == 0)
	{
		return 0;
	}

	int count = 1;

	int target[3] = {bIndex,bIndex,bIndex};

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(bIndex == index)
		{
			continue;
		}

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpTarget->X,lpTarget->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		target[count++] = index;

		if(count >= _countof(target))
		{
			break;
		}
	}

	BYTE send[256];

	PMSG_SKILL_CHAIN_SKILL_SEND pMsg;

	pMsg.header.set(0xBF,0x0A,0);

	int size = sizeof(pMsg);

	pMsg.skill[0] = SET_NUMBERHB(lpSkill->m_index);

	pMsg.skill[1] = SET_NUMBERLB(lpSkill->m_index);

	pMsg.index = aIndex;

	pMsg.count = 0;

	PMSG_SKILL_CHAIN_SKILL info;

	for(int n=0;n < _countof(target);n++)
	{
		gObjAddAttackProcMsgSendDelay(lpObj,52,target[n],(200+(n*200)),lpSkill->m_index,(n+1));

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,target[n],1);
		}

		info.index = target[n];

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);

	MsgSendV2(lpObj,send,size);

	return 1;
}

bool CSkillManager::SkillDamageReflect(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = gServerInfo->m_ReflectDamageConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_ReflectDamageConstB);

	value = ((value>gServerInfo->m_ReflectDamageMaxRate)?gServerInfo->m_ReflectDamageMaxRate:value);

	value = (value*((lpTarget->Type==OBJECT_USER)?gServerInfo->m_ReflectDamageRate[lpTarget->Class]:100))/100;

	int count = gServerInfo->m_ReflectDamageTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_ReflectDamageTimeConstB);

	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),count,value,0,0,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	if(combo != 0)
	{
		this->GCSkillAttackSend(lpObj,SKILL_COMBO,bIndex,1);
	}

	return 1;
}

bool CSkillManager::SkillSwordPower(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value1 = (lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerConstA;

	value1 = ((value1>gServerInfo->m_SwordPowerMaxRate)?gServerInfo->m_SwordPowerMaxRate:value1);

	int value2 = (lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerConstA;

	value2 = ((value2>gServerInfo->m_SwordPowerMaxRate)?gServerInfo->m_SwordPowerMaxRate:value2);

	int value3 = (((40-((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerConstB))<10)?10:(40-((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerConstB)));

	int value4 = (((40-((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerConstB))<10)?10:(40-((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerConstB)));

	int count = gServerInfo->m_SwordPowerTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SwordPowerTimeConstB);

	gEffectManager->AddEffect(lpObj,0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,value3,value4);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	if(combo != 0)
	{
		this->GCSkillAttackSend(lpObj,SKILL_COMBO,bIndex,1);
	}

	return 1;
}

bool CSkillManager::SkillSwordPowerGetDefense(int aIndex,int* defense) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDefenseRate;

	(*defense) -= (((lpObj->Dexterity+lpObj->AddDexterity)/gServerInfo->m_SUDefenseConstA)*value)/100;

	return 1;
}

bool CSkillManager::SkillSwordPowerGetPhysiDamage(int aIndex,int* DamageMin,int* DamageMax) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDamageRate;

	value = ((value>gServerInfo->m_SwordPowerMaxRate)?gServerInfo->m_SwordPowerMaxRate:value);

	(*DamageMin) += ((((lpObj->Strength+lpObj->AddStrength)+(lpObj->Dexterity+lpObj->AddDexterity))/gServerInfo->m_SUPhysiDamageMinConstA)*value)/100;

	(*DamageMax) += ((((lpObj->Strength+lpObj->AddStrength)+(lpObj->Dexterity+lpObj->AddDexterity))/gServerInfo->m_SUPhysiDamageMaxConstA)*value)/100;

	return 1;
}

bool CSkillManager::SkillSwordPowerGetMagicDamage(int aIndex,int* DamageMin,int* DamageMax) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDamageRate;

	value = ((value>gServerInfo->m_SwordPowerMaxRate)?gServerInfo->m_SwordPowerMaxRate:value);

	(*DamageMin) += (((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SUMagicDamageMinConstA)*value)/100;

	(*DamageMax) += (((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SUMagicDamageMaxConstA)*value)/100;

	return 1;
}

bool CSkillManager::SkillSwordPowerGetCurseDamage(int aIndex,int* DamageMin,int* DamageMax) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager->CheckEffect(lpObj,EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}

	int value = lpObj->EffectOption.AddSwordPowerDamageRate;

	value = ((value>gServerInfo->m_SwordPowerMaxRate)?gServerInfo->m_SwordPowerMaxRate:value);

	(*DamageMin) += (((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SUMagicDamageMinConstA)*value)/100;

	(*DamageMax) += (((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SUMagicDamageMaxConstA)*value)/100;

	return 1;
}

bool CSkillManager::SkillSleep(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type == OBJECT_USER && lpTarget->Authority == AUTHORITY_ADMINISTRATOR)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	if(lpTarget->Type == OBJECT_USER && this->CheckSkillTarget(lpObj,bIndex,-1,lpTarget->Type) == 0)
	{
		return 0;
	}

	int rate = gServerInfo->m_SleepConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SleepConstB);

	rate += lpObj->Inventory[INVENTORY_SLOT_WEAPON2].GetBookSuccessRate();

	int count = gServerInfo->m_SleepTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_SleepTimeConstB);

	count = ((count>gServerInfo->m_SleepMaxTime)?gServerInfo->m_SleepMaxTime:count);

	if((GetLargeRand()%100) >= rate)
	{
		gAttack->MissSend(lpObj,lpTarget,lpSkill,1,0);
		return 0;
	}

	if(gEffectManager->CheckEffect(lpTarget,EFFECT_IRON_DEFENSE) != 0 || gEffectManager->CheckEffect(lpTarget,EFFECT_IRON_DEFENSE_IMPROVED) != 0)
	{
		gAttack->MissSend(lpObj,lpTarget,lpSkill,1,0);
		return 0;
	}

	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),count,0,0,0,0);

	gObjSetPosition(bIndex,lpTarget->X,lpTarget->Y);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);
	return 1;
}

bool CSkillManager::SkillLesserDefense(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int rate = gServerInfo->m_LesserDefenseConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_LesserDefenseConstB);

	rate += lpObj->Inventory[INVENTORY_SLOT_WEAPON2].GetBookSuccessRate();

	int value = gServerInfo->m_LesserDefenseConstC+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_LesserDefenseConstD);

	value = ((value>gServerInfo->m_LesserDefenseMaxRate)?gServerInfo->m_LesserDefenseMaxRate:value);

	int count = gServerInfo->m_LesserDefenseTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_LesserDefenseTimeConstB);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if((GetLargeRand()%100) >= rate)
		{
			gAttack->MissSend(lpObj,&gObj[index],lpSkill,1,0);
		}
		else
		{
			gEffectManager->AddEffect(&gObj[index],0,this->GetSkillEffect(lpSkill->m_index),count,value,0,0,0);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillLesserDamage(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int rate = gServerInfo->m_LesserDamageConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_LesserDamageConstB);

	rate += lpObj->Inventory[INVENTORY_SLOT_WEAPON2].GetBookSuccessRate();

	int value = gServerInfo->m_LesserDamageConstC+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_LesserDamageConstD);

	value = ((value>gServerInfo->m_LesserDamageMaxRate)?gServerInfo->m_LesserDamageMaxRate:value);

	int count = gServerInfo->m_LesserDamageTimeConstA+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_LesserDamageTimeConstB);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if((GetLargeRand()%100) >= rate)
		{
			gAttack->MissSend(lpObj,&gObj[index],lpSkill,1,0);
		}
		else
		{
			gEffectManager->AddEffect(&gObj[index],0,this->GetSkillEffect(lpSkill->m_index),count,value,0,0,0);

			this->GCSkillAttackSend(lpObj,lpSkill->m_index,index,1);
		}
	}

	return 1;
}

bool CSkillManager::SkillSahamutt(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,x,y) == 0)
	{
		return 0;
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,x,y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,1000,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillNeil(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,x,y) == 0)
	{
		return 0;
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		
		if(this->CheckSkillRadio(lpSkill->m_index,x,y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,1000,lpSkill->m_index,0);

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillGhostPhantom(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,x,y) == 0)
	{
		return 0;
	}

	gObjAddAttackProcMsgSendDelay(lpObj,55,bIndex,1000,lpSkill->m_index,MAKE_NUMBERDW(x,y));

	gObjAddAttackProcMsgSendDelay(lpObj,55,bIndex,2000,lpSkill->m_index,MAKE_NUMBERDW(x,y));

	gObjAddAttackProcMsgSendDelay(lpObj,55,bIndex,3000,lpSkill->m_index,MAKE_NUMBERDW(x,y));

	gObjAddAttackProcMsgSendDelay(lpObj,55,bIndex,4000,lpSkill->m_index,MAKE_NUMBERDW(x,y));

	gObjAddAttackProcMsgSendDelay(lpObj,55,bIndex,5000,lpSkill->m_index,MAKE_NUMBERDW(x,y));

	return 1;
}

bool CSkillManager::SkillGhostPhantomAreaAttack(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,x,y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,0);

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillRedStorm(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,250,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillFrozenStab(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,x,y) == 0)
	{
		return 0;
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,x,y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,500,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillMagicCircle(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}

	int value1 = (((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_DWMagicDamageMinConstA)*gServerInfo->m_MagicCircleConstA)/100;

	int value2 = 0;

	int value3 = 0;

	int count = gServerInfo->m_MagicCircleTimeConstA;

	gEffectManager->AddEffect(lpObj,0,this->GetSkillEffect(lpSkill->m_index),count,value1,value2,value3,0);

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,aIndex,1);
	return 1;
}

bool CSkillManager::SkillShieldRecover(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}

	if(this->CheckSkillRange(lpObj,lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
	{
		return 0;
	}

	int value = ((lpSkill->m_level+1)*gServerInfo->m_ShieldRecoverConstA)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_ShieldRecoverConstB);

	if((lpTarget->Shield+value) > (lpTarget->MaxShield+lpTarget->AddShield))
	{
		lpTarget->Shield = lpTarget->MaxShield+lpTarget->AddShield;
	}
	else
	{
		lpTarget->Shield += value;
	}

	GCLifeSend(bIndex,0xFF,(int)lpTarget->Life,lpTarget->Shield);

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index,(value/40));
		}
	}

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);
	return 1;
}

bool CSkillManager::SkillFiveShot(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4],SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX,SkillFrustrumY,angle,lpObj->X,lpObj->Y,6.0f,7.0f,2.0f,0.0f);

	vec3_t Angle;

	Vector(0.0f,6.0f,0.0f,Angle);

	vec3_t p[4];

	Vector(0.0f,0.0f,40.0f,p[0]);
	Vector(0.0f,0.0f,20.0f,p[1]);
	Vector(0.0f,0.0f,340.0f,p[2]);
	Vector(0.0f,0.0f,320.0f,p[3]);

	float Matrix[3][4];

	vec3_t vFrustrum[5],vFrustrum2[5];

	AngleMatrix(p[0],Matrix);
	VectorRotate(Angle,Matrix,vFrustrum[0]);

	AngleMatrix(p[1],Matrix);
	VectorRotate(Angle,Matrix,vFrustrum[1]);

	Vector(0.0f,6.0f,0.0f,vFrustrum[2]);

	AngleMatrix(p[2],Matrix);
	VectorRotate(Angle,Matrix,vFrustrum[3]);

	AngleMatrix(p[3],Matrix);
	VectorRotate(Angle,Matrix,vFrustrum[4]);

	Vector(0.0f,0.0f,(vec_t)((angle*360)/255),p[0]);

	AngleMatrix((float*)p,Matrix);

	int ArrowFrustrum[5],ArrowFrustrumX[5],ArrowFrustrumY[5];

	for(int n=0;n < 5;n++)
	{
		VectorRotate(vFrustrum[n],Matrix,vFrustrum2[n]);

		ArrowFrustrumX[n] = (int)vFrustrum2[n][0]+lpObj->X;

		ArrowFrustrumY[n] = (int)vFrustrum2[n][1]+lpObj->Y;
	}

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(this->CheckSkillFrustrum(SkillFrustrumX,SkillFrustrumY,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		for(int i=0;i < 5;i++)
		{
			if((ArrowFrustrum[i]=((ArrowFrustrumX[i]-lpObj->X)*(gObj[index].Y-lpObj->Y))-((ArrowFrustrumY[i]-lpObj->Y)*(gObj[index].X-lpObj->X))) > -5 && ArrowFrustrum[i] < 5)
			{
				gAttack->Attack(lpObj,&gObj[index],lpSkill,1,0,0,0,combo);

				if(combo != 0)
				{
					this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
				}
			}
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillSwordSlash(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4],SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX,SkillFrustrumY,angle,lpObj->X,lpObj->Y,2.0f,4.0f,5.0f,0.0f);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(this->CheckSkillFrustrum(SkillFrustrumX,SkillFrustrumY,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,1,0,0,0,combo);

		gAttack->Attack(lpObj,&gObj[index],lpSkill,1,0,0,0,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillLightningStorm(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gObjAddAttackProcMsgSendDelay(lpObj,50,index,400,lpSkill->m_index,combo);

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillBirds(int aIndex,int bIndex,CSkill* lpSkill,BYTE angle,bool combo) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int SkillFrustrumX[4],SkillFrustrumY[4];

	this->GetSkillFrustrum(SkillFrustrumX,SkillFrustrumY,angle,lpObj->X,lpObj->Y,1.5f,6.0f,1.5f,0.0f);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(this->CheckSkillFrustrum(SkillFrustrumX,SkillFrustrumY,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		if(OBJECT_RANGE(bIndex) != 0 && index == bIndex)
		{
			gObjAddAttackProcMsgSendDelay(lpObj,50,index,200,lpSkill->m_index,combo);

			gObjAddAttackProcMsgSendDelay(lpObj,50,index,300,lpSkill->m_index,combo);
		}
		else
		{
			gObjAddAttackProcMsgSendDelay(lpObj,50,index,200,lpSkill->m_index,combo);
		}

		if(combo != 0)
		{
			this->GCSkillAttackSend(lpObj,SKILL_COMBO,index,1);
		}

		if(CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}

	return 1;
}

bool CSkillManager::SkillSelfExplosion(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	this->GCSkillAttackSend(lpObj,lpSkill->m_index,bIndex,1);

	int count = 0;

	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}

		int index = lpObj->VpPlayer2[n].index;

		if(this->CheckSkillTarget(lpObj,index,bIndex,lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}

		if(this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[index].X,gObj[index].Y) == 0)
		{
			continue;
		}

		gAttack->Attack(lpObj,&gObj[index],lpSkill,0,0,0,0,0);

		if(CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}

	lpObj->Life = 0;

	gObjectManager->CharacterLifeCheck(lpObj,lpObj,0,1,0,0,0,0,0);

	return 1;
}

bool CSkillManager::SkillLargeRingBlower(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillUpperBeast(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillChainDriver(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillDarkSide(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillDarkSideGetTargetIndex(int aIndex,int bIndex,CSkill* lpSkill,WORD* target) // OK
{
	return 0;
}

bool CSkillManager::SkillDragonLore(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillGreaterIgnoreDefenseRate(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillFitness(int aIndex,int bIndex,CSkill* lpSkill) // OK
{

	return 0;
}

bool CSkillManager::SkillGreaterDefenseSuccessRate(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillPhoenixShot(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillBloodStorm(int aIndex,int bIndex,CSkill* lpSkill,BYTE x,BYTE y,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillCure(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillPartyHeal(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillPoisonArrow(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillBless(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillBlind(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillEarthPrison(int aIndex,int bIndex,CSkill* lpSkill,bool combo) // OK
{
	return 0;
}

bool CSkillManager::SkillIronDefense(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

bool CSkillManager::SkillBloodHowling(int aIndex,int bIndex,CSkill* lpSkill) // OK
{
	return 0;
}

void CSkillManager::ApplyMeteoriteEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyIceStormEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),gServerInfo->m_IceStormEffectTime,0,0,0,0);
}

void CSkillManager::ApplyTwistingSlashEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyRagefulBlowEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyDeathStabEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyFireSlashEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	int value = gServerInfo->m_FireSlashConstA+((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_FireSlashConstB);

	value = ((value>gServerInfo->m_FireSlashMaxRate)?gServerInfo->m_FireSlashMaxRate:value);

	int count = gServerInfo->m_FireSlashTimeConstA;

	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),count,value,0,0,0);
}

void CSkillManager::ApplyFireBurstEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyPlasmaStormEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[(damage=(2+(GetLargeRand()%5)))];

		if(lpItem->IsItem() != 0 && lpItem->IsLuckyItem() == 0 && lpItem->m_IsPeriodicItem == 0)
		{
			lpItem->m_Durability = (lpItem->m_Durability*50)/100;
			gItemManager->GCItemDurSend(lpTarget->Index,damage,(BYTE)lpItem->m_Durability,0);
		}
	}
}

void CSkillManager::ApplyFireScreamEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{

}

void CSkillManager::ApplyEarthquakeEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	gObjBackSpring2(lpTarget,lpObj,3);
}

void CSkillManager::ApplyDrainLifeEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	damage = ((damage*gServerInfo->m_DrainLifeConstA)/100)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_DrainLifeConstB);

	if((lpObj->Life+damage) > (lpObj->MaxLife+lpObj->AddLife))
	{
		lpObj->Life = lpObj->MaxLife+lpObj->AddLife;
	}
	else
	{
		lpObj->Life += damage;
	}

	GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);
}

void CSkillManager::ApplySahamuttEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	damage = (damage*gServerInfo->m_SahamuttConstA)/100;
	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),5,lpObj->Index,1,SET_NUMBERHW(damage),SET_NUMBERLW(damage));
}

void CSkillManager::ApplyNeilEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	damage = (damage*gServerInfo->m_NeilConstA)/100;
	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),5,lpObj->Index,1,SET_NUMBERHW(damage),SET_NUMBERLW(damage));
}

void CSkillManager::ApplyGhostPhantomEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),2,0,0,0,0);
}

void CSkillManager::ApplyFrozenStabEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),gServerInfo->m_FrozenStabEffectTime,0,0,0,0);
}

void CSkillManager::ApplyFiveShotEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplySwordSlashEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	gEffectManager->AddEffect(lpTarget,0,this->GetSkillEffect(lpSkill->m_index),1,0,0,0,0);
}

void CSkillManager::ApplyLargeRingBlowerEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyUpperBeastEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyChainDriverEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyDragonLoreEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyDragonSlayerEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyPhoenixShotEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::ApplyEarthPrisonEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
}

void CSkillManager::CGMultiSkillAttackRecv(PMSG_MULTI_SKILL_ATTACK_RECV* lpMsg,int aIndex,bool type) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}

	if(lpObj->Permission[13] != 0)
	{
		return;
	}

	int skill = lpMsg->skill;

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
	{
		if(gSkillUseArea->CheckUseSkill(lpObj,skill) == 0)
		{
			return;
		}
	}

	CSkill* lpSkill = 0;

	if(lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj,skill);

		if(lpSkill == 0)
		{
			return;
		}

		if(this->CheckSkillDelay(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireClass(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if(lpSkill->IsSkill() == 0)
		{
			return;
		}
	}

	if(lpSkill->m_skill != SKILL_FLAME && lpSkill->m_skill != SKILL_TWISTER && lpSkill->m_skill != SKILL_EVIL_SPIRIT && lpSkill->m_skill != SKILL_HELL_FIRE && lpSkill->m_skill != SKILL_AQUA_BEAM && lpSkill->m_skill != SKILL_BLAST && lpSkill->m_skill != SKILL_INFERNO && lpSkill->m_skill != SKILL_TRIPLE_SHOT && lpSkill->m_skill != SKILL_IMPALE && lpSkill->m_skill != SKILL_MONSTER_AREA_ATTACK && lpSkill->m_skill != SKILL_PENETRATION && lpSkill->m_skill != SKILL_FIRE_SLASH && lpSkill->m_skill != SKILL_FIRE_SCREAM)
	{
		return;
	}

	if(lpObj->Type == OBJECT_USER && (lpObj->MultiSkillIndex == 0 || lpObj->MultiSkillIndex != lpSkill->m_index || (lpObj->MultiSkillCount++) >= 5))
	{
		return;
	}

	if(lpObj->Type == OBJECT_USER && lpSkill->m_skill == SKILL_TRIPLE_SHOT)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].IsItem() == 0 || lpObj->Inventory[INVENTORY_SLOT_WEAPON2].IsItem() == 0)
		{
			return;
		}

		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index < GET_ITEM(4,0) || lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index >= GET_ITEM(5,0))
		{
			return;
		}

		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index < GET_ITEM(4,0) || lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index >= GET_ITEM(5,0))
		{
			return;
		}
	}

	lpMsg->count = ((lpMsg->count>5)?5:lpMsg->count);

	for(int n=0;n < lpMsg->count;n++)
	{
		PMSG_MULTI_SKILL_ATTACK* lpInfo = (PMSG_MULTI_SKILL_ATTACK*)(((BYTE*)lpMsg)+sizeof(PMSG_MULTI_SKILL_ATTACK_RECV)+(sizeof(PMSG_MULTI_SKILL_ATTACK)*n));

		int bIndex = MAKE_NUMBERW(lpInfo->index[0],lpInfo->index[1]);

		if(OBJECT_RANGE(bIndex) == 0)
		{
			continue;
		}

		LPOBJ lpTarget = &gObj[bIndex];

		if(lpObj->Type == OBJECT_USER && this->CheckSkillRadio(lpSkill->m_index,lpObj->X,lpObj->Y,lpTarget->X,lpTarget->Y) == 0)
		{
			continue;
		}

		if(type != 0)
		{
			gAttack->Attack(lpObj,lpTarget,lpSkill,1,1,0,0,0);
		}
		else if(lpSkill->m_skill == SKILL_FIRE_SLASH)
		{
			gAttack->Attack(lpObj,lpTarget,lpSkill,1,1,0,0,0);
		}
		else
		{
			gAttack->Attack(lpObj,lpTarget,lpSkill,0,1,0,0,0);
		}
	}
}

void CSkillManager::CGSkillAttackRecv(PMSG_SKILL_ATTACK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Live == 0)
	{
		return;
	}

	if(lpObj->Permission[13] != 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(gObjIsConnectedGS(bIndex) == 0)
	{
		return;
	}

	if(lpTarget->Live == 0)
	{
		return;
	}

	int skill = lpMsg->skill;

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
	{
		if(gSkillUseArea->CheckUseSkill(lpObj,skill) == 0)
		{
			return;
		}
	}

	if(gMap[lpTarget->Map].CheckAttr(lpTarget->X,lpTarget->Y,1) != 0)
	{
		if(gSkillUseArea->CheckUseSkill(lpTarget,skill) == 0)
		{
			return;
		}
	}

	CSkill* lpSkill = 0;

	if(lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj,skill);

		if(lpSkill == 0)
		{
			return;
		}

		if(this->CheckSkillDelay(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireKillPoint(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireGuildStatus(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireClass(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if(lpSkill->IsSkill() == 0)
		{
			return;
		}
	}

	lpObj->MultiSkillIndex = 0;

	lpObj->MultiSkillCount = 0;

	this->UseAttackSkill(aIndex,((skill==SKILL_NOVA_START)?skill:bIndex),lpSkill);
}

void CSkillManager::CGDurationSkillAttackRecv(PMSG_DURATION_SKILL_ATTACK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}

	if(lpObj->Live == 0)
	{
		return;
	}

	if(lpObj->Permission[13] != 0)
	{
		return;
	}

	int skill = lpMsg->skill;

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0)
	{
		if(gSkillUseArea->CheckUseSkill(lpObj,skill) == 0)
		{
			return;
		}
	}

	CSkill* lpSkill = 0;

	if(lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj,skill);

		if(lpSkill == 0)
		{
			return;
		}

		if(this->CheckSkillDelay(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireKillPoint(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireGuildStatus(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}

		if(this->CheckSkillRequireClass(lpObj,lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];

		if(lpSkill->IsSkill() == 0)
		{
			return;
		}
	}

	lpObj->MultiSkillIndex = 0;

	lpObj->MultiSkillCount = 0;

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	this->UseDurationSkillAttack(lpObj->Index,bIndex,lpSkill,lpMsg->x,lpMsg->y,lpMsg->dir,lpMsg->angle);
}

void CSkillManager::CGSkillCancelRecv(PMSG_SKILL_CANCEL_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	gEffectManager->DelEffect(lpObj,this->GetSkillEffect(lpMsg->skill));
}

void CSkillManager::CGRageFighterSkillAttackRecv(PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV* lpMsg,int aIndex) // OK
{
}

void CSkillManager::CGSkillDarkSideRecv(PMSG_SKILL_DARK_SIDE_RECV* lpMsg,int aIndex) // OK
{
}

void CSkillManager::CGSkillTeleportAllyRecv(PMSG_SKILL_TELEPORT_ALLY_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (OBJECT_RANGE(lpMsg->index) == 0)
	{
		return;
	}

	if (gObj[lpMsg->index].Connected != OBJECT_ONLINE || gObj[lpMsg->index].CloseCount > 0)
	{
		lpMsg->index = aIndex;
	}

	LPOBJ lpTarget = &gObj[lpMsg->index];

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0 || OBJECT_RANGE(lpTarget->PartyNumber) == 0)
	{
		return;
	}

	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return;
	}

	if (gEffectManager->CheckStunEffect(lpObj) != 0)
	{
		return;
	}

	if (lpObj->Map != lpTarget->Map)
	{
		return;
	}

	if (gObjCheckTeleportArea(lpMsg->index, lpMsg->x, lpMsg->y) == 0)
	{
		return;
	}

	if (lpTarget->Map == MAP_CASTLE_SIEGE)
	{
		if (lpMsg->y <= 190)
		{
			lpMsg->y = 191;
		}
		if (lpMsg->y >= 215)
		{
			lpMsg->y = 214;
		}
		if (lpMsg->x >= 190 || lpMsg->x >= 189 && lpMsg->y >= 214)
		{
			lpMsg->x = 188;
		}
		if (lpMsg->x <= 162)
		{
			lpMsg->x = 163;
		}
	}

	CSkill* lpSkill = this->GetSkill(lpObj, SKILL_TELEPORT_ALLY);

	if (lpSkill != 0)
	{
		if (this->CheckSkillMana(lpObj, lpSkill->m_index) == 0 || this->CheckSkillBP(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}

		this->GCSkillAttackSend(lpObj, lpSkill->m_index, lpMsg->index, 1);

		gObjTeleportMagicUse(lpMsg->index, lpMsg->x, lpMsg->y);

		lpObj->Mana -= (this->GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100;

		lpObj->BP -= (this->GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100;

		GCManaSend(lpObj->Index, 0xFF, (int)lpObj->Mana, lpObj->BP);
	}
}

void CSkillManager::GCSkillAttackSend(LPOBJ lpObj,int skill,int aIndex,BYTE type) // OK
{
	if(lpObj->Type == OBJECT_USER && gHackSkillSpeedCheck->CheckSkillSpeed(lpObj->Index,skill) == 0)
	{
		return;
	}

	PMSG_SKILL_ATTACK_SEND pMsg;

	pMsg.header.setE(0x19,sizeof(pMsg));

	pMsg.skill = skill;

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	pMsg.target[0] = SET_NUMBERHB(aIndex) | (type*0x80);
	pMsg.target[1] = SET_NUMBERLB(aIndex);

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void CSkillManager::GCSkillCancelSend(LPOBJ lpObj,int skill) // OK
{
	PMSG_SKILL_CANCEL_SEND pMsg;

	pMsg.header.set(0x1B,sizeof(pMsg));

	pMsg.skill = skill;

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}
	
	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void CSkillManager::GCDurationSkillAttackSend(LPOBJ lpObj,int skill,BYTE x,BYTE y,BYTE dir) // OK
{
	if(lpObj->Type == OBJECT_USER && gHackSkillSpeedCheck->CheckSkillSpeed(lpObj->Index,skill) == 0)
	{
		return;
	}

	PMSG_DURATION_SKILL_ATTACK_SEND pMsg;

	pMsg.header.setE(0x1E,sizeof(pMsg));

	pMsg.skill = skill;

	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);

	pMsg.x = x;
	pMsg.y = y;
	pMsg.dir = dir;

	if(lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index,(BYTE*)&pMsg,pMsg.header.size);
	}

	MsgSendV2(lpObj,(BYTE*)&pMsg,pMsg.header.size);
}

void CSkillManager::GCRageFighterSkillAttackSend(LPOBJ lpObj,int skill,int aIndex,BYTE type) // OK
{
}

void CSkillManager::GCSkillAddSend(int aIndex,BYTE slot,int skill,BYTE level,BYTE type) // OK
{
	BYTE send[256];

	PMSG_SKILL_LIST_SEND pMsg;

	pMsg.header.set(0xF3,0x11,0);

	int size = sizeof(pMsg);

	pMsg.count = 0xFE;

	pMsg.type = type;

	PMSG_SKILL_LIST info;

	info.slot = slot;

	info.skill[0] = skill;

	info.skill[1] = (level << 3) | ((skill/255) & 7);

	memcpy(&send[size],&info,sizeof(info));
	size += sizeof(info);

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}

void CSkillManager::GCSkillDelSend(int aIndex,BYTE slot,int skill,BYTE level,BYTE type) // OK
{
	BYTE send[256];

	PMSG_SKILL_LIST_SEND pMsg;

	pMsg.header.set(0xF3,0x11,0);

	int size = sizeof(pMsg);

	pMsg.count = 0xFF;

	pMsg.type = type;

	PMSG_SKILL_LIST info;

	info.slot = slot;

	info.skill[0] = skill;

	info.skill[1] = (level << 3) | ((skill/255) & 7);

	memcpy(&send[size],&info,sizeof(info));
	size += sizeof(info);

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(aIndex,send,size);
}

void CSkillManager::GCSkillListSend(LPOBJ lpObj,BYTE type) // OK
{
	BYTE send[1024];

	PMSG_SKILL_LIST_SEND pMsg;

	pMsg.header.set(0xF3,0x11,0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	pMsg.type = type;

	PMSG_SKILL_LIST info;

	for(int n=0;n < MAX_SKILL_LIST;n++)
	{
		if(lpObj->Skill[n].IsSkill() == 0)
		{
			continue;
		}

		info.slot = n;

		info.skill[0] = (BYTE)lpObj->Skill[n].m_index;

		info.skill[1] = (lpObj->Skill[n].m_level << 3) | ((lpObj->Skill[n].m_index/255) & 7);

		memcpy(&send[size],&info,sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size = size;

	memcpy(send,&pMsg,sizeof(pMsg));

	DataSend(lpObj->Index,send,size);
}
