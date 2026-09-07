// EventEntryLevel.cpp: interface for the CEventEntryLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventEntryLevel.h"
#include "DefaultClassInfo.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventEntryLevel::CEventEntryLevel() // OK
{
	this->Init();
}

CEventEntryLevel::~CEventEntryLevel() // OK
{

}

void CEventEntryLevel::Init() // OK
{
	memset(this->m_BloodCastleEntryLevelCommon,0,sizeof(this->m_BloodCastleEntryLevelCommon));

	memset(this->m_BloodCastleEntryLevelSpecial,0,sizeof(this->m_BloodCastleEntryLevelSpecial));
	
	memset(this->m_DevilSquareEntryLevelCommon,0,sizeof(this->m_DevilSquareEntryLevelCommon));
	
	memset(this->m_DevilSquareEntryLevelSpecial,0,sizeof(this->m_DevilSquareEntryLevelSpecial));

	memset(this->m_ChaosCastleEntryLevelCommon,0,sizeof(this->m_ChaosCastleEntryLevelCommon));
	
	memset(this->m_ChaosCastleEntryLevelSpecial,0,sizeof(this->m_ChaosCastleEntryLevelSpecial));
	
	memset(this->m_KalimaEntryLevelCommon,0,sizeof(this->m_KalimaEntryLevelCommon));
	
	memset(this->m_KalimaEntryLevelSpecial,0,sizeof(this->m_KalimaEntryLevelSpecial));
	
	memset(this->m_IllusionTempleEntryLevelMin,0,sizeof(this->m_IllusionTempleEntryLevelMin));
	
	memset(this->m_IllusionTempleEntryLevelMax,0,sizeof(this->m_IllusionTempleEntryLevelMax));
}

void CEventEntryLevel::Load(char* path) // OK
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

	this->Init();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}
		
			int section = lpReadFile->GetNumber();

			while(true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					this->m_BloodCastleEntryLevelCommon[level][0] = lpReadFile->GetAsNumber();

					this->m_BloodCastleEntryLevelCommon[level][1] = lpReadFile->GetAsNumber();

					this->m_BloodCastleEntryLevelSpecial[level][0] = lpReadFile->GetAsNumber();

					this->m_BloodCastleEntryLevelSpecial[level][1] = lpReadFile->GetAsNumber();
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					this->m_DevilSquareEntryLevelCommon[level][0] = lpReadFile->GetAsNumber();

					this->m_DevilSquareEntryLevelCommon[level][1] = lpReadFile->GetAsNumber();

					this->m_DevilSquareEntryLevelSpecial[level][0] = lpReadFile->GetAsNumber();

					this->m_DevilSquareEntryLevelSpecial[level][1] = lpReadFile->GetAsNumber();
				}
				else if(section == 2)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					this->m_ChaosCastleEntryLevelCommon[level][0] = lpReadFile->GetAsNumber();

					this->m_ChaosCastleEntryLevelCommon[level][1] = lpReadFile->GetAsNumber();

					this->m_ChaosCastleEntryLevelSpecial[level][0] = lpReadFile->GetAsNumber();

					this->m_ChaosCastleEntryLevelSpecial[level][1] = lpReadFile->GetAsNumber();
				}
				else if(section == 3)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					this->m_KalimaEntryLevelCommon[level][0] = lpReadFile->GetAsNumber();

					this->m_KalimaEntryLevelCommon[level][1] = lpReadFile->GetAsNumber();

					this->m_KalimaEntryLevelSpecial[level][0] = lpReadFile->GetAsNumber();

					this->m_KalimaEntryLevelSpecial[level][1] = lpReadFile->GetAsNumber();
				}
				#if(GAMESERVER_UPDATE>=301)
				else if(section == 4)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					int level = lpReadFile->GetNumber();

					this->m_IllusionTempleEntryLevelMin[level] = lpReadFile->GetAsNumber();

					this->m_IllusionTempleEntryLevelMax[level] = lpReadFile->GetAsNumber();
				}
				#endif
				else
				{
					break;
				}
			}
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

