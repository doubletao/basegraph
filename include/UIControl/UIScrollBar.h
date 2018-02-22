#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UIButton.h"

class BASEGRAPH_UICONTROL_API UIScrollBar :
	public UIElement
{
protected:
	bool m_bHorScroll;
	int m_nWholeLength;
	int m_nPageLength;
	int m_nPos;
	int m_nStep;

	bool m_bMoving;
	CPoint m_ptBegin;
	CPoint m_ptEnd;
protected:
	UIButton * m_pBtnLeftTop;
	UIButton * m_pBtnRightBottom;
	UIButton * m_pBtnThumb;
public:
	UIScrollBar();
	virtual ~UIScrollBar();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);
	//初始化工作
	virtual void RearrangeChildPos();
	virtual void Refresh();
	void Recalculate(bool bRefresh = true);
	void MoveStep(bool bFore);
	void MovePage(bool bFore);

	bool ThumbAreaMsgHandler(UINT nMsg, LPARAM lParam, WPARAM wParam);
	CRect GetThumbAreaRect();
	CRect GetThumbRect();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nWholeLength, int nPageLength, int nID = 0, bool bHorizon = false, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	void ResetScroll(int nWholeLength, int nPageLength, int nPos = -1, bool bRefresh = true);
	void SetPos(int nPos, bool bRefresh = true);
	int GetPos();
	void SetWholeLength(int nWholeLength, bool bRefresh = true);
	int GetWholeLength();
	void SetPageLength(int nPageLength, bool bRefresh = true);
	int GetPageLength();
};
