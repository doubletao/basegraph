#pragma once
#include "UITLTimeCode.h"

class UITimeLine :
	public UIElement
{
protected:
	UITLTimeCode * m_pTimeCode;
	UIScrollBar * m_pScrollHor;
public:
	UITimeLine();
	virtual ~UITimeLine();
protected:
	virtual void Release();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void InitializeObject();
	void ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual void RearrangeChildPos();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

