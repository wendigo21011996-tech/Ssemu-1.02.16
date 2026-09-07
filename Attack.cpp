// Attack.cpp: implementation of the CAttack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Attack.h"
#include "BattleSoccer.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "ChaosCastle.h"
#include "Crywolf.h"
#include "CustomArena.h"
#include "CustomAttack.h"
#include "CustomWing.h"
#include "DamageTable.h"
#include "DarkSpirit.h"
#include "Duel.h"
#include "EffectManager.h"
#include "GuildManager.h"
#include "HackSkillSpeedCheck.h"
#include "IllusionTemple.h"
#include "Kalima.h"
#include "Map.h"
#include "MapManager.h"
#include "Monster.h"
#include "ObjectManager.h"
#include "Party.h"
#include "PetDamageTable.h"
#include "ScriptLoader.h"
#include "ServerInfo.h"
#include "SkillDamage.h"
#include "SkillEnabler.h"
#include "SkillManager.h"
#include "UnionManager.h"
#include "Util.h"
#include "WingOption.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAttack::CAttack() // OK
{

}

CAttack::~CAttack() // OK
{

}

bool CAttack::Attack(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,bool send,BYTE flag,int damage,int count,bool combo) // OK
{
	#pragma region ATTACK_CHECK

	if(lpObj->Index == lpTarget->Index)
	{
		return 0;
	}

	if(lpObj->Type == OBJECT_USER && gObjIsConnectedGP(lpObj->Index) == 0)
	{
		return 0;
	}

	if(lpTarget->Type == OBJECT_USER && gObjIsConnectedGP(lpTarget->Index) == 0)
	{
		return 0;
	}

	if(lpObj->Permission[13] != 0)
	{
		return 0;
	}

	if(lpObj->Map != lpTarget->Map || lpObj->Teleport == 2)
	{
		return 0;
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X,lpTarget->Y,1) != 0)
	{
		return 0;
	}

	if(gCastleSiege->GetState() != CS_STATE_START)
	{
		if(lpTarget->Type == OBJECT_MONSTER && lpTarget->Map == MAP_CASTLE_SIEGE && (lpTarget->Class == 277 || lpTarget->Class == 283 || lpTarget->Class == 288))
		{
			return 0;
		}
	}

	if(gCrywolf->GetCrywolfState() == CRYWOLF_STATE_READY || gCrywolf->GetCrywolfState() == CRYWOLF_STATE_END)
	{
		if(lpTarget->Type == OBJECT_MONSTER && lpTarget->Map == MAP_CRYWOLF)
		{
			return 0;
		}
	}

	if(lpObj->Guild != 0 && lpObj->Guild->WarState != GUILD_WAR_STATE_NONE)
	{
		if(lpObj->Guild->WarType == GUILD_WAR_TYPE_SOCCER && gBattleSoccer->GetState() != BS_STATE_START)
		{
			return 1;
		}

		if(lpObj->Guild->WarType == GUILD_WAR_TYPE_NORMAL && lpTarget->Type == OBJECT_MONSTER)
		{
			return 0;
		}
	}

	if(lpTarget->Type == OBJECT_NPC || lpTarget->Live == 0 || lpTarget->State != OBJECT_PLAYING || lpTarget->Teleport != 0)
	{
		return 0;
	}

	if(lpTarget->Type == OBJECT_MONSTER)
	{
		if(lpTarget->Attribute == ATTRIBUTE_KALIMA)
		{
			return 0;
		}

		if((lpTarget->Class >= 100 && lpTarget->Class <= 110) || lpTarget->Class == 523) // Trap
		{
			return 0;
		}

		if(lpTarget->Class == 221 || lpTarget->Class == 222) // Siege
		{
			return 0;
		}
	}

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
	{
		if(OBJECT_RANGE(lpObj->SummonIndex) != 0)
		{
			if(lpObj->SummonIndex == lpTarget->Index)
			{
				return 0;
			}
		}
	}

	int SummonIndex = lpObj->Index;

	if(lpObj->Type == OBJECT_MONSTER && OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		SummonIndex = lpObj->SummonIndex;
	}

	int SummonTargetIndex = lpTarget->Index;

	if(lpTarget->Type == OBJECT_MONSTER && OBJECT_RANGE(lpTarget->SummonIndex) != 0)
	{
		SummonTargetIndex = lpTarget->SummonIndex;
	}

	if(this->CheckPlayerTarget(&gObj[SummonIndex],&gObj[SummonTargetIndex]) == 0)
	{
		return 0;
	}

	#pragma endregion

	#pragma region ATTACK_RETURN

	int skill = ((lpSkill==0)?SKILL_NONE:lpSkill->m_skill);

	if(gSkillEnabler->CheckUseSkill(lpObj,lpTarget,skill) == 0)
	{
		return 0;
	}

	if(damage == 0 && skill != SKILL_PLASMA_STORM && this->DecreaseArrow(lpObj) == 0)
	{
		return 0;
	}

	if(lpObj->Type == OBJECT_USER)
	{
		lpObj->HPAutoRecuperationTime = GetTickCount();
		lpObj->MPAutoRecuperationTime = GetTickCount();
		lpObj->BPAutoRecuperationTime = GetTickCount();
		lpObj->SDAutoRecuperationTime = GetTickCount();
	}

	if(lpTarget->Type == OBJECT_USER)
	{
		lpTarget->HPAutoRecuperationTime = GetTickCount();
		lpTarget->MPAutoRecuperationTime = GetTickCount();
		lpTarget->BPAutoRecuperationTime = GetTickCount();
		lpTarget->SDAutoRecuperationTime = GetTickCount();
	}

	if(OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonSetEnemy(lpObj,lpTarget->Index);
	}

	if(lpObj->Type == OBJECT_USER)
	{
		gDarkSpirit[lpObj->Index].SetTarget(lpTarget->Index);
	}

	bool duel = gDuel->CheckDuel(lpObj,lpTarget);

	if(lpObj->Type == OBJECT_USER && duel != 0)
	{
		lpObj->DuelTickCount = GetTickCount();
	}

	if(lpTarget->Type == OBJECT_USER && duel != 0)
	{
		lpTarget->DuelTickCount = GetTickCount();
	}

	if(lpObj->Authority == AUTHORITY_USER)
	{
		gEffectManager->DelEffect(lpObj,EFFECT_INVISIBILITY);
	}

	if(gEffectManager->CheckEffect(lpTarget,EFFECT_ORDER_OF_PROTECTION) != 0 && lpSkill == 0)
	{
		this->MissSend(lpObj,lpTarget,lpSkill,send,count);
		return 1;
	}

	if(gEffectManager->CheckEffect(lpTarget,EFFECT_ORDER_OF_PROTECTION) != 0 && lpSkill != 0)
	{
		this->MissSend(lpObj,lpTarget,lpSkill,send,count);
		return 1;
	}

	if(gEffectManager->CheckEffect(lpTarget,EFFECT_PHYSI_DAMAGE_IMMUNITY) != 0 && lpSkill == 0)
	{
		this->MissSend(lpObj,lpTarget,lpSkill,send,count);
		return 1;
	}

	if(gEffectManager->CheckEffect(lpTarget,EFFECT_MAGIC_DAMAGE_IMMUNITY) != 0 && lpSkill != 0)
	{
		this->MissSend(lpObj,lpTarget,lpSkill,send,count);
		return 1;
	}

	if(lpTarget->Type == OBJECT_MONSTER)
	{
		if(lpTarget->Class == 200 && lpSkill == 0)
		{
			gObjMonsterStateProc(lpTarget,6,lpObj->Index,0);
			this->MissSend(lpObj,lpTarget,lpSkill,send,count);
			return 1;
		}

		if(lpTarget->Class == 200 && lpSkill != 0)
		{
			gObjMonsterStateProc(lpTarget,7,lpObj->Index,0);
			this->MissSend(lpObj,lpTarget,lpSkill,send,count);
			return 1;
		}

		if(gEffectManager->CheckEffect(lpTarget,EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY) != 0 && skill == SKILL_NONE)
		{
			this->MissSend(lpObj,lpTarget,lpSkill,send,count);
			return 1;
		}

		if(gEffectManager->CheckEffect(lpTarget,EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY) != 0 && skill != SKILL_NONE)
		{
			this->MissSend(lpObj,lpTarget,lpSkill,send,count);
			return 1;
		}

		if(lpTarget->MonsterSkillElementOption.CheckImmuneTime() != 0)
		{
			if(lpTarget->MonsterSkillElementOption.m_SkillElementImmuneNumber == skill)
			{
				this->MissSend(lpObj,lpTarget,lpSkill,send,count);
				return 1;
			}
		}
	}

	if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].IsItem() != 0 && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Durability > 0 && damage == 0)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index == GET_ITEM(2,4) || lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index == GET_ITEM(2,5))
		{
			if(rand()%30 == 0)
			{
				static CSkill* lpSkill2 = new CSkill;

				skill = SKILL_ICE;

				lpSkill2->Set(skill,0);

				lpSkill = lpSkill2;

				send = 1;
			}
		}
	}

	#pragma endregion

	#pragma region DAMAGE_CALC

	flag = 0;

	BYTE miss = 0;

	WORD effect = 0;

	if(damage == 0)
	{
		if((lpObj->Type != OBJECT_USER || lpTarget->Type != OBJECT_USER) && this->MissCheck(lpObj,lpTarget,lpSkill,send,count,&miss) == 0)
		{
			return 1;
		}

		if((lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER) && this->MissCheckPvP(lpObj,lpTarget,lpSkill,send,count,&miss) == 0)
		{
			return 1;
		}

		int defense = this->GetTargetDefense(lpObj,lpTarget,&effect);

		if(skill == SKILL_PLASMA_STORM)
		{
			damage = this->GetAttackDamageFenrir(lpObj,lpTarget,lpSkill,&effect,defense);
		}
		else if(lpObj->Class == CLASS_SU && (skill == SKILL_SAHAMUTT || skill == SKILL_NEIL || skill == SKILL_GHOST_PHANTOM))
		{
			damage = this->GetAttackDamageCursed(lpObj,lpTarget,lpSkill,&effect,defense);
		}
		else if((lpObj->Class == CLASS_DW || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_SU) && skill != SKILL_NONE && skill != SKILL_FALLING_SLASH && skill != SKILL_LUNGE && skill != SKILL_UPPERCUT && skill != SKILL_CYCLONE && skill != SKILL_SLASH && skill != SKILL_TWISTING_SLASH && skill != SKILL_IMPALE && skill != SKILL_FIRE_SLASH && skill != SKILL_POWER_SLASH && skill != SKILL_SPIRAL_SLASH && skill != SKILL_SWORD_SLASH && skill != SKILL_BLOOD_STORM)
		{
			damage = this->GetAttackDamageWizard(lpObj,lpTarget,lpSkill,&effect,defense);
		}
		else
		{
			damage = this->GetAttackDamage(lpObj,lpTarget,lpSkill,&effect,defense);
		}

		if(damage > 0)
		{
			this->WeaponDurabilityDown(lpObj,lpTarget);
		}

		if(miss != 0)
		{
			damage = (damage*30)/100;
		}

		for(int n=0;n < MAX_DAMAGE_REDUCTION;n++)
		{
			damage -= (damage*lpTarget->DamageReduction[n])/100;
		}

		if(lpTarget->EffectOption.AddDamageReduction > 0)
		{
			damage -= (damage*lpTarget->EffectOption.AddDamageReduction)/100;
		}

		if((GetTickCount()-lpTarget->ShieldDamageReductionTime) < ((DWORD)(gServerInfo->m_DefenseTimeConstA*1000)))
		{
			damage -= (damage*lpTarget->ShieldDamageReduction)/100;
		}

		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
		{
			damage += lpObj->DamagePvP;
		}

		this->WingSprite(lpObj,lpTarget,&damage);

		this->HelperSprite(lpObj,lpTarget,&damage);

		int MinDamage = (lpObj->Level+lpObj->MasterLevel)/10;

		MinDamage = ((MinDamage<1)?1:MinDamage);

		damage = ((damage<MinDamage)?(MinDamage+(GetLargeRand()%MinDamage)):damage);

		this->DamageSprite(lpTarget,damage);

		if(skill == SKILL_FALLING_SLASH || skill == SKILL_LUNGE || skill == SKILL_UPPERCUT || skill == SKILL_CYCLONE || skill == SKILL_SLASH || skill == SKILL_TWISTING_SLASH || skill == SKILL_RAGEFUL_BLOW || skill == SKILL_DEATH_STAB || skill == SKILL_CRESCENT_MOON_SLASH || skill == SKILL_STAR_FALL || skill == SKILL_IMPALE || skill == SKILL_FIRE_BREATH || skill == SKILL_ICE_ARROW || skill == SKILL_PENETRATION || skill == SKILL_FIRE_SLASH || skill == SKILL_POWER_SLASH || skill == SKILL_SPIRAL_SLASH || skill == SKILL_FROZEN_STAB || skill == SKILL_SWORD_SLASH || skill == SKILL_CHARGE || skill == SKILL_BLOOD_STORM)
		{
			if(skill != SKILL_IMPALE || lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,2) || lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,3) || lpObj->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,37))
			{
				if(lpObj->Class == CLASS_DK)
				{
					damage = (damage*lpObj->DKDamageMultiplierRate)/100;
				}
				else
				{
					damage = (damage*200)/100;
				}
			}
		}
		else if(skill == SKILL_FORCE || skill == SKILL_FIRE_BURST || skill == SKILL_EARTHQUAKE || skill == SKILL_ELECTRIC_SPARK || skill == SKILL_FIRE_BLAST || skill == SKILL_FIRE_SCREAM || skill == SKILL_BIRDS)
		{
			damage = (damage*lpObj->DLDamageMultiplierRate)/100;
		}
		else if(skill == SKILL_PLASMA_STORM)
		{
			damage = (damage*(200+(((lpObj->Level>300)?((lpObj->Level-300)+lpObj->MasterLevel):0)/5)))/100;
		}
		else if(skill == SKILL_CHAIN_LIGHTNING)
		{
			damage = (damage*((count==2)?70:((count==3)?50:100)))/100;
		}
		else if(skill == SKILL_RAKLION_SELUPAN1)
		{
			damage = (damage*200)/100;
		}
		else if(skill == SKILL_RAKLION_SELUPAN2)
		{
			damage = (damage*220)/100;
		}
		else if(skill == SKILL_RAKLION_SELUPAN3)
		{
			damage = (damage*230)/100;
		}
		else if(skill == SKILL_RAKLION_SELUPAN4)
		{
			damage = (damage*250)/100;
		}
		else if(skill == SKILL_LARGE_RING_BLOWER || skill == SKILL_UPPER_BEAST || skill == SKILL_PHOENIX_SHOT)
		{
			damage = (damage*lpObj->RFDamageMultiplierRate[0])/100;
		}
		else if(skill == SKILL_CHAIN_DRIVER)
		{
			damage = (damage+((lpObj->Vitality+lpObj->AddVitality)/gServerInfo->m_RFDamageMultiplierConstB));
			damage = (damage*lpObj->RFDamageMultiplierRate[0])/100;
		}
		else if(skill == SKILL_DARK_SIDE)
		{
			damage = (damage+(((lpObj->Dexterity+lpObj->AddDexterity)/gServerInfo->m_RFDamageMultiplierConstA)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_RFDamageMultiplierConstC)));
			damage = (damage*lpObj->RFDamageMultiplierRate[2])/100;
		}
		else if(skill == SKILL_DRAGON_LORE)
		{
			damage = (damage+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_RFDamageMultiplierConstC));
			damage = (damage*lpObj->RFDamageMultiplierRate[1])/100;
		}
		else if(skill == SKILL_DRAGON_SLAYER)
		{
			damage = (damage*lpObj->RFDamageMultiplierRate[1])/100;
			damage = ((lpTarget->Type==OBJECT_USER)?((damage+100)*3):damage);
		}
		else if(skill == SKILL_CHARGE)
		{
			damage = (damage*(150+lpObj->RFDamageMultiplierRate[0]))/100;
		}

		if(count > 0 && (skill == SKILL_UPPER_BEAST || skill == SKILL_DARK_SIDE))
		{
			effect |= (((count%2)==0)?0x20:0x10);
		}

		if(count > 0 && (skill == SKILL_LARGE_RING_BLOWER || skill == SKILL_CHAIN_DRIVER || skill == SKILL_DRAGON_LORE || skill == SKILL_PHOENIX_SHOT))
		{
			effect |= (((count%4)==0)?0x20:0x10);
		}

		if((GetLargeRand()%100) < ((lpObj->DoubleDamageRate)-lpTarget->ResistDoubleDamageRate))
		{
			effect |= 0x40;
			damage += damage;
		}

		if((GetLargeRand()%100) < lpObj->TripleDamageRate)
		{
			effect |= 0x100;
			damage += damage+damage;
		}

		if(combo != 0)
		{
			effect |= 0x80;
			damage += damage;
			damage += (((lpObj->Strength+lpObj->AddStrength)+(lpObj->Dexterity+lpObj->AddDexterity)+(lpObj->Energy+lpObj->AddEnergy))/gServerInfo->m_ComboDamageConstA[lpObj->Class])*gServerInfo->m_ComboDamageConstB[lpObj->Class];
			damage = (damage*gServerInfo->m_ComboDamageConstC[lpObj->Class])/100;
			skill = SKILL_COMBO;
		}
	}
	else
	{
		if(skill != SKILL_EXPLOSION)
		{
			effect = 0x04;
			damage = (damage*((lpObj->Type==OBJECT_USER&&lpTarget->Type==OBJECT_USER)?gServerInfo->m_ReflectDamageRatePvP:gServerInfo->m_ReflectDamageRatePvM))/100;

			if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
			{
				damage = (damage*gServerInfo->m_ReflectDamageRateTo[lpObj->Class][lpTarget->Class])/100;
			}
		}
	}

	#pragma endregion

	#pragma region DAMAGE_CONFIG

	if(lpObj->Type == OBJECT_USER)
	{
		if(lpTarget->Type == OBJECT_USER)
		{
			damage = (damage*gServerInfo->m_GeneralDamageRatePvP)/100;

			damage = (damage*gServerInfo->m_DamageRatePvP[lpObj->Class])/100;

			damage = (damage*gServerInfo->m_DamageRateTo[lpObj->Class][lpTarget->Class])/100;

			if(gDuel->CheckDuel(lpObj,lpTarget) != 0)
			{
				damage = (damage*gServerInfo->m_DuelDamageRate)/100;
			}
			else if(gCustomArena->CheckMap(lpObj->Map) != 0 && gCustomArena->CheckMap(lpTarget->Map) != 0)
			{
				damage = (damage*gServerInfo->m_CustomArenaDamageRate)/100;
			}
			else if(CC_MAP_RANGE(lpObj->Map) != 0 && CC_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage*gServerInfo->m_ChaosCastleDamageRate)/100;
			}
			else if(IT_MAP_RANGE(lpObj->Map) != 0 && IT_MAP_RANGE(lpTarget->Map) != 0)
			{
				damage = (damage*gServerInfo->m_IllusionTempleDamageRate)/100;
			}
			else if(gCastleSiege->GetState() == CS_STATE_START && lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE)
			{
				if(lpObj->CastleJoinSide == 0 || lpTarget->CastleJoinSide == 0 || lpObj->CastleJoinSide != lpTarget->CastleJoinSide)
				{
					damage = (damage*gServerInfo->m_CastleSiegeDamageRate1)/100;
				}
				else
				{
					damage = (damage*gServerInfo->m_CastleSiegeDamageRate2)/100;
				}
			}
		}
		else
		{
			damage = (damage*gServerInfo->m_GeneralDamageRatePvM)/100;

			damage = (damage*gServerInfo->m_DamageRatePvM[lpObj->Class])/100;
		}
	}

	#pragma endregion

	#pragma region DAMAGE_FINISH

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
	{
		if(lpTarget->Class == 277 || lpTarget->Class == 283) // Castle Gate,Guardian Statue
		{
			if(gEffectManager->CheckEffect(lpObj,EFFECT_BLESS_POTION) != 0)
			{
				damage += (damage*20)/100;
			}

			damage = (damage*gServerInfo->m_CastleSiegeDamageRate3)/100;

			lpObj->AccumulatedDamage = ((lpObj->AccumulatedDamage>100)?0:(lpObj->AccumulatedDamage+damage));
		}
	}

	damage = ((damage<0)?0:damage);

	if(lpObj->Type == OBJECT_MONSTER && lpTarget->Type == OBJECT_USER)
	{
		damage = (damage*gDamageTable->GetMonsterDamage(lpTarget))/100;
	}

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_MONSTER)
	{
		damage = (damage*gDamageTable->GetPlayerDamage(lpObj,lpTarget))/100;
	}

	#pragma endregion

	#pragma region APPLY_EFFECT

	if(lpObj->Type == OBJECT_USER && effect != 4)
	{
		if((GetLargeRand()%100) < lpObj->OffensiveFullHPRestoreRate)
		{
			lpObj->Life = lpObj->MaxLife+lpObj->AddLife;
			GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);
		}

		if((GetLargeRand()%100) < lpObj->OffensiveFullSDRestoreRate)
		{
			lpObj->Shield = lpObj->MaxShield+lpObj->AddShield;
			GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);
		}

		if((GetLargeRand()%100) < lpObj->OffensiveFullMPRestoreRate)
		{
			lpObj->Mana = lpObj->MaxMana+lpObj->AddMana;
			GCManaSend(lpObj->Index,0xFF,(int)lpObj->Mana,lpObj->BP);
		}

		if((GetLargeRand()%100) < lpObj->OffensiveFullBPRestoreRate)
		{
			lpObj->BP = lpObj->MaxBP+lpObj->AddBP;
			GCManaSend(lpObj->Index,0xFF,(int)lpObj->Mana,lpObj->BP);
		}
	}

	if(lpSkill != 0 && count <= 1)
	{
		if(this->ApplySkillEffect(lpObj,lpTarget,lpSkill,damage) == 0)
		{
			if(send != 0)
			{
				gSkillManager->GCSkillAttackSend(lpObj,skill,lpTarget->Index,0);
			}
		}
		else
		{
			if(send != 0)
			{
				gSkillManager->GCSkillAttackSend(lpObj,skill,lpTarget->Index,1);
			}
		}
	}

	#pragma endregion

	#pragma region DAMAGE_APPLY

	int ShieldDamage = 0;

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		ShieldDamage = this->GetShieldDamage(lpObj,lpTarget,damage);

		if(lpTarget->Life < (damage-ShieldDamage))
		{
			lpTarget->Life = 0;
		}
		else
		{
			lpTarget->Life -= damage-ShieldDamage;
		}

		if(lpTarget->Shield < ShieldDamage)
		{
			lpTarget->Shield = 0;
		}
		else
		{
			lpTarget->Shield -= ShieldDamage;
		}
	}
	else
	{
		if(lpTarget->Life < damage)
		{
			lpTarget->Life = 0;
		}
		else
		{
			lpTarget->Life -= damage;
		}
	}

	if(lpTarget->Type == OBJECT_MONSTER)
	{
		lpTarget->LastAttackerID = lpObj->Index;

		gObjAddMsgSendDelay(lpTarget,0,lpObj->Index,100,0);

		if(lpTarget->CurrentAI != 0)
		{
			lpTarget->Agro.IncAgro(lpObj->Index,(damage/50));
		}
	}

	#pragma endregion

	#pragma region CHECK_SELF_DEFENSE

	if(damage > 0)
	{
		if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER && lpObj->Index != lpTarget->Index)
		{
			bool CheckSelfDefense = 1;

			if(effect == 4)
			{
				CheckSelfDefense = 0;
			}

			if(gDuel->CheckDuel(lpObj,lpTarget) != 0)
			{
				CheckSelfDefense = 0;
			}

			if(gGuild->GetUnionType(lpObj,lpTarget) == GUILD_RELATION_RIVAL)
			{
				CheckSelfDefense = 0;
			}

			if(gGuild->CheckWar(lpObj,lpTarget) != 0)
			{
				CheckSelfDefense = 0;
			}

			if(lpObj->Map == MAP_CASTLE_SIEGE && gCastleSiege->GetState() == CS_STATE_START && lpObj->CastleJoinSide != 0)
			{
				CheckSelfDefense = 0;
			}

			if(gCustomArena->CheckMap(lpObj->Map) != 0 && gCustomArena->CheckMap(lpTarget->Map) != 0)
			{
				CheckSelfDefense = 0;
			}

			if(CC_MAP_RANGE(lpObj->Map) != 0 && CC_MAP_RANGE(lpTarget->Map) != 0)
			{
				CheckSelfDefense = 0;
			}

			if(IT_MAP_RANGE(lpObj->Map) != 0 && IT_MAP_RANGE(lpTarget->Map) != 0)
			{
				CheckSelfDefense = 0;
			}

			if(gMapManager->GetMapNonOutlaw(lpObj->Map) != 0)
			{
				CheckSelfDefense = 0;
			}

			if(CheckSelfDefense != 0)
			{
				gObjCheckSelfDefense(lpObj,lpTarget->Index);
			}
		}

		this->ArmorDurabilityDown(lpObj,lpTarget);
	}

	lpObj->Rest = 0;

	#pragma endregion

	#pragma region ATTACK_FINISH

	if(damage > 0)
	{
		gEffectManager->DelEffect(lpTarget,EFFECT_SLEEP);

		if(lpTarget->Type == OBJECT_USER)
		{
			if(effect != 4)
			{
				if((GetLargeRand()%100) < lpTarget->FullDamageReflectRate)
				{
					gObjAddMsgSendDelay(lpTarget,10,lpObj->Index,10,damage);
				}
				else if((lpTarget->DamageReflect+lpTarget->EffectOption.AddDamageReflect) > 0)
				{
					gObjAddMsgSendDelay(lpTarget,10,lpObj->Index,10,((damage*(lpTarget->DamageReflect+lpTarget->EffectOption.AddDamageReflect))/100));
				}

				if((GetLargeRand()%100) < lpTarget->DefensiveFullHPRestoreRate)
				{
					lpTarget->Life = lpTarget->MaxLife+lpTarget->AddLife;
					GCLifeSend(lpTarget->Index,0xFF,(int)lpTarget->Life,lpTarget->Shield);
				}

				if((GetLargeRand()%100) < lpTarget->DefensiveFullSDRestoreRate)
				{
					lpTarget->Shield = lpTarget->MaxShield+lpTarget->AddShield;
					GCLifeSend(lpTarget->Index,0xFF,(int)lpTarget->Life,lpTarget->Shield);
				}

				if((GetLargeRand()%100) < lpTarget->DefensiveFullMPRestoreRate)
				{
					lpTarget->Mana = lpTarget->MaxMana+lpTarget->AddMana;
					GCManaSend(lpTarget->Index,0xFF,(int)lpTarget->Mana,lpTarget->BP);
				}

				if((GetLargeRand()%100) < lpTarget->DefensiveFullBPRestoreRate)
				{
					lpTarget->BP = lpTarget->MaxBP+lpTarget->AddBP;
					GCManaSend(lpTarget->Index,0xFF,(int)lpTarget->Mana,lpTarget->BP);
				}
			}
			
			int effect = 1;

			if(lpTarget->Inventory[INVENTORY_SLOT_HELPER].IsItem() != 0 && lpTarget->Inventory[INVENTORY_SLOT_HELPER].m_Durability > 0)
			{
				if(lpTarget->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,2) && gServerInfo->m_DamageStuckOnPetUniria == 0) // Uniria
				{
					effect = 0;
				}
				else if(lpTarget->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,3) && gServerInfo->m_DamageStuckOnPetDinorant == 0) // Dinorant
				{
					effect = 0;
				}
				else if(lpTarget->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,4) && gServerInfo->m_DamageStuckOnPetDarkHorse == 0) // Dark Horse
				{
					effect = 0;
				}
				else if(lpTarget->Inventory[INVENTORY_SLOT_HELPER].m_Index == GET_ITEM(13,37) && gServerInfo->m_DamageStuckOnPetFenrir == 0) // Fenrir
				{
					effect = 0;
				}
			}

			if(effect != 0)
			{
				if((GetLargeRand()%100) < gServerInfo->m_DamageStuckRate[lpTarget->Class])
				{
					flag = 1;
				}
			}
		}

		gObjectManager->CharacterLifeCheck(lpObj,lpTarget,(damage-ShieldDamage),0,flag,effect,skill,ShieldDamage,combo);
	}
	else
	{
		GCDamageSend(lpObj->Index,lpTarget->Index,0,0,effect,0);
	}

	if(lpObj->Type == OBJECT_USER && lpObj->Life <= 0 && lpObj->CheckLifeTime <= 0)
	{
		lpObj->AttackObj = lpTarget;
		lpObj->AttackerKilled = ((lpTarget->Type==OBJECT_USER)?1:0);
		lpObj->CheckLifeTime = 3;
	}

	#pragma endregion

	return 1;
}

