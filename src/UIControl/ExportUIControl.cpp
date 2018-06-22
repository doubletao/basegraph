#include "stdafx.h"
#include "ExportUIControl.h"
#include "ThemeManager.h"
#include "UISysFontObtainer.h"

BASEGRAPH_UICONTROL_API CString UIControl_GetThemePath()
{
	return ThemeManager::Instance().GetThemePath();
}

BASEGRAPH_UICONTROL_API bool UIControl_SwitchTheme(int nTheme)
{
	return ThemeManager::Instance().SwitchTheme(nTheme);
}

BASEGRAPH_UICONTROL_API LOGFONT UIControl_GetFont(int nFontID)
{
	return ThemeManager::Instance().GetFont(nFontID);
}

BASEGRAPH_UICONTROL_API COLORREF UIControl_GetColor(int nColorID)
{
	return ThemeManager::Instance().GetColor(nColorID);
}

BASEGRAPH_UICONTROL_API CImage * UIControl_GetImage(int nImgID)
{
	return ThemeManager::Instance().GetImage(nImgID);
}

BASEGRAPH_UICONTROL_API int UIControl_GetIntValue(int nIntID)
{
	return ThemeManager::Instance().GetIntValue(nIntID);
}

BASEGRAPH_UICONTROL_API CString UIControl_GetCString(int nCStringID)
{
	return ThemeManager::Instance().GetCStringValue(nCStringID);
}

BASEGRAPH_UICONTROL_API void UIControl_RegistMsgDispatcher(IMsgDispatchObject * pMsgObj)
{
	return MsgDispatcher::Instance().RegistMsgDispatcher(pMsgObj);
}

BASEGRAPH_UICONTROL_API void UIControl_UnRegistDispathObj(IMsgDispatchObject * pMsgObj)
{
	return MsgDispatcher::Instance().UnRegistDispatchObj(pMsgObj);
}

BASEGRAPH_UICONTROL_API void UIControl_BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	return MsgDispatcher::Instance().BroadcastMsg(nMsgID, wParam, lParam, nIDFrom);
}

BASEGRAPH_UICONTROL_API void UIControl_BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, IMsgDispatchObject * pMsgObj)
{
	return MsgDispatcher::Instance().BroadcastMsg(nMsgID, wParam, lParam, pMsgObj);
}

BASEGRAPH_UICONTROL_API void UIControl_SendMsgTo(int nIDTo, int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	return MsgDispatcher::Instance().SendMsgTo(nIDTo, nMsgID, wParam, lParam, nIDFrom);
}

BASEGRAPH_UICONTROL_API IMsgDispatchObject * UIControl_GetMsgDispatcher(int nDispatcherID)
{
	return MsgDispatcher::Instance().GetMsgDispatcher(nDispatcherID);
}

BASEGRAPH_UICONTROL_API const std::list<CString>& UIControl_GetChnFontList()
{
	return UISysFontObtainer::Instance().GetChnFontList();
}

BASEGRAPH_UICONTROL_API const std::list<CString>& UIControl_GetEngFontList()
{
	return UISysFontObtainer::Instance().GetEngFontList();
}
