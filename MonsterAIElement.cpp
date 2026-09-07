// MonsterAIElement.cpp: implementation of the CMonsterAIElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonsterAIElement.h"
#include "ReadFile.h"
#include "Path.h"
#include "Util.h"

BOOL CMonsterAIElement::m_DataLoad;
CMonsterAIElementInfo CMonsterAIElement::m_MonsterAIElementInfoArray[MAX_MONSTER_AI_ELEMENT_INFO];
CMonsterAIMovePath CMonsterAIElement::m_MonsterAIMovePath[MAX_MAP];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonsterAIElement::CMonsterAIElement() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIElement::DelAllAIElement();

	#endif
}

CMonsterAIElement::~CMonsterAIElement() // OK
{

}

void CMonsterAIElement::LoadData(char* path) // OK
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

	CMonsterAIElement::DelAllAIElement();

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

					CMonsterAIElementInfo info;

					info.m_ElementNumber = lpReadFile->GetNumber();

					strcpy_s(info.m_ElementName,lpReadFile->GetAsString());

					info.m_ElementClass = lpReadFile->GetAsNumber();

					info.m_ElementType = lpReadFile->GetAsNumber();

					info.m_SuccessRate = lpReadFile->GetAsNumber();

					info.m_DelayTime = lpReadFile->GetAsNumber();

					info.m_TargetType = lpReadFile->GetAsNumber();

					info.m_X = lpReadFile->GetAsNumber();

					info.m_Y = lpReadFile->GetAsNumber();

					if(info.m_ElementNumber >= 0 && info.m_ElementNumber < MAX_MONSTER_AI_ELEMENT_INFO)
					{
						CMonsterAIElement::m_MonsterAIElementInfoArray[info.m_ElementNumber] = info;
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

	CMonsterAIElement::m_DataLoad = 1;

	delete lpReadFile;

	#endif
}

void CMonsterAIElement::DelAllAIElement() // OK
{
	#if(GAMESERVER_UPDATE>=201)

	CMonsterAIElement::m_DataLoad = 0;

	for(int n=0;n < MAX_MONSTER_AI_ELEMENT_INFO;n++)
	{
		CMonsterAIElement::m_MonsterAIElementInfoArray[n].Reset();
	}

	#endif
}

CMonsterAIElementInfo* CMonsterAIElement::FindAIElementInfo(int ElementNumber) // OK
{
	#if(GAMESERVER_UPDATE>=201)

	if(ElementNumber < 0 || ElementNumber >= MAX_MONSTER_AI_ELEMENT_INFO)
	{
		return 0;
	}

	if(CMonsterAIElement::m_MonsterAIElementInfoArray[ElementNumber].m_ElementNumber == ElementNumber)
	{
		return &CMonsterAIElement::m_MonsterAIElementInfoArray[ElementNumber];
	}

	#endif

	return 0;
}
