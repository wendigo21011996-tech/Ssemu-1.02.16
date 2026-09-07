// DefaultClassFreebies.cpp: implementation of the CDefaultClassFreebies class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DefaultClassFreebies.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaultClassFreebies::CDefaultClassFreebies() // OK
{
	memset(this->m_DefaultClassFreebiesInfo,0,sizeof(this->m_DefaultClassFreebiesInfo));
}

CDefaultClassFreebies::~CDefaultClassFreebies() // OK
{

}

void CDefaultClassFreebies::Load(char* path) // OK
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

	memset(this->m_DefaultClassFreebiesInfo,0,sizeof(this->m_DefaultClassFreebiesInfo));

	try
	{
		while (true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpReadFile->GetNumber();

			while (true)
			{
				if(section == 0)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					DEFAULT_CLASS_FREEBIES_INFO info;

					info.Class = lpReadFile->GetNumber();

					info.Money = lpReadFile->GetAsNumber();

					info.LevelUpPoint = lpReadFile->GetAsNumber();

					this->SetInfo(info);
				}
				else if(section == 1)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					DEFAULT_CLASS_FREEBIES_ITEM_INFO info;

					memset(&info,0,sizeof(info));

					info.Class = lpReadFile->GetNumber();

					info.ItemIndex = SafeGetItem(GET_ITEM(lpReadFile->GetAsNumber(),lpReadFile->GetAsNumber()));

					info.Level = lpReadFile->GetAsNumber();

					info.Dur = lpReadFile->GetAsNumber();

					info.Option1 = lpReadFile->GetAsNumber();

					info.Option2 = lpReadFile->GetAsNumber();

					info.Option3 = lpReadFile->GetAsNumber();

					info.NewOption = lpReadFile->GetAsNumber();

					info.SetOption = lpReadFile->GetAsNumber();

					info.Duration = lpReadFile->GetAsNumber();

					this->SetItemInfo(info);
				}
				else if(section == 2)
				{
					if(strcmp("end",lpReadFile->GetAsString()) == 0)
					{
						break;
					}

					DEFAULT_CLASS_FREEBIES_BUFF_INFO info;

					info.Class = lpReadFile->GetNumber();

					info.Effect = lpReadFile->GetAsNumber();

					info.Type = lpReadFile->GetAsNumber();

					info.Power[0] = lpReadFile->GetAsNumber();

					info.Power[1] = lpReadFile->GetAsNumber();

					info.Power[2] = lpReadFile->GetAsNumber();

					info.Power[3] = lpReadFile->GetAsNumber();

					info.Duration = lpReadFile->GetAsNumber();

					this->SetBuffInfo(info);
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CDefaultClassFreebies::SetInfo(DEFAULT_CLASS_FREEBIES_INFO info) // OK
{
	if(CHECK_RANGE(info.Class,MAX_CLASS) == 0)
	{
		return;
	}

	this->m_DefaultClassFreebiesInfo[info.Class] = info;
}

void CDefaultClassFreebies::SetItemInfo(DEFAULT_CLASS_FREEBIES_ITEM_INFO info) // OK
{
	if(CHECK_RANGE(info.Class,MAX_CLASS) == 0)
	{
		return;
	}

	this->m_DefaultClassFreebiesInfo[info.Class].ItemList.push_back(info);
}

void CDefaultClassFreebies::SetBuffInfo(DEFAULT_CLASS_FREEBIES_BUFF_INFO info) // OK
{
	if(CHECK_RANGE(info.Class,MAX_CLASS) == 0)
	{
		return;
	}

	this->m_DefaultClassFreebiesInfo[info.Class].BuffList.push_back(info);
}

void CDefaultClassFreebies::GetCharacterFreebies(LPOBJ lpObj,int status) // OK
{
	if(status == 0)
	{
		return;
	}

	DEFAULT_CLASS_FREEBIES_INFO lpInfo = this->m_DefaultClassFreebiesInfo[lpObj->Class];

	if(lpInfo.Money > 0)
	{
		if(gObjCheckMaxMoney(lpObj->Index,lpInfo.Money) == 0)
		{
			lpObj->Money = MAX_MONEY;
		}
		else
		{
			lpObj->Money += lpInfo.Money;
		}

		GCMoneySend(lpObj->Index,lpObj->Money);
	}

	if(lpInfo.LevelUpPoint > 0)
	{
		lpObj->LevelUpPoint += lpInfo.LevelUpPoint;
		GCNewCharacterInfoSend(lpObj);
		GDCharacterInfoSaveSend(lpObj->Index);
	}

	for(std::vector<DEFAULT_CLASS_FREEBIES_ITEM_INFO>::iterator it = lpInfo.ItemList.begin(); it != lpInfo.ItemList.end(); it++)
	{
		BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

		for(int u = 0; u<it->SocketCount && it->SocketCount <= MAX_SOCKET_OPTION; u++)
		{
			ItemSocketOption[u] = 0xFE;
		}

		GDCreateItemSend(lpObj->Index,0xEB,0,0,it->ItemIndex,it->Level,it->Dur,it->Option1,it->Option2,it->Option3,-1,it->NewOption,it->SetOption,0,0,ItemSocketOption,0xFF,((it->Duration>0) ? ((DWORD)time(0)+it->Duration) : 0));
	}

	for(std::vector<DEFAULT_CLASS_FREEBIES_BUFF_INFO>::iterator it = lpInfo.BuffList.begin(); it != lpInfo.BuffList.end(); it++)
	{
		if(gEffectManager->GetInfo(it->Effect) == 0)
		{
			continue;
		}

		gEffectManager->AddEffect(lpObj,(it->Type == 0) ? 0 : 1,it->Effect,(it->Type == 0) ? it->Duration : (int)(time(0)+it->Duration),it->Power[0],it->Power[1],it->Power[2],it->Power[3]);
	}
}