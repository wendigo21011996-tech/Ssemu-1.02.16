#pragma once

#define MAX_LOADSTRING 100

#define WM_TIMER_1000 100
#define WM_TIMER_2000 101
#define WM_TIMER_10000 102
#define MAP_VIEWER_TIMER 103

#define WM_JOIN_SERVER_MSG_PROC (WM_USER+1)
#define WM_DATA_SERVER_MSG_PROC (WM_USER+2)

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance,int nCmdShow);
void RenderMenu(HWND hWnd);
void EditMenuLabel(BYTE slot1,BYTE slot2,UINT itemID, LPCSTR newLabel,bool state);
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK Users(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK MapViewer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
