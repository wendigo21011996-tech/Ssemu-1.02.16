// ConnectMember.h: interface for the CConnectMember class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct CONNECT_MEMBER_INFO
{
	char Account[11];
};

class CConnectMember
{
	CConnectMember();
	virtual ~CConnectMember();
	SingletonInstance(CConnectMember)
public:
	void Load(char* path);
	bool CheckAccount(char* Account);
private:
	std::map<std::string,CONNECT_MEMBER_INFO> m_ConnectMemberInfo;
};

#define gConnectMember SingNull(CConnectMember)