bool CAttack::AttackElemental(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,bool send,BYTE flag,int damage,int count,bool combo) // OK
{
	return 0;
}

bool CAttack::DecreaseArrow(LPOBJ lpObj) // OK
{
	if(lpObj->Type != OBJECT_USER || lpObj->Class != CLASS_FE)
	{
		return 1;
	}

	if(gEffectManager->CheckEffect(lpObj,EFFECT_INFINITY_ARROW) != 0 || gEffectManager->CheckEffect(lpObj,EFFECT_INFINITY_ARROW_IMPROVED) != 0)
	{
		return 1;
	}

	if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index >= GET_ITEM(4,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index < GET_ITEM(5,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index != GET_ITEM(4,15) && lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Slot == 0)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index != GET_ITEM(4,7) || lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Durability < 1)
		{
			return 0;
		}
		else
		{
			if(gServerInfo->m_InfinityArrowSwitch[lpObj->AccountLevel] == 0)
			{
				gItemManager->DecreaseItemDur(lpObj,1,1);
			}
		}
	}

	if(lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index >= GET_ITEM(4,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index < GET_ITEM(5,0) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Index != GET_ITEM(4,7) && lpObj->Inventory[INVENTORY_SLOT_WEAPON2].m_Slot == 1)
	{
		if(lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Index != GET_ITEM(4,15) || lpObj->Inventory[INVENTORY_SLOT_WEAPON1].m_Durability < 1)
		{
			return 0;
		}
		else
		{
			if(gServerInfo->m_InfinityArrowSwitch[lpObj->AccountLevel] == 0)
			{
				gItemManager->DecreaseItemDur(lpObj,0,1);
			}
		}
	}

	return 1;
}

