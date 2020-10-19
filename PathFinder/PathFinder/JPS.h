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
	// 맵 상태
	//--------------------------------------------------
	typedef enum e_JPS_MAPSTATE
	{
		// 비어있는 길
		JPS_WHITE,
		JPS_EMPTY = JPS_WHITE,

		// 도착 지점
		JPS_RED,
		JPS_END = JPS_RED,

		// 시작 지점
		JPS_GREEN,
		JPS_START = JPS_GREEN,

		// 막힌 길
		JPS_GREY,
		JPS_BLOCK = JPS_GREY,

		// 가고싶은 길
		JPS_BLUE,
		JPS_OPEN = JPS_BLUE,

		// 가본 길
		JPS_YELLOW,
		JPS_CLOSE = JPS_YELLOW,
	} eJPSMAPSTATE;



	//--------------------------------------------------
	// 장애물 상태
	//--------------------------------------------------
	enum eJPSMAPATTRIBUTE
	{
		NONE,
		STATIC,
		DYNAMIC,
	};



	//--------------------------------------------------
	// 노드 구조체
	//--------------------------------------------------
	typedef struct st_JPS_NODE
	{
		//--------------------------------------------------
		// 부모노드 포인터
		//--------------------------------------------------
		st_JPS_NODE *pParent;

		//--------------------------------------------------
		// 타일 좌표
		//--------------------------------------------------
		int iX, iY;

		//--------------------------------------------------
		// 이동 거리
		//--------------------------------------------------
		int G;

		//--------------------------------------------------
		// 목적지와의 거리
		//--------------------------------------------------
		int H;

		//--------------------------------------------------
		// 평가값 : G + H
		//--------------------------------------------------
		int F;
	} stJPSNODE;



	//--------------------------------------------------
	// 결과 경로 좌표를 담아줄 구조체
	//--------------------------------------------------
	struct stPATHINFO
	{
		int iX, iY;
	};



	//--------------------------------------------------
	// min-heap -> multiset으로 변경
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
	// 오픈&클로즈 리스트
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
		// 가로, 세로, 검색범위를 인자로 받는다.
		//--------------------------------------------------
		CJPS(int iWidth, int iHeight, int iMaxBoundary = 0);
		~CJPS();


		//--------------------------------------------------
		// 경로찾기 시작
		//--------------------------------------------------
		void FindPath(int iStartX, int iStartY, int iEndX, int iEndY, BOOL bStraightRevision, /*OUT*/stPATHINFO *pInfo, /*IN&OUT*/int *pMaxPath);

		//--------------------------------------------------
		// 맵 상태지정
		//--------------------------------------------------
		void SetAttribute(int iX, int iY, eJPSMAPSTATE eState);

	private:
		//--------------------------------------------------
		// 점프 노드(경로) 확장
		//--------------------------------------------------
		void ExpandJumpNode(stJPSNODE *pNode, int iEndX, int iEndY);
		void ExpandJumpNode_InBoundary(stJPSNODE *pNode, int iEndX, int iEndY);

		//--------------------------------------------------
		// 점프(재귀)
		//--------------------------------------------------
		BOOL Jump(int iX, int iY, int iDir, int *iJumpX, int *iJumpY);
		BOOL Jump_InBoundary(int iStartX, int iStartY, int iX, int iY, int iDir, int *iJumpX, int *iJumpY);

		//--------------------------------------------------
		// 노드 생성
		//--------------------------------------------------
		BOOL CreateJumpNode(stJPSNODE *pParent, int iX, int iY, int iEndX, int iEndY);


	private:
		//--------------------------------------------------
		// 속성을 저장해둘 변수
		//--------------------------------------------------
		eJPSMAPSTATE **m_pStateMap;

		//--------------------------------------------------
		// 맵 가로, 세로 크기
		//--------------------------------------------------
		int m_iHeight;
		int m_iWidth;

		//--------------------------------------------------
		// 검색허용 최대 범위, 0 : 검색범위 제한 없음
		//--------------------------------------------------
		int m_iMaxBoundary;
	};
}

#endif