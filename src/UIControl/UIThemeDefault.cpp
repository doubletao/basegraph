#include "stdafx.h"
#include "UIThemeDefault.h"
#include "UIControl\ThemeResoueceID.h"
#include <assert.h>

UIThemeDefault::UIThemeDefault()
{
}


UIThemeDefault::~UIThemeDefault()
{
}

CString UIThemeDefault::GetThemePath()
{
	CString strPath = UITheme::GetThemePath();
	strPath.Append(_T("ThemeDefault\\"));
	return strPath;
}

bool UIThemeDefault::Initialize()
{
	UITheme::Initialize();
	/////////////////////////////////////////////////////////////////////
	//字体
	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -12;
	lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
	m_mapFontList.insert(std::make_pair(ThemeFont_SkinWndTitleText, lfFont));

	/////////////////////////////////////////////////////////////////////
	//整数
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleHeight, 38));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndFrameWidth, 3));

	/////////////////////////////////////////////////////////////////////
	//颜色
	m_mapColorList;

	/////////////////////////////////////////////////////////////////////
	//图片
	auto LoadOneImg = [&](int nID, CString strPath)
	{
		CImage * pImg = new CImage;
		if (E_FAIL != pImg->Load(strPath))
		{
			m_mapImageList.insert(std::make_pair(nID, pImg));
		}
		else
		{
			assert(false);//缺资源
		}
	};
	CString strThemePath = GetThemePath();
	//SkinedWnd
	do
	{
		LoadOneImg(ThemeImage_SkinWndNcBkgrd, strThemePath + _T("SkinedWnd\\SkinWndNcBkgrd.png"));
		LoadOneImg(ThemeImage_SkinFrameDefaultIcon, strThemePath + _T("SkinedWnd\\SkinFrameDefaultIcon.png"));

		LoadOneImg(ThemeImage_SkinWndMinizeBtn_N, strThemePath + _T("SkinedWnd\\SkinWndMinizeBtn_N.png"));
		LoadOneImg(ThemeImage_SkinWndMinizeBtn_F, strThemePath + _T("SkinedWnd\\SkinWndMinizeBtn_F.png"));
		LoadOneImg(ThemeImage_SkinWndMinizeBtn_D, strThemePath + _T("SkinedWnd\\SkinWndMinizeBtn_D.png"));
		LoadOneImg(ThemeImage_SkinWndMinizeBtn_X, strThemePath + _T("SkinedWnd\\SkinWndMinizeBtn_X.png"));

		LoadOneImg(ThemeImage_SkinWndMaxizeBtn_N, strThemePath + _T("SkinedWnd\\SkinWndMaxizeBtn_N.png"));
		LoadOneImg(ThemeImage_SkinWndMaxizeBtn_F, strThemePath + _T("SkinedWnd\\SkinWndMaxizeBtn_F.png"));
		LoadOneImg(ThemeImage_SkinWndMaxizeBtn_D, strThemePath + _T("SkinedWnd\\SkinWndMaxizeBtn_D.png"));
		LoadOneImg(ThemeImage_SkinWndMaxizeBtn_X, strThemePath + _T("SkinedWnd\\SkinWndMaxizeBtn_X.png"));

		LoadOneImg(ThemeImage_SkinWndCloseBtn_N, strThemePath + _T("SkinedWnd\\SkinWndCloseBtn_N.png"));
		LoadOneImg(ThemeImage_SkinWndCloseBtn_F, strThemePath + _T("SkinedWnd\\SkinWndCloseBtn_F.png"));
		LoadOneImg(ThemeImage_SkinWndCloseBtn_D, strThemePath + _T("SkinedWnd\\SkinWndCloseBtn_D.png"));
		LoadOneImg(ThemeImage_SkinWndCloseBtn_X, strThemePath + _T("SkinedWnd\\SkinWndCloseBtn_X.png"));
	} while (false);
	return true;
}
