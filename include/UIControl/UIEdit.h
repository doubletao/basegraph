#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\CustomEdit.h"

class BASEGRAPH_UICONTROL_API UIEdit
	: public CWnd
	, public UIElement
{
	DECLARE_MESSAGE_MAP()
protected:
	CustomEdit * m_pEdit;
public:
	UIEdit();
	virtual ~UIEdit();
protected:
	// ×¢²á´°¿ÚÀà
	static BOOL RegisterControlClass();
	static BOOL RegisterControlClass(HINSTANCE	hInst);
protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnEndEdit(WPARAM, LPARAM);
	afx_msg void OnEnChanged();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent);
	virtual void RearrangeChildPos();
	void SetText(CString strText);
	CString GetText();
	virtual inline void MoveTo(CRect rtNew, bool bRefresh = true);
	virtual inline void MoveTo(int nX, int nY, bool bRefresh = true);
	virtual inline void MoveTo(int nX, int nY, int nWidth, int nHeight, bool bRefresh = true);
	virtual void SetVisible(bool bVisible, bool bRefresh = true);
	CWnd * GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;
};