void CAttack::WingSprite(LPOBJ lpObj,LPOBJ lpTarget,int* damage) // OK
{
	if(lpObj != 0 && lpObj->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpObj->Inventory[INVENTORY_SLOT_WING];

		if(lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if(lpObj->Class == CLASS_DW || lpObj->Class == CLASS_FE || lpObj->Class == CLASS_SU)
			{
				lpObj->Life -= 1;
			}
			else
			{
				lpObj->Life -= 3;
			}

			GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);
			
			if(gWingOption->CheckWingByItem(lpItem->m_Index) != 0)
			{
				(*damage) = ((*damage)*gWingOption->GetWingIncDamage(lpItem->m_Index,lpItem->m_Level))/100;
			}
			else if(gCustomWing->CheckCustomWingByItem(lpItem->m_Index) != 0)
			{
				(*damage) = ((*damage)*gCustomWing->GetCustomWingIncDamage(lpItem->m_Index,lpItem->m_Level))/100;
			}
		}
	}

	if(lpTarget != 0 && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[INVENTORY_SLOT_WING];

		if(lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if(gWingOption->CheckWingByItem(lpItem->m_Index) != 0)
			{
				(*damage) = ((*damage)*gWingOption->GetWingDecDamage(lpItem->m_Index,lpItem->m_Level))/100;
			}
			else if(gCustomWing->CheckCustomWingByItem(lpItem->m_Index) != 0)
			{
				(*damage) = ((*damage)*gCustomWing->GetCustomWingDecDamage(lpItem->m_Index,lpItem->m_Level))/100;
			}
		}
	}
}

