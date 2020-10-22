#include "stdafx.h"
#include "Astar.h"

CAStar_Manager::CAStar_Manager()
{
	m_BsetList.clear();
	m_iMaxFindCount = 100;
}

CAStar_Manager::~CAStar_Manager()
{
	for (auto& pNode : m_BsetList)
		delete pNode;

	m_BsetList.clear();
}

void CAStar_Manager::PathFind(st_NODE * pStart, st_NODE * pGoal)
{
	if (!pStart || !pGoal)
		return;

	if (pStart == pGoal)
		return;

	st_NODE* pNewNode = new st_NODE;
	pNewNode->pParent = nullptr;
	pNewNode->iX = pStart->iX;
	pNewNode->iY = pStart->iY;
	pNewNode->G = 0;
	pNewNode->H = abs(pGoal->iX - pStart->iX) + abs(pGoal->iY - pStart->iY);
	pNewNode->F = pNewNode->G + pNewNode->H;
	
	m_OpenList.push_back(pNewNode);

	m_pGoal = pGoal;
	// TODO : timer setting
}

bool CAStar_Manager::FindProcess()
{
	//--------------------------------------------------
	// 최대 탐색 가능한 기회를 넘어서면 실패로 처리
	//--------------------------------------------------
	++m_iCurFindCount;
	if (m_iMaxFindCount < m_iCurFindCount)
		return false;

	st_NODE* pOldNode = m_OpenList.front();
	m_OpenList.pop_front();
	m_CloseList.push_back(pOldNode);

	//--------------------------------------------------
	// 길이 없거나 || 목적지를 찾았거나
	//--------------------------------------------------
	if (!pOldNode || !pOldNode->H)
	{
		while (nullptr != pOldNode)
		{
			m_BsetList.push_back(pOldNode);
			pOldNode = pOldNode->pParent;
		}

		return true;
	}

	CreateExpandNode(pOldNode);
	return false;
}

void CAStar_Manager::CreateExpandNode(st_NODE * pParent)
{
	int iX = pParent->iX;
	int iY = pParent->iY;

	// ↖
	if (ValidateExpand(iX - 1, iY - 1))
	{
		CreateNode(pParent, iX - 1, iY - 1, m_iWeight_Diagonal);
	}

	// ↑
	if (ValidateExpand(iX, iY - 1))
	{
		CreateNode(pParent, iX, iY - 1);
	}

	// ↗
	if (ValidateExpand(iX + 1, iY - 1))
	{
		CreateNode(pParent, iX + 1, iY - 1, m_iWeight_Diagonal);
	}

	// ←
	if (ValidateExpand(iX - 1, iY))
	{
		CreateNode(pParent, iX - 1, iY);
	}

	// →
	if (ValidateExpand(iX + 1, iY))
	{
		CreateNode(pParent, iX + 1, iY);
	}

	// ↙
	if (ValidateExpand(iX - 1, iY + 1))
	{
		CreateNode(pParent, iX - 1, iY + 1, m_iWeight_Diagonal);
	}

	// ↓
	if (ValidateExpand(iX, iY + 1))
	{
		CreateNode(pParent, iX, iY + 1);
	}

	// ↘
	if (ValidateExpand(iX + 1, iY + 1))
	{
		CreateNode(pParent, iX + 1, iY + 1, m_iWeight_Diagonal);
	}
}

bool CAStar_Manager::ValidateExpand(int iX, int iY)
{
	// 범위 밖
	if (0 > iX || 0 > iY ||
		iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return false;

	// 장애물
	if (g_ColorMap[iY][iX] == BLOCK)
		return false;

	return true;
}

void CAStar_Manager::CreateNode(st_NODE * pParent, int iNextX, int iNextY, int iWeight)
{
	if (!pParent)
		return;

	// find x, y
	st_NODE* pOldNode = nullptr;
	for (auto& pNode : m_OpenList)
	{
		if (pOldNode->iX == iNextX && pOldNode->iY == iNextY)
		{
			pOldNode = pNode;
			break;
		}
	}

	if (!pOldNode)
		return;

	int G = pParent->G + iWeight;
	if (pOldNode->G > G)
	{
		pOldNode->pParent = pParent;
		pOldNode->G = G;
		pOldNode->F = G + pOldNode->H;
		m_OpenList.sort();
	}

	// close list는 일단 생략

	st_NODE* pNewNode = new st_NODE;
	pNewNode->pParent = pParent;
	pNewNode->iX = iNextX;
	pNewNode->iY = iNextY;
	pNewNode->G = G;
	pNewNode->H = abs(iNextX - m_pGoal->iX) + abs(iNextY - m_pGoal->iY);
	pNewNode->F = G + pNewNode->H;
	
	m_OpenList.push_back(pNewNode);
	m_OpenList.sort();
}
