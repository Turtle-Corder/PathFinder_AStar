#include "stdafx.h"
#include "Bresenham.h"

CBresenham::CBresenham() 
	: iError(0), m_iAddX(0), m_iAddY(0), m_iCurX(0), m_iCurY(0),
	m_iDeltaX(0), m_iDeltaY(0), m_iStartX(0), m_iStartY(0), m_iEndX(0), m_iEndY(0)
{
}

CBresenham::~CBresenham()
{
}

void CBresenham::SetPosition(int iStartX, int iStartY, int iEndX, int iEndY)
{
	iError = 0;

	m_iCurX = m_iStartX = iStartX;
	m_iCurY = m_iStartY = iStartY;
	m_iEndX = iEndX;
	m_iEndY = iEndY;

	m_iDeltaX = iEndX - iStartX;
	m_iDeltaY = iEndY - iStartY;

	// X �������� �̵�
	if (m_iDeltaX < 0)
	{
		m_iAddX = -1;
		m_iDeltaX = -m_iDeltaX; // abs(iEndX-iStartX)
	}
	// X ���������� �̵�
	else
	{
		m_iAddX = 1;
	}

	// Y �������� �̵�
	if (m_iDeltaY < 0)
	{
		m_iAddY = -1;
		m_iDeltaY = -m_iDeltaY; // abs(iEndY-iStartY)
	}
	// Y ���������� �̵�
	else
	{
		m_iAddY = 1;
	}
}

bool CBresenham::GetNext(int * iPosX, int * iPosY)
{
	if (m_iCurX == m_iEndX && m_iCurY == m_iEndY)
		return false;

	// ���� 45�� �϶��� X,Y �ƹ��ų� ������..
	// �¿�� �� ���
	if (m_iDeltaX >= m_iDeltaY)
	{
		iError += m_iDeltaY*2;
		m_iCurX += m_iAddX;

		if (iError >= m_iDeltaX)
		{
			iError -= m_iDeltaX*2;
			m_iCurY += m_iAddY;
		}
	}
	// ���Ʒ��� �� ���
	else
	{
		iError += m_iDeltaX*2;
		m_iCurY += m_iAddY;

		if (iError >= m_iDeltaY)
		{
			iError -= m_iDeltaY*2;
			m_iCurX += m_iAddX;
		}
	}

	*iPosX = m_iCurX;
	*iPosY = m_iCurY;

	return true;
}

bool CBresenham::PeekNext(int * iPosX, int * iPosY)
{
	if (m_iCurX == m_iEndX && m_iCurY == m_iEndY)
		return false;

	int iTempX = m_iCurX;
	int iTempY = m_iCurY;
	int iTempError = iError;

	if (m_iDeltaX >= m_iDeltaY)
	{
		iTempError += m_iDeltaY*2;
		iTempX += m_iAddX;

		if (iTempError >= m_iDeltaX)
		{
			iTempError -= m_iDeltaX*2;
			iTempY += m_iAddY;
		}
	}
	else
	{
		iTempError += m_iDeltaX*2;
		iTempY += m_iAddY;

		if (iTempError >= m_iDeltaY)
		{
			iTempError -= m_iDeltaY*2;
			iTempX += m_iAddX;
		}
	}

	*iPosX = iTempX;
	*iPosY = iTempY;

	return true;
}
