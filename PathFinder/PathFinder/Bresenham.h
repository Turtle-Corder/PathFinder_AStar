#ifndef __BRESENHAM__
#define __BRESENHAM__

class CBresenham
{
public:
	CBresenham();
	~CBresenham();

	void SetPosition(int iStartX, int iStartY, int iEndX, int iEndY);
	bool GetNext(int *iPosX, int *iPosY);
	bool PeekNext(int *iPosX, int *iPosY);


private:
	int iError;
	int m_iAddX, m_iAddY;
	int m_iCurX, m_iCurY;

	int m_iDeltaX, m_iDeltaY;
	int m_iStartX, m_iStartY;
	int m_iEndX, m_iEndY;

};

#endif