#include "stdafx.h"
#include "ExportUIControl.h"
#include "ThemeManager.h"

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
