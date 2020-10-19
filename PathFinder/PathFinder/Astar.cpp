#include "stdafx.h"
#include "Astar.h"


CMinHeap g_OpenList;
CMinHeap g_CloseList;

st_NODE g_stPath[df_PATHS_MAX] = { 0, };
int g_iPathCnt = df_PATHS_MAX;


void PathFind(int iStartX, int iStartY, int iEndX, int iEndY)
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

BOOL FindProcess(int iEndX, int iEndY, st_NODE *pNode, int *pMaxPath)
{
	st_NODE *pOldNode = g_OpenList.Remove();
	g_CloseList.Insert(pOldNode);

	// 길이 없거나 || 목적지를 찾았거나 -> 루프 탈출
	// 값으로 비교? 좌표로 비교?
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

	CreateExpandNode(pOldNode, iEndX, iEndY);
	return FALSE;
}

void CreateExpandNode(st_NODE * pParent, int iEndX, int iEndY)
{
	int iX = pParent->iX;
	int iY = pParent->iY;

	// ↖
	if (ValidateExpand(iX - 1, iY - 1))
	{
		CreateNode(pParent, iX - 1, iY - 1, iEndX, iEndY, df_WEIGHT_DIAGONAL);
	}

	// ↑
	if (ValidateExpand(iX, iY - 1))
	{
		CreateNode(pParent, iX, iY - 1, iEndX, iEndY);
	}

	// ↗
	if (ValidateExpand(iX + 1, iY - 1))
	{
		CreateNode(pParent, iX + 1, iY - 1, iEndX, iEndY, df_WEIGHT_DIAGONAL);
	}

	// ←
	if (ValidateExpand(iX - 1, iY))
	{
		CreateNode(pParent, iX - 1, iY, iEndX, iEndY);
	}

	// →
	if (ValidateExpand(iX + 1, iY))
	{
		CreateNode(pParent, iX + 1, iY, iEndX, iEndY);
	}

	// ↙
	if (ValidateExpand(iX - 1, iY + 1))
	{
		CreateNode(pParent, iX - 1, iY + 1, iEndX, iEndY, df_WEIGHT_DIAGONAL);
	}

	// ↓
	if (ValidateExpand(iX, iY + 1))
	{
		CreateNode(pParent, iX, iY + 1, iEndX, iEndY);
	}

	// ↘
	if (ValidateExpand(iX + 1, iY + 1))
	{
		CreateNode(pParent, iX + 1, iY + 1, iEndX, iEndY, df_WEIGHT_DIAGONAL);
	}
}

bool ValidateExpand(int iX, int iY)
{
	if (iX < 0 || iY < 0 || iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return false;

	if (g_eColorMap[iY][iX] == eNODESTATE::BLOCK)
		return false;

	return true;
}

void CreateNode(st_NODE * pParent, int iX, int iY, int iEndX, int iEndY, int iWeight)
{
	int G = pParent->G + iWeight;

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
		return;
	}

	pOldNode = g_CloseList.GetNode(iX, iY);
	if (pOldNode != NULL)
	{
		//if (pOldNode->G > G)
		//{
		//	pOldNode->pParent = pParent;
		//	pOldNode->G = G;
		//	pOldNode->F = G + pOldNode->H;
		//}
		return;
	}

	st_NODE *pNewNode = new st_NODE();
	pNewNode->pParent = pParent;
	pNewNode->iX = iX;
	pNewNode->iY = iY;
	pNewNode->G = G;
	pNewNode->H = (abs(iX - iEndX) + abs(iY - iEndY)) * 10;
	pNewNode->F = G + pNewNode->H;
	g_OpenList.Insert(pNewNode);
}