void CAttack::HelperSprite(LPOBJ lpObj,LPOBJ lpTarget,int* damage) // OK
{
	if(lpObj != 0 && lpObj->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpObj->Inventory[INVENTORY_SLOT_HELPER];

		if(lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if(lpItem->m_Index == GET_ITEM(13,1)) // Satan
			{
				lpObj->Life -= 3;

				GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);

				(*damage) = ((*damage)*(100+gServerInfo->m_SatanIncDamageConstA))/100;
			}
			else if(lpItem->m_Index == GET_ITEM(13,3)) // Dinorant
			{
				lpObj->Life -= 1;

				GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);

				(*damage) = ((*damage)*(100+gServerInfo->m_DinorantIncDamageConstA))/100;
			}
			else if(lpItem->m_Index == GET_ITEM(13,37)) // Fenrir
			{
				if((lpItem->m_NewOption & 1) != 0)
				{
					(*damage) = ((*damage)*(100+gServerInfo->m_BlackFenrirIncDamageConstA))/100;
				}
			}
			else if(lpItem->m_Index == GET_ITEM(13,64)) // Demon
			{
				lpObj->Life -= 4;

				GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);

				(*damage) = ((*damage)*(100+gServerInfo->m_DemonIncDamageConstA))/100;
			}
			else if(lpItem->m_Index == GET_ITEM(13,123)) // Skeleton
			{
				lpObj->Life -= 2;

				GCLifeSend(lpObj->Index,0xFF,(int)lpObj->Life,lpObj->Shield);

				(*damage) = ((*damage)*(100+gServerInfo->m_SkeletonIncDamageConstA))/100;
			}
		}
	}

	if(lpTarget != 0 && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[INVENTORY_SLOT_HELPER];

		if(lpItem->IsItem() != 0 && lpItem->m_Durability > 0)
		{
			if(lpItem->m_Index == GET_ITEM(13,0)) // Angel
			{
				(*damage) = ((*damage)*(100-gServerInfo->m_AngelDecDamageConstA))/100;
			}
			else if(lpItem->m_Index == GET_ITEM(13,3)) // Dinorant
			{
				(*damage) = ((*damage)*(100-gServerInfo->m_DinorantDecDamageConstA-(((lpItem->m_NewOption & 1)==0)?0:gServerInfo->m_DinorantDecDamageConstB)))/100;
			}
			else if(lpItem->m_Index == GET_ITEM(13,4)) // Dark Horse
			{
				(*damage) = ((*damage)*(100-((gServerInfo->m_DarkHorseDecDamageConstA+lpItem->m_PetItemLevel)/gServerInfo->m_DarkHorseDecDamageConstB)))/100;
			}
			else if(lpItem->m_Index == GET_ITEM(13,37)) // Fenrir
			{
				if((lpItem->m_NewOption & 2) != 0)
				{
					(*damage) = ((*damage)*(100-gServerInfo->m_BlueFenrirDecDamageConstA))/100;
				}
			}
			else if(lpItem->m_Index == GET_ITEM(13,65)) // Guardian Spirit
			{
				(*damage) = ((*damage)*(100-gServerInfo->m_GuardianSpiritDecDamageConstA))/100;
			}
		}
	}
}

void CAttack::DamageSprite(LPOBJ lpObj,int damage) // OK
{
	if(lpObj->Type != OBJECT_USER)
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[INVENTORY_SLOT_HELPER];

	if(lpItem->IsItem() == 0 || lpItem->m_IsPeriodicItem != 0)
	{
		return;
	}

	float DurabilityValue = (1.0f/gServerInfo->m_GuardianDurabilityRate)*100;

	DurabilityValue = (DurabilityValue/lpObj->GuardianDurabilityRate)*100;

	if(lpItem->m_Index == GET_ITEM(13,0)) // Angel
	{
		gPetDamageTable->GetPetDamage(lpObj,0,&damage);
		lpItem->m_Durability -= (damage*(3.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,1)) // Satan
	{
		gPetDamageTable->GetPetDamage(lpObj,1,&damage);
		lpItem->m_Durability -= (damage*(2.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,2)) // Uniria
	{
		gPetDamageTable->GetPetDamage(lpObj,2,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,3)) // Dinorant
	{
		gPetDamageTable->GetPetDamage(lpObj,3,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,4)) // Dark Horse
	{
		if(this->DarkHorseSprite(lpObj,damage) == 0)
		{
			return;
		}
	}
	else if(lpItem->m_Index == GET_ITEM(13,37)) // Fenrir
	{
		if(this->FenrirSprite(lpObj,damage) == 0)
		{
			return;
		}
	}
	else if(lpItem->m_Index == GET_ITEM(13,64)) // Demon
	{
		gPetDamageTable->GetPetDamage(lpObj,7,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,65)) // Maria
	{
		gPetDamageTable->GetPetDamage(lpObj,8,&damage);
		lpItem->m_Durability -= (damage*(2.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,67)) // Rudolf
	{
		gPetDamageTable->GetPetDamage(lpObj,9,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,80)) // Panda
	{
		gPetDamageTable->GetPetDamage(lpObj,10,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,106)) // Unicorn
	{
		gPetDamageTable->GetPetDamage(lpObj,11,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else if(lpItem->m_Index == GET_ITEM(13,123)) // Skeleton
	{
		gPetDamageTable->GetPetDamage(lpObj,12,&damage);
		lpItem->m_Durability -= (damage*(1.0f*DurabilityValue))/100;
	}
	else
	{
		return;
	}

	gItemManager->GCItemDurSend(lpObj->Index,8,(BYTE)lpItem->m_Durability,0);

	if(lpItem->m_Durability < 1)
	{
		gItemManager->InventoryDelItem(lpObj->Index,8);

		gItemManager->GCItemDeleteSend(lpObj->Index,8,0);

		gObjectManager->CharacterMakePreviewCharSet(lpObj->Index);

		gItemManager->GCItemChangeSend(lpObj->Index,8);

		if(lpObj->Map == MAP_ICARUS && lpObj->Inventory[INVENTORY_SLOT_WING].IsItem() == 0 && (lpItem->m_Index == GET_ITEM(13,3) || lpItem->m_Index == GET_ITEM(13,37)))
		{
			gObjMoveGate(lpObj->Index,22);
		}
	}
}

bool CAttack::DarkHorseSprite(LPOBJ lpObj,int damage) // OK
{
	CItem* lpItem = &lpObj->Inventory[INVENTORY_SLOT_HELPER];

	if(lpItem->m_Durability < 1)
	{
		return 0;
	}

	gPetDamageTable->GetPetDamage(lpObj,4,&damage);

	lpItem->m_DurabilitySmall += ((damage*2)/100)+1;

	int MaxSmallDur = (1500*gServerInfo->m_PetDurabilityRate)/100;

	MaxSmallDur = (MaxSmallDur*lpObj->PetDurabilityRate)/100;

	if(lpItem->m_DurabilitySmall > MaxSmallDur)
	{
		lpItem->m_Durability = (((--lpItem->m_Durability)<1)?0:lpItem->m_Durability);
		lpItem->m_DurabilitySmall = 0;

		if(lpItem->CheckDurabilityState() != 0)
		{
			gObjectManager->CharacterCalcAttribute(lpObj->Index);
		}

		gItemManager->GCItemDurSend(lpObj->Index,8,(BYTE)lpItem->m_Durability,0);

		GCPetItemInfoSend(lpObj->Index,1,0,8,lpItem->m_PetItemLevel,lpItem->m_PetItemExp,(BYTE)lpItem->m_Durability);
	}

	return 0;
}

bool CAttack::FenrirSprite(LPOBJ lpObj,int damage) // OK
{
	CItem* lpItem = &lpObj->Inventory[INVENTORY_SLOT_HELPER];

	if(lpItem->m_Durability < 1)
	{
		return 0;
	}

	gPetDamageTable->GetPetDamage(lpObj,6,&damage);

	lpItem->m_DurabilitySmall += ((damage*2)/100)+1;

	int MaxSmallDur = (200*gServerInfo->m_GuardianDurabilityRate)/100;

	MaxSmallDur = (MaxSmallDur*lpObj->GuardianDurabilityRate)/100;

	if(lpItem->m_DurabilitySmall > MaxSmallDur)
	{
		lpItem->m_Durability = (((--lpItem->m_Durability)<1)?0:lpItem->m_Durability);
		lpItem->m_DurabilitySmall = 0;

		if(lpItem->CheckDurabilityState() != 0)
		{
			gObjectManager->CharacterCalcAttribute(lpObj->Index);
		}

		gItemManager->GCItemDurSend(lpObj->Index,8,(BYTE)lpItem->m_Durability,0);
	}

	return 1;
}

void CAttack::WeaponDurabilityDown(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(lpObj->Type != OBJECT_USER)
	{
		return;
	}

	if(gEffectManager->GetEffect(lpObj,EFFECT_TALISMAN_OF_PROTECTION) != 0)
	{
		return;
	}

	for(int n=0;n < 2;n++)
	{
		if(lpObj->Inventory[n].IsItem() != 0)
		{
			bool result = 0;

			switch((lpObj->Inventory[n].m_Index/MAX_ITEM_TYPE))
			{
				case 0:
					result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index,lpTarget->Defense,0);
					break;
				case 1:
					result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index,lpTarget->Defense,0);
					break;
				case 2:
					result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index,lpTarget->Defense,0);
					break;
				case 3:
					result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index,lpTarget->Defense,0);
					break;
				case 4:
					result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index,lpTarget->Defense,1);
					break;
				case 5:
					result = lpObj->Inventory[n].WeaponDurabilityDown(lpObj->Index,lpTarget->Defense,((lpObj->Inventory[n].m_Slot==0)?2:3));
					break;
			}

			if(result != 0)
			{
				gItemManager->GCItemDurSend(lpObj->Index,n,(BYTE)lpObj->Inventory[n].m_Durability,0);
			}
		}
	}
}

void CAttack::ArmorDurabilityDown(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(lpTarget->Type != OBJECT_USER)
	{
		return;
	}

	if(gEffectManager->GetEffect(lpTarget,EFFECT_TALISMAN_OF_PROTECTION) != 0)
	{
		return;
	}

	int slot = 1+(GetLargeRand()%6);

	if(lpTarget->Inventory[slot].IsItem() != 0)
	{
		if(slot != 1 || (lpTarget->Inventory[slot].m_Index >= GET_ITEM(6,0) && lpTarget->Inventory[slot].m_Index < GET_ITEM(7,0)))
		{
			if(lpTarget->Inventory[slot].ArmorDurabilityDown(lpTarget->Index,lpObj->PhysiDamageMin) != 0)
			{
				gItemManager->GCItemDurSend(lpTarget->Index,slot,(BYTE)lpTarget->Inventory[slot].m_Durability,0);
			}
		}
	}
}

