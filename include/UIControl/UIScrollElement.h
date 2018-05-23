#pragma once
#include "UIControl\UIElement.h"

class BASEGRAPH_UICONTROL_API UIScrollElement :
	public UIElement
{
protected:
	int m_nScrollX;
	int m_nScrollY;
	int m_nLogicWidth;
	int m_nLogicHeight;
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
public:
	UIScrollElement();
	virtual ~UIScrollElement();
public:
	//向上追溯获取自己的实际窗口坐标
	virtual int GetRealX();
	virtual int GetRealY();
	virtual inline CRect GetRect();
	bool SetScrollInfo(int nLogicWidth, int nLogicHeight, bool bRefresh = true);
	int GetScrollXPos();
	int GetScrollYPos();
	bool SetScrollXPos(int nXPos, bool bRefresh = true);
	bool SetScrollYPos(int nYPos, bool bRefresh = true);
	void SetLogicWidth(int nWidth);
	void SetLogicHeight(int nHeight);
	int GetLogicWidth();
	int GetLogicHeight();
	float GetXRatio();
	float GetYRatio();
};
