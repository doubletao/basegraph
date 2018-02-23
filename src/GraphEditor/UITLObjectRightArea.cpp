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
	CImage * pRightBkImage = UIControl_GetImage(ThemeImage_TimeLineRightObjectBkGnd_Nor);
	CImage * pRightAreaBkImage = UIControl_GetImage(ThemeImage_TimeLineObjectRightAreaBk);
	CImage * pActionBarImage = UIControl_GetImage(ThemeImage_TimeLineObjectActionBar_TotalLength);

	//绘制背景
	if (pRightAreaBkImage)
	{
		CRect rtBk(0, 0, m_nWidth, m_nHeight);
		pRightAreaBkImage->AlphaBlend(dcMem.GetSafeHdc(), rtBk.left, rtBk.top, rtBk.Width(), rtBk.Height(), 0, 0, pRightAreaBkImage->GetWidth(), pRightAreaBkImage->GetHeight());
	}

	//绘制一个总长度条底色
	if (pRightBkImage)
	{
		CRect rtLeft(0, 0, m_nWidth, nItemHeight);
		pRightBkImage->AlphaBlend(dcMem.GetSafeHdc(), rtLeft.left, rtLeft.top, rtLeft.Width(), rtLeft.Height(), 0, 0, pRightBkImage->GetWidth(), pRightBkImage->GetHeight());
	}

	//绘制总长度条
	if (pActionBarImage)
	{
		//取出当前总长及当前比例尺，画总长条
		int nBarLength = int(m_pObjectArea->GetTotalLength() * UITLRuler::Instance().GetPixelPerFrame());

		CRect rtBar(0, 0, nBarLength, nItemHeight);
		pActionBarImage->AlphaBlend(dcMem.GetSafeHdc(), rtBar.left, rtBar.top, rtBar.Width(), rtBar.Height(), 0, 0, pActionBarImage->GetWidth(), pActionBarImage->GetHeight());
	}

	pDC->BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

	bmpMem.DeleteObject();
	dcMem.DeleteDC();
}
