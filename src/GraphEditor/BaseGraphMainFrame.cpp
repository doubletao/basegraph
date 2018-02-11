#include "stdafx.h"
#include "BaseGraphMainFrame.h"
#include "GraphEditorStatusManager.h"

IMPLEMENT_DYNAMIC(BaseGraphMainFrame, CSkinFrameWnd)

BEGIN_MESSAGE_MAP(BaseGraphMainFrame, CSkinFrameWnd)
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCCREATE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_MESSAGE(EmUIElementMsg_BtnClick, BaseGraphMainFrame::OnUIElementBtnClicked)
END_MESSAGE_MAP()

BaseGraphMainFrame::BaseGraphMainFrame()
{
}

BaseGraphMainFrame::~BaseGraphMainFrame()
{
}

BOOL BaseGraphMainFrame::PreTranslateMessage(MSG * pMsg)
{
	auto DealMsg = [&](UIElement & bar)->bool
	{
		if (UIElement::NeedContinue(bar.HandleMsg(pMsg->message, pMsg->lParam, pMsg->wParam)))
		{
			return false;
		}
		return true;
	};
	if (DealMsg(m_uiActionBar)
		|| DealMsg(m_uiPlayControlBar)
		|| DealMsg(m_uiSimpleTimeLine)
		|| DealMsg(m_uiPropertyView)
		|| DealMsg(m_uiTimeLine))
	{
		return TRUE;
	}
	return CSkinFrameWnd::PreTranslateMessage(pMsg);
}

void BaseGraphMainFrame::OnNcPaint()
{
	CWindowDC dc(this);
	
	CImage * pImgBk = UIControl_GetImage(ThemeImage_SkinWndNcBkgrd);
	int nFrameWidth = UIControl_GetIntValue(ThemeInt_SkinWndFrameWidth);
	int nTitleHeight = UIControl_GetIntValue(ThemeInt_SkinWndTitleHeight);
	CImage * pImgIcon = UIControl_GetImage(ThemeImage_SkinFrameDefaultIcon);
	LOGFONT lfTitleFont = UIControl_GetFont(ThemeFont_SkinWndTitleText);
	COLORREF nFontColor = UIControl_GetColor(ThemeColor_SkinWndTitleText);

	if (nullptr != pImgBk
		&& nFrameWidth > 0
		&& nTitleHeight > 0
		&& nullptr != pImgIcon
		)
	{
		CDC MemDC;

		// 获取位置
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

		CRect rcClient;
		GetClientRect(&rcClient);

		//剪除掉客户区
		rcClient.OffsetRect(nFrameWidth, nTitleHeight);
		dc.ExcludeClipRect(rcClient.left, rcClient.top,
			rcClient.right, rcClient.bottom);

		MemDC.CreateCompatibleDC(&dc);

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
		MemDC.SelectObject(&bmp);
		MemDC.SetBkMode(TRANSPARENT);

		CRect destRect;

		//绘制底部
		destRect.left = 0;
		destRect.top = rcWindow.Height() - nFrameWidth;
		destRect.right = destRect.left + rcWindow.Width();
		destRect.bottom = destRect.top + nFrameWidth;
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);

		//左边框
		destRect.left = 0;
		destRect.top = 0;
		destRect.right = destRect.left + nFrameWidth;
		destRect.bottom = destRect.top + rcWindow.Height();
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);


		//右边框
		destRect.left = rcWindow.Width() - nFrameWidth;
		destRect.top = 0;
		destRect.right = destRect.left + nFrameWidth;
		destRect.bottom = destRect.top + rcWindow.Height();
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);

		//绘制标题栏
		destRect.left = 0;
		destRect.top = 0;
		destRect.right = destRect.left + rcWindow.Width() + nFrameWidth;
		destRect.bottom = destRect.top + nTitleHeight;
		m_rtTitleBar = destRect;
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);

		int nXOffset = 0;
		int nYOffset = 0;
		DWORD	dwStyle = GetStyle();
		if (dwStyle & WS_MAXIMIZE)
		{
			nXOffset = nFrameWidth;
			nYOffset = nFrameWidth * 2;
		}

		//绘制ICON
		int nIconWidth = pImgIcon->GetWidth();
		int nIconHeight = pImgIcon->GetHeight();
		int xIconPos = nFrameWidth + nXOffset;
		int yIconPos = (nTitleHeight + nFrameWidth - nIconHeight + nYOffset) / 2;
		m_rcIcon.SetRect(xIconPos, yIconPos, xIconPos + nIconWidth, yIconPos + nIconHeight);
		BufferFunction::DrawImageWithAplha(&MemDC, pImgIcon, m_rcIcon, m_rcIcon,
			CRect(0, 0, nIconWidth, nIconHeight), CRect(), TRUE, 255, FALSE);

		//沿着窗口四周画一圈一个像素宽的蓝色矩形
		int nSavedDC = MemDC.SaveDC();
		COLORREF col = RGB(142, 170, 195);
		CPen pen(PS_SOLID, 1, col);
		MemDC.SelectObject(&pen);
		(CBrush*)MemDC.SelectStockObject(NULL_BRUSH);//空心的笔
		MemDC.RoundRect(&rcWindow, CPoint(6, 6));
		MemDC.RoundRect(&rcWindow, CPoint(8, 8));
		pen.DeleteObject();
		MemDC.RestoreDC(nSavedDC);

		//绘制标题栏按钮
		CRect rcPaint;
		dc.GetClipBox(&rcPaint);
		for (size_t i = 0; i < m_vecButton.size(); i++)
		{
			UIButton *& pButton = m_vecButton[i];
			if (pButton)
			{
				CRect rcControl = pButton->GetRect();

				// 判断当前按钮是否需要重绘
				if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
				{
					continue;
				}

				pButton->DrawElement(&MemDC);
			}
		}

		//绘制标题
		CString strTitle;
		GetWindowText(strTitle);
		CFont font;
		font.CreateFontIndirect(&lfTitleFont);
		MemDC.SelectObject(&font);
		MemDC.SetTextColor(nFontColor);
		CRect rtTitle(xIconPos + nIconWidth + nFrameWidth + nXOffset, 0, rcWindow.Width(), nTitleHeight + nYOffset);
		MemDC.DrawText(strTitle, rtTitle, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

		//绘制物件条
		m_uiObjectBar.DrawElement(&MemDC);

		dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &MemDC, 0, 0, SRCCOPY);

		dc.SelectClipRgn(NULL);

		MemDC.DeleteDC();
		bmp.DeleteObject();
	}
}

void BaseGraphMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptTmp = point;
	ptTmp.x -= rcWindow.left;
	ptTmp.y -= rcWindow.top;
	OnNcPaint();
	if (m_uiObjectBar.HitTest(ptTmp))
	{
		return;
	}
	CSkinFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}

void BaseGraphMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptTmp = point;
	ptTmp.x -= rcWindow.left;
	ptTmp.y -= rcWindow.top;
	m_uiObjectBar.HandleMsg(WM_LBUTTONDOWN, MAKELPARAM(ptTmp.x, ptTmp.y), 0);
	OnNcPaint();
	if (m_uiObjectBar.HitTest(ptTmp))
	{
		return;
	}
	CSkinFrameWnd::OnNcLButtonDown(nHitTest, point);
}

void BaseGraphMainFrame::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptTmp = point;
	ptTmp.x -= rcWindow.left;
	ptTmp.y -= rcWindow.top;
	m_uiObjectBar.HandleMsg(WM_LBUTTONUP, MAKELPARAM(ptTmp.x, ptTmp.y), 0);
	OnNcPaint();
	if (m_uiObjectBar.HitTest(ptTmp))
	{
		return;
	}
	CSkinFrameWnd::OnNcLButtonUp(nHitTest, point);
}

void BaseGraphMainFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptTmp = point;
	ptTmp.x -= rcWindow.left;
	ptTmp.y -= rcWindow.top;
	m_uiObjectBar.HandleMsg(WM_MOUSEMOVE, MAKELPARAM(ptTmp.x, ptTmp.y), 0);
	OnNcPaint();
	if (m_uiObjectBar.HitTest(ptTmp))
	{
		return;
	}
	CSkinFrameWnd::OnNcMouseMove(nHitTest, point);
}

BOOL BaseGraphMainFrame::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CSkinFrameWnd::OnNcCreate(lpCreateStruct))
	{
		return FALSE;
	}

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);

	//创建并加入物件条
	int nTitleHeight = UIControl_GetIntValue(ThemeInt_SkinWndTitleHeight);
	int nFrameWidth = UIControl_GetIntValue(ThemeInt_SkinWndFrameWidth);
	int nY = 0;
	int nX = UIControl_GetIntValue(ThemeInt_SkinWndTitleObjectBarX);
	DWORD	dwStyle = GetStyle();
	if (dwStyle & WS_MAXIMIZE)
	{
		nY += nFrameWidth;
		nX += nFrameWidth;
	}
	m_uiObjectBar.Create(nX, nY, rcWnd.Width(), nTitleHeight, GetSafeHwnd());

	return TRUE;
}

int BaseGraphMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSkinFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("图文编辑器"));

	m_uiActionBar.Create(0, 0, 100, 100, GetSafeHwnd());
	m_uiPlayControlBar.Create(0, 0, 100, 100, GetSafeHwnd());
	m_uiSimpleTimeLine.Create(0, 0, 100, 100, GetSafeHwnd());
	m_uiPropertyView.Create(0, 0, 100, 100, GetSafeHwnd());
	m_uiTimeLine.Create(0, 0, 100, 100, GetSafeHwnd());

	//测试代码
	CString strObjID = _T("");
	std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam = DataBase_GetDefaultParam(BaseGraph_DataBase::ObjectType_Image);
	mapParam[BaseGraph_DataBase::Image_Path].SetValue(_T("F:\\Media\\图片\\1.png"));
	mapParam[BaseGraph_DataBase::Image_DispWidth].SetValue(800, BaseGraph_Common::ValueType_INT);
	mapParam[BaseGraph_DataBase::Image_DispHeight].SetValue(600, BaseGraph_Common::ValueType_INT);
	DataBase_CreateItem(BaseGraph_DataBase::ObjectType_Image, mapParam, strObjID);
	if (!strObjID.IsEmpty())
	{
		DataRender_UpdateRender(strObjID);
		byte * pBuf = nullptr;
		int nWidth = 0;
		int nHeight = 0;
		if (DataRender_GetBuffer(strObjID, pBuf, nWidth, nHeight))
		{
			BufferFunction::WriteBitmap(_T("e:\\4.bmp"), nWidth, nHeight, 32, pBuf);
		}
	}

	//CEdit * m_pEdit = new CEdit;
	//m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("Edit"), _T("myedit"), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(1800, 250, 1900, 350), this, 100081);

	return 0;
}

void BaseGraphMainFrame::OnDestroy()
{
	CSkinFrameWnd::OnDestroy();
}

void BaseGraphMainFrame::OnSetFocus(CWnd * pOldWnd)
{
	CSkinFrameWnd::OnSetFocus(pOldWnd);
}

void BaseGraphMainFrame::OnPaint()
{
	CPaintDC dc(this);

	//先从绘制区域中排除掉MFC自带控件，避免闪烁问题
	CWnd *pChild = GetWindow(GW_CHILD); //取第一个子窗口
	while (pChild != NULL && ::IsWindow(pChild->GetSafeHwnd()))
	{
		CRect rtChild;
		pChild->GetWindowRect(rtChild);
		ScreenToClient(rtChild);
		dc.ExcludeClipRect(rtChild);
		pChild = pChild->GetNextWindow(); //取下一个子窗口
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	CDC dcMem;
	CBitmap bmpMem;
	dcMem.CreateCompatibleDC(NULL);
	bmpMem.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	dcMem.SelectObject(&bmpMem);
	dcMem.SetBkMode(TRANSPARENT);

	//绘制物件条
	m_uiActionBar.DrawElement(&dcMem);
	//绘制播控条
	m_uiPlayControlBar.DrawElement(&dcMem);
	//绘制简单时间线
	m_uiSimpleTimeLine.DrawElement(&dcMem);
	//绘制属性窗
	m_uiPropertyView.DrawElement(&dcMem);
	if (false)
	{
		CImage img;
		img.Attach((HBITMAP)dcMem.GetCurrentBitmap()->GetSafeHandle());
		img.Save(_T("F:\\test.bmp"));
		img.Detach();
	}
	//绘制时间线
	m_uiTimeLine.DrawElement(&dcMem);
	if (false)
	{
		CImage img;
		img.Attach((HBITMAP)dcMem.GetCurrentBitmap()->GetSafeHandle());
		img.Save(_T("F:\\test.bmp"));
		img.Detach();
	}

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

	bmpMem.DeleteObject();
	dcMem.DeleteDC();
}

BOOL BaseGraphMainFrame::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

void BaseGraphMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CRect rtClient;
	GetClientRect(rtClient);

	int nActionBarWidth = UIControl_GetIntValue(ThemeInt_ActionBarWidth);
	int nTimeLineHeight = UIControl_GetIntValue(ThemeInt_TimeLineHeight);
	int nPlayControlBarWidth = UIControl_GetIntValue(ThemeInt_PlayControlBarWidth);
	int nPlayControlBarHeight = UIControl_GetIntValue(ThemeInt_PlayControlBarHeight);
	int nPropertyViewWidth = UIControl_GetIntValue(ThemeInt_PropretyViewWidth);

	//action条在左上
	m_uiActionBar.MoveTo(0, 0, nActionBarWidth, rtClient.Height() - nTimeLineHeight);
	//playcontrol条在顶部
	m_uiPlayControlBar.MoveTo(nActionBarWidth, 0, nPlayControlBarWidth, nPlayControlBarHeight);
	//simpletimeline在顶部右侧
	m_uiSimpleTimeLine.MoveTo(nActionBarWidth + nPlayControlBarWidth, 0, rtClient.Width() - nActionBarWidth - nPlayControlBarWidth - nPropertyViewWidth, nPlayControlBarHeight);
	//属性窗在整个窗口右侧
	m_uiPropertyView.MoveTo(rtClient.Width() - nPropertyViewWidth, 0, nPropertyViewWidth, rtClient.Height());
	//时间线在整个窗口底部
	m_uiTimeLine.MoveTo(0, rtClient.Height() - nTimeLineHeight, rtClient.Width() - nPropertyViewWidth, nTimeLineHeight);

	CSkinFrameWnd::OnSize(nType, cx, cy);
}

