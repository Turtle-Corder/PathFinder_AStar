#pragma once

#include "resource.h"

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern RECT g_WinRect;
extern BOOL g_bProcessEnd;
extern BOOL g_bOnProcess;

extern BOOL g_bLineTest;
extern BOOL g_bBresenhamEnd;

extern COLORREF g_LineTestMap[df_TILE_HEIGHT][df_TILE_WIDTH];


BOOL InitialWindow(HINSTANCE hInstance, WNDPROC WndPorc, TCHAR *pAppName = (TCHAR*)_T("PathFinder"));