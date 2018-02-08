#pragma once
#ifdef BASEGRAPH_UICONTROL_EXPORTS
#define BASEGRAPH_UICONTROL_API __declspec(dllexport)
#else
#define BASEGRAPH_UICONTROL_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "UIControlD.lib")
#else
#	pragma comment(lib, "UIControl.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_UICONTROL_EXPORTS

#include "UIControl\SkinDlgWnd.h"
#include "UIControl\SkinFrameWnd.h"
#include "UIControl\SkinWnd.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIButton.h"
#include "UIControl\UIColorDrawer.h"
#include "UIControl\UIDrawer.h"
#include "UIControl\UIElement.h"
#include "UIControl\UIImageDrawer.h"
#include "UIControl\UITextDrawer.h"
#include "UIControl\UICheckBtn.h"

BASEGRAPH_UICONTROL_API bool UIControl_SwitchTheme(int nTheme);
BASEGRAPH_UICONTROL_API LOGFONT UIControl_GetFont(int nFontID);
BASEGRAPH_UICONTROL_API COLORREF UIControl_GetColor(int nColorID);
BASEGRAPH_UICONTROL_API CImage * UIControl_GetImage(int nImgID);
BASEGRAPH_UICONTROL_API int UIControl_GetIntValue(int nIntID);