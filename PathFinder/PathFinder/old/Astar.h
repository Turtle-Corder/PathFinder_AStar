#ifndef __ASTAR__
#define __ASTAR__

#define df_WEIGHT_STRAIGHT	10
#define df_WEIGHT_DIAGONAL	14


#define df_PATHS_MAX		100

extern CMinHeap g_OpenList;
extern CMinHeap g_CloseList;

extern st_NODE g_stPath[df_PATHS_MAX];
extern int g_iPathCnt;


void PathFind(int iStartX, int iStartY, int iEndX, int iEndY);

BOOL FindProcess(int iEndX, int iEndY, st_NODE *pNode, int *pMaxPath);

void CreateExpandNode(st_NODE *pParent, int iEndX, int iEndY);

bool ValidateExpand(int iX, int iY);

void CreateNode(st_NODE *pParent, int iX, int iY, int iEndX, int iEndY, int iWeight = df_WEIGHT_STRAIGHT);

#endif