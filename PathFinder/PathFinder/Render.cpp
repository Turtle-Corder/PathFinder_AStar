#include "stdafx.h"
#include "Render.h"


HBRUSH g_WhiteBrush = NULL;
HBRUSH g_RedBrush = NULL;
HBRUSH g_GreenBrush = NULL;
HBRUSH g_GreyBrush = NULL;
HBRUSH g_BlueBrush = NULL;
HBRUSH g_YellowBrush = NULL;
HBRUSH g_PurpleBrush = NULL;
HBRUSH g_ColorBrush = NULL;

HPEN g_RectPen = NULL;
HPEN g_BresenhamPen = NULL;

TCHAR g_szShotName[64];


void InitDrawObjects()
{
	g_WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	g_RedBrush = CreateSolidBrush(RGB(255, 0, 0));
	g_GreenBrush = CreateSolidBrush(RGB(0, 255, 0));
	g_GreyBrush = CreateSolidBrush(RGB(97, 97, 97));
	g_BlueBrush = CreateSolidBrush(RGB(0, 0, 255));
	g_YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	g_PurpleBrush = CreateSolidBrush(RGB(137, 119, 173));
	g_RectPen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
	g_BresenhamPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
}

void Render(HDC hDC)
{
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, g_WinRect.right, g_WinRect.bottom);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, g_WinRect.right, g_WinRect.bottom, NULL, 0, 0, WHITENESS);

	DrawGrid(hMemDC);
	if (g_bLineTest)
	{
		DrawTIle(hMemDC);
//		DrawBresenham(hMemDC);
	}
	else
	{
		DrawTIle(hMemDC);
		DrawPathLine(hMemDC);
		DrawKeyInfo(hMemDC);
	}

	BitBlt(hDC, 0, 0, g_WinRect.right, g_WinRect.bottom, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
}

void DrawGrid(HDC hDC)
{
	for (int iHeight = 0; iHeight <= df_TILE_HEIGHT; iHeight++)
	{
		MoveToEx(hDC, 0, df_TILE_SIZE * iHeight, NULL);
		LineTo(hDC, df_TILE_WIDTH * df_TILE_SIZE, df_TILE_SIZE * iHeight);

		for (int iWidth = 0; iWidth <= df_TILE_WIDTH; iWidth++)
		{
			MoveToEx(hDC, df_TILE_SIZE * iWidth, 0, NULL);
			LineTo(hDC, df_TILE_SIZE * iWidth, df_TILE_HEIGHT * df_TILE_SIZE);
		}
	}
}

void DrawTIle(HDC hDC)
{
	HBRUSH hOldBrush = NULL;
	HPEN hOldPen = (HPEN)SelectObject(hDC, g_RectPen);

	for (int iHeight = 0; iHeight < df_TILE_HEIGHT; iHeight++)
	{
		for (int iWidth = 0; iWidth < df_TILE_WIDTH; iWidth++)
		{
			switch (g_eStateMap[iHeight][iWidth])
			{
			case eNODESTATE::EMPTY:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_WhiteBrush);
				break;

			case eNODESTATE::START:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_GreenBrush);
				break;

			case eNODESTATE::END:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_RedBrush);
				break;

			case eNODESTATE::BLOCK:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_GreyBrush);
				break;

			case eNODESTATE::BLUE:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_BlueBrush);
				break;

			case eNODESTATE::YELLOW:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_YellowBrush);
				break;

			case eNODESTATE::PURPLE:
				hOldBrush = (HBRUSH)SelectObject(hDC, g_PurpleBrush);
				break;

			}

			Rectangle(hDC, (iWidth * df_TILE_SIZE) + 1, (iHeight * df_TILE_SIZE) + 1,
				((iWidth + 1) * df_TILE_SIZE) + 1, ((iHeight + 1) * df_TILE_SIZE) + 1);

			SelectObject(hDC, hOldBrush);
			if (g_ColorBrush != NULL)
			{
				DeleteObject(g_ColorBrush);
				g_ColorBrush = NULL;
			}
		}
	}

	SelectObject(hDC, hOldPen);
}

void DrawPathLine(HDC hDC)
{

}

