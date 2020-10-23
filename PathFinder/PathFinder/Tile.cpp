#include "stdafx.h"
#include "Tile.h"

void InitTiles()
{
	for (int iHeight = 0; iHeight < df_TILE_HEIGHT; iHeight++)
	{
		for (int iWidth = 0; iWidth < df_TILE_WIDTH; iWidth++)
		{
			SetTileState(eNODESTATE::EMPTY, iWidth, iHeight);
		}
	}
}

BOOL PosToTile(LPARAM lParam, int * iX, int * iY)
{
	int iPosX = GET_X_LPARAM(lParam);
	int iPosY = GET_Y_LPARAM(lParam);

	if (iPosX < 0 || iPosY < 0 || iPosX > df_TILE_WIDTH * df_TILE_SIZE || iPosY > df_TILE_HEIGHT * df_TILE_SIZE)
		return FALSE;

	*iX = iPosX / df_TILE_SIZE;
	*iY = iPosY / df_TILE_SIZE;

	return TRUE;
}

BOOL GetInvalidateArea(int iX, int iY, RECT * pRect)
{
	if (iX < 0 || iY < 0 || iX >= df_TILE_WIDTH, iY >= df_TILE_HEIGHT)
		return FALSE;

	pRect->left = iX * df_TILE_SIZE + 1;
	pRect->top = iY * df_TILE_SIZE + 1;
	pRect->right = (iX + 1) * df_TILE_SIZE + 1;
	pRect->bottom = (iY + 1) * df_TILE_SIZE + 1;
	return TRUE;
}

void SetTileState(eNODESTATE eState, int iX, int iY)
{
	if (iX < 0 || iY < 0 || iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return;

	g_eStateMap[iY][iX] = eState;
}

eNODESTATE GetTileState(int iX, int iY)
{
	if (iX < 0 || iY < 0 || iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return eNODESTATE::EMPTY;

	return g_eStateMap[iY][iX];
}

void SetTileColor(COLORREF color, int iX, int iY)
{
	if (iX < 0 || iY < 0 || iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return;

	g_ColorMap[iY][iX] = color;
}

COLORREF GetTileColor(int iX, int iY)
{
	if (iX < 0 || iY < 0 || iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return RGB(255, 255, 255);

	return g_ColorMap[iY][iX];
}

void Reset()
{
	g_bProcessEnd = FALSE;
	g_bOnProcess = FALSE;
	g_bDrawBr = FALSE;
}

void ResetAttribute()
{
	for (int iHeight = 0; iHeight < df_TILE_HEIGHT; iHeight++)
	{
		for (int iWidth = 0; iWidth < df_TILE_WIDTH; iWidth++)
		{

		}
	}
}

void PathClear()
{
	Reset();

	for (int iHeight = 0; iHeight < df_TILE_HEIGHT; iHeight++)
	{
		for (int iWidth = 0; iWidth < df_TILE_WIDTH; iWidth++)
		{
			SetTileColor(RGB(255, 255, 255), iWidth, iHeight);

			if (GetTileState(iWidth, iHeight) != eNODESTATE::BLOCK)
				SetTileState(eNODESTATE::EMPTY, iWidth, iHeight);
		}
	}

	SetTileState(eNODESTATE::START, g_iStartX, g_iStartY);
	SetTileState(eNODESTATE::END, g_iEndX, g_iEndY);

	InvalidateRect(g_hWnd, NULL, TRUE);
}

void AllClear()
{
	Reset();
	InitTiles();
	InvalidateRect(g_hWnd, NULL, TRUE);
}

void BresenhamClear()
{
	//g_bBresenhamEnd = FALSE;
	//g_iLineTestPathCnt = df_PATHS_MAX;

	//for (int iHeight = 0; iHeight < df_TILE_HEIGHT; iHeight++)
	//{
	//	for (int iWidth = 0; iWidth < df_TILE_WIDTH; iWidth++)
	//	{
	//		g_LineTestMap[iHeight][iWidth] = RGB(255, 255, 255);
	//	}
	//}

	//InvalidateRect(g_hWnd, NULL, TRUE);
}