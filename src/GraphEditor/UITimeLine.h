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
	void OnFrameChanged(int nFrame);
public:
	virtual void OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom);
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual void DrawElement(CDC * pDC);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void AdjustScrollZoom();//横向滚动条适配到全屏显示
	void AdjustScrollZoom(int nDeta);//调整横向滚动条,nDeta可正可负,每单位调整百分之n
	void SetFrame(int nFrame, float fFPS = 25.0);//这个浮点目前没有意义，内部会直接转int
};

