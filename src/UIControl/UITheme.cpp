#include "stdafx.h"
#include "UITheme.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UITheme::UITheme()
{
}


UITheme::~UITheme()
{
	Release();
}

CString UITheme::GetThemePath()
{
	CString strPath = CommonFunction::GetBinPath();
	strPath.Append(_T("\\Theme\\"));
	return strPath;
}

bool UITheme::Initialize()
{
	Release();
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
	if (false)
	{
		m_DefaultImage.Save(_T("e:\\test.jpg"));
	}

	::ReleaseDC(NULL, hdc);
	::DeleteDC(hdcMem);
	::DeleteObject(hb);

	lstrcpy((LPTSTR)m_DefaultLogFont.lfFaceName, _T("微软雅黑"));
	m_DefaultLogFont.lfWeight = 0;
	m_DefaultLogFont.lfWidth = 12;
	m_DefaultLogFont.lfHeight = 30;
	m_DefaultLogFont.lfEscapement = 0;
	m_DefaultLogFont.lfUnderline = FALSE;
	m_DefaultLogFont.lfItalic = FALSE;
	m_DefaultLogFont.lfStrikeOut = FALSE;
	m_DefaultLogFont.lfCharSet = GB2312_CHARSET;

	m_DefaultColor = RGB(255, 0, 0);
	m_nDefaultInt = 1;

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

int UITheme::GetIntValue(int nIntID)
{
	if (m_mapIntValue.count(nIntID) > 0)
	{
		return m_mapIntValue[nIntID];
	}
	return m_nDefaultInt;
}

CString UITheme::GetCStringValue(int nCStringID)
{
	if (m_mapCStringValue.count(nCStringID) > 0)
	{
		return m_mapCStringValue[nCStringID];
	}
	return _T("");
}
