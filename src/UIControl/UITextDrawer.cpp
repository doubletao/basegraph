#include "stdafx.h"
#include "UITextDrawer.h"
#include "ThemeManager.h"

UITextDrawer::UITextDrawer(int nResID/* = 0*/)
	: UIDrawer(nResID)
{
}


UITextDrawer::~UITextDrawer()
{
}

void UITextDrawer::Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText)
{
	if (nullptr != strText)
	{
		LOGFONT lgfont = ThemeManager::Instance().GetFont(GetResID());
		CFont font;
		font.CreateFontIndirect(&lgfont);
		CFont * pOldFont = pDC->SelectObject(&font);
		CRect rtText(nX, nY, nWidth, nHeight);
		pDC->DrawText(strText, rtText, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		pDC->SelectObject(pOldFont);
	}
}
