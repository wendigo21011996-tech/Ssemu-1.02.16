// UnionManager.h: interface for the CUnionManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_UNION_MEMBER 5

enum eGuildRelation
{
	GUILD_RELATION_NONE = 0,
	GUILD_RELATION_UNION = 1,
	GUILD_RELATION_RIVAL = 2,
};

struct UNION_INFO
{
	UNION_INFO()
	{
		this->Index = 0;

		memset(this->Name,0,sizeof(this->Name));

		this->UnionMemberList.clear();

		this->RivalMemberList.clear();
	}

	int Index;
	char Name[9];
	std::vector<int> UnionMemberList;
	std::vector<int> RivalMemberList;
};

class CUnionManager
{
	CUnionManager();
	virtual ~CUnionManager();
	SingletonInstance(CUnionManager)
public:
	void AddUnion(int index,char* Name);
	void DelUnion(int index);
	UNION_INFO* GetUnion(int index);
	int GetGuildRelationShip(int index,int bindex);
	int GetGuildRelationShipCount(int index,int type);
	bool GetUnionName(int index,char* Name);
	bool GetGuildUnionMemberList(int index,int& count,int*list);
	void SetGuildUnionMemberList(int index,int count,int const* list);
	void SetGuildRivalMemberList(int index,int count,int const* list);
private:
	std::map<int,UNION_INFO*> m_UnionInfoList;
};

#define gUnionManager SingNull(CUnionManager)