LRESULT BaseGraphMainFrame::OnUIElementBtnClicked(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case UIElementMsg_CreateTitle:
		OnBtnCreateTitle();
		break;
	case UIElementMsg_CreateScroll:
		OnBtnCreateScroll();
		break;
	case UIElementMsg_CreateImage:
		OnBtnCreateImage();
		break;
	case UIElementMsg_NewFile:
		OnBtnNewFile();
		break;
	case UIElementMsg_SaveFile:
		OnBtnSaveFile();
		break;
	case UIElementMsg_SaveAs:
		OnBtnSaveAs();
		break;
	case UIElementMsg_DefaultAction:
		OnBtnDefaultAction();
		break;
	case UIElementMsg_MoveAction:
		OnBtnMoveAction();
		break;
	case UIElementMsg_ZoomAction:
		OnBtnZoomAction();
		break;
	case UIElementMsg_RotateAction:
		OnBtnRotateAction();
		break;
	case UIElementMsg_SystemSetting:
		OnBtnSystemSetting();
		break;
	default:
		break;
	}
	return 0;
}

void BaseGraphMainFrame::OnBtnCreateTitle()
{
	OutputDebugString(_T("创建标题字\n"));
	GraphEditorStatusManager::Instance().SetCurrentObjectTool(UIElementMsg_CreateTitle);
	m_uiObjectBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnCreateScroll()
{
	OutputDebugString(_T("创建滚屏\n"));
	GraphEditorStatusManager::Instance().SetCurrentObjectTool(UIElementMsg_CreateScroll);
	m_uiObjectBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnCreateImage()
{
	OutputDebugString(_T("创建图片\n"));
	GraphEditorStatusManager::Instance().SetCurrentObjectTool(UIElementMsg_CreateImage);
	m_uiObjectBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnNewFile()
{
	OutputDebugString(_T("新建\n"));
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnSaveFile()
{
	OutputDebugString(_T("保存\n"));
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnSaveAs()
{
	OutputDebugString(_T("另存\n"));
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnDefaultAction()
{
	OutputDebugString(_T("默认\n"));
	GraphEditorStatusManager::Instance().SetCurrentActionTool(UIElementMsg_DefaultAction);
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnMoveAction()
{
	OutputDebugString(_T("移动\n"));
	GraphEditorStatusManager::Instance().SetCurrentActionTool(UIElementMsg_MoveAction);
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnZoomAction()
{
	OutputDebugString(_T("缩放\n"));
	GraphEditorStatusManager::Instance().SetCurrentActionTool(UIElementMsg_ZoomAction);
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnRotateAction()
{
	OutputDebugString(_T("旋转\n"));
	GraphEditorStatusManager::Instance().SetCurrentActionTool(UIElementMsg_RotateAction);
	m_uiActionBar.UpdateStatus();
}

void BaseGraphMainFrame::OnBtnSystemSetting()
{
	OutputDebugString(_T("设置\n"));
	m_uiActionBar.UpdateStatus();
}
