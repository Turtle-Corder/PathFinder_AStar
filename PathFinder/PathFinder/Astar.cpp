#include "stdafx.h"
#include "Astar.h"

CAStar_Manager::CAStar_Manager()
{
	m_iMaxFindCount = 1000;
}

CAStar_Manager::~CAStar_Manager()
{
	for (auto& pNode : m_OpenList)
		delete pNode;

	for (auto& pNode : m_CloseList)
		delete pNode;

	for (auto& pNode : m_BestList)
		delete pNode;

	m_OpenList.clear();
	m_CloseList.clear();
	m_BestList.clear();
}

HRESULT CAStar_Manager::PathFind(int iStartX, int iStartY, int iGoalX, int iGoalY)
{
	if (0 > iStartX || 0 > iStartY ||
		df_TILE_WIDTH <= iGoalX || df_TILE_HEIGHT <= iGoalY)
		return E_FAIL;

	ClearInfo();

	m_iStartX = iStartX;
	m_iStartY = iStartY;
	m_iGoalX = iGoalX;
	m_iGoalY = iGoalY;


	//--------------------------------------------------
	// ó�� �ϳ� ���� �־��ش�.
	//--------------------------------------------------
	st_NODE* pNewNode = new st_NODE;
	pNewNode->pParent = nullptr;
	pNewNode->iX = iStartX;
	pNewNode->iY = iStartY;
	pNewNode->G = 0;
	pNewNode->H = abs(iGoalX - iStartX) + abs(iGoalY - iStartY);
	pNewNode->F = pNewNode->G + pNewNode->H;
	
	m_OpenList.push_back(pNewNode);

	// TODO : timer setting
	return S_OK;
}

// �׽�Ʈ�� ���� �ϴ� ��� Ȯ���� 1ȸ��(8����) �õ��Ѵ�
// S_OK		: ���� ����
// E_FAIL	: ���� ���
HRESULT CAStar_Manager::FindProcess()
{
	//--------------------------------------------------
	// �ִ� Ž�� ������ ��ȸ�� �Ѿ�� ���з� ó��
	//  -> BestList�� ��������Ƿ� ������ ������ �� ����
	//--------------------------------------------------
	if (m_iMaxFindCount <= m_iCurFindCount)
		return S_OK;


	//--------------------------------------------------
	// open list �տ��� �ϳ� �̴´�.
	//--------------------------------------------------
	if (0 >= (int)m_OpenList.size())
		return S_OK;

	++m_iCurFindCount;
	st_NODE* pOldNode = m_OpenList.front();
	m_OpenList.pop_front();
	m_CloseList.push_back(pOldNode);

	if (!(g_eStateMap[pOldNode->iY][pOldNode->iX] == eNODESTATE::START 
	   || g_eStateMap[pOldNode->iY][pOldNode->iX] == eNODESTATE::END))
	{
		g_eStateMap[pOldNode->iY][pOldNode->iX] = eNODESTATE::CLOSE;
	}



	//--------------------------------------------------
	// ���� ���ų� || �������� ã�Ұų�
	//--------------------------------------------------
	if (!pOldNode || !pOldNode->H)
	{
		while (nullptr != pOldNode)
		{
			m_BestList.push_front(pOldNode);
			pOldNode = pOldNode->pParent;
		}

		return S_OK;
	}

	CreateExpandNode(pOldNode);
	return E_FAIL;
}

void CAStar_Manager::CreateExpandNode(st_NODE * pParent)
{
	int iX = pParent->iX;
	int iY = pParent->iY;

	// ��
	if (ValidateExpand(iX - 1, iY - 1))
	{
		CreateNode(pParent, iX - 1, iY - 1, m_iWeight_Diagonal);
	}

	// ��
	if (ValidateExpand(iX + 1, iY - 1))
	{
		CreateNode(pParent, iX + 1, iY - 1, m_iWeight_Diagonal);
	}

	// ��
	if (ValidateExpand(iX - 1, iY + 1))
	{
		CreateNode(pParent, iX - 1, iY + 1, m_iWeight_Diagonal);
	}

	// ��
	if (ValidateExpand(iX + 1, iY + 1))
	{
		CreateNode(pParent, iX + 1, iY + 1, m_iWeight_Diagonal);
	}

	// ��
	if (ValidateExpand(iX, iY - 1))
	{
		CreateNode(pParent, iX, iY - 1);
	}

	// ��
	if (ValidateExpand(iX, iY + 1))
	{
		CreateNode(pParent, iX, iY + 1);
	}

	// ��
	if (ValidateExpand(iX - 1, iY))
	{
		CreateNode(pParent, iX - 1, iY);
	}

	// ��
	if (ValidateExpand(iX + 1, iY))
	{
		CreateNode(pParent, iX + 1, iY);
	}
}

