#ifndef	__INC_BaseGraphDROPDOWNWND__
#define	__INC_BaseGraphDROPDOWNWND__

#include "UIControl/CustomScrollBar.h"

#define WM_DROPWND_DISFOCUS (WM_USER + 3701)
#define WM_DROPWND_SELECT   (WM_USER + 3702)

class CustomDropDownWnd : public CWnd
{
// Construction
public:
	CustomDropDownWnd();

// Attributes
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Operations
public:
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	
	void EnsureVisible();  //要显示的条目显示在中间
	void EnsureVisibleEx(int nItem);//要显示的条目显示在最上面或者最下面
	int AddString(LPCTSTR lpszString);  //返回总item数
	int AddStringEx(LPCTSTR lpszString);//返回Item序号
	int AddStringArray(const CStringArray & aryString);//返回总item数
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszString);
	void ResetContent();
	virtual int FindString(int nStartAfter, LPCTSTR lpszString) const;
	int ReplaceString(int nIndex,LPCTSTR lpszValue);

	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;
	
	int   SetItemData(int nIndex, DWORD_PTR dwItemData );
	DWORD_PTR GetItemData( int nIndex ) const;

	int SetItemHeight( int nItemHeight );
	int GetItemHeight() const;

	int HitTest( CPoint pt , int& nRow );
	
	int GetNums();
	
	void ReSetScrollWndPos();
	
	BOOL SetImageList(CImageList* pImageList);
	CImageList* GetImageList();

	BOOL SetItemImage(int nItemIndex,UINT nImageIndex);
	int  GetItemImage(int nItemIndex )const;

	void ShowTip(int nIndex);

protected:
	virtual  void DrawOneItem( CDC* pDC , int nItem );
	virtual  void DrawBorder( CDC* pDC );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CustomDropDownWnd)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CustomDropDownWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CustomDropDownWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpParams);
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CImageList      m_ItemImagelist;    //项目的图标
	CMap<UINT,UINT, UINT,UINT&> m_mapItemImage; //项所对应的ImageList
public:
	CustomScrollBar *	 m_pVScrollBar;		//	垂直移动条
	CWnd*		m_pMessageWnd;	  //发送消息的父窗口。
	int			m_nItemHeight;		//条目的高度。

private:
	CStringArray	m_AllItemArray;		//字符串内容。
	CDWordArray  m_arrItemData;      //与每个项所对应的附加数据
	int			m_nCurSel;			//当前选中的条目序号

	int			m_nTop;				//当前滚动条所处位置下，窗口显示的第一个条目序号值。
	int			m_nRows;			//当前滚动条所处位置下，窗口显示总的条目数。
	
	BOOL		m_bScrollBarShow;	//显示\隐藏滚动条。
	
	static long	sm_lRef;
	BOOL		m_bSortString;

//	CDYTipWnd	*m_pToolTip;
	CPoint		m_ptOldCursorPos;
	int			m_nPreIndex;

public:
	void		CalRowsNum(); //计算条目总数
	void		SetVerScrollBarRange(); //设置垂直滚动条滚动范围
	void		ReDrawOneItem( int nItem ); //重画条目。
	void		SetSortString(BOOL bSort){ m_bSortString = bSort;};
protected:
	static LRESULT CALLBACK EXPORT WndProcHook(HWND,UINT,WPARAM,LPARAM);
	int			AddStringSort(CString strInsert,BOOL	bAsc=FALSE);
public:
	static BOOL RegisterDropDownWndClass(HINSTANCE hInst);
	static BOOL UnRegisterDropDownWndClass();
protected:
	static WNDPROC m_lpSuperDropDownWnd;
	BOOL m_bIsAutoDelete;//是否为自创建。
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
