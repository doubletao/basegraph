#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UIButton.h"

class BASEGRAPH_UICONTROL_API UISliderButton
	: public UIButton
{
public:
	UISliderButton();
	~UISliderButton();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent);
	virtual void DrawElement(CDC * pDC);
};

class BASEGRAPH_UICONTROL_API UISliderCtr
	: public UIElement
{
protected:
	bool m_bHorScroll;
	int m_nMin;
	int m_nMax;
	int m_nLogicPos;

	bool m_bMoving;

	UISliderButton * m_pSliderBtn;
public:
	UISliderCtr();
	~UISliderCtr();
protected:
	virtual void RearrangeChildPos();
	void Recalculate(bool bRefresh = true);
	virtual void Refresh();
	int LogicToReal(int nLogicPos);
	int RealToLogic(int nRealPos);
	bool SliderMsgHandle(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nMin, int nMax, int nID = 0, bool bHorizon = false, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void SetPos(int nPos, bool bRefresh = true, bool bChanging = true);
	int GetPos();
	void SetLimit(int nMin, int nMax, bool bRefresh = true);
	int GetMax();
	int GetMin();
	virtual void DrawElement(CDC * pDC);
};
