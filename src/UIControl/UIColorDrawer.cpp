#include "stdafx.h"
#include "UIControl\UIColorDrawer.h"
#include "ThemeManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIColorDrawer::UIColorDrawer(int nResID/* = 0*/)
	: UIDrawer(nResID)
{
}


UIColorDrawer::~UIColorDrawer()
{
}

void UIColorDrawer::Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText/* = nullptr*/)
{
	COLORREF nColor = ThemeManager::Instance().GetColor(GetResID());
	pDC->FillSolidRect(nX, nY, nWidth, nHeight, nColor);
}