int CEventEntryLevel::GetBCLevel(LPOBJ lpObj) // OK
{
	int level = -1;

	#if(GAMESERVER_UPDATE>=401)

	if(lpObj->ChangeUp >= 2)
	{
		return 7;
	}

	#endif

	if(lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		for(int n=0;n < 7;n++)
		{
			if(this->m_BloodCastleEntryLevelSpecial[n][0] != -1 && this->m_BloodCastleEntryLevelSpecial[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_BloodCastleEntryLevelSpecial[n][1] != -1 && this->m_BloodCastleEntryLevelSpecial[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}
	else
	{
		for(int n=0;n < 7;n++)
		{
			if(this->m_BloodCastleEntryLevelCommon[n][0] != -1 && this->m_BloodCastleEntryLevelCommon[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_BloodCastleEntryLevelCommon[n][1] != -1 && this->m_BloodCastleEntryLevelCommon[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}

	return level;
}

int CEventEntryLevel::GetDSLevel(LPOBJ lpObj) // OK
{
	int level = -1;

	#if(GAMESERVER_UPDATE>=401)

	if(lpObj->ChangeUp >= 2)
	{
		return 6;
	}

	#endif

	if(lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		for(int n=0;n < 6;n++)
		{
			if(this->m_DevilSquareEntryLevelSpecial[n][0] != -1 && this->m_DevilSquareEntryLevelSpecial[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_DevilSquareEntryLevelSpecial[n][1] != -1 && this->m_DevilSquareEntryLevelSpecial[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}
	else
	{
		for(int n=0;n < 6;n++)
		{
			if(this->m_DevilSquareEntryLevelCommon[n][0] != -1 && this->m_DevilSquareEntryLevelCommon[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_DevilSquareEntryLevelCommon[n][1] != -1 && this->m_DevilSquareEntryLevelCommon[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}

	return level;
}

int CEventEntryLevel::GetCCLevel(LPOBJ lpObj) // OK
{
	int level = -1;

	#if(GAMESERVER_UPDATE>=401)

	if(lpObj->ChangeUp >= 2)
	{
		return 6;
	}

	#endif

	if(lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		for(int n=0;n < 6;n++)
		{
			if(this->m_ChaosCastleEntryLevelSpecial[n][0] != -1 && this->m_ChaosCastleEntryLevelSpecial[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_ChaosCastleEntryLevelSpecial[n][1] != -1 && this->m_ChaosCastleEntryLevelSpecial[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}
	else
	{
		for(int n=0;n < 6;n++)
		{
			if(this->m_ChaosCastleEntryLevelCommon[n][0] != -1 && this->m_ChaosCastleEntryLevelCommon[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_ChaosCastleEntryLevelCommon[n][1] != -1 && this->m_ChaosCastleEntryLevelCommon[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}

	return level;
}

int CEventEntryLevel::GetKALevel(LPOBJ lpObj) // OK
{
	int level = -1;

	if(lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		for(int n=0;n < 7;n++)
		{
			if(this->m_KalimaEntryLevelSpecial[n][0] != -1 && this->m_KalimaEntryLevelSpecial[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_KalimaEntryLevelSpecial[n][1] != -1 && this->m_KalimaEntryLevelSpecial[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}
	else
	{
		for(int n=0;n < 7;n++)
		{
			if(this->m_KalimaEntryLevelCommon[n][0] != -1 && this->m_KalimaEntryLevelCommon[n][0] > lpObj->Level)
			{
				continue;
			}

			if(this->m_KalimaEntryLevelCommon[n][1] != -1 && this->m_KalimaEntryLevelCommon[n][1] < lpObj->Level)
			{
				continue;
			}

			level = n;
		}
	}

	return level;
}

int CEventEntryLevel::GetITLevel(LPOBJ lpObj) // OK
{
	int level = -1;

	#if(GAMESERVER_UPDATE>=301)

	#if(GAMESERVER_UPDATE>=401)

	if(lpObj->ChangeUp >= 2)
	{
		return 5;
	}

	#endif

	for(int n=0;n < 5;n++)
	{
		if(this->m_IllusionTempleEntryLevelMin[n] != -1 && this->m_IllusionTempleEntryLevelMin[n] > lpObj->Level)
		{
			continue;
		}

		if(this->m_IllusionTempleEntryLevelMax[n] != -1 && this->m_IllusionTempleEntryLevelMax[n] < lpObj->Level)
		{
			continue;
		}

		level = n;
	}

	#endif

	return level;
}