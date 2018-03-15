#include "stdafx.h"
#include "UIControl\UIImageDrawer.h"
#include "ThemeManager.h"

UIImageDrawer::UIImageDrawer(int nResID/* = 0*/)
	: UIDrawer(nResID)
{
}


UIImageDrawer::~UIImageDrawer()
{
}

void UIImageDrawer::Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText/* = nullptr*/)
{
	CImage * pImg = ThemeManager::Instance().GetImage(GetResID());
	if (pImg)
	{
		int nImgWidth = pImg->GetWidth();
		int nImgHeight = pImg->GetHeight();
		if (nImgWidth == nWidth
			&& nImgHeight == nHeight)
		{
			CRect rcDrawer(nX, nY, nX + nWidth, nY + nHeight);
			BufferFunction::DrawImageWithAplha(pDC, pImg, rcDrawer, rcDrawer, CRect(0, 0, nWidth, nHeight), CRect(), TRUE, 255, FALSE);
		}
		else
		{
			pImg->AlphaBlend(pDC->GetSafeHdc(), nX, nY, nWidth, nHeight, 0, 0, pImg->GetWidth(), pImg->GetHeight());
		}
	}
}
