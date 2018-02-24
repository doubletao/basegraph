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
	int nXStart = GetRealX() + m_nScrollX;
	int nYStart = GetRealY() + m_nScrollY;
	CRect rcClient(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);

	CRgn rgnDC;
	rgnDC.CreateRectRgnIndirect(rcClient);
	pDC->SelectClipRgn(&rgnDC);

	int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
	int nItemTabWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeTabWidth);
	LOGFONT logFont = UIControl_GetFont(ThemeFont_TimeLineObjectTrackText);
	int nGraListTextColor = UIControl_GetColor(ThemeColor_TimeLineObjectTrackText_Nor);
	CImage * pLeftBkImage = UIControl_GetImage(ThemeImage_TimeLineLeftObjectBkGnd_Nor);
	CImage * pLeftAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectLeftAreaBk);

	//绘制背景
	if (pLeftAreaBkImage)
	{
		CRect rtBk(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);
		pLeftAreaBkImage->AlphaBlend(pDC->GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pLeftAreaBkImage->GetWidth(), pLeftAreaBkImage->GetHeight());
	}

	//绘制首条底色
	if (pLeftBkImage)
	{
		CRect rtLeft(nXStart, nYStart, nXStart + m_nWidth, nYStart + nItemHeight);
		rtLeft.OffsetRect(-m_nScrollX, -m_nScrollY);
		pLeftBkImage->AlphaBlend(pDC->GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pLeftBkImage->GetWidth(), pLeftBkImage->GetHeight());
	}
	//在两个tab的地方绘制文本绘制文本
	CString strText = _T("物件列表");
	CFont font;
	font.CreateFontIndirect(&logFont);
	CFont * pOldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(nGraListTextColor);
	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	CSize sizText = pDC->GetTextExtent(strText);
	CRect rtText(nXStart + nItemTabWidth, nYStart, nXStart + nItemTabWidth + sizText.cx, nYStart + nItemHeight);
	rtText.OffsetRect(-m_nScrollX, -m_nScrollY);
	pDC->DrawText(strText, rtText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nOldBkMode);

	pDC->SelectClipRgn(NULL);
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
