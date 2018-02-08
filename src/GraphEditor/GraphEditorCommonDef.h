#pragma once

typedef void(*FuncBtnClicked)(int nID);

enum UIElementMsgDef
{
	UIElementMsg_NULL,
	UIElementMsg_CreateTitle = 1,
	UIElementMsg_CreateScroll,
	UIElementMsg_CreateImage,
};