// Tick.h: interface for the CTick class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CTick
{
public:
	CTick();
	virtual ~CTick();
	void Set(DWORD time);
private:
	DWORD TickCount;
};