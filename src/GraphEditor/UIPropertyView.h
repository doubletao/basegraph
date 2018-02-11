#pragma once
#include "UIPropertyTabBar.h"

class UIPropertyView :
	public UIElement
{
protected:
	UIPropertyTabBar * m_pPropertyTabBar;
public:
	UIPropertyView();
	virtual ~UIPropertyView();
protected:
	virtual void Release();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void RearrangeChildPos();
	virtual void InitializeObject();
	void ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

