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
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement);
	virtual void DrawElement(CDC * pDC);
};