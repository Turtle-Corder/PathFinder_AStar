#ifndef __JPS__
#define __JPS__

#include <Windows.h>
#include <list>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

namespace Turtle
{
	//----------------------------------------------------------------------------------------------------
	// JUMP POINT SEARCH
	//----------------------------------------------------------------------------------------------------


	//--------------------------------------------------
	// �� ����
	//--------------------------------------------------
	typedef enum e_JPS_MAPSTATE
	{
		// ����ִ� ��
		JPS_WHITE,
		JPS_EMPTY = JPS_WHITE,

		// ���� ����
		JPS_RED,
		JPS_END = JPS_RED,

		// ���� ����
		JPS_GREEN,
		JPS_START = JPS_GREEN,

		// ���� ��
		JPS_GREY,
		JPS_BLOCK = JPS_GREY,

		// ������� ��
		JPS_BLUE,
		JPS_OPEN = JPS_BLUE,

		// ���� ��
		JPS_YELLOW,
		JPS_CLOSE = JPS_YELLOW,
	} eJPSMAPSTATE;



	//--------------------------------------------------
	// ��ֹ� ����
	//--------------------------------------------------
	enum eJPSMAPATTRIBUTE
	{
		NONE,
		STATIC,
		DYNAMIC,
	};



	//--------------------------------------------------
	// ��� ����ü
	//--------------------------------------------------
	typedef struct st_JPS_NODE
	{
		//--------------------------------------------------
		// �θ��� ������
		//--------------------------------------------------
		st_JPS_NODE *pParent;

		//--------------------------------------------------
		// Ÿ�� ��ǥ
		//--------------------------------------------------
		int iX, iY;

		//--------------------------------------------------
		// �̵� �Ÿ�
		//--------------------------------------------------
		int G;

		//--------------------------------------------------
		// ���������� �Ÿ�
		//--------------------------------------------------
		int H;

		//--------------------------------------------------
		// �򰡰� : G + H
		//--------------------------------------------------
		int F;
	} stJPSNODE;



	//--------------------------------------------------
	// ��� ��� ��ǥ�� ����� ����ü
	//--------------------------------------------------
	struct stPATHINFO
	{
		int iX, iY;
	};



	//--------------------------------------------------
	// min-heap -> multiset���� ����
	//--------------------------------------------------
	template<typename T>
	struct NODE_COMPARE
	{
		bool operator() (T NodeA, T NodeB) const
		{
			return NodeA->F < NodeB->F;
		}
	};

	struct NODE_FINDER
	{
		NODE_FINDER(int iX, int iY) : iPosX(iX), iPosY(iY)
		{ }

		bool operator() (st_JPS_NODE *pNode) const
		{
			if (pNode->iX == iPosX && pNode->iY == iPosY)
				return true;

			return false;
		}

		int iPosX, iPosY;
	};

	void ResetList();

	//--------------------------------------------------
	// ����&Ŭ���� ����Ʈ
	//--------------------------------------------------
	extern multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>> l_Open;
	extern multiset<st_JPS_NODE*, NODE_COMPARE<st_JPS_NODE*>> l_Close;


#define df_JPS_PATHS_MAX 100

	extern stPATHINFO stJPSPath[df_JPS_PATHS_MAX];
	extern int iJPSPathCnt;
	extern stPATHINFO g_LineTestPathInfo[df_JPS_PATHS_MAX];
	extern int g_iLineTestPathCnt;


	//--------------------------------------------------
	// JPS CLASS
	//--------------------------------------------------
#define df_JPS_DIR_LU	1
#define df_JPS_DIR_UU	2
#define df_JPS_DIR_RU	3
#define df_JPS_DIR_LL	4
#define df_JPS_DIR_RR	5
#define df_JPS_DIR_LD	6
#define df_JPS_DIR_DD	7
#define df_JPS_DIR_RD	8

#define JPSCHECKWALKABLE(x, y) (((0 <= x && x < m_iWidth) && (0 <= y && y < m_iHeight)) && m_pStateMap[y][x] != eNODESTATE::BLOCK)


	class CJPS
	{
	public:
		//--------------------------------------------------
		// ����, ����, �˻������� ���ڷ� �޴´�.
		//--------------------------------------------------
		CJPS(int iWidth, int iHeight, int iMaxBoundary = 0);
		~CJPS();


		//--------------------------------------------------
		// ���ã�� ����
		//--------------------------------------------------
		void FindPath(int iStartX, int iStartY, int iEndX, int iEndY, BOOL bStraightRevision, /*OUT*/stPATHINFO *pInfo, /*IN&OUT*/int *pMaxPath);

		//--------------------------------------------------
		// �� ��������
		//--------------------------------------------------
		void SetAttribute(int iX, int iY, eJPSMAPSTATE eState);

	private:
		//--------------------------------------------------
		// ���� ���(���) Ȯ��
		//--------------------------------------------------
		void ExpandJumpNode(stJPSNODE *pNode, int iEndX, int iEndY);
		void ExpandJumpNode_InBoundary(stJPSNODE *pNode, int iEndX, int iEndY);

		//--------------------------------------------------
		// ����(���)
		//--------------------------------------------------
		BOOL Jump(int iX, int iY, int iDir, int *iJumpX, int *iJumpY);
		BOOL Jump_InBoundary(int iStartX, int iStartY, int iX, int iY, int iDir, int *iJumpX, int *iJumpY);

		//--------------------------------------------------
		// ��� ����
		//--------------------------------------------------
		BOOL CreateJumpNode(stJPSNODE *pParent, int iX, int iY, int iEndX, int iEndY);


	private:
		//--------------------------------------------------
		// �Ӽ��� �����ص� ����
		//--------------------------------------------------
		eJPSMAPSTATE **m_pStateMap;

		//--------------------------------------------------
		// �� ����, ���� ũ��
		//--------------------------------------------------
		int m_iHeight;
		int m_iWidth;

		//--------------------------------------------------
		// �˻���� �ִ� ����, 0 : �˻����� ���� ����
		//--------------------------------------------------
		int m_iMaxBoundary;
	};
}

#endif