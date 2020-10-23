#ifndef __COMMON_DEFINES__
#define __COMMON_DEFINES__


#define df_SCREEN_WIDTH 1000
#define df_SCREEN_HEIGHT 1000

#define df_TILE_WIDTH 50
#define df_TILE_HEIGHT 50

#define df_TILE_SIZE 16

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

	PURPLE,
	BEST = PURPLE,

} eNODESTATE;

typedef struct st_VEC2
{
	int iX;
	int iY;
} _vec2;

#endif