// Filter.cpp: implementation of the CFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Filter.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilter::CFilter() // OK
{
	this->m_FilterInfo.clear();
}

CFilter::~CFilter() // OK
{

}

void CFilter::Load(char* path) // OK
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

	this->m_FilterInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadFile->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end", lpReadFile->GetString()) == 0)
			{
				break;
			}

			char Syntax[32] = {0};

			strcpy_s(Syntax,lpReadFile->GetString());

			this->m_FilterInfo.push_back(Syntax);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadFile->GetLastError());
	}

	delete lpReadFile;
}

void CFilter::CheckSyntax(char* text) // OK
{
	for(std::vector<std::string>::iterator it = this->m_FilterInfo.begin(); it != this->m_FilterInfo.end(); it++)
	{
		char* temp = text;

		std::string label = *it;

		while(true)
		{
			temp = strstr(temp,label.c_str());

			if(temp == 0)
			{
				break;
			}

			int len = label.size();

			memset(temp,0x2A,len);

			temp += len;
		}
	}
}