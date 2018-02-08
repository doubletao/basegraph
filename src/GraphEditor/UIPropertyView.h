#pragma once

class UIPropertyView :
	public UIElement
{
public:
	UIPropertyView();
	virtual ~UIPropertyView();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void RearrangeChildPos();
	virtual void InitializeObject();
	void ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