bool CAttack::CheckPlayerTarget(LPOBJ lpObj,LPOBJ lpTarget) // OK
{
	if(lpObj->Type != OBJECT_USER || lpTarget->Type != OBJECT_USER)
	{
		return 1;
	}
	
	if(gScriptLoader->OnCheckUserTarget(lpObj->Index,lpTarget->Index) == 0)
	{
		return 0;
	}

	if(lpTarget->Authority == AUTHORITY_ADMINISTRATOR)
	{
		return 0;
	}

	if(gServerInfo->m_PartyDisableKillBetweenMembers != 0 && gParty->CheckSameParty(lpObj,lpTarget) != 0)
	{
		return 0;
	}

	if(lpObj->Helper.Started != 0 && gParty->IsMember(lpObj->PartyNumber,lpTarget->Index) != 0)
	{
		return 0;
	}

	if(gGuild->GetUnionType(lpObj,lpTarget) == GUILD_RELATION_RIVAL && gMapManager->GetMapNonPK(lpTarget->Map) == 0)
	{
		return 1;
	}

	if(lpObj->Guild != 0 && lpTarget->Guild != 0)
	{
		if(lpObj->Guild->WarState != 0 && lpTarget->Guild->WarState != 0)
		{
			if(lpObj->Guild->Index == lpTarget->Guild->Index)
			{
				return 0;
			}
		}
	}

	if(gGuild->CheckWar(lpObj,lpTarget) == 0)
	{
		if(lpTarget->Guild != 0 && lpTarget->Guild->WarState != 0)
		{
			if(lpTarget->Guild->WarType == GUILD_WAR_TYPE_SOCCER && lpTarget->Map != MAP_ARENA && gMapManager->GetMapNonPK(lpTarget->Map) == 0)
			{
				return 1;
			}

			if(gCustomArena->CheckMap(lpTarget->Map) == 0 && CC_MAP_RANGE(lpTarget->Map) == 0 && IT_MAP_RANGE(lpTarget->Map) == 0 && gDuel->CheckDuel(lpObj,lpTarget) == 0)
			{
				if(gServerInfo->m_GuildWarAttackEnable == 0)
				{
					return 0;
				}
			}
		}
	}

	if(gCustomArena->CheckMap(lpObj->Map) != 0 && gCustomArena->CheckMap(lpTarget->Map) != 0)
	{
		return ((gCustomArena->CheckPlayerTarget(lpObj,lpTarget)==0)?0:1);
	}

	if(DS_MAP_RANGE(lpObj->Map) != 0 && DS_MAP_RANGE(lpTarget->Map) != 0)
	{
		return 0;
	}

	if(BC_MAP_RANGE(lpObj->Map) != 0 && BC_MAP_RANGE(lpTarget->Map) != 0)
	{
		return 0;
	}

	if(CC_MAP_RANGE(lpObj->Map) != 0 && CC_MAP_RANGE(lpTarget->Map) != 0)
	{
		return ((gChaosCastle->GetState(GET_CC_LEVEL(lpObj->Map))==CC_STATE_START)?1:0);
	}

	if(lpObj->Map == MAP_CASTLE_SIEGE && lpTarget->Map == MAP_CASTLE_SIEGE)
	{
		if(gCastleSiegeSync->GetState() == CS_STATE_START)
		{
			if(lpObj->CastleJoinSide != 0 && lpTarget->CastleJoinSide != 0)
			{
				if(gServerInfo->m_CastleSiegeDamageRate2 == 0 && lpObj->CastleJoinSide == lpTarget->CastleJoinSide)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
		}
	}

	if(lpObj->Map == MAP_KANTURU3 && lpTarget->Map == MAP_KANTURU3)
	{
		return 0;
	}

	if(IT_MAP_RANGE(lpObj->Map) != 0 && IT_MAP_RANGE(lpTarget->Map) != 0)
	{
		return gIllusionTemple->CheckPlayerTarget(lpObj,lpTarget);
	}

	if(lpObj->Level <= 5 || lpTarget->Level <= 5)
	{
		return 0;
	}

	if(gMapManager->GetMapNonPK(lpTarget->Map) != 0)
	{
		return 0;
	}

	return 1;
}

void CAttack::MissSend(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int send,int count) // OK
{
	WORD effect = 0;

	if(count > 0 && lpSkill != 0 && (lpSkill->m_skill == SKILL_UPPER_BEAST || lpSkill->m_skill == SKILL_DARK_SIDE))
	{
		effect |= (((count%2)==0)?0x20:0x10);
	}

	if(count > 0 && lpSkill != 0 && (lpSkill->m_skill == SKILL_LARGE_RING_BLOWER || lpSkill->m_skill == SKILL_CHAIN_DRIVER || lpSkill->m_skill == SKILL_DRAGON_LORE || lpSkill->m_skill == SKILL_PHOENIX_SHOT))
	{
		effect |= (((count%4)==0)?0x20:0x10);
	}

	GCDamageSend(lpObj->Index,lpTarget->Index,0,0,effect,0);

	if(send != 0 && lpSkill != 0)
	{
		gSkillManager->GCSkillAttackSend(lpObj,lpSkill->m_index,lpTarget->Index,0);
	}
}

bool CAttack::MissCheck(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int send,int count,BYTE* miss) // OK
{
	int AttackSuccessRate = lpObj->AttackSuccessRate;

	AttackSuccessRate += lpObj->EffectOption.AddAttackSuccessRate;

	AttackSuccessRate += (AttackSuccessRate*lpObj->EffectOption.MulAttackSuccessRate)/100;

	AttackSuccessRate -= (AttackSuccessRate*lpObj->EffectOption.DivAttackSuccessRate)/100;

	AttackSuccessRate = ((AttackSuccessRate<0)?0:AttackSuccessRate);

	int DefenseSuccessRate = lpTarget->DefenseSuccessRate;

	DefenseSuccessRate += lpTarget->EffectOption.AddDefenseSuccessRate;

	DefenseSuccessRate += (DefenseSuccessRate*lpTarget->EffectOption.MulDefenseSuccessRate)/100;

	DefenseSuccessRate -= (DefenseSuccessRate*lpTarget->EffectOption.DivDefenseSuccessRate)/100;

	DefenseSuccessRate = ((DefenseSuccessRate<0)?0:DefenseSuccessRate);

	if(AttackSuccessRate < DefenseSuccessRate)
	{
		(*miss) = 1;

		if((GetLargeRand()%100) >= 5)
		{
			this->MissSend(lpObj,lpTarget,lpSkill,send,count);
			return 0;
		}
	}
	else
	{
		(*miss) = 0;

		if((GetLargeRand()%((AttackSuccessRate==0)?1:AttackSuccessRate)) < DefenseSuccessRate)
		{
			this->MissSend(lpObj,lpTarget,lpSkill,send,count);
			return 0;
		}
	}

	return 1;
}

bool CAttack::MissCheckPvP(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int send,int count,BYTE* miss) // OK
{
	(*miss) = 0;

	int AttackSuccessRate = (int)(((100*(((lpObj->AttackSuccessRatePvP*10000.0f)/(lpObj->AttackSuccessRatePvP+lpTarget->DefenseSuccessRatePvP))/10000.0f))*gServerInfo->m_ShieldGaugeAttackRate)*(((lpObj->Level*10000.0f)/(lpObj->Level+lpTarget->Level))/10000.0f));

	if((lpTarget->Level-lpObj->Level) >= 100 && (lpTarget->Level-lpObj->Level) < 199)
	{
		AttackSuccessRate -= 5;
	}
	else if((lpTarget->Level-lpObj->Level) >= 200 && (lpTarget->Level-lpObj->Level) < 299)
	{
		AttackSuccessRate -= 10;
	}
	else if((lpTarget->Level-lpObj->Level) >= 300)
	{
		AttackSuccessRate -= 15;
	}

	if((GetLargeRand()%100) > AttackSuccessRate)
	{
		(*miss) = 1;

		this->MissSend(lpObj,lpTarget,lpSkill,send,count);

		if(gServerInfo->m_ShieldGaugeAttackComboMiss != 0 && lpObj->ComboSkill.m_index >= 0)
		{
			lpObj->ComboSkill.Init();
		}

		return 0;
	}

	return 1;
}

bool CAttack::MissCheckElemental(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int send,int count,BYTE* miss) // OK
{
	return 0;
}

bool CAttack::ApplySkillEffect(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,int damage) // OK
{
	if(lpTarget->Type != OBJECT_USER && ((lpTarget->Class >= 204 && lpTarget->Class <= 209) || (lpTarget->Class >= 215 && lpTarget->Class <= 219) || lpTarget->Class == 277 || lpTarget->Class == 278 || lpTarget->Class == 283 || lpTarget->Class == 288))
	{
		return 0;
	}

	if(lpTarget->Type == OBJECT_USER && (gEffectManager->CheckEffect(lpTarget,EFFECT_IRON_DEFENSE) != 0 || gEffectManager->CheckEffect(lpTarget,EFFECT_IRON_DEFENSE_IMPROVED) != 0))
	{
		return 0;
	}

	if(gSkillManager->GetSkillType(lpSkill->m_index) != -1 && gObjCheckResistance(lpTarget,gSkillManager->GetSkillType(lpSkill->m_index)) != 0)
	{
		return 0;
	}

	if(gSkillDamage->GetEffectRate(lpSkill) != 0)
	{
		return 0;
	}

	switch(lpSkill->m_skill)
	{
		case SKILL_POISON:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),gServerInfo->m_PoisonEffectTime,lpObj->Index,gServerInfo->m_PoisonEffectConstA,gServerInfo->m_PoisonEffectConstB,0);
			break;
		case SKILL_METEORITE:
			gSkillManager->ApplyMeteoriteEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_LIGHTNING:
			gObjAddMsgSendDelay(lpTarget,2,lpObj->Index,150,0);
			break;
		case SKILL_ICE:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),gServerInfo->m_IceEffectTime,lpObj->Index,0,0,0);
			break;
		case SKILL_FALLING_SLASH:
			gObjAddMsgSendDelay(lpTarget,2,lpObj->Index,150,0);
			break;
		case SKILL_LUNGE:
			gObjAddMsgSendDelay(lpTarget,2,lpObj->Index,150,0);
			break;
		case SKILL_UPPERCUT:
			gObjAddMsgSendDelay(lpTarget,2,lpObj->Index,150,0);
			break;
		case SKILL_CYCLONE:
			gObjAddMsgSendDelay(lpTarget,2,lpObj->Index,150,0);
			break;
		case SKILL_SLASH:
			gObjAddMsgSendDelay(lpTarget,2,lpObj->Index,150,0);
			break;
		case SKILL_DECAY:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),gServerInfo->m_DecayEffectTime,lpObj->Index,gServerInfo->m_DecayEffectConstA,gServerInfo->m_DecayEffectConstB,0);
			break;
		case SKILL_ICE_STORM:
			gSkillManager->ApplyIceStormEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_ICE_ARROW:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),gServerInfo->m_IceArrowEffectTime,0,0,0,0);
			break;
		case SKILL_TWISTING_SLASH:
			gSkillManager->ApplyTwistingSlashEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_RAGEFUL_BLOW:
			gSkillManager->ApplyRagefulBlowEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_DEATH_STAB:
			gSkillManager->ApplyDeathStabEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_FIRE_SLASH:
			gSkillManager->ApplyFireSlashEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_FIRE_BURST:
			gSkillManager->ApplyFireBurstEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_PLASMA_STORM:
			gSkillManager->ApplyPlasmaStormEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_FIRE_SCREAM:
			gSkillManager->ApplyFireScreamEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_EARTHQUAKE:
			gSkillManager->ApplyEarthquakeEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_DRAIN_LIFE:
			gSkillManager->ApplyDrainLifeEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_SAHAMUTT:
			gSkillManager->ApplySahamuttEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_NEIL:
			gSkillManager->ApplyNeilEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_GHOST_PHANTOM:
			gSkillManager->ApplyGhostPhantomEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_RED_STORM:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),1,0,0,0,0);
			break;
		case SKILL_FROZEN_STAB:
			gSkillManager->ApplyFrozenStabEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_FIVE_SHOT:
			gSkillManager->ApplyFiveShotEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_SWORD_SLASH:
			gSkillManager->ApplySwordSlashEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_LIGHTNING_STORM:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),1,0,0,0,0);
			break;
		case SKILL_LARGE_RING_BLOWER:
			gSkillManager->ApplyLargeRingBlowerEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_UPPER_BEAST:
			gSkillManager->ApplyUpperBeastEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_CHAIN_DRIVER:
			gSkillManager->ApplyChainDriverEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_DRAGON_LORE:
			gSkillManager->ApplyDragonLoreEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_DRAGON_SLAYER:
			gSkillManager->ApplyDragonSlayerEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_PHOENIX_SHOT:
			gSkillManager->ApplyPhoenixShotEffect(lpObj,lpTarget,lpSkill,damage);
			break;
		case SKILL_POISON_ARROW:
			gEffectManager->AddEffect(lpTarget,0,gSkillManager->GetSkillEffect(lpSkill->m_index),gServerInfo->m_PoisonArrowEffectTime,lpObj->Index,gServerInfo->m_PoisonArrowEffectConstA,gServerInfo->m_PoisonArrowEffectConstB,0);
			break;
		case SKILL_EARTH_PRISON:
			gSkillManager->ApplyEarthPrisonEffect(lpObj,lpTarget,lpSkill,damage);
			break;
	}

	return 1;
}

