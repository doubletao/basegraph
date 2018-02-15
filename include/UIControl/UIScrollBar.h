#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UIButton.h"

class UIScrollBar :
	public UIElement
{
protected:
	enum
	{
		EmLeftTopButton,
		EmRightBottomButton,
		EmThumbButton,
	};
protected:
	bool m_bHorScroll;
	int m_nWholeLength;
	int m_nPageLength;
	int m_nPos;
protected:
	UIButton * m_pBtnLeftTop;
	UIButton * m_pBtnRightBottom;
	UIButton * m_pBtnThumb;
public:
	UIScrollBar();
	virtual ~UIScrollBar();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
	virtual void RearrangeChildPos();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nWholeLength, int nPageLength, int nID = 0, bool bHorizon = false, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
};
