#pragma once

class UISimpleTimeLine :
	public UIElement
{
public:
	UISimpleTimeLine();
	virtual ~UISimpleTimeLine();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void InitializeObject();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
};

