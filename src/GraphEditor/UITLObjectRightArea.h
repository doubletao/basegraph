#pragma once

class UITLObjectArea;

class UITLObjectRightArea :
	public UIScrollElement
{
private:
	UITLObjectArea * m_pObjectArea;
public:
	UITLObjectRightArea();
	~UITLObjectRightArea();
protected:
	int DrawItemRight(CDC * pDC, CString strID, int nCurrentY);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UITLObjectArea * pParentElement);
	virtual void DrawElement(CDC * pDC);
};