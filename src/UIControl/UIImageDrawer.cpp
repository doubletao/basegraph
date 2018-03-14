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
		//pImg->AlphaBlend(pDC->GetSafeHdc(), nX, nY, nWidth, nHeight, 0, 0, pImg->GetWidth(), pImg->GetHeight(), );
		pImg->TransparentBlt(pDC->GetSafeHdc(), nX, nY, nWidth, nHeight, 0, 0, pImg->GetWidth(), pImg->GetHeight(), RGB(255, 255, 255));
	}
}
