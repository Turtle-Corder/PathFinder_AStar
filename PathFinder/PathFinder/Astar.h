#pragma once
#ifndef __ASTAR_H__
#define __ASTART_H__

#define df_TIMER_STEP_FIND	1
#define df_PERIOD_STEP_FIND	20


typedef struct tagNODE
{
	tagNODE *pParent;	// �θ��� ������
	int iX, iY;			// Ÿ�� X, Y ��ǥ
	int G;				// �̵� �Ÿ�
	int H;				// ���������� �Ÿ� (dx + dy) (* weight ���� �ټ��� ����) 
	int F;				// G + H (* weight ���� �ټ��� ����)

} st_NODE;

class CAStar_Manager
{
public:
	CAStar_Manager();
	~CAStar_Manager();

public:
	HRESULT PathFind(int iStartX, int iStartY, int iGoalX, int iGoalY);

	HRESULT FindProcess();

	void CreateExpandNode(st_NODE* pParent);

	bool ValidateExpand(int iNextX, int iNextY);

	void CreateNode(st_NODE* pParent, int iNextX, int iNextY, int iWeight = 10);

	void ClearInfo();

public:
	//	��� NavAgent Component ������ְ�, �갡 ��� �ִ� bestlist�� �������ش�.
	//	�� tile_pos ���� ����ü�� ���⼭ ����Ʈ�� ť�� ��� �ְ�, ����ٰ� position�̳� index �������ش�.
	//	queue<CGameObject*> m_AgentList;

	list<st_NODE*> m_OpenList;
	list<st_NODE*> m_CloseList;
	list<st_NODE*> m_BestList;

	int		m_iStartX	= -1; 
	int		m_iStartY	= -1;
	int		m_iGoalX	= -1; 
	int		m_iGoalY	= -1;

	int		m_iWeight_Staraight = 10;
	int		m_iWeight_Diagonal = 14;

	int		m_iMaxFindCount = 0;
	int		m_iCurFindCount = 0;
};



#endif // !__ASART_H__
