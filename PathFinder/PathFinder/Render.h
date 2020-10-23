#ifndef __RENDER__
#define __RENDER__

extern HBRUSH g_WhiteBrush;
extern HBRUSH g_RedBrush;
extern HBRUSH g_GreenBrush;
extern HBRUSH g_GreyBrush;
extern HBRUSH g_BlueBrush;
extern HBRUSH g_YellowBrush;
extern HBRUSH g_ColorBrush;

extern HPEN g_RectPen;
extern HPEN g_BresenhamPen;

extern TCHAR g_szShotName[64];

extern _vec2 g_vPathPos[100];
extern int g_iPathCnt;

extern _vec2 g_vLinePos[100];
extern int g_iLineCnt;

void InitDrawObjects();

void Render(HDC hDC);

void DrawGrid(HDC hDC);

void DrawTIle(HDC hDC);

void DrawPathLine(HDC hDC);

void DrawKeyInfo(HDC hDC);

void DrawBresenham(HDC hDC);

BOOL ScreenShot();

BOOL GetTimeStamp(TCHAR* szName = (TCHAR*)_T("screenshot_"));


#endif