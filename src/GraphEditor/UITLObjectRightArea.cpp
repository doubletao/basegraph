#include "stdafx.h"
#include "UITLObjectRightArea.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"

UITLObjectRightArea::UITLObjectRightArea()
	: m_pObjectArea(nullptr)
{
}

UITLObjectRightArea::~UITLObjectRightArea()
{
}

bool UITLObjectRightArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);
		return true;
	}
	return false;
}

void UITLObjectRightArea::DrawElement(CDC * pDC)
{
	int nXStart = GetRealX() + m_nScrollX;
	int nYStart = GetRealY() + m_nScrollY;
	CRect rcClient(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);

	CRgn rgnDC;
	rgnDC.CreateRectRgnIndirect(rcClient);
	pDC->SelectClipRgn(&rgnDC);

	int nItemHeight = UIControl_GetIntValue(ThemeInt_TimeLineItemActionHeight);
	CImage * pRightBkImage = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Nor);
	CImage * pRightAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectRightAreaBk);
	CImage * pActionBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_TotalLength);

	//绘制背景
	if (pRightAreaBkImage)
	{
		CRect rtBk(nXStart, nYStart, nXStart + m_nWidth, nYStart + m_nHeight);
		pRightAreaBkImage->AlphaBlend(pDC->GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pRightAreaBkImage->GetWidth(), pRightAreaBkImage->GetHeight());
	}

	//绘制一个总长度条底色
	if (pRightBkImage)
	{
		CRect rtLeft(nXStart, nYStart, nXStart + m_nWidth, nYStart + nItemHeight);
		pRightBkImage->AlphaBlend(pDC->GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pRightBkImage->GetWidth(), pRightBkImage->GetHeight());
	}

	//绘制总长度条
	if (pActionBarImage)
	{
		//取出当前总长及当前比例尺，画总长条
		int nBarLength = int(m_pObjectArea->GetTotalLength() * UITLRuler::Instance().GetPixelPerFrame());

		CRect rtBar(nXStart, nYStart, nXStart + nBarLength, nYStart + nItemHeight);
		rtBar.OffsetRect(-m_nScrollX, -m_nScrollY);
		pActionBarImage->AlphaBlend(pDC->GetSafeHdc(), rtBar.left, rtBar.top, rtBar.Width(), rtBar.Height(), 0, 0, pActionBarImage->GetWidth(), pActionBarImage->GetHeight());
	}

	pDC->SelectClipRgn(NULL);
}
