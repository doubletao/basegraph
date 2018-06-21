#ifndef	__INC_REPLCOMBOX2COMBOBOX__
#define	__INC_REPLCOMBOX2COMBOBOX__

#include "UIControl/CustomEdit.h"

class CustomDropDownWnd;
class BASEGRAPH_UICONTROL_API CustomComboBox : public CWnd
{
	// Construction
	DECLARE_DYNAMIC(CustomComboBox)

// Construction
public:
	CustomComboBox();

public:
// 注册窗口类
	static BOOL RegisterControlClass();
	static BOOL RegisterControlClass(HINSTANCE	hInst);
	static BOOL UnregisterControlClass(HINSTANCE hInst);

// 创建该种ComboBox的方法
	BOOL SubclassComboBox( UINT nID, CWnd* pParent );//通过子类化创建combobox控件。
	BOOL AutoLoad( UINT nID, CWnd* pParent, BOOL fAutoDelete );//自动加载combobox控件。

public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
		
// Operations
public:
	//窗口显示文字操作。
	void GetWindowText( CString &rString );
	void SetWindowText( CString rString );

	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect,BOOL bSendChangeMessageToParent = FALSE);
	
	int FindStringExact( int nIndexStart, LPCTSTR lpszFind ) const;
	int SelectString( int nStartAfter, LPCTSTR lpszString );

	int AddString(LPCTSTR lpszString); //返回总条目数
	int AddStringEx(LPCTSTR lpszString);//返回插入的item序号
	int AddStringArray(const CStringArray & aryString); //返回总条目数
	int DeleteString(UINT nIndex);
	int DelString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszString);
	int ReplaceString(int nIndex,LPCTSTR lpszValue);
	void ResetContent();
	void RereshContent();
	int FindString(int nStartAfter, LPCTSTR lpszString) const;

	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;

	int SetItemData(int nIndex, DWORD_PTR dwItemData );
	DWORD_PTR GetItemData( int nIndex ) const;

	CString GetLBText(int nIndex);
	CString GetEditText();
	void	SetEditText(CString strText);

	int SetItemHeight( int nItemHeight );
	int GetItemHeight() const;

	virtual  void GetDropWndRect( CRect &rtDropWnd);
	virtual  void GetDropBtnRect( CRect &rtDropBtn);
	virtual  void GetCtrlRect( CRect &rtCtrlBtn );

	virtual  void DrawText( CDC *pDC );
	virtual  void DrawDropBtn( CDC* pDC , BOOL bIsPress );

	int	 HitTest( CPoint pt );

	//	set and get dropped width of dropped window
	int GetDroppedWidth() const;
	int SetDroppedWidth(UINT nWidth);

	//	Set and get text format. This text format formated the 
	//	text shown in combobox when combobox is closed.
	UINT GetTextFormat() const;
	void SetTextFormat(UINT nFormat);

	BOOL EnableWindow(BOOL bEnable = TRUE);
    
	BOOL SetItemImage(int nItemIndex,UINT nImageIndex);
	int  GetItemImage(int nItemIndex )const;

	BOOL IsDropDown(){return m_bIsDropDown;};
	
protected:
	CustomDropDownWnd*	m_pDropDownWnd;//combobox 窗口的下拉窗口。
	CustomEdit			*m_pEdit;
// 	CInComboEdit	*m_pEdit;//带有编辑功能的 combobox 窗口的编辑框。
// 	CDYTipWnd		*m_pToolTip;//提示栏
	BOOL			m_bIsDropDown;//combobox是否处于下拉状态。
	
	int				m_nHit;//鼠标点击的位置判断。-1:combox以外；0：下拉按钮之内。
	int				m_nCtrlHeight;//控件与下拉窗口中条目的高度。
	int				m_nCtrlWidth;//控件的宽度。
	
	CString			m_strCurSel;//当前选中的条目的字符串。
	CString			m_strKeyDown;//作用主要用于保存keydown时的m_strCurSel值,以判断keyup时变化否,因为在down和up之间onchangEdit改变了m_strCurSel的值
	int				m_nCurSel;
	
	int				m_nWndHeight;//窗口高度
	
	int				m_nStyle; //是否带edit控件。
	BOOL			m_bAutoDelete; // 该C++ CWnd object 需要自动删除

	BOOL			m_bEnableRunTimeList;
	CSize			m_szTopLeftBmp;
	CSize			m_szArrowBtn;//按钮图片大小

	CImage *		m_pDropDownBtnU;
	CImage *		m_pDropDownBtnD;
	CImage *		m_pDropDownBtnF;
	CImage *		m_pDropDownBtnX;

	int		m_nDropDownBtnWidth;
	int		m_nDropDownBtnHeight;

	COLORREF	m_clGray;
	COLORREF	m_clText;
	COLORREF	m_clGrayText;
	COLORREF	m_clSelectText;

	CBrush	m_brCombBK;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CustomComboBox)
	public:
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CustomComboBox();

protected:
	// 该类的Proc
	static LRESULT CALLBACK EXPORT WndProcHook(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);	
	// Generated message map functions
protected:
	//{{AFX_MSG(CustomComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeEdit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSetCurSel(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetCurSel(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSelectString(WPARAM wParam,LPARAM lParam);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	virtual afx_msg LRESULT	OnDropWndSelect(WPARAM wParam,LPARAM lParam);
	virtual afx_msg LRESULT	OnDropWndKillFocus(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMsgGetTextLength(WPARAM wP,LPARAM lP);
	afx_msg LRESULT OnMsgGetText(WPARAM wP,LPARAM lP);
	afx_msg LRESULT OnMsgSetText(WPARAM wP,LPARAM lP);
	afx_msg LRESULT OnFlushWnd(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

private:
	static WNDPROC	lpfnSuperComboBox;	//系统ComboBox的Proc

protected:
	int m_nDroppedWidth;				//	Dropped window witdh
	UINT m_nTextFormat;				//	Text format

	CString m_strImeBuffer;				
//add for skin function
//static variant and function.

public:	
	BOOL EnableRunTimeList(BOOL bEnable);

protected:
	BOOL CreateChildWindows(DWORD dwStyle,CRect rtControl);
	void	 CalcClientRect(CRect& rtClient);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !__INC_REPLCOMBOX2COMBOBOX__