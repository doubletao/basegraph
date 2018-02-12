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
	//×ÖÌå
	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -14;
		lstrcpy(lfFont.lfFaceName, _T("Î¢ÈíÑÅºÚ"));
		m_mapFontList.insert(std::make_pair(ThemeFont_SkinWndTitleText, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -20;
		lfFont.lfWeight = 700;
		lstrcpy(lfFont.lfFaceName, _T("Î¢ÈíÑÅºÚ"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineTimeCode, lfFont));
	}

	/////////////////////////////////////////////////////////////////////
	//ÕûÊý
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleHeight, 35));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndFrameWidth, 3));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleObjectBarX, 150));
	m_mapIntValue.insert(std::make_pair(ThemeInt_ActionBarWidth, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineHeight, 300));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PlayControlBarWidth, 260));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PlayControlBarHeight, 40));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewWidth, 300));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewTabWidth, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeWidth, 200));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeHeight, 35));

	/////////////////////////////////////////////////////////////////////
	//ÑÕÉ«
	m_mapColorList.insert(std::make_pair(ThemeColor_SkinWndTitleText, RGB(233, 233, 233)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineTimeCodeText, RGB(233, 233, 0)));

	/////////////////////////////////////////////////////////////////////
	//Í¼Æ¬
	auto LoadOneImg = [&](int nID, CString strPath)
	{
		CImage * pImg = new CImage;
		if (E_FAIL != pImg->Load(strPath))
		{
			m_mapImageList.insert(std::make_pair(nID, pImg));
		}
		else
		{
			assert(false);//È±×ÊÔ´
		}
	};
	CString strThemePath = GetThemePath();
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

		LoadOneImg(ThemeImage_ObjectBtnBKImg_N, strThemePath + _T("Common\\ObjectBtnBKImg_N.png"));
		LoadOneImg(ThemeImage_ObjectBtnBKImg_F, strThemePath + _T("Common\\ObjectBtnBKImg_F.png"));
		LoadOneImg(ThemeImage_ObjectBtnBKImg_D, strThemePath + _T("Common\\ObjectBtnBKImg_D.png"));
		LoadOneImg(ThemeImage_ObjectBtnBKImg_X, strThemePath + _T("Common\\ObjectBtnBKImg_X.png"));

		LoadOneImg(ThemeImage_ElementArea, strThemePath + _T("Common\\ElementArea.png"));

		LoadOneImg(ThemeImage_TimeLineTimeCodeBk, strThemePath + _T("TimeLine\\TimeLineTimeCodeBk.png"));
	} while (false);
	return true;
}
