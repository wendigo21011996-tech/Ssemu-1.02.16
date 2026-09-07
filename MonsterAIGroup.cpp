// MonsterAIGroup.cpp: implementation of the CMonsterAIGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIGroup.h"
#include "ReadFile.h"
#include "Monster.h"
#include "Util.h"

BOOL CMonsterAIGroup::m_DataLoad;
CMonsterAIGroupMember CMonsterAIGroup::m_MonsterAIGroupMemberArray[MAX_MONSTER_AI_GROUP][MAX_MONSTER_AI_GROUP_MEMBER];
int CMonsterAIGroup::m_MonsterAIGroupMemberCount[MAX_MONSTER_AI_GROUP];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIGroup::CMonsterAIGroup() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIGroup::DelAllGroupInfo();

	#endif
}

CMonsterAIGroup::~CMonsterAIGroup() // OK
{

}

void CMonsterAIGroup::LoadData(char* path) // OK
{
	#if(GAMESERVER_UPDATE>=201)

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

	CMonsterAIGroup::DelAllGroupInfo();

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

					CMonsterAIGroupMember info;

					info.m_GroupNumber = lpReadFile->GetNumber();

					info.m_Guid = lpReadFile->GetAsNumber();

					info.MonsterClass = lpReadFile->GetAsNumber();

					info.m_Rank = lpReadFile->GetAsNumber();

					info.m_StartAI = lpReadFile->GetAsNumber();

					info.m_AI01 = lpReadFile->GetAsNumber();

					info.m_AI02 = lpReadFile->GetAsNumber();

					info.m_AI03 = lpReadFile->GetAsNumber();

					info.m_CreateType = lpReadFile->GetAsNumber();

					info.m_MapNumber = lpReadFile->GetAsNumber();

					info.m_StartX = lpReadFile->GetAsNumber();

					info.m_StartY = lpReadFile->GetAsNumber();

					info.m_StartDir = lpReadFile->GetAsNumber();

					info.m_RegenType = lpReadFile->GetAsNumber();

					if((info.m_GroupNumber >= 0 && info.m_GroupNumber < MAX_MONSTER_AI_GROUP) && (info.m_Guid >= 0 && info.m_Guid < MAX_MONSTER_AI_GROUP_MEMBER))
					{
						CMonsterAIGroup::m_MonsterAIGroupMemberArray[info.m_GroupNumber][info.m_Guid] = info;
						CMonsterAIGroup::m_MonsterAIGroupMemberCount[info.m_GroupNumber]++;
					}
				}
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

	CMonsterAIGroup::m_DataLoad = 1;

	delete lpReadFile;

	#endif
}

void CMonsterAIGroup::ChangeAIOrder(int GroupNumber,int AIOrder) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(GroupNumber < 0 || GroupNumber >= MAX_MONSTER_AI_GROUP)
	{
		return;
	}

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(AIOrder == 0)
		{
			gObj[lpMonsterAIGroupMember->m_ObjIndex].BasicAI = lpMonsterAIGroupMember->m_StartAI;
			gObj[lpMonsterAIGroupMember->m_ObjIndex].CurrentAI = lpMonsterAIGroupMember->m_StartAI;
			continue;
		}

		if(AIOrder == 1)
		{
			gObj[lpMonsterAIGroupMember->m_ObjIndex].BasicAI = lpMonsterAIGroupMember->m_AI01;
			gObj[lpMonsterAIGroupMember->m_ObjIndex].CurrentAI = lpMonsterAIGroupMember->m_AI01;
			continue;
		}

		if(AIOrder == 2)
		{
			gObj[lpMonsterAIGroupMember->m_ObjIndex].BasicAI = lpMonsterAIGroupMember->m_AI02;
			gObj[lpMonsterAIGroupMember->m_ObjIndex].CurrentAI = lpMonsterAIGroupMember->m_AI02;
			continue;
		}

		if(AIOrder == 3)
		{
			gObj[lpMonsterAIGroupMember->m_ObjIndex].BasicAI = lpMonsterAIGroupMember->m_AI03;
			gObj[lpMonsterAIGroupMember->m_ObjIndex].CurrentAI = lpMonsterAIGroupMember->m_AI03;
			continue;
		}
	}

	#endif
}

BOOL CMonsterAIGroup::DelGroupInstance(int GroupNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(GroupNumber < 0 || GroupNumber >= MAX_MONSTER_AI_GROUP)
	{
		return 0;
	}

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(OBJECT_RANGE(lpMonsterAIGroupMember->m_ObjIndex) != 0)
		{
			gObjDel(lpMonsterAIGroupMember->m_ObjIndex);
		}
	}

	return 1;

	#else

	return 0;

	#endif
}

void CMonsterAIGroup::DelAllGroupInfo() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIGroup::m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_AI_GROUP;n++)for(int i=0;i < MAX_MONSTER_AI_GROUP_MEMBER;i++)
	{
		CMonsterAIGroup::m_MonsterAIGroupMemberArray[n][i].Reset();
	}

	#endif
}

