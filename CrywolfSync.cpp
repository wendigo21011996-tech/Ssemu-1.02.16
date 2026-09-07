// CrywolfSync.cpp: implementation of the CCrywolfSync class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrywolfSync.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrywolfSync::CCrywolfSync() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_CrywolfEnable = 0;
	this->m_ApplyBenefit = 0;
	this->m_ApplyPenalty = 0;
	this->m_CrywolfState = 0;
	this->m_OccupationState = 1;
	this->m_PlusChaosRate = 0;
	this->m_GemDropPenaltyRate = 100;
	this->m_GettingExpPenaltyRate = 100;
	this->m_MinusMonHPBenefitRate = 100;
	this->m_KundunHPRefillState = 1;
	
	#endif
}

CCrywolfSync::~CCrywolfSync() // OK
{

}

BOOL CCrywolfSync::CheckEnableCrywolf() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_CrywolfEnable;
	
	#else

	return 0;

	#endif
}

BOOL CCrywolfSync::CheckApplyBenefit() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_ApplyBenefit;
	
	#else

	return 0;

	#endif
}

BOOL CCrywolfSync::CheckApplyPenalty() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_ApplyPenalty;
	
	#else

	return 0;

	#endif
}

void CCrywolfSync::SetEnableCrywolf(int EnableCrywolf) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_CrywolfEnable = EnableCrywolf;
	
	#endif
}

void CCrywolfSync::SetApplyBenefit(int ApplyBenefit) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_ApplyBenefit = ApplyBenefit;
	
	#endif
}

void CCrywolfSync::SetApplyPenalty(int ApplyPenalty) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_ApplyPenalty = ApplyPenalty;
	
	#endif
}

void CCrywolfSync::SetCrywolfState(int CrywolfState) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_CrywolfState = CrywolfState;
	
	#endif
}

void CCrywolfSync::SetOccupationState(int OccupationState) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_OccupationState = OccupationState;
	
	#endif
}

void CCrywolfSync::SetPlusChaosRate(int PlusChaosRate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_PlusChaosRate = PlusChaosRate;
	
	#endif
}

void CCrywolfSync::SetGemDropPenaltiyRate(int GemDropPenaltyRate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_GemDropPenaltyRate = GemDropPenaltyRate;
	
	#endif
}

void CCrywolfSync::SetGettingExpPenaltyRate(int GettingExpPenaltyRate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_GettingExpPenaltyRate = GettingExpPenaltyRate;
	
	#endif
}

void CCrywolfSync::SetMonHPBenefitRate(int MinusMonHPBenefitRate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_MinusMonHPBenefitRate = MinusMonHPBenefitRate;
	
	#endif
}

void CCrywolfSync::SetKundunHPRefillState(int KundunHPRefillState) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	this->m_KundunHPRefillState = KundunHPRefillState;

	#endif
}

int CCrywolfSync::GetCrywolfState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_CrywolfState;
	
	#else

	return 0;

	#endif
}

int CCrywolfSync::GetOccupationState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_OccupationState;
	
	#else

	return 0;

	#endif
}

int CCrywolfSync::GetPlusChaosRate() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(this->CheckApplyBenefit() != 0 && this->GetOccupationState() == 0)
	{
		return this->m_PlusChaosRate;
	}
	else
	{
		return 0;
	}
	
	#else

	return 0;

	#endif
}

int CCrywolfSync::GetGemDropPenaltiyRate() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_GemDropPenaltyRate;
	
	#else

	return 0;

	#endif
}

int CCrywolfSync::GetGettingExpPenaltyRate() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_GettingExpPenaltyRate;
	
	#else

	return 0;

	#endif
}

int CCrywolfSync::GetMonHPBenefitRate() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_MinusMonHPBenefitRate;
	
	#else

	return 0;

	#endif
}

int CCrywolfSync::GetKundunHPRefillState() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	return this->m_KundunHPRefillState;

	#else

	return 0;

	#endif
}
