#pragma once

class UITLTimeCode :
	public UIElement
{
public:
	UITLTimeCode();
	virtual ~UITLTimeCode();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd, UIElement * pParent);
	void SetFrame(int nFrame, float fFPS = 25.0);//这个浮点目前没有意义，内部会直接转int
	virtual void DrawElement(CDC * pDC);
};
