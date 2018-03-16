#pragma once
#include "UIControl\UIElement.h"

class BASEGRAPH_UICONTROL_API UIButtonBar : public UIElement
{
public:
	UIButtonBar();
	virtual ~UIButtonBar();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void InitializeObject() = 0;
	virtual void AddSplitLine(int nWidth, int nHeight, COLORREF clColor);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

