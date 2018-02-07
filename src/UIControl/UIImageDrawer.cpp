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
		CRect rtDes(nX, nY, nX + nWidth, nY + nHeight);
		BufferFunction::DrawImageWithAplha(pDC, pImg, rtDes, rtDes,
			CRect(0, 0, nWidth, nHeight), CRect(), TRUE, 255, FALSE);
	}
}