void DrawKeyInfo(HDC hDC)
{
	TextOut(hDC, 820, 120, _T("L.Click   : Obstacle"), 20);
	TextOut(hDC, 820, 140, _T("R.Click   : Empty"), 17);
	TextOut(hDC, 820, 160, _T("LD.Click : Start"), 16);
	TextOut(hDC, 820, 180, _T("RD.Click : End"), 14);

	TextOut(hDC, 820, 220, _T("F1           : Normal Run"), 25);
	TextOut(hDC, 820, 260, _T("F3           : Path Clear"), 25);
	TextOut(hDC, 820, 320, _T("F6           : ScreenShot"), 25);
}

void DrawBresenham(HDC hDC)
{
	HBRUSH hOldBrush = NULL;
	HPEN hOldPen = (HPEN)SelectObject(hDC, g_RectPen);

	SelectObject(hDC, hOldPen);
}

BOOL GetTimeStamp(TCHAR * szName)
{
	TCHAR szTime[17];
	time_t timer;
	struct tm TM;

	ZeroMemory(g_szShotName, sizeof(g_szShotName));
	time(&timer);
	if (localtime_s(&TM, &timer) != 0) return FALSE;

	_stprintf_s(szTime, sizeof(szTime) / sizeof(TCHAR), _T("%02d%02d_%02d%02d%02d"),
		TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);

	_tcscat_s(g_szShotName, sizeof(g_szShotName) / sizeof(TCHAR), szName);
	_tcscat_s(g_szShotName, sizeof(g_szShotName) / sizeof(TCHAR), szTime);
	_tcscat_s(g_szShotName, sizeof(g_szShotName) / sizeof(TCHAR), _T(".bmp"));
	return TRUE;
}

BOOL ScreenShot()
{
	HDC hDC = GetDC(g_hWnd);
	if (!GetTimeStamp())
	{
		ReleaseDC(g_hWnd, hDC);
		return FALSE;
	}

	BYTE *bypData = NULL;
	int iFromX = 0, iFromY = 0;
	int iWidth = 0, iHeight = 0;

	HDC hShotDC = CreateCompatibleDC(hDC);
	iWidth = (df_TILE_SIZE * df_TILE_WIDTH) + 1;
	iHeight = (df_TILE_SIZE * df_TILE_HEIGHT) + 1;

	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hShotDC, hBitmap);

	BitBlt(hShotDC, 0, 0, iWidth, iHeight, hDC, 0, 0, SRCCOPY);

	FILE *fp = NULL;

	if (_tfopen_s(&fp, g_szShotName, _T("wb")) != 0)
	{
		SelectObject(hShotDC, hOldBitmap);
		DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
		return FALSE;
	}

	BITMAP bitmap;
	BITMAPINFO info;
	ZeroMemory(&info, sizeof(info));
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biBitCount = 0;

	if (GetDIBits(hShotDC, hBitmap, 0, 0, NULL, &info, DIB_RGB_COLORS) == 0)
	{
		SelectObject(hShotDC, hOldBitmap);
		DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
	}
	bitmap.bmWidth = info.bmiHeader.biWidth;
	bitmap.bmHeight = info.bmiHeader.biHeight;

	BITMAPFILEHEADER fileheader;
	fileheader.bfType = 0x4d42;
	fileheader.bfSize = (((3 * bitmap.bmWidth + 3) & ~3) * bitmap.bmHeight) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	fileheader.bfReserved1 = fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	info.bmiHeader.biCompression = 0;

	if (1 > fwrite(&fileheader, sizeof(BITMAPFILEHEADER), 1, fp))
	{
		SelectObject(hShotDC, hOldBitmap); 
		DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
		return FALSE;
	}

	if (1 > fwrite(&info, sizeof(BITMAPINFOHEADER), 1, fp))
	{
		SelectObject(hShotDC, hOldBitmap);
		DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
		return FALSE;
	}

	bypData = new BYTE[info.bmiHeader.biSizeImage];
	if (GetDIBits(hShotDC, hBitmap, 0, bitmap.bmHeight, bypData, &info, DIB_RGB_COLORS) == 0)
	{
		SelectObject(hShotDC, hOldBitmap);
		DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
		return FALSE;
	}

	if (bypData == NULL)
	{
		SelectObject(hShotDC, hOldBitmap);
		DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
		return FALSE;
	}

	if (1 > fwrite(bypData, info.bmiHeader.biSizeImage, 1, fp))
	{
	}

	SelectObject(hShotDC, hOldBitmap);
	DeleteObject(hBitmap);	DeleteDC(hShotDC); ReleaseDC(g_hWnd, hDC);
	fclose(fp);
	delete bypData;

	return TRUE;
}
