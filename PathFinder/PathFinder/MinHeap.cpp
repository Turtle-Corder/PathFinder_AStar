#include "stdafx.h"
#include "MinHeap.h"

CMinHeap::CMinHeap()
{
	ZeroMemory(m_pArr, sizeof(m_pArr));
	ClearHeap();
}

CMinHeap::~CMinHeap()
{
	// TODO : 밖에서 정리?
}

void CMinHeap::ClearHeap()
{
	RemoveAll();

	m_iCurCount = 0;
	m_iMaxCount = df_HEAPSIZE_MAX;
}

void CMinHeap::ReleaseNode()
{
	for (int iCnt = 0; iCnt < m_iCurCount; iCnt++)
	{
		delete m_pArr[iCnt];
		m_pArr[iCnt] = NULL;
	}
}

void CMinHeap::SetState(eNODESTATE eColor)
{
	m_eColor = eColor;
}

void CMinHeap::DebugList(HDC hDC, int iWdith, int iHeight)
{
	TCHAR szVal[32];
	for (int iCnt = 0; iCnt < m_iCurCount; iCnt++)
	{
		ZeroMemory(szVal, sizeof(szVal));
		wsprintf(szVal, _T("%d, %d"), m_pArr[iCnt]->iX, m_pArr[iCnt]->iY);
		TextOut(hDC, iWdith, iHeight + (20 * iCnt), szVal, _tcslen(szVal));
	}
}

int CMinHeap::GetHeapCount()
{
	return m_iCurCount;
}

bool CMinHeap::Insert(st_NODE *pNode)
{
	if (pNode == NULL)
		return false;

	if (m_iCurCount >= df_HEAPSIZE_MAX)
	{
		return false;
	}

	m_pArr[m_iCurCount] = pNode;
	SetTileState(m_eColor, pNode->iX, pNode->iY);
	SetTileColor(RGB(255, 255, 255), pNode->iX, pNode->iY);

	// heapify down -> up
	int iCurIndex = m_iCurCount;
	int iParent = PARENT(iCurIndex);

	//while (iCurIndex > 0 && m_pArr[iParent]->H > m_pArr[iCurIndex]->H)
	while (iCurIndex > 0 && m_pArr[iParent]->F >= m_pArr[iCurIndex]->F)
	{
		if (m_pArr[iParent]->F == m_pArr[iCurIndex]->F)
		{
			if (m_pArr[iParent]->H > m_pArr[iCurIndex]->H)
			{
				st_NODE *temp = m_pArr[iParent];
				m_pArr[iParent] = m_pArr[iCurIndex];
				m_pArr[iCurIndex] = temp;
			}
		}

		else
		{
			st_NODE *temp = m_pArr[iParent];
			m_pArr[iParent] = m_pArr[iCurIndex];
			m_pArr[iCurIndex] = temp;
		}

		iCurIndex = iParent;
		iParent = PARENT(iCurIndex);
	}

	m_iCurCount++;
	return true;
}

st_NODE * CMinHeap::Remove()
{
	if (0 >= m_iCurCount)
		return NULL;

	st_NODE *pRet = m_pArr[0];
	if (m_iCurCount == 1)
	{
		m_iCurCount--;
		// m_pArr[0] = NULL;
	}
	else
	{
		m_iCurCount--;
		m_pArr[0] = m_pArr[m_iCurCount];

		// heapify up -> down
		int iCurIndex = 0;
		int iNextIndex = 0;
		int iLChild = LEFT_CHILD(iCurIndex);
		int iRChild = RIGHT_CHILD(iCurIndex);
		
		while (1)
		{
			// 양쪽 자식노드 없음
			if (iLChild >= m_iCurCount)
				break;

			// 오른쪽 자식노드 없음
			if (iRChild >= m_iCurCount)
			{
				iNextIndex = iLChild;
			}
			// 자식노드 모두 있음
			else
			{
				//if (m_pArr[iLChild]->H > m_pArr[iRChild]->H)
				if (m_pArr[iLChild]->F >= m_pArr[iRChild]->F)
				{
					if (m_pArr[iLChild]->F == m_pArr[iRChild]->F)
					{
						if (m_pArr[iLChild]->H > m_pArr[iRChild]->H)
						{
							iNextIndex = iRChild;
						}
						else
						{
							iNextIndex = iLChild;
						}
					}
					else
					{
						iNextIndex = iRChild;
					}
				}
				else
				{
					iNextIndex = iLChild;
				}
			}


			//if (m_pArr[iCurIndex]->H > m_pArr[iNextIndex]->H)
			if (m_pArr[iCurIndex]->F >= m_pArr[iNextIndex]->F)
			{
				if (m_pArr[iCurIndex]->F == m_pArr[iNextIndex]->F)
				{
					if (m_pArr[iCurIndex]->H > m_pArr[iNextIndex]->H)
					{
						st_NODE *temp = m_pArr[iNextIndex];
						m_pArr[iNextIndex] = m_pArr[iCurIndex];
						m_pArr[iCurIndex] = temp;
					}
				}
				else
				{
					st_NODE *temp = m_pArr[iNextIndex];
					m_pArr[iNextIndex] = m_pArr[iCurIndex];
					m_pArr[iCurIndex] = temp;
				}

				iCurIndex = iNextIndex;
			}
			else 
				break;

			iLChild = LEFT_CHILD(iCurIndex);
			iRChild = RIGHT_CHILD(iCurIndex);
		}
	}

	return pRet;
}

void CMinHeap::RemoveAll()
{
	st_NODE *pNode = Remove();
	while (pNode != NULL)
	{
		delete pNode;
		pNode = Remove();
	}
}

st_NODE * CMinHeap::GetNode(int iX, int iY)
{
	if (m_iCurCount <= 0)
		return NULL;

	for (int iCnt = 0; iCnt < m_iCurCount; iCnt++)
	{
		if (m_pArr[iCnt]->iX == iX && m_pArr[iCnt]->iY == iY)
			return m_pArr[iCnt];
	}

	return NULL;
}

void CMinHeap::ResortHeap(st_NODE *pNode)
{
	if (pNode == NULL)
		return;

	int iCurIndex = -1;
	for (int iCnt = 0; iCnt < m_iCurCount; iCnt++)
	{
		if (m_pArr[iCnt] == pNode)
		{
			iCurIndex = iCnt;
			break;
		}
	}

	if (iCurIndex == -1)
		return;

	int iParent = PARENT(iCurIndex);

	//while (iCurIndex > 0 && m_pArr[iParent]->H > m_pArr[iCurIndex]->H)
	while (iCurIndex > 0 && m_pArr[iParent]->F >= m_pArr[iCurIndex]->F)
	{
		if (m_pArr[iParent]->F == m_pArr[iCurIndex]->F)
		{
			if (m_pArr[iParent]->H > m_pArr[iCurIndex]->H)
			{
				st_NODE *temp = m_pArr[iParent];
				m_pArr[iParent] = m_pArr[iCurIndex];
				m_pArr[iCurIndex] = temp;
			}
		}

		else
		{
			st_NODE *temp = m_pArr[iParent];
			m_pArr[iParent] = m_pArr[iCurIndex];
			m_pArr[iCurIndex] = temp;
		}

		iCurIndex = iParent;
		iParent = PARENT(iCurIndex);
	}
}
