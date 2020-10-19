#ifndef __JUMP_POINT_SEARCH__
#define __JUMP_POINT_SEARCH__

#define df_DIR_LU	1
#define df_DIR_UU	2
#define df_DIR_RU	3
#define df_DIR_LL	4
#define df_DIR_RR	5
#define df_DIR_LD	6
#define df_DIR_DD	7
#define df_DIR_RD	8

#define df_TIMER_STEP_FIND	1
#define df_PERIOD_STEP_FIND	20

#define CHECKWALKABLE(iX, iY) (((0 <= iX && iX < df_TILE_WIDTH) && (0 <= iY && iY < df_TILE_HEIGHT)) && g_eStateMap[iY][iX] != eNODESTATE::BLOCK)

extern CMinHeap g_OpenList;
extern CMinHeap g_CloseList;

extern st_NODE g_stPath[df_PATHS_MAX];
extern int g_iPathCnt;

void StartJump(int iStartX, int iStartY, int iEndX, int iEndY);

BOOL JumpProcess(int iEndX, int iEndY, st_NODE *pNode, int *pMaxPath);

BOOL Jump(int iX, int iY, int iDir, int *iJumpX, int *iJumpY, COLORREF Color);

//BOOL CheckWalkable(int iX, int iY);

void ExpandJumpNode(st_NODE *pNode, int iEndX, int iEndY);

st_NODE *CreateJumpNode(st_NODE *pNode, int iX, int iY, int iEndX, int iEndY);

#endif