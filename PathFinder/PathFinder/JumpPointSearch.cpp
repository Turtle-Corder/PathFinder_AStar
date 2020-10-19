#include "stdafx.h"
#include "JumpPointSearch.h"


CMinHeap g_OpenList;
CMinHeap g_CloseList;
st_NODE g_stPath[df_PATHS_MAX] = { 0, };
int g_iPathCnt = df_PATHS_MAX;

void StartJump(int iStartX, int iStartY, int iEndX, int iEndY)
{
	// 시작점 노드 생성, 셋팅
	st_NODE *newNode = new st_NODE();
	newNode->pParent = NULL;
	newNode->iX = iStartX;
	newNode->iY = iStartY;
	newNode->G = 0;
	newNode->H = abs(iStartX - iEndX) + abs(iStartY - iEndY);
	newNode->F = newNode->G + newNode->H;
	g_OpenList.Insert(newNode);

	SetTimer(g_hWnd, df_TIMER_STEP_FIND, df_PERIOD_STEP_FIND, NULL);
}

BOOL JumpProcess(int iEndX, int iEndY, st_NODE *pNode, int *pMaxPath)
{
	st_NODE *pOldNode = g_OpenList.Remove();
	g_CloseList.Insert(pOldNode);

	if (pOldNode == NULL || pOldNode->H == 0)
	{
		// pNode에 담아주고 끝내야함 DomainToIP 처럼
		int iPathCnt = 0;
		while (pOldNode != NULL && iPathCnt < *pMaxPath)
		{
			pNode[iPathCnt] = *pOldNode;
			pOldNode = pOldNode->pParent;
			iPathCnt++;
		}
		// path max count renew
		*pMaxPath = iPathCnt;
		return TRUE;
	}

	ExpandJumpNode(pOldNode, iEndX, iEndY);
	return FALSE;
}

BOOL Jump(int iX, int iY, int iDir, int * iJumpX, int * iJumpY, COLORREF Color)
{
	// 범위 밖, 벽이면 중단
	if (!CHECKWALKABLE(iX, iY))
		return FALSE;

	if (g_eStateMap[iY][iX] == eNODESTATE::END)
	{
		*iJumpX = iX;
		*iJumpY = iY;
		return TRUE;
	}

	g_ColorMap[iY][iX] = Color;

	if (iDir == df_DIR_UU)
	{
		if ((CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX - 1, iY))
			|| (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX, iY - 1, df_DIR_UU, iJumpX, iJumpY, Color);
	}

	else if (iDir == df_DIR_DD)
	{
		if ((CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX - 1, iY))
			|| (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX, iY + 1, df_DIR_DD, iJumpX, iJumpY, Color);
	}

	else if (iDir == df_DIR_LL)
	{
		if ((CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			|| (CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX - 1, iY , df_DIR_LL, iJumpX, iJumpY, Color);
	}

	else if (iDir == df_DIR_RR)
	{
		if ((CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			|| (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX + 1, iY, df_DIR_RR, iJumpX, iJumpY, Color);
	}

	else if (iDir == df_DIR_LU)
	{
		if ((CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1))
			|| (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY - 1, df_DIR_UU, iJumpX, iJumpY, Color)
		|| Jump(iX - 1, iY, df_DIR_LL, iJumpX, iJumpY, Color))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX - 1, iY - 1, df_DIR_LU, iJumpX, iJumpY, Color))
		{
			//*iJumpX = iX;
			//*iJumpY = iY;
			return TRUE;
		}
	}
	
	else if (iDir == df_DIR_LD)
	{
		if ((CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			|| (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY + 1, df_DIR_DD, iJumpX, iJumpY, Color)
			|| Jump(iX - 1, iY, df_DIR_LL, iJumpX, iJumpY, Color))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX - 1, iY + 1, df_DIR_LD, iJumpX, iJumpY, Color))
		{
			//*iJumpX = iX;
			//*iJumpY = iY;
			return TRUE;
		}
	}

	else if (iDir == df_DIR_RU)
	{
		if ((CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX - 1, iY))
			|| (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY - 1, df_DIR_UU, iJumpX, iJumpY, Color)
			|| Jump(iX + 1, iY, df_DIR_RR, iJumpX, iJumpY, Color))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX + 1, iY - 1, df_DIR_RU, iJumpX, iJumpY, Color))
		{
			//*iJumpX = iX;
			//*iJumpY = iY;
			return TRUE;
		}
	}

	else if (iDir == df_DIR_RD)
	{
		if ((CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX - 1, iY))
			|| (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY + 1, df_DIR_DD, iJumpX, iJumpY, Color)
			|| Jump(iX + 1, iY, df_DIR_RR, iJumpX, iJumpY, Color))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX + 1, iY + 1, df_DIR_RD, iJumpX, iJumpY, Color))
		{
			//*iJumpX = iX;
			//*iJumpY = iY;
			return TRUE;
		}
	}

	return FALSE;
}

//BOOL CheckWalkable(int iX, int iY)
//{
//	if (((0 <= iX && iX < df_TILE_WIDTH) && (0 <= iY && iY < df_TILE_HEIGHT)) && g_eStateMap[iY][iX] != eNODESTATE::BLOCK)
//		return TRUE;
//
//	return FALSE;
//}

