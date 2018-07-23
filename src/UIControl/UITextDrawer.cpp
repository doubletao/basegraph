#include "stdafx.h"
#include "UIControl\UITextDrawer.h"
#include "ThemeManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
		CRect rtText(nX, nY, nX + nWidth, nY + nHeight);
		pDC->SetTextColor(m_nColor);
		pDC->DrawText(strText, rtText, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		pDC->SelectObject(pOldFont);
	}
}

void UITextDrawer::SetColor(COLORREF nColor)
{
	m_nColor = nColor;
}
