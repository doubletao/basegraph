#include "stdafx.h"
#include "BaseGraphMainFrame.h"
#include "GraphEditorStatusManager.h"
#include "MessageHandlerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(BaseGraphMainFrame, CSkinFrameWnd)

BEGIN_MESSAGE_MAP(BaseGraphMainFrame, CSkinFrameWnd)
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCCREATE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_MESSAGE(EmUIElementMsg_BtnClick, BaseGraphMainFrame::OnUIElementBtnClicked)
	ON_MESSAGE(EmUIElementMsg_SetCapture, BaseGraphMainFrame::OnUIElementSetCapture)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

BaseGraphMainFrame::BaseGraphMainFrame()
	: m_pCaptureElement(nullptr)
	, m_nWholeFrame(0)
{
}

BaseGraphMainFrame::~BaseGraphMainFrame()
{
}

BOOL BaseGraphMainFrame::PreTranslateMessage(MSG * pMsg)
{
	auto DealMsg = [&](UIElement & elem)->bool
	{
		if (UIElement::NeedContinue(elem.HandleMsg(pMsg->message, pMsg->wParam, pMsg->lParam)))
		{
			return false;
		}
		return true;
	};
	auto DealUIElementMsg = [&]()->bool
	{
		if (DealMsg(m_uiActionBar)
			|| DealMsg(m_uiPlayControlBar)
			|| DealMsg(m_uiSimpleTimeLine)
			|| DealMsg(m_uiPropertyView)
			|| DealMsg(m_uiTimeLine))
		{
			return true;
		}
		return false;
	};
	//如果当前是capture状态，则优先将消息给capture
	if (m_pCaptureElement)
	{
		if (DealMsg(*m_pCaptureElement))
		{
			return TRUE;
		}
	}
	if (pMsg->hwnd == m_hWnd)//如果是子窗口的消息，UIElement不拦截
	{
		if (DealUIElementMsg())
		{
			return TRUE;
		}
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

		dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &MemDC, 0, 0, SRCCOPY);

		dc.SelectClipRgn(NULL);

		MemDC.DeleteDC();
		bmp.DeleteObject();
	}
}

void BaseGraphMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS * lpncsp)
{
	CSkinFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);

	int nTitleHeight = UIControl_GetIntValue(ThemeInt_SkinWndTitleHeight);
	int nFrameWidth = UIControl_GetIntValue(ThemeInt_SkinWndFrameWidth);
	int nY = (nTitleHeight - m_uiObjectBar.GetHeight()) / 2;
	int nX = UIControl_GetIntValue(ThemeInt_SkinWndTitleObjectBarX);
	DWORD	dwStyle = GetStyle();
	if (dwStyle & WS_MAXIMIZE)
	{
		nY += nFrameWidth;
		nX += nFrameWidth;
	}
	m_uiObjectBar.MoveTo(nX, nY);
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
	m_uiObjectBar.HandleMsg(WM_LBUTTONDOWN, 0, MAKELPARAM(ptTmp.x, ptTmp.y));
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
	m_uiObjectBar.HandleMsg(WM_LBUTTONUP, 0, MAKELPARAM(ptTmp.x, ptTmp.y));
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
	m_uiObjectBar.HandleMsg(WM_MOUSEMOVE, 0, MAKELPARAM(ptTmp.x, ptTmp.y));
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
	m_uiObjectBar.Create(nX, nY, rcWnd.Width(), nTitleHeight - nY, GetSafeHwnd());

	return TRUE;
}

void BaseGraphMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (::IsWindow(m_pwndMainView.GetSafeHwnd()))
	{
		m_pwndMainView.OnKeyDown(nChar, nRepCnt, nFlags);
	}
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
	if (m_pwndMainView.Create(NULL, _T("主回显窗"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 200, 400), this, 0, NULL) == -1)
	{
		OutputDebugString(_T("主回显窗创建失败\n"));
		return 0;
	}

	//测试代码
	auto CreateTestImgObj = [](CString strName, CString strImgPath, CString strDebugPath, int nBegin, int nEnd)
	{
		CString strObjID = _T("");
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam = DataBase_GetDefaultParam(BaseGraph_DataBase::ObjectType_Image);
		mapParam[BaseGraph_DataBase::Image_Path_Str].SetValue(strImgPath);
		mapParam[BaseGraph_DataBase::Obj_Name_Str].SetValue(strName);
		mapParam[BaseGraph_DataBase::Obj_Width_Int].SetValue(800, BaseGraph_Common::ValueType_INT);
		mapParam[BaseGraph_DataBase::Obj_Height_Int].SetValue(600, BaseGraph_Common::ValueType_INT);
		mapParam[BaseGraph_DataBase::Obj_ActionBegin_Int].SetValue(nBegin, BaseGraph_Common::ValueType_INT);
		mapParam[BaseGraph_DataBase::Obj_ActionEnd_Int].SetValue(nEnd, BaseGraph_Common::ValueType_INT);
		DataBase_CreateItem(BaseGraph_DataBase::ObjectType_Image, mapParam, strObjID);
		if (!strObjID.IsEmpty())
		{
			DataRender_UpdateRender(strObjID);
			StuSimpleBuffer stuSimpleBuf;
			if (DataRender_GetBuffer(strObjID, stuSimpleBuf))
			{
				if (false)
				{
					BufferFunction::WriteBitmap(strDebugPath, stuSimpleBuf.nWidth, stuSimpleBuf.nHeight, 32, stuSimpleBuf.pBuffer);
				}
			}
		}
	};
	std::vector<int> vecImg;
	vecImg.push_back(1);
	vecImg.push_back(5);
	vecImg.push_back(6);
	for (int i = 0; i < 0; i++)
	{
		CString strName;
		strName.Format(_T("图片%d"), i);
		CString strImg;
		strImg.Format(_T("F:\\Media\\图片\\%d.png"), vecImg[i % 3]);
		CString strDebug;
		strDebug.Format(_T("e:\\%d.bmp"), i);
		int nBegin = 100 * i;
		int nEnd = 500 + 100 * i;

		CreateTestImgObj(strName, strImg, strDebug, nBegin, nEnd);
	}

	auto CreateTestTitlewordObj = [](CString strName, CString strText, CString strFontName, CString strDebugPath, int nBegin, int nEnd,
		float fXPos, float fYPos, float fZPos,
		float fXRorate, float fYRorate, float fZRorate,
		int nFontWidth, int nFontHeight,
		bool bHor)
	{
		CString strObjID = _T("");
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam = DataBase_GetDefaultParam(BaseGraph_DataBase::ObjectType_Titleword);
		mapParam[BaseGraph_DataBase::Obj_Name_Str].SetValue(strName);
		mapParam[BaseGraph_DataBase::Obj_ActionBegin_Int].SetValue(nBegin, BaseGraph_Common::ValueType_INT);
		mapParam[BaseGraph_DataBase::Obj_ActionEnd_Int].SetValue(nEnd, BaseGraph_Common::ValueType_INT);
		mapParam[BaseGraph_DataBase::Titleword_Text_Str].SetValue(strText);
		mapParam[BaseGraph_DataBase::Titleword_FontName_Str].SetValue(strFontName);
		mapParam[BaseGraph_DataBase::Titleword_Horizontal_Bool].SetValue(bHor, BaseGraph_Common::ValueType_BOOL);
		mapParam[BaseGraph_DataBase::SpacePosition_X_Float].SetValue(fXPos);
		mapParam[BaseGraph_DataBase::SpacePosition_Y_Float].SetValue(fYPos);
		mapParam[BaseGraph_DataBase::SpacePosition_Z_Float].SetValue(fYPos);
		mapParam[BaseGraph_DataBase::SpaceRotate_X_Float].SetValue(fXRorate);
		mapParam[BaseGraph_DataBase::SpaceRotate_Y_Float].SetValue(fYRorate);
		mapParam[BaseGraph_DataBase::SpaceRotate_Z_Float].SetValue(fZRorate);
		mapParam[BaseGraph_DataBase::Titleword_FontWidth_Int].SetValue(nFontWidth);
		mapParam[BaseGraph_DataBase::Titleword_FontHeight_Int].SetValue(nFontHeight);
		BaseGraph_DataBase::faceinfo face;
		face.disable = 0;
		face.color.type = 0;
		face.color.color = ARGBColor(255, 0, 255, 0);
		mapParam[BaseGraph_DataBase::ColorParam_Face_Stu].SetValue(face);
		DataBase_CreateItem(BaseGraph_DataBase::ObjectType_Titleword, mapParam, strObjID);
		if (!strObjID.IsEmpty())
		{
			DataRender_UpdateRender(strObjID);
			StuSimpleBuffer stuSimpleBuf;
			if (DataRender_GetBuffer(strObjID, stuSimpleBuf))
			{
				if (false)
				{
					BufferFunction::WriteBitmap(strDebugPath, stuSimpleBuf.nWidth, stuSimpleBuf.nHeight, 32, stuSimpleBuf.pBuffer);
				}
			}
		}
	};

	std::map<CString, CString> mapText;
	//mapText[_T("宋体_文本样例")] = _T("宋体");
	//mapText[_T("楷体_文本样例")] = _T("楷体");
	//mapText[_T("微软雅黑_文本样例")] = _T("微软雅黑");
	//mapText[_T("黑体_文本样例")] = _T("黑体");
	//mapText[_T("仿宋_文本样例")] = _T("仿宋");
	//mapText[_T("新宋体_文本样例")] = _T("新宋体");
	{
		int i = 0;
		for (std::map<CString, CString>::iterator it = mapText.begin(); it != mapText.end(); it++)
		{
			for (size_t loop = 0; loop < 1; loop++, i++)
			{
				CString strName = DataBase_GetNextItemName(BaseGraph_DataBase::ObjectType_Titleword);
				CString strDebug;
				strDebug.Format(_T("e:\\%s.bmp"), it->first);
				int nBegin = 100 * i;
				int nEnd = 500 * i + 100;

				float fXPos = 0.0f;
				float fYPos = 0.0f;
				float fZPos = 0.0f;

				float fXRotate = 0.0f;
				float fYRotate = 0.0f;
				float fZRotate = 0.0f;

				int nWidth = 30;
				int nHeight = 30;
				bool bHor = i % 2 == 0;
				if (bHor)
				{
					fXPos = 100.0f;
					fYPos = 100.0f * i / 2;
					nHeight += 20;
					//fXRotate = i * 3.14f / 6;
				}
				else
				{
					fXPos = -100.0f * i / 2;
					fYPos = 300.0f;
					nWidth += 20;
					//fYRotate = i * 3.14f / 6;
				}
				//fZRotate = i * 3.14f / 6;
				fZRotate = i * 60;

				if (i % 3)
				{
					CreateTestTitlewordObj(strName, it->first, it->second, strDebug, nBegin, nEnd, fXPos, fYPos, fZPos,
						fXRotate, fYRotate, fZRotate,
						nWidth, nHeight,
						bHor);
				}
				else
				{
					CreateTestTitlewordObj(strName, it->first, it->second, strDebug, nBegin, nEnd, fXPos, fYPos, fZPos,
						0.0f, 0.0f, 0.0f,
						nWidth, nHeight,
						bHor);
				}
			}
		}
	}

	OnUpdateWholeLength();

	//CEdit * m_pEdit = new CEdit;
	//m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("Edit"), _T("myedit"), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(1800, 250, 1900, 350), this, 100081);

	//注册消息中心
	UIControl_RegistMsgDispatcher(this);

	//设置默认工具
	OnBtnDefaultAction();

	return 0;
}