void ExpandJumpNode(st_NODE * pNode, int iEndX, int iEndY)
{
	int iX = pNode->iX;
	int iY = pNode->iY;

	int iNewX = 0;
	int iNewY = 0;

	// color pickup
	COLORREF Color = RGB(rand() % 255, rand() % 255, rand() % 255);
	g_PrevColor = g_CurColor;
	g_CurColor = Color;

	// 시작노드
	if (pNode->pParent == NULL)
	{
		// 8방향 점프

		// ↑
		if (Jump(iX, iY - 1, df_DIR_UU, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↓
		if (Jump(iX, iY + 1, df_DIR_DD, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ←
		if (Jump(iX - 1, iY, df_DIR_LL, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// →
		if (Jump(iX + 1, iY, df_DIR_RR, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↖
		if (Jump(iX - 1, iY - 1, df_DIR_LU, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↙
		if (Jump(iX - 1, iY + 1, df_DIR_LD, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↗
		if (Jump(iX + 1, iY - 1, df_DIR_RU, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↘
		if (Jump(iX + 1, iY + 1, df_DIR_RD, &iNewX, &iNewY, Color))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

	}
	else
	{
		int iDX = iX - pNode->pParent->iX;
		int iDY = iY - pNode->pParent->iY;

		// ↑
		if (iDX == 0 && iDY < 0)
		{
			if (Jump(iX, iY - 1, df_DIR_UU, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY - 1, df_DIR_LU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY - 1, df_DIR_RU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ←
		else if (iDX < 0 && iDY == 0)
		{
			if (Jump(iX - 1, iY, df_DIR_LL, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX - 1, iY - 1, df_DIR_LU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX - 1, iY + 1, df_DIR_LD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// →
		else if (iDX > 0 && iDY == 0)
		{
			if (Jump(iX + 1, iY, df_DIR_RR, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX + 1, iY - 1, df_DIR_RU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX + 1, iY + 1, df_DIR_RD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↓
		else if (iDX == 0 && iDY > 0)
		{
			if (Jump(iX, iY + 1, df_DIR_DD, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX-1, iY+1) && !CHECKWALKABLE(iX-1, iY))
			{
				if (Jump(iX - 1, iY + 1, df_DIR_LD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY + 1, df_DIR_RD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↖
		else if (iDX < 0 && iDY < 0)
		{
			if (Jump(iX, iY - 1, df_DIR_UU, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump(iX - 1, iY, df_DIR_LL, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX - 1, iY - 1, df_DIR_LU, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX - 1, iY + 1, df_DIR_LD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY - 1, df_DIR_RU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↗
		else if (iDX > 0 && iDY < 0)
		{
			if (Jump(iX, iY - 1, df_DIR_UU, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX + 1, iY, df_DIR_RR, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX + 1, iY - 1, df_DIR_RU, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY - 1, df_DIR_LU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX + 1, iY + 1, df_DIR_RD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↙
		else if (iDX < 0 && iDY > 0)
		{
			if (Jump(iX, iY + 1, df_DIR_DD, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump(iX - 1, iY, df_DIR_LL, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX - 1, iY + 1, df_DIR_LD, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX - 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX - 1, iY - 1, df_DIR_LU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY + 1) && !CHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY + 1, df_DIR_RD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↘
		else if (iDX > 0 && iDY > 0)
		{
			if (Jump(iX, iY + 1, df_DIR_DD, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump(iX + 1, iY, df_DIR_RR, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX + 1, iY + 1, df_DIR_RD, &iNewX, &iNewY, Color))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (CHECKWALKABLE(iX - 1, iY + 1) && !CHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY + 1, df_DIR_LD, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (CHECKWALKABLE(iX + 1, iY - 1) && !CHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX + 1, iY - 1, df_DIR_RU, &iNewX, &iNewY, Color))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}
	}
}

st_NODE *CreateJumpNode(st_NODE *pParent, int iX, int iY, int iEndX, int iEndY)
{	
	int idG = abs(iX - pParent->iX) + abs(iY - pParent->iY);
	int G = pParent->G + idG;

	st_NODE *pOldNode = g_OpenList.GetNode(iX, iY);
	if (pOldNode != NULL)
	{
		if (pOldNode->G > G)
		{
			pOldNode->pParent = pParent;
			pOldNode->G = G;
			pOldNode->F = G + pOldNode->H;
			g_OpenList.ResortHeap(pOldNode);
		}
		return NULL;
	}

	pOldNode = g_CloseList.GetNode(iX, iY);
	if (pOldNode != NULL)
	{
		if (pOldNode->G > G)
		{
			pOldNode->pParent = pParent;
			pOldNode->G = G;
			pOldNode->F = G + pOldNode->H;
		}
		return NULL;
	}

	st_NODE *pNewNode = new st_NODE();
	pNewNode->pParent = pParent;
	pNewNode->iX = iX;
	pNewNode->iY = iY;
	pNewNode->G = G;
	pNewNode->H = (abs(iX - iEndX) + abs(iY - iEndY));
	pNewNode->F = G + pNewNode->H;
	g_OpenList.Insert(pNewNode);

	return pNewNode;
}