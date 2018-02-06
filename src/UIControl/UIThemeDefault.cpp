#include "stdafx.h"
#include "UIThemeDefault.h"
#include "ThemeResoueceID.h"

UIThemeDefault::UIThemeDefault()
{
}


UIThemeDefault::~UIThemeDefault()
{
}

bool UIThemeDefault::Initialize()
{
	/////////////////////////////////////////////////////////////////////
	//×ÖÌå
	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -12;
	lstrcpy(lfFont.lfFaceName, _T("Î¢ÈíÑÅºÚ"));
	m_mapFontList.insert(std::make_pair(ThemeFont_SkinWndTitleText, lfFont));

	/////////////////////////////////////////////////////////////////////
	//ÕûÊý
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleHeight, 38));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndFrameWidth, 3));

	/////////////////////////////////////////////////////////////////////
	//ÑÕÉ«
	m_mapColorList;

	/////////////////////////////////////////////////////////////////////
	//Í¼Æ¬
	m_mapImageList;
	return true;
}
