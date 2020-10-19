#ifndef __TILE__
#define __TILE__

// pixel이 아닌 타일위치
extern eNODESTATE g_eStateMap[df_TILE_HEIGHT][df_TILE_WIDTH];
extern COLORREF g_ColorMap[df_TILE_HEIGHT][df_TILE_WIDTH];

extern int g_iStartX, g_iStartY;
extern int g_iEndX, g_iEndY;



void InitTiles();

BOOL PosToTile(LPARAM lParam, int *iX, int *iY);
BOOL GetInvalidateArea(int iX, int iY, RECT *pRect);

void SetTileState(eNODESTATE eState, int iX, int iY);
eNODESTATE GetTileState(int iX, int iY);

void SetTileColor(COLORREF iColor, int iX, int iY);
COLORREF GetTileColor(int iX, int iY);

void Reset();
void PathClear();
void AllClear();
void BresenhamClear();
void ResetAttribute();

#endif