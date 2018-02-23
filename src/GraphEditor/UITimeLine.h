#pragma once
#include "UITLTimeCode.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"

class UITimeLine :
	public UIElement
{
protected:
	UITLTimeCode * m_pTimeCode;
	UIScrollBar * m_pScrollHor;
	UIScrollBar * m_pScrollVer;
	UITLObjectArea * m_pObjectArea;
public:
	UITimeLine();
	virtual ~UITimeLine();
protected:
	virtual void Release();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void RearrangeChildPos();
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);

	void InitializeObject();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void AdjustScrollZoom();
};

