// ReadFile.h: interface for the CReadFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#define READ_FILE_ALLOC_ERROR "[%s] Could not alloc memory for ReadFile"

enum eTokenResult
{
	TOKEN_ERROR = -1,
	TOKEN_END = 0,
	TOKEN_NUMBER = 1,
	TOKEN_STRING = 2,
};

class CReadFile
{
public:
	CReadFile();
	~CReadFile();
	bool SetBuffer(char* path);
	char* GetLastError();
	eTokenResult GetToken();
	int GetNumber();
	int GetAsNumber();
	float GetFloatNumber();
	float GetAsFloatNumber();
	char* GetString();
	char* GetAsString();
private:
	char m_path[100];
	FILE* m_file;
	float m_number;
	char m_string[100];
	int m_error;
};