int CAttack::GetTargetDefense(LPOBJ lpObj,LPOBJ lpTarget,WORD* effect) // OK
{
	int defense = lpTarget->Defense;

	defense += lpTarget->EffectOption.AddDefense;
	defense -= lpTarget->EffectOption.SubDefense;

	gSkillManager->SkillSwordPowerGetDefense(lpObj->Index,&defense);

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		defense += lpTarget->DefensePvP;
	}

	if(lpTarget->MonsterSkillElementOption.CheckDefenseTime() != 0)
	{
		defense += lpTarget->MonsterSkillElementOption.m_SkillElementDefense;
	}

	if(lpTarget->EffectOption.MulDefense > 0)
	{
		defense += (defense*lpTarget->EffectOption.MulDefense)/100;
	}

	if(lpTarget->EffectOption.DivDefense > 0)
	{
		defense -= (defense*lpTarget->EffectOption.DivDefense)/100;
	}

	if(lpTarget->Type == OBJECT_USER)
	{
		defense = (defense*50)/100;
	}

	if((GetLargeRand()%100) < ((lpObj->IgnoreDefenseRate+lpObj->EffectOption.AddIgnoreDefenseRate)-lpTarget->ResistIgnoreDefenseRate))
	{
		(*effect) = 1;

		defense = 0;
	}

	defense = ((defense<0)?0:defense);

	return defense;
}

int CAttack::GetTargetElementalDefense(LPOBJ lpObj,LPOBJ lpTarget,WORD* effect) // OK
{
	return 0;
}

