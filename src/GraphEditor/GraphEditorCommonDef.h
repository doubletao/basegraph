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
	UIElementMsg_MoveAction,
	UIElementMsg_ZoomAction,
	UIElementMsg_RotateAction,

	UIElementMsg_SystemSetting,

	UIElementMsg_PlayCtrlFirstFrame,
	UIElementMsg_PlayCtrlPreFrame,
	UIElementMsg_PlayCtrlPlayPause,
	UIElementMsg_PlayCtrlNextFrame,
	UIElementMsg_PlayCtrlLastFrame,

	UIElementMsg_PropertyTabColor,
	UIElementMsg_PropertyTabObject,
	UIElementMsg_PropertyTabSpace,
	UIElementMsg_PropertyTabFx,
	UIElementMsg_PropertyTabText,

	UIElementMsg_TimeLineHorScroll,
};