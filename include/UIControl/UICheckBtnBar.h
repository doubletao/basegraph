#pragma once
#include "UIControl\UIElement.h"

class BASEGRAPH_UICONTROL_API UICheckBtnBar : public UIElement
{
public:
	UICheckBtnBar();
	virtual ~UICheckBtnBar();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void InitializeObject() = 0;
	void ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void UnCkeckAll(int nExceptID = 0);
};

