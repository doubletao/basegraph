#pragma once
#include "UIElement.h"

class BASEGRAPH_UICONTROL_API UIEdit :
	public UIElement
{
protected:
	CEdit * m_pEdit;
public:
	UIEdit();
	virtual ~UIEdit();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, int nEditID, HWND hParentWnd, UIElement * pParent);
	virtual void RearrangeChildPos();
	void SetText(CString strText);
	CString GetText();
};
