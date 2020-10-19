#include "stdafx.h"
#include "JPS.h"

multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>> Turtle::l_Open;
multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>> Turtle::l_Close;

Turtle::stPATHINFO Turtle::stJPSPath[df_JPS_PATHS_MAX] = { 0, };
int Turtle::iJPSPathCnt = df_JPS_PATHS_MAX;

Turtle::stPATHINFO Turtle::g_LineTestPathInfo[df_JPS_PATHS_MAX] = { 0, };
int Turtle::g_iLineTestPathCnt = df_JPS_PATHS_MAX;



void Turtle::ResetList()
{
	multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>>::iterator iter;
	for (iter = l_Open.begin(); iter != l_Open.end(); ++iter)
	{
		delete (*iter);
	}

	for (iter = l_Close.begin(); iter != l_Close.end(); ++iter)
	{
		delete (*iter);
	}

	l_Open.clear();
	l_Close.clear();
}


Turtle::CJPS::CJPS(int iWidth, int iHeight, int iMaxBoundary)
{
	m_pStateMap = NULL;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iMaxBoundary = iMaxBoundary;

	m_pStateMap = new eJPSMAPSTATE*[iHeight];
	for (int iCnt = 0; iCnt < iHeight; iCnt++)
		m_pStateMap[iCnt] = new eJPSMAPSTATE[iWidth];

	for (int iHeight = 0; iHeight < m_iHeight; iHeight++)
	{
		for (int iWidth = 0; iWidth < m_iWidth; iWidth++)
		{
			m_pStateMap[iHeight][iWidth] = eJPSMAPSTATE::JPS_EMPTY;
			//SetAttribute(iWidth, iHeight, eNODESTATE::EMPTY);
		}
	}
}

Turtle::CJPS::~CJPS()
{
	for (int iCnt = 0; iCnt < m_iHeight; iCnt++)
		delete[] m_pStateMap[iCnt];

	delete[] m_pStateMap;
}

void Turtle::CJPS::FindPath(int iStartX, int iStartY, int iEndX, int iEndY, BOOL bStraightRevision, stPATHINFO *pInfo, int *pMaxPath)
{
	ResetList();

	stJPSNODE *newNode = new stJPSNODE;
	newNode->pParent = NULL;
	newNode->iX = iStartX;
	newNode->iY = iStartY;
	newNode->G = 0;
	newNode->H = abs(iStartX - iEndX) + abs(iStartY - iEndY);
	newNode->F = newNode->G + newNode->H;
	l_Open.insert(newNode);

	while (1)
	{
		stJPSNODE *pOldNode = NULL;
		multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>>::iterator iter = l_Open.begin();
		if (iter != l_Open.end())
		{
			pOldNode = *iter;
			l_Open.erase(iter);
			l_Close.insert(pOldNode);
		}
		else
		{
			pOldNode = NULL;
		}

		if (pOldNode == NULL || pOldNode->H == 0)
		{
			// pNode에 담아주고 끝내야함 DomainToIP 처럼
			std::list<stPATHINFO> RoughPath;

			while (pOldNode != NULL)
			{
				stPATHINFO PathPoint;
				PathPoint.iX = pOldNode->iX;
				PathPoint.iY = pOldNode->iY;
				RoughPath.push_back(PathPoint);

				pOldNode = pOldNode->pParent;
			}
			RoughPath.reverse();
			
			int iCnt = 0;
			std::list<stPATHINFO>::iterator start = RoughPath.begin();
			
			if (bStraightRevision && RoughPath.size() > 2)
			{
				std::list<stPATHINFO>::iterator terminal = start;
				++terminal;
				CBresenham Bresenham;
				int iX, iY;
				BOOL bRefine = FALSE;

				// 첫 노드 넣음
				pInfo[0] = *start;
				iCnt++;
				while (1)
				{		

					// 뒷 노드 없음 (마지막 노드)
					++terminal;
					if (terminal == RoughPath.end())
					{
						--terminal;
						pInfo[iCnt] = *terminal;
						iCnt++;
						break;
					}
					
					Bresenham.SetPosition(start->iX, start->iY, terminal->iX, terminal->iY);
					
					// 노드 사이에 장애물이 있는지 체크
					while (1)
					{
						if (!Bresenham.GetNext(&iX, &iY))
						{
							bRefine = TRUE;
							break;
						}

						if (!JPSCHECKWALKABLE(iX, iY))
						{
							bRefine = FALSE;
							break;
						}
					}

					// 장애물 있음
					if (!bRefine)
					{
						--terminal;
						pInfo[iCnt] = *terminal;
						iCnt++;
						if (iCnt >= *pMaxPath)
							break;

						start = terminal;
						++terminal;
					}
				}
			}
			else
			{
				for (start = RoughPath.begin(); start != RoughPath.end(); ++start)
				{
					pInfo[iCnt] = *start;
					iCnt++;

					if (iCnt >= *pMaxPath)
						break;
				}
			}

			// path max count renew
			*pMaxPath = iCnt;
			break;
		}

		if (m_iMaxBoundary <= 0)
			ExpandJumpNode(pOldNode, iEndX, iEndY);
		else
			ExpandJumpNode_InBoundary(pOldNode, iEndX, iEndY);
	}
}

