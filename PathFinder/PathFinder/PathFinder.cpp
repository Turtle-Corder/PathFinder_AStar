﻿// PathFinder.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "PathFinder.h"
#include "Astar.h"
#include "Bresenham.h"

HWND g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
RECT g_WinRect = {0,};
BOOL g_bProcessEnd = FALSE;
BOOL g_bOnProcess = FALSE;

eNODESTATE g_eStateMap[df_TILE_HEIGHT][df_TILE_WIDTH] = { EMPTY, };
COLORREF g_ColorMap[df_TILE_HEIGHT][df_TILE_WIDTH] = { RGB(255,255,255), };

int g_iStartX = 0, g_iStartY = 0;
int g_iEndX = 0, g_iEndY = 0;


BOOL g_bLineTest = FALSE;
BOOL g_bDrawBr = FALSE;
COLORREF g_LineTestMap[df_TILE_HEIGHT][df_TILE_WIDTH] = { RGB(255,255,255), };

int g_iLineStartX = 0, g_iLineStartY = 0;
int g_iLineEndX = 0, g_iLineEndY = 0;

CAStar_Manager* g_pAStarMgr = new CAStar_Manager;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	if (!InitialWindow(hInstance, WndProc))
		goto END;

	InitDrawObjects();
	InitTiles();

	//g_OpenList.SetState(eNODESTATE::OPEN);
	//g_CloseList.SetState(eNODESTATE::CLOSE);

	InvalidateRect(g_hWnd, NULL, TRUE);

    MSG msg;
	msg.message = WM_NULL;
    while (1)
    {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(1);
		}
    }

END:
	//g_OpenList.ClearHeap();
	//g_CloseList.ClearHeap();


    return (int) msg.wParam;
}