bool CAStar_Manager::ValidateExpand(int iX, int iY)
{
	// ���� ��
	if (0 > iX || 0 > iY ||
		iX > df_TILE_WIDTH - 1 || iY > df_TILE_HEIGHT - 1)
		return false;

	// ��ֹ�
	if (g_eStateMap[iY][iX] == eNODESTATE::BLOCK)
		return false;

	return true;
}

void CAStar_Manager::CreateNode(st_NODE * pParent, int iNextX, int iNextY, int iWeight)
{
	if (!pParent)
		return;

	//--------------------------------------------------
	// case 1 :
	// ������ ������ ��� �߿� �ִ��� ã�ƺ���.
	//--------------------------------------------------
	st_NODE* pOldNode = nullptr;
	for (auto& pNode : m_OpenList)
	{
		if (pNode->iX == iNextX && pNode->iY == iNextY)
		{
			pOldNode = pNode;
			break;
		}
	}

	// ������..
	if (pOldNode)
	{
		//--------------------------------------------------
		// ���ο� ��ΰ� ����� �� ������ �����ϰ� ������ �Ѵ�.
		//--------------------------------------------------
		int G = pParent->G + iWeight;
		if (pOldNode->G > G)
		{
			pOldNode->pParent = pParent;
			pOldNode->G = G;
			// H�� �״���̹Ƿ� ���� ����
			pOldNode->F = G + pOldNode->H;
			m_OpenList.sort([](const st_NODE* Src, const st_NODE* Dst)
			{
				if (Src->G < Dst->G)
					return true;

				if (Src->G == Dst->G)
				{
					if (Src->H < Dst->H)
						return true;

					if (Src->H == Dst->H)
					{
						if (Src->F < Dst->F)
							return true;
					}
				}
				return false;
			});
		}
		return;
	}

	
	//--------------------------------------------------
	// case 2 :
	// �̹� ����ģ ���
	//--------------------------------------------------
	for (auto& pNode : m_CloseList)
	{
		if (pNode->iX == iNextX && pNode->iY == iNextY)
		{
			pOldNode = pNode;
			break;
		}
	}

	if (pOldNode)
	{
		//--------------------------------------------------
		//  -> ����� ���ؼ� �����ص� �ȴ�. (�幮 ���)
		//--------------------------------------------------
		return;
	}


	//--------------------------------------------------
	// case 3 :
	// ó�� �߰��� ���, �����Ƿ� ���� �߰� �� ������
	//--------------------------------------------------
	st_NODE* pNewNode = new st_NODE;
	pNewNode->pParent = pParent;
	pNewNode->iX = iNextX;
	pNewNode->iY = iNextY;
	pNewNode->G = pParent->G + iWeight;
	pNewNode->H = (abs(iNextX - m_iGoalX) + abs(iNextY - m_iGoalY)) * 10;
	pNewNode->F = pNewNode->G + pNewNode->H;
	
	m_OpenList.push_back(pNewNode);
	m_OpenList.sort([](const st_NODE* Src, const st_NODE* Dst)
	{
		if (Src->G < Dst->G)
			return true;

		if (Src->G == Dst->G)
		{
			if (Src->H < Dst->H)
				return true;

			if (Src->H == Dst->H)
			{
				if (Src->F < Dst->F)
					return true;
			}
		}
		return false;
	});

	g_eStateMap[iNextY][iNextX] = eNODESTATE::OPEN;
}

void CAStar_Manager::ClearInfo()
{
	for (auto& pNode : m_OpenList)
		delete pNode;

	for (auto& pNode : m_CloseList)
		delete pNode;

	m_OpenList.clear();
	m_CloseList.clear();
	m_BestList.clear();

	m_iStartX	= -1;
	m_iStartY	= -1;
	m_iGoalX	= -1;
	m_iGoalY	= -1;

	m_iCurFindCount = 0;
}
