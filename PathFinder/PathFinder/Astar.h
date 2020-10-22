#pragma once
#ifndef __ASTAR_H__
#define __ASTART_H__

#define df_TIMER_STEP_FIND	1
#define df_PERIOD_STEP_FIND	20


typedef struct tagNODE
{
	tagNODE *pParent;	// 부모노드 포인터
	int iX, iY;			// 타일 X, Y 좌표
	int G;				// 이동 거리
	int H;				// 목적지와의 거리 (dx + dy) (* weight 값을 줄수도 있음) 
	int F;				// G + H (* weight 값을 줄수도 있음)

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
	//	얘는 NavAgent Component 등록해주고, 얘가 들고 있는 bestlist에 전달해준다.
	//	ㄴ tile_pos 같은 구조체를 여기서 리스트나 큐로 들고 있고, 여기다가 position이나 index 셋팅해준다.
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
