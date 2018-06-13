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
	UIElementMsg_SimpleTimeLineSlider,

	UIElementMsg_PropertyView,
	UIElementMsg_PropertyView_Color,
	UIElementMsg_PropertyView_Object,
	UIElementMsg_PropertyView_Space,
	UIElementMsg_PropertyView_Fx,
	UIElementMsg_PropertyView_Text,

	UIElementMsg_PropertyTabColor,
	UIElementMsg_PropertyTabObject,
	UIElementMsg_PropertyTabSpace,
	UIElementMsg_PropertyTabFx,
	UIElementMsg_PropertyTabText,

	UIElementMsg_TimeLine,
	UIElementMsg_TimeLineHorScroll,
	UIElementMsg_TimeLineVerScroll,
	UIElementMsg_TimeLineObjectArea,
	UIElementMsg_TimeLineObjectLeftArea,
	UIElementMsg_TimeLineObjectRightArea,
	UIElementMsg_TimeLineRuler,
	UIElementMsg_TimeLinePointer,

	UIElementMsg_BaseGraphMainFrame,
	UIElementMsg_MainViewWnd,

	WM_MSG_BEGIN = EmUIElementMsg_WndUserEnd + 1,
	WM_MSG_SerRenderTask,//触发一次刷新
	WM_MSG_ExcuteRenderTask,//窗口的刷新消息
	WM_MSG_FrameChanged,//wParam为帧数，lParam为引发change的控件ID
	WM_MSG_WholeFrameLengthChanged,//wParam为帧数，lParam为引发change的控件ID
	WM_MSG_OnSelectChanged,//选中项发生变化
	WM_MSG_OnMessageHandlerChg,//当前消息处理器发生变化
	WM_MSG_TimeLinePointerChanged,//通知时间线刷新一下pointer
	WM_MSG_ObjectSpaceChanged,//空间属性变更
	WM_MSG_OnCreateObjTool,//创建物件工具被加载
	WM_MSG_ObjectCreateDelete,//物件增删动作
};