void BaseGraphMainFrame::OnDestroy()
{
	IMsgDispatchObject::UnRegistDispatchObj();
	CSkinFrameWnd::OnDestroy();
}

void BaseGraphMainFrame::OnSetFocus(CWnd * pOldWnd)
{
	CSkinFrameWnd::OnSetFocus(pOldWnd);
}

void BaseGraphMainFrame::OnPaint()
{
	CPaintDC dc(this);

	//CRect rtInvalidate = dc.m_ps.rcPaint;

	//先从绘制区域中排除掉子窗口，避免闪烁问题
	CWnd *pChild = GetWindow(GW_CHILD); //取第一个子窗口
	while (pChild != NULL && ::IsWindow(pChild->GetSafeHwnd()) && pChild->IsWindowVisible())
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
	dcMem.FillSolidRect(rcClient, RGB(0, 0, 0));

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
	int nGap = UIControl_GetIntValue(ThemeInt_UIModualGap);

	//action条在左上
	m_uiActionBar.MoveTo(0, nGap, nActionBarWidth, rtClient.Height() - nTimeLineHeight - nGap);
	//playcontrol条在顶部
	m_uiPlayControlBar.MoveTo(nActionBarWidth + nGap, nGap, nPlayControlBarWidth, nPlayControlBarHeight);
	//simpletimeline在顶部右侧
	m_uiSimpleTimeLine.MoveTo(nActionBarWidth + nPlayControlBarWidth + nGap * 2, nGap, rtClient.Width() - nActionBarWidth - nPlayControlBarWidth - nPropertyViewWidth - nGap * 3, nPlayControlBarHeight);
	//属性窗在整个窗口右侧
	m_uiPropertyView.MoveTo(rtClient.Width() - nPropertyViewWidth, nGap, nPropertyViewWidth, rtClient.Height());
	//时间线在整个窗口底部
	m_uiTimeLine.MoveTo(0, rtClient.Height() - nTimeLineHeight + nGap, rtClient.Width() - nPropertyViewWidth, nTimeLineHeight);
	//回显窗在中间
	m_pwndMainView.MoveWindow(nActionBarWidth + nGap, nPlayControlBarHeight + nGap * 2, rtClient.Width() - nActionBarWidth - nPropertyViewWidth - nGap * 2, rtClient.Height() - nPlayControlBarHeight - nTimeLineHeight - nGap * 2);

	CSkinFrameWnd::OnSize(nType, cx, cy);
}

