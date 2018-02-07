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
