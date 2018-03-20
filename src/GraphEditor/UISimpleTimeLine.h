#pragma once
#include "UIControl\UISliderCtr.h"

class UISimpleTimeLine :
	public UIElement
{
protected:
	UISliderCtr * m_pSimpTimeLine;
public:
	UISimpleTimeLine();
	virtual ~UISimpleTimeLine();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void SetWholeFrame(int nFrame);
	void SetFrame(int nFrame);
};
