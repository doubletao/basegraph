#include "stdafx.h"
#include "UITheme.h"


UITheme::UITheme()
{
}


UITheme::~UITheme()
{
}

bool UITheme::Initialize()
{
	//初始化三个默认值
	HDC hdc = ::GetDC(NULL);
	int width = 20;
	int height = 20;
	HBITMAP hbmp = ::CreateCompatibleBitmap(hdc, width, height);
	HDC hdcMem = ::CreateCompatibleDC(hdc);
	::SelectObject(hdcMem, hbmp);
	HBRUSH hb = ::CreateSolidBrush(RGB(255, 0, 0));
	RECT rc = { 0, 0, width, height };
	::FillRect(hdcMem, &rc, hb);

	m_DefaultImage.Attach(hbmp);
	m_DefaultImage.Save(_T("test.jpg"));

	::ReleaseDC(NULL, hdc);
	::DeleteDC(hdcMem);
	::DeleteObject(hb);
	return true;
}

bool UITheme::Release()
{
	m_mapFontList.clear();
	m_mapColorList.clear();
	for (std::map<int, CImage * >::iterator it = m_mapImageList.begin(); it != m_mapImageList.end(); it++)
	{
		delete it->second;
		it->second = nullptr;
	}
	m_mapImageList.clear();
	return true;
}

LOGFONT UITheme::GetFont(int nFontID)
{
	if (m_mapFontList.count(nFontID) > 0)
	{
		return m_mapFontList[nFontID];
	}
	return m_DefaultLogFont;
}

COLORREF UITheme::GetColor(int nColorID)
{
	if (m_mapColorList.count(nColorID) > 0)
	{
		return m_mapColorList[nColorID];
	}
	return m_DefaultColor;
}

CImage * UITheme::GetImage(int nImgID)
{
	if (m_mapImageList.count(nImgID) > 0)
	{
		return m_mapImageList[nImgID];
	}
	return &m_DefaultImage;
}
