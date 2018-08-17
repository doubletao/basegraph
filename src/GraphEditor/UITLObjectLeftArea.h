#pragma once

class UITLObjectArea;

class UITLObjectLeftArea :
	public UIScrollElement
{
private:
	UITLObjectArea * m_pObjectArea;
public:
	UITLObjectLeftArea();
	~UITLObjectLeftArea();
protected:
	//本函数会递归绘制item的子，返回绘制完成后的Y值
	int DrawItemLeft(CDC * pDC, CString strID, int nCol, int nCurrentY);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UITLObjectArea * pParentElement);
	virtual void DrawElement(CDC * pDC);
};
