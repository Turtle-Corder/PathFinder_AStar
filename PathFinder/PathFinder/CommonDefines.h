#ifndef __COMMON_DEFINES__
#define __COMMON_DEFINES__


#define df_SCREEN_WIDTH 1000
#define df_SCREEN_HEIGHT 1000

#define df_TILE_WIDTH 50
#define df_TILE_HEIGHT 50

#define df_TILE_SIZE 16

#define df_PATHS_MAX		100

typedef enum e_NODE_STATE
{
	WHITE,	// 비어있는 길
	EMPTY = WHITE,

	RED,	// 도착 지점
	END = RED,

	GREEN,	// 시작 지점
	START = GREEN,

	GREY,	// 막힌 길
	BLOCK = GREY,

	BLUE,	// 가고싶은 길 (후보군?)
	OPEN = BLUE,

	YELLOW,	// 가본 길
	CLOSE = YELLOW,
} eNODESTATE;



#define df_HEAPSIZE_MAX 1000

#define PARENT(x)		((x-1)/2)
#define LEFT_CHILD(x)	((2*x)+1)
#define RIGHT_CHILD(x)	((2*x)+2)

struct st_NODE
{
	st_NODE *pParent;	// 부모노드 포인터
	int iX, iY;			// 타일 X, Y 좌표
	int G;				// 이동 거리
	int H;				// 목적지와의 거리 (dx + dy) (* weight 값을 줄수도 있음) 
	int F;				// G + H (* weight 값을 줄수도 있음)

	bool operator()(st_NODE* pSrc, st_NODE* pDst)
	{
		if (pSrc->G < pDst->G)
			return true;

		if (pSrc->G == pDst->G)
		{
			if (pSrc->H < pDst->H)
				return true;

			if (pSrc->H == pDst->H)
			{
				if (pSrc->F < pSrc->F)
					return true;
			}
		}

		return false;
	}
};

typedef struct st_VEC2
{
	int iX;
	int iY;
} _vec2;

#endif