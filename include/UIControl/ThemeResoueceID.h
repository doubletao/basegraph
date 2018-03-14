#pragma once
enum ThemeIntValueDef
{
	ThemeInt_SkinWndTitleHeight,
	ThemeInt_SkinWndFrameWidth,
	ThemeInt_SkinWndFrameSysBtnGap,
	ThemeInt_SkinWndTitleObjectBarX,
	ThemeInt_ActionBarWidth,
	ThemeInt_TimeLineHeight,
	ThemeInt_PlayControlBarWidth,
	ThemeInt_PlayControlBarHeight,
	ThemeInt_PropretyViewWidth,
	ThemeInt_PropretyViewTabWidth,
	ThemeInt_TimeLineTimeCodeWidth,
	ThemeInt_TimeLineTimeCodeHeight,
	ThemeInt_TimeLineItemActionHeight,//时间线物件条高度
	ThemeInt_TimeLineItemTrackHeight,//时间线物件关键帧轨道高度
	ThemeInt_TimeLineItemTreeTabWidth,//时间线物件tree的tab宽度
	ThemeInt_TimeLineItemTreeWidth,//时间线物件tree的宽度
	ThemeInt_TimeLinePixelPerFrameMax,//时间线比例尺每帧最多对应的像素数(放大100倍取整)
	ThemeInt_TimeLinePixelPerFrameMin,//时间线比例尺每像素最多对应的帧数(放大100倍取整)
	ThemeInt_TimeLineAdjustPreservePercent,//时间线adjust横向滚动条后预留的空间百分比
	ThemeInt_TimeLineRulerShortLineLength,//时间线刻度尺短线长
	ThemeInt_TimeLineRulerLongLineLength,//时间线刻度尺长线长
	ThemeInt_TimeLineRulerGapMinPixel,//时间线刻度尺每刻度的最小像素数
	ThemeInt_TimeLineRulerGapMaxPixel,//时间线刻度尺每刻度的最大像素数
};

enum ThemeFontValueDef
{
	ThemeFont_SkinWndTitleText,
	ThemeFont_TimeLineTimeCode,
	ThemeFont_TimeLineObjectTrackText,
	ThemeFont_TimeLineRuler,//时间线刻度尺字体
};

enum ThemeColorValueDef
{
	ThemeColor_SkinWndTitleText,
	ThemeColor_TimeLineTimeCodeText,
	ThemeColor_TimeLineObjectTrackText_Nor,
	ThemeColor_TimeLineObjectTrackText_Sel,
	ThemeColor_TimeLineRulerLine,
	ThemeColor_TimeLineRulerText,
};

enum ThemeImageValueDef
{
	ThemeImage_SkinWndNcBkgrd,
	ThemeImage_SkinFrameDefaultIcon,

	ThemeImage_SkinWndCloseBtn_N,
	ThemeImage_SkinWndCloseBtn_F,
	ThemeImage_SkinWndCloseBtn_D,
	ThemeImage_SkinWndCloseBtn_X,

	ThemeImage_SkinWndMinizeBtn_N,
	ThemeImage_SkinWndMinizeBtn_F,
	ThemeImage_SkinWndMinizeBtn_D,
	ThemeImage_SkinWndMinizeBtn_X,

	ThemeImage_SkinWndMaxizeBtn_N,
	ThemeImage_SkinWndMaxizeBtn_F,
	ThemeImage_SkinWndMaxizeBtn_D,
	ThemeImage_SkinWndMaxizeBtn_X,

	ThemeImage_ObjectBtnBKImg_N,
	ThemeImage_ObjectBtnBKImg_F,
	ThemeImage_ObjectBtnBKImg_D,
	ThemeImage_ObjectBtnBKImg_X,

	ThemeImage_ElementArea,

	ThemeImage_TimeLineTimeCodeBk,

	ThemeImage_ScrollLeftTopBtn_N,
	ThemeImage_ScrollLeftTopBtn_F,
	ThemeImage_ScrollLeftTopBtn_D,
	ThemeImage_ScrollLeftTopBtn_X,

	ThemeImage_ScrollRightBottomBtn_N,
	ThemeImage_ScrollRightBottomBtn_F,
	ThemeImage_ScrollRightBottomBtn_D,
	ThemeImage_ScrollRightBottomBtn_X,

	ThemeImage_ScrollThumbBtn_N,
	ThemeImage_ScrollThumbBtn_F,
	ThemeImage_ScrollThumbBtn_D,
	ThemeImage_ScrollThumbBtn_X,

	ThemeImage_TimeLineLeftObjectBkGnd_Nor,
	ThemeImage_TimeLineLeftObjectBkGnd_Sel,
	ThemeImage_TimeLineRightObjectBkGnd_Nor,
	ThemeImage_TimeLineRightObjectBkGnd_Sel,
	ThemeImage_TimeLineObjectActionBar_TotalLength,
	ThemeImage_TimeLineObjectActionBar_Nor,
	ThemeImage_TimeLineObjectActionBar_Sel,
	ThemeImage_TimeLineObjectActionBar_Group_Nor,
	ThemeImage_TimeLineObjectActionBar_Group_Sel,
	ThemeImage_TimeLineObjectActionBar_Lock,
	ThemeImage_TimeLineObjectLeftAreaBk,
	ThemeImage_TimeLineObjectRightAreaBk,
	ThemeImage_TimeLineRulerBk,
};
