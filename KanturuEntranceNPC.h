// KanturuEntranceNPC.h: interface for the CKanturuEntranceNPC class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CKanturuEntranceNPC
{
	CKanturuEntranceNPC();
	virtual ~CKanturuEntranceNPC();
	SingletonInstance(CKanturuEntranceNPC)
public:
	void CGKanturuEnterInfoRecv(int aIndex);
	void CGKanturuEnterRecv(int aIndex);
};

#define gKanturuEntranceNPC SingNull(CKanturuEntranceNPC)