void Turtle::CJPS::SetAttribute(int iX, int iY, eJPSMAPSTATE eState)
{
	if (iX < 0 || iY < 0 || iX > m_iWidth - 1 || iY > m_iHeight - 1)
		return;

	m_pStateMap[iY][iX] = eState;
}

void Turtle::CJPS::ExpandJumpNode(stJPSNODE * pNode, int iEndX, int iEndY)
{
	int iX = pNode->iX;
	int iY = pNode->iY;

	int iNewX = 0;
	int iNewY = 0;

	// 시작노드
	if (pNode->pParent == NULL)
	{
		// 8방향 점프

		// ↑
		if (Jump(iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↓
		if (Jump(iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ←
		if (Jump(iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// →
		if (Jump(iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↖
		if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↙
		if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↗
		if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↘
		if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
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
			if (Jump(iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ←
		else if (iDX < 0 && iDY == 0)
		{
			if (Jump(iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// →
		else if (iDX > 0 && iDY == 0)
		{
			if (Jump(iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↓
		else if (iDX == 0 && iDY > 0)
		{
			if (Jump(iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↖
		else if (iDX < 0 && iDY < 0)
		{
			if (Jump(iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump(iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↗
		else if (iDX > 0 && iDY < 0)
		{
			if (Jump(iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↙
		else if (iDX < 0 && iDY > 0)
		{
			if (Jump(iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump(iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↘
		else if (iDX > 0 && iDY > 0)
		{
			if (Jump(iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump(iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}
	}
}

void Turtle::CJPS::ExpandJumpNode_InBoundary(stJPSNODE * pNode, int iEndX, int iEndY)
{
	int iX = pNode->iX;
	int iY = pNode->iY;

	int iNewX = 0;
	int iNewY = 0;

	// 시작노드
	if (pNode->pParent == NULL)
	{
		// 8방향 점프

		// ↑
		if (Jump_InBoundary(iX, iY, iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↓
		if (Jump_InBoundary(iX, iY, iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ←
		if (Jump_InBoundary(iX, iY, iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// →
		if (Jump_InBoundary(iX, iY, iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↖
		if (Jump_InBoundary(iX, iY, iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↙
		if (Jump_InBoundary(iX, iY, iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↗
		if (Jump_InBoundary(iX, iY, iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
		{
			CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
		}

		// ↘
		if (Jump_InBoundary(iX, iY, iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
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
			if (Jump_InBoundary(iX, iY, iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ←
		else if (iDX < 0 && iDY == 0)
		{
			if (Jump_InBoundary(iX, iY, iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// →
		else if (iDX > 0 && iDY == 0)
		{
			if (Jump_InBoundary(iX, iY, iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↓
		else if (iDX == 0 && iDY > 0)
		{
			if (Jump_InBoundary(iX, iY, iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↖
		else if (iDX < 0 && iDY < 0)
		{
			if (Jump_InBoundary(iX, iY, iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump_InBoundary(iX, iY, iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump_InBoundary(iX, iY, iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↗
		else if (iDX > 0 && iDY < 0)
		{
			if (Jump_InBoundary(iX, iY, iX, iY - 1, df_JPS_DIR_UU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump_InBoundary(iX, iY, iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump_InBoundary(iX, iY, iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↙
		else if (iDX < 0 && iDY > 0)
		{
			if (Jump_InBoundary(iX, iY, iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump_InBoundary(iX, iY, iX - 1, iY, df_JPS_DIR_LL, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump_InBoundary(iX, iY, iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY - 1, df_JPS_DIR_LU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}

		// ↘
		else if (iDX > 0 && iDY > 0)
		{
			if (Jump_InBoundary(iX, iY, iX, iY + 1, df_JPS_DIR_DD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}
			if (Jump_InBoundary(iX, iY, iX + 1, iY, df_JPS_DIR_RR, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (Jump_InBoundary(iX, iY, iX + 1, iY + 1, df_JPS_DIR_RD, &iNewX, &iNewY))
			{
				CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
			}

			if (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			{
				if (Jump_InBoundary(iX, iY, iX - 1, iY + 1, df_JPS_DIR_LD, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}

			if (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			{
				if (Jump_InBoundary(iX, iY, iX + 1, iY - 1, df_JPS_DIR_RU, &iNewX, &iNewY))
				{
					CreateJumpNode(pNode, iNewX, iNewY, iEndX, iEndY);
				}
			}
		}
	}
}

BOOL Turtle::CJPS::Jump(int iX, int iY, int iDir, int * iJumpX, int * iJumpY)
{
	// 범위 밖, 벽이면 중단
	if (!JPSCHECKWALKABLE(iX, iY))
		return FALSE;

	if (g_eStateMap[iY][iX] == eNODESTATE::END)
	{
		*iJumpX = iX;
		*iJumpY = iY;
		return TRUE;
	}

	if (iDir == df_JPS_DIR_UU)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX, iY - 1, df_JPS_DIR_UU, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_DD)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX, iY + 1, df_JPS_DIR_DD, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_LL)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			|| (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX - 1, iY, df_JPS_DIR_LL, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_RR)
	{
		if ((JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump(iX + 1, iY, df_JPS_DIR_RR, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_LU)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			|| (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY - 1, df_JPS_DIR_UU, iJumpX, iJumpY) || Jump(iX - 1, iY, df_JPS_DIR_LL, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX - 1, iY - 1, df_JPS_DIR_LU, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	else if (iDir == df_JPS_DIR_LD)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY + 1, df_JPS_DIR_DD, iJumpX, iJumpY) || Jump(iX - 1, iY, df_JPS_DIR_LL, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX - 1, iY + 1, df_JPS_DIR_LD, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	else if (iDir == df_JPS_DIR_RU)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY - 1, df_JPS_DIR_UU, iJumpX, iJumpY) || Jump(iX + 1, iY, df_JPS_DIR_RR, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX + 1, iY - 1, df_JPS_DIR_RU, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	else if (iDir == df_JPS_DIR_RD)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX, iY + 1, df_JPS_DIR_DD, iJumpX, iJumpY) || Jump(iX + 1, iY, df_JPS_DIR_RR, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump(iX + 1, iY + 1, df_JPS_DIR_RD, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL Turtle::CJPS::Jump_InBoundary(int iStartX, int iStartY, int iX, int iY, int iDir, int * iJumpX, int *iJumpY)
{
	// 범위를 벗어나면 중단
	int dx = abs(iX - iStartX);
	int dy = abs(iY - iStartY);

	if (dx > m_iMaxBoundary || dy > m_iMaxBoundary)
		return FALSE;

	// 범위 밖, 벽이면 중단
	if (!JPSCHECKWALKABLE(iX, iY))
		return FALSE;

	if (g_eStateMap[iY][iX] == eNODESTATE::END)
	{
		*iJumpX = iX;
		*iJumpY = iY;
		return TRUE;
	}

	if (iDir == df_JPS_DIR_UU)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump_InBoundary(iStartX, iStartY, iX, iY - 1, df_JPS_DIR_UU, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_DD)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump_InBoundary(iStartX, iStartY, iX, iY + 1, df_JPS_DIR_DD, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_LL)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			|| (JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump_InBoundary(iStartX, iStartY, iX - 1, iY, df_JPS_DIR_LL, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_RR)
	{
		if ((JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		return Jump_InBoundary(iStartX, iStartY, iX + 1, iY, df_JPS_DIR_RR, iJumpX, iJumpY);
	}

	else if (iDir == df_JPS_DIR_LU)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1))
			|| (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX, iY - 1, df_JPS_DIR_UU, iJumpX, iJumpY)
			|| Jump_InBoundary(iStartX, iStartY, iX - 1, iY, df_JPS_DIR_LL, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX - 1, iY - 1, df_JPS_DIR_LU, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	else if (iDir == df_JPS_DIR_LD)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX + 1, iY)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX, iY + 1, df_JPS_DIR_DD, iJumpX, iJumpY)
			|| Jump_InBoundary(iStartX, iStartY, iX - 1, iY, df_JPS_DIR_LL, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX - 1, iY + 1, df_JPS_DIR_LD, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	else if (iDir == df_JPS_DIR_RU)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY - 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY + 1) && !JPSCHECKWALKABLE(iX, iY + 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX, iY - 1, df_JPS_DIR_UU, iJumpX, iJumpY)
			|| Jump_InBoundary(iStartX, iStartY, iX + 1, iY, df_JPS_DIR_RR, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX + 1, iY - 1, df_JPS_DIR_RU, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	else if (iDir == df_JPS_DIR_RD)
	{
		if ((JPSCHECKWALKABLE(iX - 1, iY + 1) && !JPSCHECKWALKABLE(iX - 1, iY))
			|| (JPSCHECKWALKABLE(iX + 1, iY - 1) && !JPSCHECKWALKABLE(iX, iY - 1)))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX, iY + 1, df_JPS_DIR_DD, iJumpX, iJumpY)
			|| Jump_InBoundary(iStartX, iStartY, iX + 1, iY, df_JPS_DIR_RR, iJumpX, iJumpY))
		{
			*iJumpX = iX;
			*iJumpY = iY;
			return TRUE;
		}

		if (Jump_InBoundary(iStartX, iStartY, iX + 1, iY + 1, df_JPS_DIR_RD, iJumpX, iJumpY))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL Turtle::CJPS::CreateJumpNode(stJPSNODE * pParent, int iX, int iY, int iEndX, int iEndY)
{
	int idG = abs(pParent->iX - iX) + abs(pParent->iY - iY);
	int G = pParent->G + idG;

	stJPSNODE *pOldNode = NULL;
	multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>>::iterator iter;
	iter = find_if(l_Open.begin(), l_Open.end(), NODE_FINDER(iX, iY));
	if (iter != l_Open.end())
	{
		pOldNode = *iter;
		if (pOldNode->G > G)
		{
			pOldNode->pParent = pParent;
			pOldNode->G = G;
			pOldNode->F = G + pOldNode->H;

			l_Open.erase(iter);
			l_Open.insert(pOldNode);
		}
		return FALSE;
	}

	iter = find_if(l_Close.begin(), l_Close.end(), NODE_FINDER(iX, iY));
	if (iter != l_Close.end())
	{
		pOldNode = *iter;
		if (pOldNode->G > G)
		{
			pOldNode->pParent = pParent;
			pOldNode->G = G;
			pOldNode->F = G + pOldNode->H;
		}
		return FALSE;
	}

	stJPSNODE *pNewNode = new stJPSNODE;
	pNewNode->pParent = pParent;
	pNewNode->iX = iX;
	pNewNode->iY = iY;
	pNewNode->G = G;
	pNewNode->H = (abs(iX - iEndX) + abs(iY - iEndY));
	pNewNode->F = G + pNewNode->H;
	l_Open.insert(pNewNode);

	return TRUE;
}