int CMonsterAIGroup::FindGroupLeader(int GroupNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(GroupNumber < 0 || GroupNumber >= MAX_MONSTER_AI_GROUP)
	{
		return -1;
	}

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(lpMonsterAIGroupMember->m_Rank == 0)
		{
			return lpMonsterAIGroupMember->m_ObjIndex;
		}
	}

	#endif

	return -1;
}

int CMonsterAIGroup::FindGroupMemberObjectIndex(int GroupNumber,int guid) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(GroupNumber < 0 || GroupNumber >= MAX_MONSTER_AI_GROUP)
	{
		return -1;
	}

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(lpMonsterAIGroupMember->m_Guid == guid)
		{
			return lpMonsterAIGroupMember->m_ObjIndex;
		}
	}

	#endif

	return -1;
}

CMonsterAIGroupMember* CMonsterAIGroup::FindGroupMember(int GroupNumber,int guid) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(GroupNumber < 0 || GroupNumber >= MAX_MONSTER_AI_GROUP)
	{
		return 0;
	}

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(lpMonsterAIGroupMember->m_Guid == guid)
		{
			return lpMonsterAIGroupMember;
		}
	}

	#endif

	return 0;
}

CMonsterAIGroupMember* CMonsterAIGroup::FindGroupMemberToHeal(int ObjIndex,int GroupNumber,int guid,int distance,int rate) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(lpMonsterAIGroupMember->m_ObjIndex == ObjIndex)
		{
			continue;
		}

		if(gObjCalcDistance(&gObj[ObjIndex],&gObj[lpMonsterAIGroupMember->m_ObjIndex]) > distance)
		{
			continue;
		}

		if(((gObj[lpMonsterAIGroupMember->m_ObjIndex].Life*100)/(gObj[lpMonsterAIGroupMember->m_ObjIndex].MaxLife+gObj[lpMonsterAIGroupMember->m_ObjIndex].AddLife)) < rate)
		{
			return lpMonsterAIGroupMember;
		}
	}

	#endif

	return 0;
}

CMonsterAIGroupMember* CMonsterAIGroup::FindGroupMemberToSommon(int ObjIndex,int GroupNumber,int guid) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		if(lpMonsterAIGroupMember->m_ObjIndex == ObjIndex)
		{
			continue;
		}

		if(gObj[lpMonsterAIGroupMember->m_ObjIndex].RegenType == -1)
		{
			continue;
		}

		if(gObj[lpMonsterAIGroupMember->m_ObjIndex].Live == 0)
		{
			return lpMonsterAIGroupMember;
		}
	}

	#endif

	return 0;
}

void CMonsterAIGroup::InitAll() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	for(int n=0;n < MAX_MONSTER_AI_GROUP;n++)
	{
		CMonsterAIGroup::Init(n);
	}

	#endif
}

void CMonsterAIGroup::Init(int GroupNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(GroupNumber < 0 || GroupNumber >= MAX_MONSTER_AI_GROUP)
	{
		return;
	}

	CMonsterAIGroup::DelGroupInstance(GroupNumber);

	for(int n=0;n < MAX_MONSTER_AI_GROUP_MEMBER;n++)
	{
		CMonsterAIGroupMember* lpMonsterAIGroupMember = &CMonsterAIGroup::m_MonsterAIGroupMemberArray[GroupNumber][n];

		if(lpMonsterAIGroupMember->m_Guid == -1)
		{
			continue;
		}

		int index = gObjAddMonster(lpMonsterAIGroupMember->m_MapNumber);

		if(OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[index];

		int px = lpMonsterAIGroupMember->m_StartX;
		int py = lpMonsterAIGroupMember->m_StartY;

		if(lpMonsterAIGroupMember->m_CreateType == 1 && gObjGetRandomFreeLocation(lpMonsterAIGroupMember->m_MapNumber,&px,&py,10,10,100) == 0)
		{
			px = lpMonsterAIGroupMember->m_StartX;
			py = lpMonsterAIGroupMember->m_StartY;
		}

		lpObj->PosNum = -1;
		lpObj->X = px;
		lpObj->Y = py;
		lpObj->TX = px;
		lpObj->TY = py;
		lpObj->OldX = px;
		lpObj->OldY = py;
		lpObj->StartX = px;
		lpObj->StartY = py;
		lpObj->Dir = lpMonsterAIGroupMember->m_StartDir;
		lpObj->Map = lpMonsterAIGroupMember->m_MapNumber;

		if(gObjSetMonster(index,lpMonsterAIGroupMember->MonsterClass) == 0)
		{
			gObjDel(index);
			continue;
		}

		lpObj->GroupNumber = lpMonsterAIGroupMember->m_GroupNumber;
		lpObj->GroupMemberGuid = lpMonsterAIGroupMember->m_Guid;
		lpObj->CurrentAI = lpMonsterAIGroupMember->m_StartAI;
		lpObj->BasicAI = lpMonsterAIGroupMember->m_StartAI;
		lpObj->RegenType = lpMonsterAIGroupMember->m_RegenType;

		lpMonsterAIGroupMember->m_ObjIndex = index;
	}

	#endif
}
