#pragma once
#include "UIControl\UIElement.h"
#include "GraphEditorCommonDef.h"

class UIObjectBar : public UIElement
{
public:
	UIObjectBar();
	virtual ~UIObjectBar();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	void InitializeObject();
	void ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

