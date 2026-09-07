// Filter.h: interface for the CFilter class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CFilter
{
	CFilter();
	virtual ~CFilter();
	SingletonInstance(CFilter)
public:
	void Load(char* path);
	void CheckSyntax(char* text);
private:
	std::vector<std::string> m_FilterInfo;
};

#define gFilter SingNull(CFilter)