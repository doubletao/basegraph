#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\CustomComboBox.h"

class BASEGRAPH_UICONTROL_API UIComboBox
	: public CWnd
	, public UIElement
{
	DECLARE_MESSAGE_MAP()
protected:
	CustomComboBox * m_pComboBox;
public:
	UIComboBox();
	virtual ~UIComboBox();
protected:
	// 注册窗口类
	static BOOL RegisterControlClass();
	static BOOL RegisterControlClass(HINSTANCE	hInst);
protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnCbnSelchange();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent);
	virtual void RearrangeChildPos();
	virtual inline void MoveTo(CRect rtNew, bool bRefresh = true);
	virtual inline void MoveTo(int nX, int nY, bool bRefresh = true);
	virtual inline void MoveTo(int nX, int nY, int nWidth, int nHeight, bool bRefresh = true);
	virtual void SetVisible(bool bVisible, bool bRefresh = true);
	CWnd * GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;

	void GetWindowText(CString &rString);
	void SetWindowText(CString rString);
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect, BOOL bSendChangeMessageToParent = FALSE);
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int SelectString(int nStartAfter, LPCTSTR lpszString);
	int AddString(LPCTSTR lpszString); //返回总条目数
	int AddStringEx(LPCTSTR lpszString);//返回插入的item序号
	int AddStringArray(const CStringArray & aryString); //返回总条目数
	int DeleteString(UINT nIndex);
	int DelString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszString);
	int ReplaceString(int nIndex, LPCTSTR lpszValue);
	void ResetContent();
	void RereshContent();
	int FindString(int nStartAfter, LPCTSTR lpszString) const;
	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;
	int SetItemData(int nIndex, DWORD_PTR dwItemData);
	DWORD_PTR GetItemData(int nIndex) const;
	CString GetLBText(int nIndex);
	CString GetEditText();
	void	SetEditText(CString strText);
	UINT GetTextFormat() const;
	void SetTextFormat(UINT nFormat);
	BOOL EnableWindow(BOOL bEnable = TRUE);
	BOOL SetItemImage(int nItemIndex, UINT nImageIndex);
	int  GetItemImage(int nItemIndex)const;
	BOOL IsDropDown();
};
