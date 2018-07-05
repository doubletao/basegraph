#include "stdafx.h"
#include "UIControl\UIRectDrawer.h"
#include "ThemeManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
