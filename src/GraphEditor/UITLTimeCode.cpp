#include "stdafx.h"
#include "UITLTimeCode.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UITLTimeCode::UITLTimeCode()
{
}


UITLTimeCode::~UITLTimeCode()
{
}

bool UITLTimeCode::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd, pParent);
		SetFrame(0);
	}
	return true;
}

void UITLTimeCode::SetFrame(int nFrame, float fFPS/* = 25.0*/)
{
	CString strDisp = CommonFunction::ConverFrameToText(nFrame, fFPS);
	SetText(strDisp);
}

void UITLTimeCode::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		CImage * pImgBk = UIControl_GetImage(ThemeImage_TimeLineTimeCodeBk);
		LOGFONT lgFont = UIControl_GetFont(ThemeFont_TimeLineTimeCode);
		COLORREF nFontColor = UIControl_GetColor(ThemeColor_TimeLineTimeCodeText);
		int nTimeCodeGap = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeLeftGap);
		int nX = GetRealX();
		int nY = GetRealY();

		pImgBk->AlphaBlend(pDC->GetSafeHdc(), nX, nY, m_nWidth, m_nHeight, 0, 0, pImgBk->GetWidth(), pImgBk->GetHeight());

		CFont font;
		font.CreateFontIndirect(&lgFont);
		CFont * pOldFont = pDC->SelectObject(&font);
		CRect rtText(nX + nTimeCodeGap, nY, nX + m_nWidth, nY + m_nHeight);
		pDC->SetTextColor(nFontColor);
		pDC->DrawText(m_strText, rtText, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
		pDC->SelectObject(pOldFont);
	}
}
