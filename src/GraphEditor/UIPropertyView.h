#pragma once
#include "UIPropertyTabBar.h"
#include "UIPropertyViewElement.h"

class UIPropertyView :
	public UIElement
{
protected:
	UIPropertyTabBar * m_pPropertyTabBar;
	UIPropertyViewElement * m_pSpaceElement;
public:
	UIPropertyView();
	virtual ~UIPropertyView();
protected:
	virtual void Release();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void RearrangeChildPos();
	virtual void InitializeObject();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

