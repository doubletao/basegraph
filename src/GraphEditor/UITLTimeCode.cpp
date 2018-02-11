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
	int nFps = (int)fFPS;
	int nWholeSec = nFrame / nFps;
	int nFrameTail = nFrame % nFps;
	int nHour = nWholeSec / 3600;
	int nMin = nWholeSec % 3600 / 60;
	int nSec = nWholeSec % 3600 % 60;
	CString strDisp;
	strDisp.Format(_T("%02d:%02d:%02d:%02d"), nHour, nMin, nSec, nFrameTail);
	SetText(strDisp);
}
