#include "stdafx.h"
#include "UITLObjectLeftArea.h"
#include "UITLObjectArea.h"

UITLObjectLeftArea::UITLObjectLeftArea()
	: m_pObjectArea(nullptr)
{
}

UITLObjectLeftArea::~UITLObjectLeftArea()
{
}

void UITLObjectLeftArea::DrawElement(CDC * pDC)
{
	int nXStart = GetRealX();
	int nYStart = GetRealY();
	CRect rcClient(nXStart, nYStart, nXStart + m_nLogicWidth, nYStart + m_nLogicHeight);

	CDC dcMem;
	CBitmap bmpMem;
	dcMem.CreateCompatibleDC(NULL);
	bmpMem.CreateCompatibleBitmap(pDC, m_nLogicWidth, m_nLogicHeight);
	dcMem.SelectObject(&bmpMem);
	dcMem.SetBkMode(TRANSPARENT);

	int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
	int nItemTabWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeTabWidth);
	LOGFONT logFont = UIControl_GetFont(ThemeFont_TimeLineObjectTrackText);
	int nGraListTextColor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Nor);
	CImage * pLeftBkImage = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Nor);
	CImage * pLeftAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectLeftAreaBk);

	//绘制背景
	if (pLeftAreaBkImage)
	{
		CRect rtBk(0, 0, m_nWidth, m_nHeight);
		pLeftAreaBkImage->AlphaBlend(dcMem.GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pLeftAreaBkImage->GetWidth(), pLeftAreaBkImage->GetHeight());
	}

	//绘制首条底色
	if (pLeftBkImage)
	{
		CRect rtLeft(0, 0, m_nWidth, nItemHeight);
		pLeftBkImage->AlphaBlend(dcMem.GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pLeftBkImage->GetWidth(), pLeftBkImage->GetHeight());
	}
	//在两个tab的地方绘制文本绘制文本
	CFont font;
	font.CreateFontIndirect(&logFont);
	CFont * pOldFont = dcMem.SelectObject(&font);
	CRect rtText(nItemTabWidth, 0, nItemTabWidth * 2 + m_nWidth, nItemHeight);
	dcMem.SetTextColor(nGraListTextColor);
	int nOldBkMode = dcMem.SetBkMode(TRANSPARENT);
	dcMem.DrawText(_T("字幕单"), rtText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	dcMem.SelectObject(pOldFont);
	dcMem.SetBkMode(nOldBkMode);

	pDC->BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

	bmpMem.DeleteObject();
	dcMem.DeleteDC();
}

bool UITLObjectLeftArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);
		return true;
	}
	return false;
}
