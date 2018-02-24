#include "stdafx.h"
#include "UITLTimeCode.h"


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

		UIImageDrawer * pImgDrawer = new UIImageDrawer(ThemeImage_TimeLineTimeCodeBk);
		m_mapDrawer[0].push_back(pImgDrawer);

		UITextDrawer * pTextDrawer = new UITextDrawer(ThemeFont_TimeLineTimeCode);
		COLORREF nFontColor = UIControl_GetColor(ThemeColor_TimeLineTimeCodeText);
		pTextDrawer->SetColor(nFontColor);
		m_mapDrawer[0].push_back(pTextDrawer);

		SetFrame(0);
	}
	return true;
}

void UITLTimeCode::SetFrame(int nFrame, float fFPS/* = 25.0*/)
{
	CString strDisp = CommonFunction::ConverFrameToText(nFrame, fFPS);
	SetText(strDisp);
}