int CAttack::GetAttackDamage(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,WORD* effect,int TargetDefense) // OK
{
	CItem* Right = &lpObj->Inventory[INVENTORY_SLOT_WEAPON1];
	CItem* Left = &lpObj->Inventory[INVENTORY_SLOT_WEAPON2];

	int damage = 0;
	int DamageMin = 0;
	int DamageMax = 0;
	int SkillDamageMin = 0;
	int SkillDamageMax = 0;
	bool DualHandWeapon = 0;

	if(lpObj->Type == OBJECT_MONSTER || lpObj->Type == OBJECT_NPC)
	{
		DamageMin = lpObj->PhysiDamageMin;
		DamageMax = lpObj->PhysiDamageMax;

		DamageMin += lpObj->EffectOption.AddPhysiDamage;
		DamageMax += lpObj->EffectOption.AddPhysiDamage;

		DamageMin += lpObj->EffectOption.AddMinPhysiDamage;
		DamageMax += lpObj->EffectOption.AddMaxPhysiDamage;

		DamageMin += (DamageMin*lpObj->EffectOption.MulPhysiDamage)/100;
		DamageMax += (DamageMax*lpObj->EffectOption.MulPhysiDamage)/100;

		DamageMin -= (DamageMin*lpObj->EffectOption.DivPhysiDamage)/100;
		DamageMax -= (DamageMax*lpObj->EffectOption.DivPhysiDamage)/100;

		int range = (DamageMax-DamageMin);

		range = ((range<1)?1:range);

		damage = DamageMin+(GetLargeRand()%range);
	}
	else
	{
		if(lpObj->Class == CLASS_DK || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
		{
			if(Right->m_Index >= GET_ITEM(0,0) && Right->m_Index < GET_ITEM(4,0) && Left->m_Index >= GET_ITEM(0,0) && Left->m_Index < GET_ITEM(4,0))
			{
				if(Right->m_IsValidItem != 0 && Left->m_IsValidItem != 0)
				{
					DualHandWeapon = 1;
				}
			}
		}

		if(lpSkill != 0)
		{
			SkillDamageMin = lpSkill->m_DamageMin;
			SkillDamageMax = lpSkill->m_DamageMax;

			SkillDamageMin += lpObj->SkillDamageBonus;
			SkillDamageMax += lpObj->SkillDamageBonus;

			int type = gSkillManager->GetSkillType(lpSkill->m_index);

			if(CHECK_RANGE(type,MAX_RESISTANCE_TYPE) != 0)
			{
				SkillDamageMin += lpObj->AddResistance[type];
				SkillDamageMax += lpObj->AddResistance[type];
			}

			if(lpObj->Class == CLASS_DL)
			{
				switch(lpSkill->m_skill)
				{
					case SKILL_EARTHQUAKE:
						SkillDamageMin += ((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_EarthquakeDamageConstA)+((lpObj->Leadership+lpObj->AddLeadership)/gServerInfo->m_EarthquakeDamageConstB)+(lpObj->Inventory[INVENTORY_SLOT_HELPER].m_PetItemLevel*gServerInfo->m_EarthquakeDamageConstC);
						SkillDamageMax += ((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_EarthquakeDamageConstA)+((lpObj->Leadership+lpObj->AddLeadership)/gServerInfo->m_EarthquakeDamageConstB)+(lpObj->Inventory[INVENTORY_SLOT_HELPER].m_PetItemLevel*gServerInfo->m_EarthquakeDamageConstC);
						break;
					case SKILL_ELECTRIC_SPARK:
						SkillDamageMin += ((lpObj->Leadership+lpObj->AddLeadership)/gServerInfo->m_ElectricSparkDamageConstA)+gServerInfo->m_ElectricSparkDamageConstB;
						SkillDamageMax += ((lpObj->Leadership+lpObj->AddLeadership)/gServerInfo->m_ElectricSparkDamageConstA)+gServerInfo->m_ElectricSparkDamageConstB;
						break;
					default:
						SkillDamageMin += ((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_DLSkillDamageConstA)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_DLSkillDamageConstB);
						SkillDamageMax += ((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_DLSkillDamageConstA)+((lpObj->Energy+lpObj->AddEnergy)/gServerInfo->m_DLSkillDamageConstB);
						break;
				}
			}
		}

		if(DualHandWeapon != 0)
		{
			DamageMin = lpObj->PhysiDamageMinRight+lpObj->PhysiDamageMinLeft+SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxRight+lpObj->PhysiDamageMaxLeft+SkillDamageMax;
		}
		else if((Right->m_Index >= GET_ITEM(0,0) && Right->m_Index < GET_ITEM(4,0)) || (Right->m_Index >= GET_ITEM(5,0) && Right->m_Index < GET_ITEM(6,0)))
		{
			DamageMin = lpObj->PhysiDamageMinRight+SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxRight+SkillDamageMax;
		}
		else if(Right->m_Index >= GET_ITEM(4,0) && Right->m_Index < GET_ITEM(5,0) && Right->m_Index != GET_ITEM(4,15) && Right->m_Slot == 0)
		{
			DamageMin = lpObj->PhysiDamageMinRight+SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxRight+SkillDamageMax;
		}
		else if(Left->m_Index >= GET_ITEM(4,0) && Left->m_Index < GET_ITEM(5,0) && Left->m_Index != GET_ITEM(4,7) && Left->m_Slot == 1)
		{
			DamageMin = lpObj->PhysiDamageMinLeft+SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxLeft+SkillDamageMax;
		}
		else
		{
			DamageMin = lpObj->PhysiDamageMinLeft+SkillDamageMin;
			DamageMax = lpObj->PhysiDamageMaxLeft+SkillDamageMax;
		}

		DamageMin += lpObj->EffectOption.AddPhysiDamage;
		DamageMax += lpObj->EffectOption.AddPhysiDamage;

		DamageMin += lpObj->EffectOption.AddMinPhysiDamage;
		DamageMax += lpObj->EffectOption.AddMaxPhysiDamage;

		gSkillManager->SkillSwordPowerGetPhysiDamage(lpObj->Index,&DamageMin,&DamageMax);

		DamageMin += (DamageMin*lpObj->EffectOption.MulPhysiDamage)/100;
		DamageMax += (DamageMax*lpObj->EffectOption.MulPhysiDamage)/100;

		DamageMin -= (DamageMin*lpObj->EffectOption.DivPhysiDamage)/100;
		DamageMax -= (DamageMax*lpObj->EffectOption.DivPhysiDamage)/100;

		int range = (DamageMax-DamageMin);

		range = ((range<1)?1:range);

		damage = DamageMin+(GetLargeRand()%range);

		if((GetLargeRand()%100) < ((lpObj->CriticalDamageRate+lpObj->EffectOption.AddCriticalDamageRate)-lpTarget->ResistCriticalDamageRate))
		{
			(*effect) = 3;

			damage = DamageMax;
			damage += lpObj->CriticalDamage;
			damage += lpObj->EffectOption.AddCriticalDamage;
		}

		if((GetLargeRand()%100) < ((lpObj->ExcellentDamageRate+lpObj->EffectOption.AddExcellentDamageRate)-lpTarget->ResistExcellentDamageRate))
		{
			(*effect) = 2;

			damage = (DamageMax*120)/100;
			damage += lpObj->ExcellentDamage;
			damage += lpObj->EffectOption.AddExcellentDamage;
		}
	}

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	if(lpObj->MonsterSkillElementOption.CheckAttackTime() != 0)
	{
		damage += lpObj->MonsterSkillElementOption.m_SkillElementAttack;
	}

	damage -= TargetDefense;

	damage = ((damage<0)?0:damage);

	return gSkillDamage->GetDamageRate(lpSkill,damage);
}

int CAttack::GetAttackDamageWizard(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,WORD* effect,int TargetDefense) // OK
{
	CItem* Right = &lpObj->Inventory[INVENTORY_SLOT_WEAPON1];
	CItem* Left = &lpObj->Inventory[INVENTORY_SLOT_WEAPON2];

	int DamageMin = lpObj->MagicDamageMin;
	int DamageMax = lpObj->MagicDamageMax;

	if(lpSkill->m_skill == SKILL_NOVA && lpObj->SkillNovaCount >= 0)
	{
		DamageMin += (gServerInfo->m_NovaDamageConstA*(lpObj->SkillNovaCount*gServerInfo->m_NovaDamageConstB))+((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_NovaDamageConstC);
		DamageMax += (gServerInfo->m_NovaDamageConstA*(lpObj->SkillNovaCount*gServerInfo->m_NovaDamageConstB))+((lpObj->Strength+lpObj->AddStrength)/gServerInfo->m_NovaDamageConstC);
	}
	else
	{
		DamageMin += lpSkill->m_DamageMin;
		DamageMax += lpSkill->m_DamageMax;
	}

	DamageMin += lpObj->SkillDamageBonus;
	DamageMax += lpObj->SkillDamageBonus;

	DamageMin += lpObj->EffectOption.AddMagicDamage;
	DamageMax += lpObj->EffectOption.AddMagicDamage;

	DamageMin += lpObj->EffectOption.AddMinMagicDamage;
	DamageMax += lpObj->EffectOption.AddMaxMagicDamage;

	gSkillManager->SkillSwordPowerGetMagicDamage(lpObj->Index,&DamageMin,&DamageMax);

	int type = gSkillManager->GetSkillType(lpSkill->m_index);

	if(CHECK_RANGE(type,MAX_RESISTANCE_TYPE) != 0)
	{
		DamageMin += lpObj->AddResistance[type];
		DamageMax += lpObj->AddResistance[type];
	}

	DamageMin += (DamageMin*lpObj->EffectOption.MulMagicDamage)/100;
	DamageMax += (DamageMax*lpObj->EffectOption.MulMagicDamage)/100;

	DamageMin -= (DamageMin*lpObj->EffectOption.DivMagicDamage)/100;
	DamageMax -= (DamageMax*lpObj->EffectOption.DivMagicDamage)/100;

	if(Right->IsItem() != 0 && Right->m_IsValidItem != 0 && ((Right->m_Index >= GET_ITEM(0,0) && Right->m_Index < GET_ITEM(1,0)) || (Right->m_Index >= GET_ITEM(5,0) && Right->m_Index < GET_ITEM(6,0))))
	{
		int rise = (int)(((Right->m_MagicDamageRate/2)+(Right->m_Level*2))*Right->m_CurrentDurabilityState);
		DamageMin += (DamageMin*rise)/100;
		DamageMax += (DamageMax*rise)/100;
	}

	int range = (DamageMax-DamageMin);

	range = ((range<1)?1:range);

	int damage = DamageMin+(GetLargeRand()%range);

	if((GetLargeRand()%100) < ((lpObj->CriticalDamageRate+lpObj->EffectOption.AddCriticalDamageRate)-lpTarget->ResistCriticalDamageRate))
	{
		(*effect) = 3;

		damage = DamageMax;
		damage += lpObj->CriticalDamage;
		damage += lpObj->EffectOption.AddCriticalDamage;
	}

	if((GetLargeRand()%100) < ((lpObj->ExcellentDamageRate+lpObj->EffectOption.AddExcellentDamageRate)-lpTarget->ResistExcellentDamageRate))
	{
		(*effect) = 2;

		damage = (DamageMax*120)/100;
		damage += lpObj->ExcellentDamage;
		damage += lpObj->EffectOption.AddExcellentDamage;
	}

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	damage -= TargetDefense;

	damage = ((damage<0)?0:damage);

	return gSkillDamage->GetDamageRate(lpSkill,damage);
}

int CAttack::GetAttackDamageCursed(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,WORD* effect,int TargetDefense) // OK
{
	CItem* Right = &lpObj->Inventory[INVENTORY_SLOT_WEAPON1];
	CItem* Left = &lpObj->Inventory[INVENTORY_SLOT_WEAPON2];

	int DamageMin = lpObj->CurseDamageMin;
	int DamageMax = lpObj->CurseDamageMax;

	DamageMin += lpSkill->m_DamageMin;
	DamageMax += lpSkill->m_DamageMax;

	DamageMin += lpObj->SkillDamageBonus;
	DamageMax += lpObj->SkillDamageBonus;

	DamageMin += lpObj->EffectOption.AddCurseDamage;
	DamageMax += lpObj->EffectOption.AddCurseDamage;

	DamageMin += lpObj->EffectOption.AddMinCurseDamage;
	DamageMax += lpObj->EffectOption.AddMaxCurseDamage;

	gSkillManager->SkillSwordPowerGetCurseDamage(lpObj->Index,&DamageMin,&DamageMax);

	int type = gSkillManager->GetSkillType(lpSkill->m_index);

	if(CHECK_RANGE(type,MAX_RESISTANCE_TYPE) != 0)
	{
		DamageMin += lpObj->AddResistance[type];
		DamageMax += lpObj->AddResistance[type];
	}

	DamageMin += (DamageMin*lpObj->EffectOption.MulCurseDamage)/100;
	DamageMax += (DamageMax*lpObj->EffectOption.MulCurseDamage)/100;

	DamageMin -= (DamageMin*lpObj->EffectOption.DivCurseDamage)/100;
	DamageMax -= (DamageMax*lpObj->EffectOption.DivCurseDamage)/100;

	if(Left->IsItem() != 0 && Left->m_IsValidItem != 0 && Left->m_Index >= GET_ITEM(5,21) && Left->m_Index <= GET_ITEM(5,23))
	{
		int rise = (int)(((Left->m_MagicDamageRate/2)+(Left->m_Level*2))*Left->m_CurrentDurabilityState);
		DamageMin += (DamageMin*rise)/100;
		DamageMax += (DamageMax*rise)/100;
	}

	int range = (DamageMax-DamageMin);

	range = ((range<1)?1:range);

	int damage = DamageMin+(GetLargeRand()%range);

	if((GetLargeRand()%100) < ((lpObj->CriticalDamageRate+lpObj->EffectOption.AddCriticalDamageRate)-lpTarget->ResistCriticalDamageRate))
	{
		(*effect) = 3;

		damage = DamageMax;
		damage += lpObj->CriticalDamage;
		damage += lpObj->EffectOption.AddCriticalDamage;
	}

	if((GetLargeRand()%100) < ((lpObj->ExcellentDamageRate+lpObj->EffectOption.AddExcellentDamageRate)-lpTarget->ResistExcellentDamageRate))
	{
		(*effect) = 2;

		damage = (DamageMax*120)/100;
		damage += lpObj->ExcellentDamage;
		damage += lpObj->EffectOption.AddExcellentDamage;
	}

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	damage -= TargetDefense;

	damage = ((damage<0)?0:damage);

	return gSkillDamage->GetDamageRate(lpSkill,damage);
}

int CAttack::GetAttackDamageFenrir(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,WORD* effect,int TargetDefense) // OK
{
	int BaseDamage = 0;

	if(lpObj->Class == CLASS_DW)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_DWPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_DWPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_DWPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_DWPlasmaStormDamageConstD);
	}
	else if(lpObj->Class == CLASS_DK)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_DKPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_DKPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_DKPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_DKPlasmaStormDamageConstD);
	}
	else if(lpObj->Class == CLASS_FE)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_FEPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_FEPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_FEPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_FEPlasmaStormDamageConstD);
	}
	else if(lpObj->Class == CLASS_MG)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_MGPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_MGPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_MGPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_MGPlasmaStormDamageConstD);
	}
	else if(lpObj->Class == CLASS_DL)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_DLPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_DLPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_DLPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_DLPlasmaStormDamageConstD)+(lpObj->Leadership/gServerInfo->m_DLPlasmaStormDamageConstE);
	}
	else if(lpObj->Class == CLASS_SU)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_SUPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_SUPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_SUPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_SUPlasmaStormDamageConstD);
	}
	else if(lpObj->Class == CLASS_RF)
	{
		BaseDamage = (lpObj->Strength/gServerInfo->m_RFPlasmaStormDamageConstA)+(lpObj->Dexterity/gServerInfo->m_RFPlasmaStormDamageConstB)+(lpObj->Vitality/gServerInfo->m_RFPlasmaStormDamageConstC)+(lpObj->Energy/gServerInfo->m_RFPlasmaStormDamageConstD);
	}

	int range = (lpSkill->m_DamageMax-lpSkill->m_DamageMin);

	range = ((range<1)?1:range);

	int damage = (BaseDamage+lpSkill->m_DamageMin)+(GetLargeRand()%range);

	if((GetLargeRand()%100) < ((lpObj->CriticalDamageRate+lpObj->EffectOption.AddCriticalDamageRate)-lpTarget->ResistCriticalDamageRate))
	{
		(*effect) = 3;

		damage = (BaseDamage+lpSkill->m_DamageMax);
		damage += lpObj->CriticalDamage;
		damage += lpObj->EffectOption.AddCriticalDamage;
	}

	if((GetLargeRand()%100) < ((lpObj->ExcellentDamageRate+lpObj->EffectOption.AddExcellentDamageRate)-lpTarget->ResistExcellentDamageRate))
	{
		(*effect) = 2;

		damage = ((BaseDamage+lpSkill->m_DamageMax)*120)/100;
		damage += lpObj->ExcellentDamage;
		damage += lpObj->EffectOption.AddExcellentDamage;
	}

	if(lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		damage += lpObj->DamagePvP;
	}

	damage -= TargetDefense;

	damage = ((damage<0)?0:damage);

	return gSkillDamage->GetDamageRate(lpSkill,damage);
}

int CAttack::GetAttackDamageElemental(LPOBJ lpObj,LPOBJ lpTarget,CSkill* lpSkill,WORD* effect,int AttackDamage,int TargetDefense) // OK
{
	return 0;
}

