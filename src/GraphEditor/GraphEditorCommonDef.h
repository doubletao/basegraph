#pragma once

typedef void(*FuncBtnClicked)(int nID);

enum UIElementMsgDef
{
	UIElementMsg_NULL,
	UIElementMsg_CreateTitle = UIBaseElementMsgDefEnd + 1,
	UIElementMsg_CreateScroll,
	UIElementMsg_CreateImage,

	UIElementMsg_NewFile,
	UIElementMsg_SaveFile,
	UIElementMsg_SaveAs,

	UIElementMsg_DefaultAction,
	UIElementMsg_ZoomAction,
	UIElementMsg_RotateAction,

	UIElementMsg_SystemSetting,

	UIElementMsg_PlayCtrlFirstFrame,
	UIElementMsg_PlayCtrlPreFrame,
	UIElementMsg_PlayCtrlPlayPause,
	UIElementMsg_PlayCtrlNextFrame,
	UIElementMsg_PlayCtrlLastFrame,

	UIElementMsg_SimpleTimeLine,

	UIElementMsg_PropertyTabColor,
	UIElementMsg_PropertyTabObject,
	UIElementMsg_PropertyTabSpace,
	UIElementMsg_PropertyTabFx,
	UIElementMsg_PropertyTabText,

	UIElementMsg_TimeLineHorScroll,
	UIElementMsg_TimeLineVerScroll,
	UIElementMsg_TimeLineObjectArea,
	UIElementMsg_TimeLineObjectLeftArea,
	UIElementMsg_TimeLineObjectRightArea,
	UIElementMsg_TimeLineRuler,

	WM_MSG_BEGIN = WM_USER + 1,
	WM_SET_RENDERTASK,//触发一次刷新
	WM_EXCUTE_RENDERTASK,//窗口的刷新消息
};