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
	WHITE,	// ����ִ� ��
	EMPTY = WHITE,

	RED,	// ���� ����
	END = RED,

	GREEN,	// ���� ����
	START = GREEN,

	GREY,	// ���� ��
	BLOCK = GREY,

	BLUE,	// ������� �� (�ĺ���?)
	OPEN = BLUE,

	YELLOW,	// ���� ��
	CLOSE = YELLOW,
} eNODESTATE;



#define df_HEAPSIZE_MAX 1000

#define PARENT(x)		((x-1)/2)
#define LEFT_CHILD(x)	((2*x)+1)
#define RIGHT_CHILD(x)	((2*x)+2)

struct st_NODE
{
	st_NODE *pParent;	// �θ��� ������
	int iX, iY;			// Ÿ�� X, Y ��ǥ
	int G;				// �̵� �Ÿ�
	int H;				// ���������� �Ÿ� (dx + dy) (* weight ���� �ټ��� ����) 
	int F;				// G + H (* weight ���� �ټ��� ����)
};


#endif