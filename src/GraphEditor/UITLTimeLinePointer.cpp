#include "stdafx.h"
#include "UITLTimeLinePointer.h"
#include "UITLRuler.h"

UITLTimeLinePointer::UITLTimeLinePointer()
	: m_clLineColor(0)
{
}

UITLTimeLinePointer::~UITLTimeLinePointer()
{
}

void UITLTimeLinePointer::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		//绘制分两部分，先画个小三角在尺子上，然后画下边的一根线
		CImage * pHeaderImage = UIControl_GetImage(ThemeImage_TimeLinePointerHeader);
		if (pHeaderImage)
		{
			//设定其绘制裁剪区应该为标尺区域加右侧物件区域
			UITimeLine * pTimeLine = UITLRuler::Instance().GetTimeLine();
			if (pTimeLine)
			{
				CRgn rgnDC;
				CRect rtClipArea = pTimeLine->GetTimeLinePointerArea();
				rgnDC.CreateRectRgnIndirect(rtClipArea);
				pDC->SelectClipRgn(&rgnDC);
			}

			int nHeaderWidth = pHeaderImage->GetWidth();
			int nHeaderHeight = pHeaderImage->GetHeight();
			int nX = GetRealX();
			int nY = GetRealY();

			//小三角顶着Y值画，以X为中点
			CRect rcDrawer(nX - nHeaderWidth * 1.0f / 2, nY, nX + nHeaderWidth * 1.0f / 2, nY + nHeaderHeight);
			BufferFunction::DrawImageWithAplha(pDC, pHeaderImage, rcDrawer, rcDrawer, CRect(0, 0, nHeaderWidth, nHeaderHeight), CRect(), TRUE, 255, FALSE);

			//线直接从头画到脚
			pDC->FillSolidRect(nX - m_nWidth * 1.0f / 2, nY + nHeaderHeight, m_nWidth, m_nHeight - nHeaderHeight, m_clLineColor);

			pDC->SelectClipRgn(nullptr);
		}
	}
}

bool UITLTimeLinePointer::Create(int nID, HWND hParentWnd, UIElement * pParentElement)
{
	if (!m_bCreated && pParentElement)
	{
		int nHeight = pParentElement->GetHeight();
		int nTimeLinePointerWidth = UIControl_GetIntValue(ThemeInt_TimeLinePointerWidth);
		Initialize(0, 0, nTimeLinePointerWidth, nHeight, nID, hParentWnd, pParentElement);
		COLORREF clLineColor = UIControl_GetColor(ThemeColor_TimeLinePointerLine);
		SetLineColor(clLineColor, false);

		return true;
	}

	return false;
}

void UITLTimeLinePointer::SetLineColor(COLORREF clLineColor, bool bRefresh/* = true*/)
{
	m_clLineColor = clLineColor;
	if (bRefresh)
	{
		Refresh();
	}
}
