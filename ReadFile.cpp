// ReadFile.cpp: implementation of the CReadFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReadFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReadFile::CReadFile()  // OK
{
	this->m_error = 1;
}

CReadFile::~CReadFile()  // OK
{
	if(this->m_file != 0) 
	{
		fclose(this->m_file);
	}
}

bool CReadFile::SetBuffer(char* path)  // OK
{
	strcpy_s(this->m_path,path);

	if(fopen_s(&this->m_file,path,"rt") != 0)
	{
		this->m_error = 0;
		return 0;
	}
	
	BYTE buffer[4];

	fread(buffer,1,4,this->m_file);

	rewind(this->m_file);

	if(buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) 
	{
		fseek(this->m_file,3,SEEK_SET);
	}

	return 1;
}

char* CReadFile::GetLastError() // OK
{
	static char buff[256] = {0};
	
	switch(this->m_error)
	{
		case 0:
			sprintf_s(buff,"[%s] Could not open file",this->m_path);
			break;
		case 1:
			sprintf_s(buff,"[%s] File was not configured correctly",this->m_path);
			break;
	}

	return buff;
}

eTokenResult CReadFile::GetToken()  // OK
{
	char ch, *p, str[100];

	this->m_string[0] = '\0';

	do
	{
		if((ch = fgetc(this->m_file)) == -1)
		{
			return TOKEN_END;
		}

		if(ch == '/')
		{
			if((ch = fgetc(this->m_file)) == '/')
			{
				while((ch != '\n') && (ch != -1))
				{
					ch = fgetc(this->m_file);
				}

				if(ch == -1)
				{
					return TOKEN_END;
				}
			}
		}
	} while(isspace(ch) != 0);

	if(isdigit(ch) != 0 || ch == '.' || ch == '-' || ch == '*')
	{
		ungetc(ch,this->m_file);

		p = str;
		
		while(((ch = getc(this->m_file)) != -1) && (isdigit(ch) != 0 || ch == '.' || ch == '-' || ch == '*'))
		{
			if(p - str < sizeof(str)-1)
			{
				*p++ = ch;
			} 
			else 
			{
				break;
			}
		}

		*p = 0;

		if(strcmp(str,"*") == 0)
		{
			this->m_number = -1;
		}
		else
		{
			this->m_number = (float)atof(str);
		}

		return TOKEN_NUMBER;
	}
	else if(ch == '"')
	{
		p = this->m_string;

		while(((ch = getc(this->m_file)) != -1) && (ch != '"'))
		{
			*p++ = ch;
		}

		if(ch != '"')
		{
			ungetc(ch,this->m_file);
		}

		*p = 0;

		return TOKEN_STRING;
	}
	else if(isalpha(ch) != 0)
	{
		p = this->m_string;

		*p++ = ch;

		while(((ch = getc(this->m_file)) != -1) && (ch == '.' || ch == '_' || isalnum(ch) != 0))
		{
			*p++ = ch;
		}

		ungetc(ch,this->m_file);
		
		*p = 0;
		
		return TOKEN_STRING;
	}

	return TOKEN_ERROR;
}

int CReadFile::GetNumber()  // OK
{
	return (int)this->m_number;
}

int CReadFile::GetAsNumber()  // OK
{
	this->GetToken();

	return (int)this->m_number;
}

float CReadFile::GetFloatNumber()  // OK
{
	return this->m_number;
}

float CReadFile::GetAsFloatNumber()  // OK
{
	this->GetToken();

	return this->m_number;
}

char* CReadFile::GetString()  // OK
{
	return this->m_string;
}

char* CReadFile::GetAsString()  // OK
{
	this->GetToken();

	return this->m_string;
}