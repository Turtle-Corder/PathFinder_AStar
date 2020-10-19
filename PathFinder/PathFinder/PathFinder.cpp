// PathFinder.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "PathFinder.h"

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
BOOL g_bBresenhamEnd = FALSE;
COLORREF g_LineTestMap[df_TILE_HEIGHT][df_TILE_WIDTH] = { RGB(255,255,255), };

int g_iLineStartX = 0, g_iLineStartY = 0;
int g_iLineEndX = 0, g_iLineEndY = 0;



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

	case WM_LBUTTONDOWN:
	{
		if (g_bOnProcess)
			break;

		int iTileX, iTileY;
		RECT stRect;
		if (!PosToTile(lParam, &iTileX, &iTileY))
			break;

		if (g_bLineTest)
		{
			if (g_LineTestMap[iTileY][iTileX] != RGB(0, 255, 0))
			{
				if (g_LineTestMap[g_iLineStartY][g_iLineStartX] == RGB(0, 255, 0))
				{
					g_LineTestMap[g_iLineStartY][g_iLineStartX] = RGB(255, 255, 255);
					if (GetInvalidateArea(g_iLineStartX, g_iLineStartY, &stRect))
						InvalidateRect(g_hWnd, &stRect, TRUE);
				}

				g_LineTestMap[iTileY][iTileX] = RGB(0, 255, 0);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);

				g_iLineStartX = iTileX;
				g_iLineStartY = iTileY;
			}
			else
			{
				g_LineTestMap[iTileY][iTileX] = RGB(255, 255, 255);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}
		}
		else
		{
			if (GetTileState(iTileX, iTileY) != eNODESTATE::BLOCK)
			{
				SetTileState(eNODESTATE::BLOCK, iTileX, iTileY);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}
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

		if (g_bLineTest)
		{
			if (g_LineTestMap[iTileY][iTileX] != RGB(255, 0, 0))
			{
				if (g_LineTestMap[g_iLineEndY][g_iLineEndX] == RGB(255, 0, 0))
				{
					g_LineTestMap[g_iLineEndY][g_iLineEndX] = RGB(255, 255, 255);
					if (GetInvalidateArea(g_iLineEndX, g_iLineEndY, &stRect))
						InvalidateRect(g_hWnd, &stRect, TRUE);
				}

				g_LineTestMap[iTileY][iTileX] = RGB(255, 0, 0);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);

				g_iLineEndX = iTileX;
				g_iLineEndY = iTileY;
			}
			else
			{
				g_LineTestMap[iTileY][iTileX] = RGB(255, 255, 255);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}
		}
		else
		{
			RECT stRect;
			if (GetTileState(iTileX, iTileY) != eNODESTATE::EMPTY)
			{
				SetTileState(eNODESTATE::EMPTY, iTileX, iTileY);
				if (GetInvalidateArea(iTileX, iTileY, &stRect))
					InvalidateRect(g_hWnd, &stRect, TRUE);
			}
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (g_bOnProcess || g_bLineTest)
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
		if (g_bOnProcess || g_bLineTest)
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
		if (g_bOnProcess || g_bLineTest)
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

		}
		break;
		
		case VK_F3:
		{
			AllClear();
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