#include "stdafx.h"
#include "UIControl\UIRectDrawer.h"
#include "ThemeManager.h"

UIRectDrawer::UIRectDrawer(int nResID/* = 0*/)
	: UIDrawer(nResID)
{
}


UIRectDrawer::~UIRectDrawer()
{
}

void UIRectDrawer::Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText)
{
	COLORREF clColor = ThemeManager::Instance().GetColor(GetResID());
	pDC->FillSolidRect(nX, nY, nWidth, nHeight, clColor);
}
