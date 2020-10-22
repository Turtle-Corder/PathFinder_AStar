#pragma once
#ifndef __ASART_H__
#define __ASART_H__

class CAStar_Manager
{
public:
	CAStar_Manager();
	~CAStar_Manager();


public:
	void PathFind(st_NODE* pStart, st_NODE* pGoal);

	bool FindProcess();

	void CreateExpandNode(st_NODE* pParent);

	bool ValidateExpand(int iNextX, int iNextY);

	void CreateNode(st_NODE* pParent, int iNextX, int iNextY, int iWeight = 10);

	

private:
	//	얘는 NavAgent Component 등록해주고, 얘가 들고 있는 bestlist에 전달해준다.
	//	ㄴ tile_pos 같은 구조체를 여기서 리스트나 큐로 들고 있고, 여기다가 position이나 index 셋팅해준다.
	//	queue<CGameObject*> m_AgentList;

	st_NODE* m_pGoal = nullptr;

	list<st_NODE*> m_OpenList;
	list<st_NODE*> m_CloseList;
	list<st_NODE*> m_BsetList;



	int		m_iWeight_Staraight = 10;
	int		m_iWeight_Diagonal = 14;
	int		m_iMaxFindCount = 0;
	int		m_iCurFindCount = 0;
};



#endif // !__ASART_H__
