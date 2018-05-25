#pragma once

class UITLTimeLinePointer :
	public UIElement
{
public:
	UITLTimeLinePointer();
	virtual ~UITLTimeLinePointer();
private:
	DWORD m_clLineColor;
public:
	virtual void DrawElement(CDC * pDC);
	bool Create(int nID, HWND hParentWnd, UIElement * pParentElement);//时间线指针没有坐标及宽高，其宽度根据绘制参数决定，其位置及高度根据当前帧号及父窗口宽高来决定。
	void SetLineColor(COLORREF clLineColor, bool bRefresh = true);
};