int BaseGraphMainFrame::GetMsgDispatchID()
{
	return UIElementMsg_BaseGraphMainFrame;
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
	case UIElementMsg_ZoomAction:
		OnBtnZoomAction();
		break;
	case UIElementMsg_RotateAction:
		OnBtnRotateAction();
		break;
	case UIElementMsg_SystemSetting:
		OnBtnSystemSetting();
		break;
	case UIElementMsg_PropertyTabColor:
		OnBtnPropertyTabColor();
		break;
	case UIElementMsg_PropertyTabObject:
		OnBtnPropertyTabObject();
		break;
	case UIElementMsg_PropertyTabSpace:
		OnBtnPropertyTabSpace();
		break;
	case UIElementMsg_PropertyTabFx:
		OnBtnPropertyTabFx();
		break;
	case UIElementMsg_PropertyTabText:
		OnBtnPropertyTabText();
		break;
	default:
		break;
	}
	OnUpdateBarStatus();
	return 0;
}

LRESULT BaseGraphMainFrame::OnUIElementSetCapture(WPARAM wParam, LPARAM lParam)
{
	BOOL bCapture = (BOOL)wParam;
	UIElement * pElement = (UIElement *)lParam;
	if (bCapture)
	{
		SetCapture();
		m_pCaptureElement = pElement;
	}
	else
	{
		ReleaseCapture();
		m_pCaptureElement = nullptr;
	}
	return 0;
}

void BaseGraphMainFrame::OnUpdateWholeLength()
{
	int nFrameLength = 0;
	if (DataBase_GetItemLength(nFrameLength))
	{
		nFrameLength = nFrameLength / 2;
	}
	else
	{
		nFrameLength = 5;
	}
	if (m_nWholeFrame != nFrameLength)
	{
		m_nWholeFrame = nFrameLength;
		UIControl_BroadcastMsg(WM_MSG_WholeFrameLengthChanged, m_nWholeFrame, GetMsgDispatchID(), this);
	}
}

