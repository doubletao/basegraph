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
		lfFont.lfHeight = -26;
		lfFont.lfWeight = 700;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineTimeCode, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -14;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineObjectTrackText, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -14;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_TimeLineRuler, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -12;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_CustomEditCtr, lfFont));
	}

	{
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -12;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		m_mapFontList.insert(std::make_pair(ThemeFont_CustomStatic, lfFont));
	}

	/////////////////////////////////////////////////////////////////////
	//整数
	m_mapIntValue.insert(std::make_pair(ThemeInt_UIModualGap, 1));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleHeight, 29));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndFrameWidth, 3));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndFrameSysBtnGap, 3));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SkinWndTitleObjectBarX, 150));
	m_mapIntValue.insert(std::make_pair(ThemeInt_ActionBarWidth, 36));
	m_mapIntValue.insert(std::make_pair(ThemeInt_ActionBarButtonGap, 10));
	m_mapIntValue.insert(std::make_pair(ThemeInt_ActionBarSplitWidth, 25));
	m_mapIntValue.insert(std::make_pair(ThemeInt_ActionBarSplitHeight, 1));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineHeight, 300));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PlayControlBarWidth, 412));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PlayControlBarHeight, 33));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewWidth, 284));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewTabWidth, 36));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewTabGap, 10));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewControlLeftTopGap, 15));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewTabSplitWidth, 22));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropretyViewTabSplitHeight, 1));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewCtrlGap, 3));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewCtrlTitleWidth, 55));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewCtrlNumEditWidth, 35));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewCtrlColorBtnWidth, 80));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewCtrlColorBtnHeight, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewComboBoxBtnWidth, 130));
	m_mapIntValue.insert(std::make_pair(ThemeInt_PropertyViewComboBoxBtnHeight, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeWidth, 449));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeHeight, 50));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeCodeLeftGap, 10));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineTimeScrollHeight, 10));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemActionHeight, 28));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemActionGap, 1));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemTrackHeight, 28));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemTreeTabWidth, 30));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineItemTreeWidth, 449));
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLinePixelPerFrameMax, 1000));//每帧最多对应10像素(放大一百倍)
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLinePixelPerFrameMin, 5));//每帧最少对应0.05像素（放大一百倍）
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineAdjustPreservePercent, 10));//时间线adjust横向滚动条后预留的空间百分比
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerShortLineLength, 10));//时间线刻度尺短线长
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerLongLineLength, 14));//时间线刻度尺长线长
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerGapMinPixel, 20));//时间线刻度尺每刻度的最小像素数
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLineRulerGapMaxPixel, 35));//时间线刻度尺每刻度的最大像素数
	m_mapIntValue.insert(std::make_pair(ThemeInt_TimeLinePointerWidth, 1));//时间线指针宽度
	m_mapIntValue.insert(std::make_pair(ThemeInt_SimpleTimeLineHorGap, 10));
	m_mapIntValue.insert(std::make_pair(ThemeInt_SimpleTimeLineHeight, 15));

	/////////////////////////////////////////////////////////////////////
	//颜色
	m_mapColorList.insert(std::make_pair(ThemeColor_SkinWndTitleText, RGB(255, 255, 255)));
	m_mapColorList.insert(std::make_pair(ThemeColor_ButtonBarSplitLine, RGB(0, 0, 0)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineTimeCodeText, RGB(12, 168, 4)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineObjectTrackText_Nor, RGB(255, 255, 255)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineObjectTrackText_Sel, RGB(0x0a, 0xa9, 0x01)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineRulerLine, RGB(112, 112, 112)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLinePointerLine, RGB(50, 150, 250)));
	m_mapColorList.insert(std::make_pair(ThemeColor_TimeLineRulerText, RGB(255, 255, 255)));
	m_mapColorList.insert(std::make_pair(ThemeColor_ScrollThumbColor_N, RGB(0x70, 0x70, 0x70)));
	m_mapColorList.insert(std::make_pair(ThemeColor_ScrollThumbColor_F, RGB(0x60, 0x60, 0x60)));
	m_mapColorList.insert(std::make_pair(ThemeColor_ScrollThumbColor_D, RGB(0x4b, 0x4b, 0x4b)));
	m_mapColorList.insert(std::make_pair(ThemeColor_ScrollThumbColor_X, RGB(0x54, 0x54, 0x54)));
	m_mapColorList.insert(std::make_pair(ThemeColor_ScrollBkColor, RGB(0x2c, 0x2c, 0x2c)));
	m_mapColorList.insert(std::make_pair(ThemeColor_SliderLightArea, RGB(0x78, 0xe3, 0x73)));
	m_mapColorList.insert(std::make_pair(ThemeColor_SliderDarkArea, RGB(0x44, 0x44, 0x44)));
	m_mapColorList.insert(std::make_pair(ThemeColor_SliderButton_N, RGB(0xf0, 0xf0, 0xf0)));
	m_mapColorList.insert(std::make_pair(ThemeColor_SliderButton_F, RGB(0xff, 0xff, 0xff)));
	m_mapColorList.insert(std::make_pair(ThemeColor_SliderButton_D, RGB(0xaf, 0xaf, 0xaf)));
	m_mapColorList.insert(std::make_pair(ThemeColor_SliderButton_X, RGB(0x0a, 0x0a, 0x0a)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditBk, RGB(50, 50, 50)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditBkFocus, RGB(50, 50, 50)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditGray, RGB(65, 65, 65)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditNorText, RGB(125, 255, 200)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditHighLightText, RGB(255, 255, 255)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditGrayText, RGB(100, 100, 100)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditTopLeftBoard, RGB(255, 255, 255)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditBottomRightBoard, RGB(200, 200, 200)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditTopLeftBoardGray, RGB(255, 255, 255)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditBottomRightBoardGray, RGB(200, 200, 200)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomEditUnderLay, RGB(0, 200, 200)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomStatic, RGB(200, 200, 200)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomDropDownWndBk, RGB(30, 30, 30)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomDropGreyBk, RGB(74, 70, 72)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomDropText, RGB(230, 230, 230)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomDropGreyText, RGB(200, 200, 200)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomDropSelectText, RGB(0, 20, 50)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomDropNorBk, RGB(54, 54, 54)));
	m_mapColorList.insert(std::make_pair(ThemeColor_CustomComboxBk, RGB(54, 54, 54)));

	/////////////////////////////////////////////////////////////////////
	//文本
	m_mapCStringValue.insert(std::make_pair(ThemeCString_CursorOpenHandName, _T("Common\\HandDragOpen.cur")));
	m_mapCStringValue.insert(std::make_pair(ThemeCString_CursorCloseHandName, _T("Common\\HandDragClose.cur")));
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

		LoadOneImg(ThemeImage_BtnNewFile_N, strThemePath + _T("Button\\BtnNewFile_N.png"));
		LoadOneImg(ThemeImage_BtnNewFile_F, strThemePath + _T("Button\\BtnNewFile_F.png"));
		LoadOneImg(ThemeImage_BtnNewFile_D, strThemePath + _T("Button\\BtnNewFile_D.png"));
		LoadOneImg(ThemeImage_BtnNewFile_X, strThemePath + _T("Button\\BtnNewFile_X.png"));
		LoadOneImg(ThemeImage_BtnSave_N, strThemePath + _T("Button\\BtnSave_N.png"));
		LoadOneImg(ThemeImage_BtnSave_F, strThemePath + _T("Button\\BtnSave_F.png"));
		LoadOneImg(ThemeImage_BtnSave_D, strThemePath + _T("Button\\BtnSave_D.png"));
		LoadOneImg(ThemeImage_BtnSave_X, strThemePath + _T("Button\\BtnSave_X.png"));
		LoadOneImg(ThemeImage_BtnSaveAs_N, strThemePath + _T("Button\\BtnSaveAs_N.png"));
		LoadOneImg(ThemeImage_BtnSaveAs_F, strThemePath + _T("Button\\BtnSaveAs_F.png"));
		LoadOneImg(ThemeImage_BtnSaveAs_D, strThemePath + _T("Button\\BtnSaveAs_D.png"));
		LoadOneImg(ThemeImage_BtnSaveAs_X, strThemePath + _T("Button\\BtnSaveAs_X.png"));
		LoadOneImg(ThemeImage_BtnDefaultAction_N, strThemePath + _T("Button\\BtnDefaultAction_N.png"));
		LoadOneImg(ThemeImage_BtnDefaultAction_F, strThemePath + _T("Button\\BtnDefaultAction_F.png"));
		LoadOneImg(ThemeImage_BtnDefaultAction_D, strThemePath + _T("Button\\BtnDefaultAction_D.png"));
		LoadOneImg(ThemeImage_BtnDefaultAction_X, strThemePath + _T("Button\\BtnDefaultAction_X.png"));
		LoadOneImg(ThemeImage_BtnScaleAction_N, strThemePath + _T("Button\\BtnScaleAction_N.png"));
		LoadOneImg(ThemeImage_BtnScaleAction_F, strThemePath + _T("Button\\BtnScaleAction_F.png"));
		LoadOneImg(ThemeImage_BtnScaleAction_D, strThemePath + _T("Button\\BtnScaleAction_D.png"));
		LoadOneImg(ThemeImage_BtnScaleAction_X, strThemePath + _T("Button\\BtnScaleAction_X.png"));
		LoadOneImg(ThemeImage_BtnRotateAction_N, strThemePath + _T("Button\\BtnRotateAction_N.png"));
		LoadOneImg(ThemeImage_BtnRotateAction_F, strThemePath + _T("Button\\BtnRotateAction_F.png"));
		LoadOneImg(ThemeImage_BtnRotateAction_D, strThemePath + _T("Button\\BtnRotateAction_D.png"));
		LoadOneImg(ThemeImage_BtnRotateAction_X, strThemePath + _T("Button\\BtnRotateAction_X.png"));
		LoadOneImg(ThemeImage_BtnSysSetting_N, strThemePath + _T("Button\\BtnSysSetting_N.png"));
		LoadOneImg(ThemeImage_BtnSysSetting_F, strThemePath + _T("Button\\BtnSysSetting_F.png"));
		LoadOneImg(ThemeImage_BtnSysSetting_D, strThemePath + _T("Button\\BtnSysSetting_D.png"));
		LoadOneImg(ThemeImage_BtnSysSetting_X, strThemePath + _T("Button\\BtnSysSetting_X.png"));
		LoadOneImg(ThemeImage_BtnObjTitleWord_N, strThemePath + _T("Button\\BtnObjTitleWord_N.png"));
		LoadOneImg(ThemeImage_BtnObjTitleWord_F, strThemePath + _T("Button\\BtnObjTitleWord_F.png"));
		LoadOneImg(ThemeImage_BtnObjTitleWord_D, strThemePath + _T("Button\\BtnObjTitleWord_D.png"));
		LoadOneImg(ThemeImage_BtnObjTitleWord_X, strThemePath + _T("Button\\BtnObjTitleWord_X.png"));
		LoadOneImg(ThemeImage_BtnObjScroll_N, strThemePath + _T("Button\\BtnObjScroll_N.png"));
		LoadOneImg(ThemeImage_BtnObjScroll_F, strThemePath + _T("Button\\BtnObjScroll_F.png"));
		LoadOneImg(ThemeImage_BtnObjScroll_D, strThemePath + _T("Button\\BtnObjScroll_D.png"));
		LoadOneImg(ThemeImage_BtnObjScroll_X, strThemePath + _T("Button\\BtnObjScroll_X.png"));
		LoadOneImg(ThemeImage_BtnObjImage_N, strThemePath + _T("Button\\BtnObjImage_N.png"));
		LoadOneImg(ThemeImage_BtnObjImage_F, strThemePath + _T("Button\\BtnObjImage_F.png"));
		LoadOneImg(ThemeImage_BtnObjImage_D, strThemePath + _T("Button\\BtnObjImage_D.png"));
		LoadOneImg(ThemeImage_BtnObjImage_X, strThemePath + _T("Button\\BtnObjImage_X.png"));
		LoadOneImg(ThemeImage_BtnFirstFrame_N, strThemePath + _T("Button\\BtnFirstFrame_N.png"));
		LoadOneImg(ThemeImage_BtnFirstFrame_F, strThemePath + _T("Button\\BtnFirstFrame_F.png"));
		LoadOneImg(ThemeImage_BtnFirstFrame_D, strThemePath + _T("Button\\BtnFirstFrame_D.png"));
		LoadOneImg(ThemeImage_BtnFirstFrame_X, strThemePath + _T("Button\\BtnFirstFrame_X.png"));
		LoadOneImg(ThemeImage_BtnPreFrame_N, strThemePath + _T("Button\\BtnPreFrame_N.png"));
		LoadOneImg(ThemeImage_BtnPreFrame_F, strThemePath + _T("Button\\BtnPreFrame_F.png"));
		LoadOneImg(ThemeImage_BtnPreFrame_D, strThemePath + _T("Button\\BtnPreFrame_D.png"));
		LoadOneImg(ThemeImage_BtnPreFrame_X, strThemePath + _T("Button\\BtnPreFrame_X.png"));
		LoadOneImg(ThemeImage_BtnPlay_N, strThemePath + _T("Button\\BtnPlay_N.png"));
		LoadOneImg(ThemeImage_BtnPlay_F, strThemePath + _T("Button\\BtnPlay_F.png"));
		LoadOneImg(ThemeImage_BtnPlay_D, strThemePath + _T("Button\\BtnPlay_D.png"));
		LoadOneImg(ThemeImage_BtnPlay_X, strThemePath + _T("Button\\BtnPlay_X.png"));
		LoadOneImg(ThemeImage_BtnNextFrame_N, strThemePath + _T("Button\\BtnNextFrame_N.png"));
		LoadOneImg(ThemeImage_BtnNextFrame_F, strThemePath + _T("Button\\BtnNextFrame_F.png"));
		LoadOneImg(ThemeImage_BtnNextFrame_D, strThemePath + _T("Button\\BtnNextFrame_D.png"));
		LoadOneImg(ThemeImage_BtnNextFrame_X, strThemePath + _T("Button\\BtnNextFrame_X.png"));
		LoadOneImg(ThemeImage_BtnLastFrame_N, strThemePath + _T("Button\\BtnLastFrame_N.png"));
		LoadOneImg(ThemeImage_BtnLastFrame_F, strThemePath + _T("Button\\BtnLastFrame_F.png"));
		LoadOneImg(ThemeImage_BtnLastFrame_D, strThemePath + _T("Button\\BtnLastFrame_D.png"));
		LoadOneImg(ThemeImage_BtnLastFrame_X, strThemePath + _T("Button\\BtnLastFrame_X.png"));
		LoadOneImg(ThemeImage_BtnLoopPlay_N, strThemePath + _T("Button\\BtnLoopPlay_N.png"));
		LoadOneImg(ThemeImage_BtnLoopPlay_F, strThemePath + _T("Button\\BtnLoopPlay_F.png"));
		LoadOneImg(ThemeImage_BtnLoopPlay_D, strThemePath + _T("Button\\BtnLoopPlay_D.png"));
		LoadOneImg(ThemeImage_BtnLoopPlay_X, strThemePath + _T("Button\\BtnLoopPlay_X.png"));
		LoadOneImg(ThemeImage_TabBtnColor_N, strThemePath + _T("Button\\TabBtnColor_N.png"));
		LoadOneImg(ThemeImage_TabBtnColor_F, strThemePath + _T("Button\\TabBtnColor_F.png"));
		LoadOneImg(ThemeImage_TabBtnColor_D, strThemePath + _T("Button\\TabBtnColor_D.png"));
		LoadOneImg(ThemeImage_TabBtnColor_X, strThemePath + _T("Button\\TabBtnColor_X.png"));
		LoadOneImg(ThemeImage_TabBtnObject_N, strThemePath + _T("Button\\TabBtnObject_N.png"));
		LoadOneImg(ThemeImage_TabBtnObject_F, strThemePath + _T("Button\\TabBtnObject_F.png"));
		LoadOneImg(ThemeImage_TabBtnObject_D, strThemePath + _T("Button\\TabBtnObject_D.png"));
		LoadOneImg(ThemeImage_TabBtnObject_X, strThemePath + _T("Button\\TabBtnObject_X.png"));
		LoadOneImg(ThemeImage_TabBtnSpace_N, strThemePath + _T("Button\\TabBtnSpace_N.png"));
		LoadOneImg(ThemeImage_TabBtnSpace_F, strThemePath + _T("Button\\TabBtnSpace_F.png"));
		LoadOneImg(ThemeImage_TabBtnSpace_D, strThemePath + _T("Button\\TabBtnSpace_D.png"));
		LoadOneImg(ThemeImage_TabBtnSpace_X, strThemePath + _T("Button\\TabBtnSpace_X.png"));
		LoadOneImg(ThemeImage_TabBtnEffect_N, strThemePath + _T("Button\\TabBtnEffect_N.png"));
		LoadOneImg(ThemeImage_TabBtnEffect_F, strThemePath + _T("Button\\TabBtnEffect_F.png"));
		LoadOneImg(ThemeImage_TabBtnEffect_D, strThemePath + _T("Button\\TabBtnEffect_D.png"));
		LoadOneImg(ThemeImage_TabBtnEffect_X, strThemePath + _T("Button\\TabBtnEffect_X.png"));
		LoadOneImg(ThemeImage_TabBtnText_N, strThemePath + _T("Button\\TabBtnText_N.png"));
		LoadOneImg(ThemeImage_TabBtnText_F, strThemePath + _T("Button\\TabBtnText_F.png"));
		LoadOneImg(ThemeImage_TabBtnText_D, strThemePath + _T("Button\\TabBtnText_D.png"));
		LoadOneImg(ThemeImage_TabBtnText_X, strThemePath + _T("Button\\TabBtnText_X.png"));
		LoadOneImg(ThemeImage_BtnSpinUp_N, strThemePath + _T("Button\\BtnSpinUp_N.png"));
		LoadOneImg(ThemeImage_BtnSpinUp_F, strThemePath + _T("Button\\BtnSpinUp_F.png"));
		LoadOneImg(ThemeImage_BtnSpinUp_D, strThemePath + _T("Button\\BtnSpinUp_D.png"));
		LoadOneImg(ThemeImage_BtnSpinUp_X, strThemePath + _T("Button\\BtnSpinUp_X.png"));
		LoadOneImg(ThemeImage_BtnSpinDown_N, strThemePath + _T("Button\\BtnSpinDown_N.png"));
		LoadOneImg(ThemeImage_BtnSpinDown_F, strThemePath + _T("Button\\BtnSpinDown_F.png"));
		LoadOneImg(ThemeImage_BtnSpinDown_D, strThemePath + _T("Button\\BtnSpinDown_D.png"));
		LoadOneImg(ThemeImage_BtnSpinDown_X, strThemePath + _T("Button\\BtnSpinDown_X.png"));
		LoadOneImg(ThemeImage_ScrollBk, strThemePath + _T("Button\\ScrollBk.png"));
		LoadOneImg(ThemeImage_ScrollBkHot, strThemePath + _T("Button\\ScrollBkHot.png"));
		LoadOneImg(ThemeImage_ScrollBottomArrow, strThemePath + _T("Button\\ScrollBottomArrow.png"));
		LoadOneImg(ThemeImage_ScrollBottomArrowHot, strThemePath + _T("Button\\ScrollBottomArrowHot.png"));
		LoadOneImg(ThemeImage_ScrollLeftArrow, strThemePath + _T("Button\\ScrollLeftArrow.png"));
		LoadOneImg(ThemeImage_ScrollLeftArrowHot, strThemePath + _T("Button\\ScrollLeftArrowHot.png"));
		LoadOneImg(ThemeImage_ScrollRightArrow, strThemePath + _T("Button\\ScrollRightArrow.png"));
		LoadOneImg(ThemeImage_ScrollRightArrowHot, strThemePath + _T("Button\\ScrollRightArrowHot.png"));
		LoadOneImg(ThemeImage_ScrollThumb, strThemePath + _T("Button\\ScrollThumb.png"));
		LoadOneImg(ThemeImage_ScrollThumbHot, strThemePath + _T("Button\\ScrollThumbHot.png"));
		LoadOneImg(ThemeImage_ScrollTopArrow, strThemePath + _T("Button\\ScrollTopArrow.png"));
		LoadOneImg(ThemeImage_ScrollTopArrowHot, strThemePath + _T("Button\\ScrollTopArrowHot.png"));
		LoadOneImg(ThemeImage_CheckBtn_Bk_N, strThemePath + _T("Button\\CheckBtn_Bk_N.png"));
		LoadOneImg(ThemeImage_CheckBtn_Bk_F, strThemePath + _T("Button\\CheckBtn_Bk_F.png"));
		LoadOneImg(ThemeImage_CheckBtn_Bk_D, strThemePath + _T("Button\\CheckBtn_Bk_D.png"));
		LoadOneImg(ThemeImage_CheckBtn_Bk_X, strThemePath + _T("Button\\CheckBtn_Bk_X.png"));
		LoadOneImg(ThemeImage_CheckBtn_Fore_N, strThemePath + _T("Button\\CheckBtn_Fore_N.png"));
		LoadOneImg(ThemeImage_CheckBtn_Fore_F, strThemePath + _T("Button\\CheckBtn_Fore_F.png"));
		LoadOneImg(ThemeImage_CheckBtn_Fore_D, strThemePath + _T("Button\\CheckBtn_Fore_D.png"));
		LoadOneImg(ThemeImage_CheckBtn_Fore_X, strThemePath + _T("Button\\CheckBtn_Fore_X.png"));

		LoadOneImg(ThemeImage_ElementArea, strThemePath + _T("Common\\ElementArea.png"));
		LoadOneImg(ThemeImage_SceneLayoutBk, strThemePath + _T("Common\\SceneLayoutBk.png"));
		LoadOneImg(ThemeImage_ColorButtonTranspBmp, strThemePath + _T("Common\\transparent_small.bmp"));

		LoadOneImg(ThemeImage_TimeLineTimeCodeBk, strThemePath + _T("TimeLine\\TimeLineTimeCodeBk.png"));
		LoadOneImg(ThemeImage_TimeLineLeftObjectBkGnd_Nor, strThemePath + _T("TimeLine\\TimeLineLeftObjectBkGnd_Nor.png"));
		LoadOneImg(ThemeImage_TimeLineLeftObjectBkGnd_Sel, strThemePath + _T("TimeLine\\TimeLineLeftObjectBkGnd_Sel.png"));
		LoadOneImg(ThemeImage_TimeLineRightObjectBkGnd_Nor, strThemePath + _T("TimeLine\\TimeLineRightObjectBkGnd_Nor.png"));
		LoadOneImg(ThemeImage_TimeLineRightObjectBkGnd_Sel, strThemePath + _T("TimeLine\\TimeLineRightObjectBkGnd_Sel.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_TotalLength, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_TotalLength.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_Nor, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_Nor.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_Sel, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_Sel.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_Group_Nor, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_Group_Nor.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_Group_Sel, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_Group_Sel.png"));
		LoadOneImg(ThemeImage_TimeLineObjectActionBar_Lock, strThemePath + _T("TimeLine\\TimeLineObjectActionBar_Lock.png"));
		LoadOneImg(ThemeImage_TimeLineObjectLeftAreaBk, strThemePath + _T("TimeLine\\TimeLineObjectLeftAreaBk.png"));
		LoadOneImg(ThemeImage_TimeLineObjectRightAreaBk, strThemePath + _T("TimeLine\\TimeLineObjectRightAreaBk.png"));
		LoadOneImg(ThemeImage_TimeLineRulerBk, strThemePath + _T("TimeLine\\TimeLineRulerBk.png"));
		LoadOneImg(ThemeImage_TimeLinePointerHeader, strThemePath + _T("TimeLine\\TimeLinePointerHeader.png"));
		LoadOneImg(ThemeImage_ComboxDropDownBtn_U, strThemePath + _T("Button\\ComboxDropDownBtn_U.png"));
		LoadOneImg(ThemeImage_ComboxDropDownBtn_D, strThemePath + _T("Button\\ComboxDropDownBtn_D.png"));
		LoadOneImg(ThemeImage_ComboxDropDownBtn_F, strThemePath + _T("Button\\ComboxDropDownBtn_F.png"));
		LoadOneImg(ThemeImage_ComboxDropDownBtn_X, strThemePath + _T("Button\\ComboxDropDownBtn_X.png"));
	} while (false);
	return true;
}
