// HackSkillSpeedCheck.cpp: implementation of the CHackSkillSpeedCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HackSkillSpeedCheck.h"
#include "Console.h"
#include "ReadFile.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "Log.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHackSkillSpeedCheck::CHackSkillSpeedCheck() // OK
{
	this->m_HackSkillSpeedCheckInfo.clear();
}

CHackSkillSpeedCheck::~CHackSkillSpeedCheck() // OK
{

}

void CHackSkillSpeedCheck::Load(char* path) // OK
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

	this->m_HackSkillSpeedCheckInfo.clear();

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

			HACK_SKILL_SPEED_CHECK_INFO info;

			info.Index = lpReadFile->GetNumber();

			info.Class = lpReadFile->GetAsNumber();
			
			info.MinLevel = lpReadFile->GetAsNumber();
			
			info.MaxLevel = lpReadFile->GetAsNumber();
			
			info.MinReset = lpReadFile->GetAsNumber();
			
			info.MaxReset = lpReadFile->GetAsNumber();

			info.MinSpeed = lpReadFile->GetAsNumber();

			info.MaxSpeed = lpReadFile->GetAsNumber();

			info.MaxDelay = lpReadFile->GetAsNumber();

			info.MinCount = lpReadFile->GetAsNumber();

			info.MaxCount = lpReadFile->GetAsNumber();

			info.Action = lpReadFile->GetAsNumber();

			this->m_HackSkillSpeedCheckInfo.push_back(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

bool CHackSkillSpeedCheck::CheckSkillSpeed(int aIndex,int index) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	int Speed = ((lpObj->Class == CLASS_DW || lpObj->Class == CLASS_SU) ? lpObj->MagicSpeed : lpObj->PhysiSpeed);

	for (std::vector<HACK_SKILL_SPEED_CHECK_INFO>::iterator it = this->m_HackSkillSpeedCheckInfo.begin(); it != this->m_HackSkillSpeedCheckInfo.end(); it++)
	{
		if(it->Index != index)
		{
			continue;
		}

		if(it->Class != -1 && it->Class != lpObj->Class)
		{
			continue;
		}

		if(it->MinLevel != -1 && it->MinLevel > lpObj->Level)
		{
			continue;
		}

		if(it->MaxLevel != -1 && it->MaxLevel < lpObj->Level)
		{
			continue;
		}

		if(it->MinReset != -1 && it->MinReset > lpObj->Reset)
		{
			continue;
		}

		if(it->MaxReset != -1 && it->MaxReset < lpObj->Reset)
		{
			continue;
		}

		if(it->MinSpeed != -1 && it->MinSpeed > Speed)
		{
			continue;
		}

		if(it->MaxSpeed != -1 && it->MaxSpeed < Speed)
		{
			continue;
		}

		if(this->CheckSkillMaxDelay(aIndex,index,it->MaxDelay) == 0)
		{
			lpObj->HackSkillDelay[index] = GetTickCount();
			lpObj->HackSkillCount[index] = 0;
		}

		if(this->CheckSkillMinCount(aIndex,index,it->MinCount) == 0)
		{
			gLog->Output(LOG_HACK, "[HackSkillSpeedCheck][%s][%s] Skill Speed count error (Index: %d, Speed: %d, Count: [%d][%d])", gObj[aIndex].Account, gObj[aIndex].Name, index,Speed,gObj[aIndex].HackSkillCount[index],it->MaxCount);

			if(this->CheckSkillMaxCount(aIndex,index,it->MaxCount) == 0)
			{
				if(it->Action == 1)
				{
					GCNewMessageSend(aIndex,0,0,gMessage->GetMessage(719));
				}
				else if(it->Action == 2)
				{
					gObjUserKill(aIndex);
				}

				return 0;
			}
		}
	}

	return 1;
}

bool CHackSkillSpeedCheck::CheckSkillMaxDelay(int aIndex,int index,int MaxDelay) // OK
{
	return ((MaxDelay==-1)?1:(((GetTickCount()-gObj[aIndex].HackSkillDelay[index])>=((DWORD)MaxDelay))?0:1));
}

bool CHackSkillSpeedCheck::CheckSkillMinCount(int aIndex,int index,int MinCount) // OK
{
	return ((MinCount==-1)?1:(((++gObj[aIndex].HackSkillCount[index])>((DWORD)MinCount))?0:1));
}

bool CHackSkillSpeedCheck::CheckSkillMaxCount(int aIndex,int index,int MaxCount) // OK
{
	return ((MaxCount==-1)?1:((gObj[aIndex].HackSkillCount[index]>((DWORD)MaxCount))?0:1));
}