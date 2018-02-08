#pragma once

typedef void(*FuncBtnClicked)(int nID);

enum UIElementMsgDef
{
	UIElementMsg_CreateTitle,
	UIElementMsg_CreateScroll,
	UIElementMsg_CreateImage,
};