//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_TIMER:
	{
		switch (wParam)
		{
		case df_TIMER_STEP_FIND:
		{
			HRESULT hr = g_pAStarMgr->FindProcess();
			if (!hr) g_bProcessEnd = true;

			if (g_bProcessEnd)
			{
				KillTimer(g_hWnd, df_TIMER_STEP_FIND);
				g_bOnProcess = FALSE;

				g_iPathCnt = 0;
				for (auto& pNode : g_pAStarMgr->m_BestList)
				{
					g_vPathPos[g_iPathCnt++] = { pNode->iX, pNode->iY };
					g_eStateMap[pNode->iY][pNode->iX] = eNODESTATE::PURPLE;
				}

				StartInterpolation();
			}

			InvalidateRect(g_hWnd, NULL, TRUE);
		}
			break;
		default:
			break;
		}

	}
		break;

	case WM_LBUTTONDOWN:
	{
		if (g_bOnProcess)
			break;

		int iTileX, iTileY;
		RECT stRect;
		if (!PosToTile(lParam, &iTileX, &iTileY))
			break;
		
		if (GetTileState(iTileX, iTileY) != eNODESTATE::BLOCK)
		{
			SetTileState(eNODESTATE::BLOCK, iTileX, iTileY);
			if (GetInvalidateArea(iTileX, iTileY, &stRect))
				InvalidateRect(g_hWnd, &stRect, TRUE);
		}
	}
	break;

	case WM_RBUTTONDOWN:
	{
		if (g_bOnProcess)
			break;

		int iTileX, iTileY;
		RECT stRect;
		if (!PosToTile(lParam, &iTileX, &iTileY))
			break;

		if (GetTileState(iTileX, iTileY) != eNODESTATE::EMPTY)
		{
			SetTileState(eNODESTATE::EMPTY, iTileX, iTileY);
			if (GetInvalidateArea(iTileX, iTileY, &stRect))
				InvalidateRect(g_hWnd, &stRect, TRUE);
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (g_bOnProcess)
			break;

		int iTileX, iTileY;
		if (!PosToTile(lParam, &iTileX, &iTileY))
			break;

		RECT stRect;
		if (wParam & MK_LBUTTON)
		{
			if (GetTileState(iTileX, iTileY) != eNODESTATE::BLOCK)
			{
				SetTileState(eNODESTATE::BLOCK, iTileX, iTileY);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}
		}

		else if (wParam & MK_RBUTTON)
		{
			if (GetTileState(iTileX, iTileY) != eNODESTATE::EMPTY)
			{
				SetTileState(eNODESTATE::EMPTY, iTileX, iTileY);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}
		}
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		if (g_bOnProcess)
			break;

		int iTileX, iTileY;
		if (!PosToTile(lParam, &iTileX, &iTileY))
			break;

		RECT stRect;
		if (GetTileState(iTileX, iTileY) != eNODESTATE::START)
		{
			if (GetTileState(g_iStartX, g_iStartY) == eNODESTATE::START)
			{
				SetTileState(eNODESTATE::EMPTY, g_iStartX, g_iStartY);
				if (GetInvalidateArea(g_iStartX, g_iStartY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}

			SetTileState(eNODESTATE::START, iTileX, iTileY);
			if (GetInvalidateArea(iTileX, iTileY, &stRect))
				InvalidateRect(g_hWnd, &stRect, TRUE);
			g_iStartX = iTileX; g_iStartY = iTileY;
		}
	}
	break;

	case WM_RBUTTONDBLCLK:
	{
		if (g_bOnProcess)
			break;

		int iTileX, iTileY;
		if (!PosToTile(lParam, &iTileX, &iTileY))
			break;

		RECT stRect;
		if (GetTileState(iTileX, iTileY) != eNODESTATE::END)
		{
			if (GetTileState(g_iEndX, g_iEndY) == eNODESTATE::END)
			{
				SetTileState(eNODESTATE::EMPTY, g_iEndX, g_iEndY);
				if (GetInvalidateArea(g_iEndX, g_iEndY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}

			SetTileState(eNODESTATE::END, iTileX, iTileY);
			if (GetInvalidateArea(iTileX, iTileY, &stRect))
				InvalidateRect(g_hWnd, &stRect, TRUE);
			g_iEndX = iTileX, g_iEndY = iTileY;
		}
	}
	break;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		// normal run
		case VK_F1:
		{
			if (g_bOnProcess)
				break;
	
			HRESULT hr = g_pAStarMgr->PathFind(g_iStartX, g_iStartY, g_iEndX, g_iEndY);
			if (!hr)
			{
				g_bOnProcess = true;
				SetTimer(g_hWnd, df_TIMER_STEP_FIND, df_PERIOD_STEP_FIND, NULL);
			}
		}
		break;
		
		case VK_F3:
		{
			PathClear();
		}
		break;

		case VK_F6:
		{
			ScreenShot();
		}
		break;

		default:
			break;
		}
	}
	break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			Render(hdc);

            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL InitialWindow(HINSTANCE hInstance, WNDPROC WndPorc, TCHAR * pAppName)
{
	WNDCLASSEXW wcex;
	HWND hWnd;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PATHFINDER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PATHFINDER);
	wcex.lpszClassName = pAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	hWnd = CreateWindowEx(0, pAppName, pAppName,
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		df_SCREEN_WIDTH, df_SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		DWORD errRet = GetLastError();
		return FALSE;
	}

	g_hWnd = hWnd;
	g_hInst = hInstance;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	SetRect(&g_WinRect, 0, 0, df_SCREEN_WIDTH, df_SCREEN_HEIGHT);
	AdjustWindowRectEx(&g_WinRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowStyle(g_hWnd));

	return TRUE;
}

void StartInterpolation()
{
	ZeroMemory(g_vLinePos, sizeof(_vec2) * 100);

	//----------------------------------------------------------------------------------------------------
	// 노드 자연스럽게 보간 처리
	//----------------------------------------------------------------------------------------------------
	CBresenham Bresenham;
	int iX = 0, iY = 0;
	bool bRefine = false;

	//--------------------------------------------------
	// node 2개 이상 있는지 검사
	//--------------------------------------------------
	auto iter_start = g_pAStarMgr->m_BestList.begin();
	if (g_pAStarMgr->m_BestList.size() > 2)
	{
		auto iter_terminal = iter_start;
		++iter_terminal;

		g_vLinePos[g_iLineCnt++] = { (*iter_start)->iX, (*iter_start)->iY };

		while (true)
		{
			//--------------------------------------------------
			// 다음 노드 없음, 마지막
			//--------------------------------------------------
			++iter_terminal;
			if (g_pAStarMgr->m_BestList.end() == iter_terminal)
			{
				--iter_terminal;
				g_vLinePos[g_iLineCnt++] = { (*iter_terminal)->iX, (*iter_terminal)->iY };
				break;
			}


			//--------------------------------------------------
			// 보간처리 할 두 위치 셋팅
			//--------------------------------------------------
			Bresenham.SetPosition((*iter_start)->iX, (*iter_start)->iY, (*iter_terminal)->iX, (*iter_terminal)->iY);

			while (true)
			{
				//--------------------------------------------------
				// 보간 작업 수행..
				//--------------------------------------------------
				if (!Bresenham.GetNext(&iX, &iY))
				{
					bRefine = true;
					break;
				}

				//--------------------------------------------------
				// 장애물이 있는지 체크
				//--------------------------------------------------
				if (!g_pAStarMgr->ValidateExpand(iX, iY))
				{
					bRefine = false;
					break;
				}
			}

			//--------------------------------------------------
			// 장애물 있음
			//--------------------------------------------------
			if (!bRefine)
			{
				--iter_terminal;
				g_vLinePos[g_iLineCnt++] = { (*iter_terminal)->iX, (*iter_terminal)->iY };

				if (g_iLineCnt >= (int)g_pAStarMgr->m_BestList.size())
					break;

				iter_start = iter_terminal;
				++iter_terminal;
			}
		}
	}


	g_bDrawBr = TRUE;
}
