#ifndef __MIN_HEAP__
#define __MIN_HEAP__

class CMinHeap
{
public:
	CMinHeap();
	~CMinHeap();

	bool Insert(st_NODE *pNode);
	st_NODE *Remove();
	void RemoveAll();
	st_NODE *GetNode(int iX, int iY);
	void ResortHeap(st_NODE *pNode);

	void ClearHeap();
	void ReleaseNode();
	void SetState(eNODESTATE eColor);

	void DebugList(HDC hDC, int iWdith, int iHeight);

	int GetHeapCount();

private:
	st_NODE *m_pArr[df_HEAPSIZE_MAX];
	int m_iCurCount;
	int m_iMaxCount;
	eNODESTATE m_eColor; // state
};

#endif