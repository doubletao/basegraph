#pragma once
#include "UIObjectBar.h"
#include "UIActionBar.h"
#include "UIPlayControlBar.h"
#include "UISimpleTimeLine.h"
#include "UIPropertyView.h"
#include "UITimeLine.h"
#include "3DMainViewWnd.h"

struct SafeLineSettingParam
{
	BOOL		bDrawSafeLine1;
	float		fSafeLine1Percent;
	DWORD		nColorSafeLine1;
	BOOL		bDrawSafeLine2;
	float		fSafeLine2Percent;
	BOOL		bDrawCrossLine;
	DWORD		nColorCrossLine;
	BOOL		bUse4_3SafeLine;
	DWORD		nColorSafeLine2;

	SafeLineSettingParam()
	{
		bDrawSafeLine1 = TRUE;
		fSafeLine1Percent = 0.8f;
		nColorSafeLine1 = ABGR(255, 0, 255, 255);//黄色警告

		bDrawSafeLine2 = TRUE;
		fSafeLine2Percent = 0.9f;
		nColorSafeLine2 = ABGR(255, 0, 0, 255);//红色禁止

		bDrawCrossLine = TRUE;
		nColorCrossLine = ABGR(255, 70, 48, 37);
		bUse4_3SafeLine = FALSE;
	}
};

class BaseGraphMainFrame :
	public CSkinFrameWnd
{
private:
	UIObjectBar m_uiObjectBar;
	UIActionBar m_uiActionBar;
	UIPlayControlBar m_uiPlayControlBar;
	UISimpleTimeLine m_uiSimpleTimeLine;
	UIPropertyView m_uiPropertyView;
	UITimeLine m_uiTimeLine;
	C3DMainViewWnd m_pwndMainView;

	UIElement * m_pCaptureElement;
	SafeLineSettingParam m_stuSafeLineParam;
public:
	BaseGraphMainFrame();
	virtual ~BaseGraphMainFrame();
	DECLARE_DYNAMIC(BaseGraphMainFrame)
	// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg virtual void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg virtual void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg virtual void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg virtual void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg virtual void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg virtual BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnUIElementBtnClicked(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnUIElementSetCapture(WPARAM wParam = 0, LPARAM lParam = 0);
public:
	void OnBtnCreateTitle();
	void OnBtnCreateScroll();
	void OnBtnCreateImage();
	void OnBtnNewFile();
	void OnBtnSaveFile();
	void OnBtnSaveAs();
	void OnBtnDefaultAction();
	void OnBtnZoomAction();
	void OnBtnRotateAction();
	void OnBtnSystemSetting();
	SafeLineSettingParam & GetSafeLineSettingParam();
};

