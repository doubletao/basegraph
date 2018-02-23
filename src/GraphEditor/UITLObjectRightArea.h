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
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement);
	virtual void DrawElement(CDC * pDC);
};