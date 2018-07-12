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
#include "UIControl\UIElement.h"
#include "UIControl\UIButton.h"
#include "UIControl\UIColorDrawer.h"
#include "UIControl\UIDrawer.h"
#include "UIControl\UIImageDrawer.h"
#include "UIControl\UITextDrawer.h"
#include "UIControl\UICombinedCtrl.h"
#include "UIControl\UIColorButton.h"
#include "UIControl\UICheckBtn.h"
#include "UIControl\UIButtonBar.h"
#include "UIControl\UICheckBtnBar.h"
#include "UIControl\UIScrollElement.h"
#include "UIControl\UIScrollBar.h"
#include "UIControl\UIFlexibleScrollBar.h"
#include "UIControl\UIControlDef.h"
#include "UIControl\MsgDispatcher.h"
#include "UIControl\UIEdit.h"
#include "UIControl\UIEditMutiLine.h"
#include "UIControl\UIEditNum.h"
#include "UIControl\UIEditNumSlider.h"
#include "UIControl\UIEditNumSpin.h"
#include "UIControl\UIEditPath.h"
#include "UIControl\UIEditSingleLine.h"
#include "UIControl\UIStatic.h"
#include "UIControl\CustomEdit.h"
#include "UIControl\CustomScrollBar.h"
#include "UIControl\CustomComboBox.h"
#include "UIControl\UIComboBox.h"

//主题
BASEGRAPH_UICONTROL_API CString UIControl_GetThemePath();
BASEGRAPH_UICONTROL_API bool UIControl_SwitchTheme(int nTheme);
BASEGRAPH_UICONTROL_API LOGFONT UIControl_GetFont(int nFontID);
BASEGRAPH_UICONTROL_API COLORREF UIControl_GetColor(int nColorID);
BASEGRAPH_UICONTROL_API CImage * UIControl_GetImage(int nImgID);
BASEGRAPH_UICONTROL_API int UIControl_GetIntValue(int nIntID);
BASEGRAPH_UICONTROL_API CString UIControl_GetCString(int nCStringID);
//消息
BASEGRAPH_UICONTROL_API void UIControl_RegistMsgDispatcher(IMsgDispatchObject * pMsgObj);
BASEGRAPH_UICONTROL_API void UIControl_UnRegistDispathObj(IMsgDispatchObject * pMsgObj);
BASEGRAPH_UICONTROL_API void UIControl_BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom = 0);
BASEGRAPH_UICONTROL_API void UIControl_BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, IMsgDispatchObject * pMsgObj = nullptr);
BASEGRAPH_UICONTROL_API void UIControl_SendMsgTo(int nIDTo, int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom = 0);
BASEGRAPH_UICONTROL_API IMsgDispatchObject * UIControl_GetMsgDispatcher(int nDispatcherID);//取得消息处理器对象指针
//拿字体列表
BASEGRAPH_UICONTROL_API const std::list<CString> & UIControl_GetChnFontList();
BASEGRAPH_UICONTROL_API const std::list<CString> & UIControl_GetEngFontList();