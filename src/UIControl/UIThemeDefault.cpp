#include "stdafx.h"
#include "UIThemeDefault.h"
#include "UIControl\ThemeResoueceID.h"
#include <assert.h>
#include <vector>

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
	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -14;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_SkinWndTitleText, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -20;
		lfFont.lfWeight = 700;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineTimeCode, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -16;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineObjectTrackText, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -10;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineRuler, lfFont));
	}

	/////////////////////////////////////////////////////////////////////
	//整数
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleHeight, 35));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndFrameWidth, 3));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleObjectBarX, 150));
	m_mapIntValue.insert(std::make_pair(ThemeInt_ActionBarWidth, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineHeight, 300));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PlayControlBarWidth, 260));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PlayControlBarHeight, 40));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewWidth, 300));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewTabWidth, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeWidth, 400));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeHeight, 35));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemActionHeight, 20));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemTrackHeight, 20));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemTreeTabWidth, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemTreeWidth, 400));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLinePixelPerFrameMax, 1000));//每帧最多对应10像素(放大一百倍)
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLinePixelPerFrameMin, 5));//每帧最少对应0.05像素（放大一百倍）
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineAdjustPreservePercent, 10));//时间线adjust横向滚动条后预留的空间百分比
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerShortLineLength, 5));//时间线刻度尺短线长
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerLongLineLength, 10));//时间线刻度尺长线长
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerGapMinPixel, 20));//时间线刻度尺每刻度的最小像素数
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerGapMaxPixel, 35));//时间线刻度尺每刻度的最大像素数

	/////////////////////////////////////////////////////////////////////
	//颜色
	m_mapColorList.insert(std::make_pair(ThemeColor_SkinWndTitleText, RGB(233, 233, 233)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineTimeCodeText, RGB(233, 233, 0)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineObjectTrackText_Nor, RGB(233, 233, 0)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineObjectTrackText_Sel, RGB(233, 233, 233)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineRulerLine, RGB(233, 233, 0)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineRulerText, RGB(233, 233, 0)));

	/////////////////////////////////////////////////////////////////////
	//图片
	auto LoadOneImg = [&](int nID, CString strPath)
	{
#ifdef DEBUG
		std::vector<CString> strMissingImg;
#endif // DEBUG
		CImage * pImg = new CImage;
		if (E_FAIL != pImg->Load(strPath))
		{
			m_mapImageList.insert(std::make_pair(nID, pImg));
		}
#ifdef DEBUG
		else
		{
			strMissingImg.push_back(strPath);
		}
		if (strMissingImg.size() > 0)
		{
			//这些图片资源缺失或加载失败，查一下怎么回事
			assert(false);
		}
#endif // DEBUG
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

		LoadOneImg(ThemeImage_ScrollLeftTopBtn_N, strThemePath + _T("TimeLine\\TimeLineLeftTopBtn_N.png"));
		LoadOneImg(ThemeImage_ScrollLeftTopBtn_F, strThemePath + _T("TimeLine\\TimeLineLeftTopBtn_F.png"));
		LoadOneImg(ThemeImage_ScrollLeftTopBtn_D, strThemePath + _T("TimeLine\\TimeLineLeftTopBtn_D.png"));
		LoadOneImg(ThemeImage_ScrollLeftTopBtn_X, strThemePath + _T("TimeLine\\TimeLineLeftTopBtn_X.png"));
		LoadOneImg(ThemeImage_ScrollRightBottomBtn_N, strThemePath + _T("TimeLine\\TimeLineRightBottomBtn_N.png"));
		LoadOneImg(ThemeImage_ScrollRightBottomBtn_F, strThemePath + _T("TimeLine\\TimeLineRightBottomBtn_F.png"));
		LoadOneImg(ThemeImage_ScrollRightBottomBtn_D, strThemePath + _T("TimeLine\\TimeLineRightBottomBtn_D.png"));
		LoadOneImg(ThemeImage_ScrollRightBottomBtn_X, strThemePath + _T("TimeLine\\TimeLineRightBottomBtn_X.png"));
		LoadOneImg(ThemeImage_ScrollThumbBtn_N, strThemePath + _T("TimeLine\\TimeLineThumbBtn_N.png"));
		LoadOneImg(ThemeImage_ScrollThumbBtn_F, strThemePath + _T("TimeLine\\TimeLineThumbBtn_F.png"));
		LoadOneImg(ThemeImage_ScrollThumbBtn_D, strThemePath + _T("TimeLine\\TimeLineThumbBtn_D.png"));
		LoadOneImg(ThemeImage_ScrollThumbBtn_X, strThemePath + _T("TimeLine\\TimeLineThumbBtn_X.png"));
		LoadOneImg(ThemeImage_TimeLineLeftObjectBkGnd_Nor, strThemePath + _T("TimeLine\\TimeLineLeftObjectBkGnd_Nor.png"));
		LoadOneImg(ThemeImage_TimeLineLeftObjectBkGnd_Sel, strThemePath + _T("TimeLine\\TimeLineLeftObjectBkGnd_Sel.png"));
		LoadOneImg(ThemeImage_TimeLineRightObjectBkGnd_Nor, strThemePath + _T("TimeLine\\TimeLineRightObjectBkGnd_Nor.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_TotalLength, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_TotalLength.png"));
		LoadOneImg(ThemeImage_TimeLineObjectLeftAreaBk, strThemePath + _T("TimeLine\\TimeLineObjectLeftAreaBk.png"));
		LoadOneImg(ThemeImage_TimeLineObjectRightAreaBk, strThemePath + _T("TimeLine\\TimeLineObjectRightAreaBk.png"));
		LoadOneImg(ThemeImage_TimeLineRulerBk, strThemePath + _T("TimeLine\\TimeLineRulerBk.png"));
	} while (false);
	return true;
}
