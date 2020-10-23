#ifndef __COMMON_DEFINES__
#define __COMMON_DEFINES__


#define df_SCREEN_WIDTH 1000
#define df_SCREEN_HEIGHT 1000

#define df_TILE_WIDTH 50
#define df_TILE_HEIGHT 50

#define df_TILE_SIZE 16

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

	PURPLE,
	BEST = PURPLE,

} eNODESTATE;

typedef struct st_VEC2
{
	int iX;
	int iY;
} _vec2;

#endif