int CAttack::GetShieldDamage(LPOBJ lpObj,LPOBJ lpTarget,int damage) // OK
{
	int rate = lpTarget->ShieldGaugeRate;

	if((GetLargeRand()%100) < ((lpObj->IgnoreShieldGaugeRate)-lpTarget->ResistIgnoreShieldGaugeRate))
	{
		rate = 0;
	}
	else
	{
		rate -= lpObj->DecreaseShieldGaugeRate;
	}

	rate = ((rate<0)?0:((rate>100)?100:rate));

	int SDDamage = (damage*rate)/100;
	int HPDamage = damage-SDDamage;

	if(lpTarget->Shield < SDDamage)
	{
		HPDamage = HPDamage+(SDDamage-lpTarget->Shield);
		SDDamage = lpTarget->Shield;

		if(lpTarget->Shield > 0 && HPDamage > (((lpTarget->MaxLife+lpTarget->AddLife)*20)/100))
		{
			if(CC_MAP_RANGE(lpTarget->Map) == 0 || IT_MAP_RANGE(lpTarget->Map) == 0)
			{
				GCEffectInfoSend(lpTarget->Index,17);
			}
		}
	}

	return SDDamage;
}

void CAttack::GetPreviewDefense(LPOBJ lpObj,DWORD* defense) // OK
{
	(*defense) = lpObj->Defense;

	(*defense) += lpObj->EffectOption.AddDefense;

	(*defense) -= lpObj->EffectOption.SubDefense;

	gSkillManager->SkillSwordPowerGetDefense(lpObj->Index,(int*)defense);

	(*defense) += ((*defense)*lpObj->EffectOption.MulDefense)/100;

	(*defense) -= ((*defense)*lpObj->EffectOption.DivDefense)/100;
}

void CAttack::GetPreviewPhysiDamage(LPOBJ lpObj,DWORD* DamageMin,DWORD* DamageMax,DWORD* MulDamage,DWORD* DivDamage) // OK
{
	CItem* Right = &lpObj->Inventory[INVENTORY_SLOT_WEAPON1];
	CItem* Left = &lpObj->Inventory[INVENTORY_SLOT_WEAPON2];

	bool DualHandWeapon = 0;

	if(lpObj->Class == CLASS_DK || lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		if(Right->m_Index >= GET_ITEM(0,0) && Right->m_Index < GET_ITEM(4,0) && Left->m_Index >= GET_ITEM(0,0) && Left->m_Index < GET_ITEM(4,0))
		{
			if(Right->m_IsValidItem != 0 && Left->m_IsValidItem != 0)
			{
				DualHandWeapon = 1;
			}
		}
	}

	if(DualHandWeapon != 0)
	{
		(*DamageMin) = lpObj->PhysiDamageMinRight+lpObj->PhysiDamageMinLeft;
		(*DamageMax) = lpObj->PhysiDamageMaxRight+lpObj->PhysiDamageMaxLeft;
	}
	else if((Right->m_Index >= GET_ITEM(0,0) && Right->m_Index < GET_ITEM(4,0)) || (Right->m_Index >= GET_ITEM(5,0) && Right->m_Index < GET_ITEM(6,0)))
	{
		(*DamageMin) = lpObj->PhysiDamageMinRight;
		(*DamageMax) = lpObj->PhysiDamageMaxRight;
	}
	else if(Right->m_Index >= GET_ITEM(4,0) && Right->m_Index < GET_ITEM(5,0) && Right->m_Index != GET_ITEM(4,15) && Right->m_Slot == 0)
	{
		(*DamageMin) = lpObj->PhysiDamageMinRight;
		(*DamageMax) = lpObj->PhysiDamageMaxRight;
	}
	else if(Left->m_Index >= GET_ITEM(4,0) && Left->m_Index < GET_ITEM(5,0) && Left->m_Index != GET_ITEM(4,7) && Left->m_Slot == 1)
	{
		(*DamageMin) = lpObj->PhysiDamageMinLeft;
		(*DamageMax) = lpObj->PhysiDamageMaxLeft;
	}
	else
	{
		(*DamageMin) = lpObj->PhysiDamageMinLeft;
		(*DamageMax) = lpObj->PhysiDamageMaxLeft;
	}

	(*DamageMin) += lpObj->EffectOption.AddPhysiDamage;
	(*DamageMax) += lpObj->EffectOption.AddPhysiDamage;

	(*DamageMin) += lpObj->EffectOption.AddMinPhysiDamage;
	(*DamageMax) += lpObj->EffectOption.AddMaxPhysiDamage;

	gSkillManager->SkillSwordPowerGetPhysiDamage(lpObj->Index,(int*)DamageMin,(int*)DamageMax);

	(*MulDamage) = lpObj->EffectOption.MulPhysiDamage;
	(*DivDamage) = lpObj->EffectOption.DivPhysiDamage;
}

void CAttack::GetPreviewMagicDamage(LPOBJ lpObj,DWORD* DamageMin,DWORD* DamageMax,DWORD* MulDamage,DWORD* DivDamage,DWORD* DamageRate) // OK
{
	CItem* Right = &lpObj->Inventory[INVENTORY_SLOT_WEAPON1];
	CItem* Left = &lpObj->Inventory[INVENTORY_SLOT_WEAPON2];

	(*DamageMin) = lpObj->MagicDamageMin;
	(*DamageMax) = lpObj->MagicDamageMax;

	(*DamageMin) += lpObj->EffectOption.AddMagicDamage;
	(*DamageMax) += lpObj->EffectOption.AddMagicDamage;

	(*DamageMin) += lpObj->EffectOption.AddMinMagicDamage;
	(*DamageMax) += lpObj->EffectOption.AddMaxMagicDamage;

	gSkillManager->SkillSwordPowerGetMagicDamage(lpObj->Index,(int*)DamageMin,(int*)DamageMax);

	(*MulDamage) = lpObj->EffectOption.MulMagicDamage;
	(*DivDamage) = lpObj->EffectOption.DivMagicDamage;

	if(Right->IsItem() != 0 && Right->m_IsValidItem != 0 && ((Right->m_Index >= GET_ITEM(0,0) && Right->m_Index < GET_ITEM(1,0)) || (Right->m_Index >= GET_ITEM(5,0) && Right->m_Index < GET_ITEM(6,0))))
	{
		(*DamageRate) = (int)(((Right->m_MagicDamageRate/2)+(Right->m_Level*2))*Right->m_CurrentDurabilityState);
	}
	else
	{
		(*DamageRate) = 0;
	}
}

void CAttack::GetPreviewCurseDamage(LPOBJ lpObj,DWORD* DamageMin,DWORD* DamageMax,DWORD* MulDamage,DWORD* DivDamage,DWORD* DamageRate) // OK
{
	CItem* Right = &lpObj->Inventory[INVENTORY_SLOT_WEAPON1];
	CItem* Left = &lpObj->Inventory[INVENTORY_SLOT_WEAPON2];

	(*DamageMin) = lpObj->CurseDamageMin;
	(*DamageMax) = lpObj->CurseDamageMax;

	(*DamageMin) += lpObj->EffectOption.AddCurseDamage;
	(*DamageMax) += lpObj->EffectOption.AddCurseDamage;

	(*DamageMin) += lpObj->EffectOption.AddMinCurseDamage;
	(*DamageMax) += lpObj->EffectOption.AddMaxCurseDamage;

	gSkillManager->SkillSwordPowerGetCurseDamage(lpObj->Index,(int*)DamageMin,(int*)DamageMax);

	(*MulDamage) = lpObj->EffectOption.MulCurseDamage;
	(*DivDamage) = lpObj->EffectOption.DivCurseDamage;

	if(Left->IsItem() != 0 && Left->m_IsValidItem != 0 && Left->m_Index >= GET_ITEM(5,21) && Left->m_Index <= GET_ITEM(5,23))
	{
		(*DamageRate) = (int)(((Left->m_MagicDamageRate/2)+(Left->m_Level*2))*Left->m_CurrentDurabilityState);
	}
	else
	{
		(*DamageRate) = 0;
	}
}

void CAttack::GetPreviewDamageMultiplier(LPOBJ lpObj,DWORD* DamageMultiplier,DWORD* RFDamageMultiplierA,DWORD* RFDamageMultiplierB,DWORD* RFDamageMultiplierC) // OK
{
	switch(lpObj->Class)
	{
		case CLASS_DW:
			(*DamageMultiplier) = 200;
			(*RFDamageMultiplierA) = 100;
			(*RFDamageMultiplierB) = 100;
			(*RFDamageMultiplierC) = 100;
			break;
		case CLASS_DK:
			(*DamageMultiplier) = lpObj->DKDamageMultiplierRate;
			(*RFDamageMultiplierA) = 100;
			(*RFDamageMultiplierB) = 100;
			(*RFDamageMultiplierC) = 100;
			break;
		case CLASS_FE:
			(*DamageMultiplier) = 200;
			(*RFDamageMultiplierA) = 100;
			(*RFDamageMultiplierB) = 100;
			(*RFDamageMultiplierC) = 100;
			break;
		case CLASS_MG:
			(*DamageMultiplier) = 200;
			(*RFDamageMultiplierA) = 100;
			(*RFDamageMultiplierB) = 100;
			(*RFDamageMultiplierC) = 100;
			break;
		case CLASS_DL:
			(*DamageMultiplier) = lpObj->DLDamageMultiplierRate;
			(*RFDamageMultiplierA) = 100;
			(*RFDamageMultiplierB) = 100;
			(*RFDamageMultiplierC) = 100;
			break;
		case CLASS_SU:
			(*DamageMultiplier) = 200;
			(*RFDamageMultiplierA) = 100;
			(*RFDamageMultiplierB) = 100;
			(*RFDamageMultiplierC) = 100;
			break;
		case CLASS_RF:
			(*DamageMultiplier) = 200;
			(*RFDamageMultiplierA) = lpObj->RFDamageMultiplierRate[0];
			(*RFDamageMultiplierB) = lpObj->RFDamageMultiplierRate[1];
			(*RFDamageMultiplierC) = lpObj->RFDamageMultiplierRate[2];
			break;
	}
}

void CAttack::CGAttackRecv(PMSG_ATTACK_RECV* lpMsg,int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if(gObjIsConnected(aIndex) == 0)
	{
		return;
	}

	int bIndex = MAKE_NUMBERW(lpMsg->index[0],lpMsg->index[1]);

	if(OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}

	LPOBJ lpTarget = &gObj[bIndex];

	if(lpTarget->Live == 0)
	{
		return;
	}

	if(lpObj->Map != lpTarget->Map)
	{
		return;
	}

	if(gMap[lpObj->Map].CheckAttr(lpObj->X,lpObj->Y,1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X,lpTarget->Y,1) != 0)
	{
		return;
	}

	if(lpObj->Type == OBJECT_USER && sqrt(pow(((float)lpObj->X-(float)lpTarget->X),2)+pow(((float)lpObj->Y-(float)lpTarget->Y),2)) > ((lpObj->Class==CLASS_FE)?6:3))
	{
		return;
	}

	lpObj->Dir = lpMsg->dir;

	lpObj->MultiSkillIndex = 0;

	lpObj->MultiSkillCount = 0;

	GCActionSend(lpObj,lpMsg->action,aIndex,bIndex);

	lpObj->ComboSkill.Init();

	this->Attack(lpObj,lpTarget,0,0,0,0,0,0);
}