void BaseGraphMainFrame::OnUpdateBarStatus()
{
	m_uiObjectBar.UpdateStatus();
	m_uiActionBar.UpdateStatus();
	m_uiPropertyView.UpdateStatus();
	OnNcPaint();
}

void BaseGraphMainFrame::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_WholeFrameLengthChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnUpdateWholeLength();
		}
		break;
	case WM_MSG_ObjectCreateDelete:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnUpdateWholeLength();
		}
		break;
	case WM_MSG_OnMessageHandlerChg:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnUpdateBarStatus();
		}
		break;
	default:
		break;
	}
}

void BaseGraphMainFrame::OnBtnCreateTitle()
{
	OutputDebugString(_T("创建标题字\n"));
	MessageHandlerManager::Instance().PushHandler(MessageHandlerType_CreateTitle);
}

void BaseGraphMainFrame::OnBtnCreateScroll()
{
	OutputDebugString(_T("创建滚屏\n"));
	MessageHandlerManager::Instance().PushHandler(MessageHandlerType_CreateScroll);
}

void BaseGraphMainFrame::OnBtnCreateImage()
{
	OutputDebugString(_T("创建图片\n"));
	MessageHandlerManager::Instance().PushHandler(MessageHandlerType_CreateImage);
}

void BaseGraphMainFrame::OnBtnNewFile()
{
	OutputDebugString(_T("新建\n"));
	OnUpdateBarStatus();
	m_uiTimeLine.AdjustScrollZoom();
}

void BaseGraphMainFrame::OnBtnSaveFile()
{
	OutputDebugString(_T("保存\n"));
	OnUpdateBarStatus();
	m_uiTimeLine.AdjustScrollZoom(10);
}

void BaseGraphMainFrame::OnBtnSaveAs()
{
	OutputDebugString(_T("另存\n"));
	OnUpdateBarStatus();
	m_uiTimeLine.AdjustScrollZoom(-10);
}

void BaseGraphMainFrame::OnBtnDefaultAction()
{
	OutputDebugString(_T("默认\n"));
	MessageHandlerManager::Instance().ClearHandlerTo(MessageHandlerType_Default);
}

void BaseGraphMainFrame::OnBtnZoomAction()
{
	OutputDebugString(_T("缩放\n"));
	MessageHandlerManager::Instance().ClearHandlerTo(MessageHandlerType_Scale);
}

void BaseGraphMainFrame::OnBtnRotateAction()
{
	OutputDebugString(_T("旋转\n"));
	MessageHandlerManager::Instance().ClearHandlerTo(MessageHandlerType_Rotate);
}

void BaseGraphMainFrame::OnBtnSystemSetting()
{
	OutputDebugString(_T("设置\n"));
	OnUpdateBarStatus();
}

void BaseGraphMainFrame::OnBtnPropertyTabColor()
{
	m_uiPropertyView.OnBtnPropertyTabColor();
	OnUpdateBarStatus();
}

void BaseGraphMainFrame::OnBtnPropertyTabObject()
{
	m_uiPropertyView.OnBtnPropertyTabObject();
	OnUpdateBarStatus();
}

void BaseGraphMainFrame::OnBtnPropertyTabSpace()
{
	m_uiPropertyView.OnBtnPropertyTabSpace();
	OnUpdateBarStatus();
}

void BaseGraphMainFrame::OnBtnPropertyTabFx()
{
	m_uiPropertyView.OnBtnPropertyTabFx();
	OnUpdateBarStatus();
}

void BaseGraphMainFrame::OnBtnPropertyTabText()
{
	m_uiPropertyView.OnBtnPropertyTabText();
	OnUpdateBarStatus();
}

SafeLineSettingParam & BaseGraphMainFrame::GetSafeLineSettingParam()
{
	return m_stuSafeLineParam;
}

EditorSettingParam & BaseGraphMainFrame::GetEditorSettingParam()
{
	return m_stuEditorSettingParam;
}

int BaseGraphMainFrame::GetWholeFrameLength()
{
	return m_nWholeFrame;
}
