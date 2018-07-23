#pragma once
#include <vector>
#include <map>
#include <set>
#include "UIControl\UIDrawer.h"
#include "UIControl\MsgDispatcher.h"

enum EmUIElementMsg
{
	EmUIElementMsg_BtnClick = WM_USER + 0x00000100,
	EmUIElementMsg_BtnRefresh,
	EmUIElementMsg_SetCapture,//wParam为set和release，lParam为需要capture的element指针
	EmUIElementMsg_OnHScroll,
	EmUIElementMsg_OnVScroll,
	EmUIElementMsg_OnDBLClickScroll,//双击滚动条
	EmUIElementMsg_OnScrollFlexed,//滚动条伸缩了
	EmUIElementMsg_OnSliderChanging,
	EmUIElementMsg_OnSliderChanged,
	EmUIElementMsg_OnEditEnded,
	EmUIElementMsg_OnEditChged,
	EmUIElementMsg_OnColorChanged,
	EmUIElementMsg_OnCombinedCtrlValueChged,
	EmUIElementMsg_OnComboBoxSelChged,
	EmUIElementMsg_WndUserEnd,
};

enum EmUIElementMsgRet
{
	UIElementMsgSucc			= 0x00000001,	//执行是否成功
	UIElementMsgContinue		= 0x00000002,	//消息是否继续传递
	UIElementMsgSuccContinue	= UIElementMsgSucc | UIElementMsgContinue,
	UIElementMsgFailContinue	= UIElementMsgContinue,
	UIElementMsgSuccBreak		= UIElementMsgSucc,
	UIElementMsgFailBreak		= 0,
};

class BASEGRAPH_UICONTROL_API UIElement : public IMsgDispatchObject
{
protected:
	HWND m_hParentWnd;//父窗口句柄
	UIElement * m_pParentElement;//父元素指针
	int m_nID;//一个身份标识，用来做消息发送时标志
	int m_nX;
	int m_nY;
	int m_nWidth;
	int m_nHeight;
	int m_nState;
	CString m_strText;
	bool m_bCreated;
	bool m_bVisible;
	bool m_bEnable;
	std::vector<UIElement *> m_vecChildElement;//子控件
	std::map<int, std::vector<UIDrawer *>> m_mapDrawer;//绘制管理器
	std::set<UINT> m_setMsgFilter;//消息过滤器
public:
	static bool IsSucceed(EmUIElementMsgRet emMsgRet);
	static bool NeedContinue(EmUIElementMsgRet emMsgRet);
protected:
	virtual int GetMsgDispatchID();
	virtual void Release();
	virtual void SetText(CString strText);
	virtual void Refresh();
	virtual void ChildHandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual void EraseChild(UIElement * pChild);
	virtual void EraseChild(std::set<UIElement *> setChildPointer);
	virtual void EraseAllChild();
public:
	UIElement();
	virtual ~UIElement();
public:
	virtual void RearrangeChildPos();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual inline bool HitTest(CPoint point);
	virtual inline bool HitTest(int nX, int nY);
	virtual inline bool HitTest(LPARAM lParam);
	virtual inline CRect GetRect();
	virtual inline void MoveTo(CRect rtNew, bool bRefresh = true);
	virtual inline void MoveTo(int nX, int nY, bool bRefresh = true);
	virtual inline void MoveTo(int nX, int nY, int nWidth, int nHeight, bool bRefresh = true);
	virtual void DrawElement(CDC * pDC);
	virtual void OnMouseHover();
	virtual void OnMouseLeave();
	//向上追溯获取自己的实际窗口坐标
	virtual int GetRealX();
	virtual int GetRealY();
	virtual bool EmitMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void SetVisible(bool bVisible, bool bRefresh = true);
	virtual void SetEnable(bool bEnable, bool bRefresh = true);
	//向上追溯添加自己的敏感消息
	virtual void RegistWndMsg(DWORD nMsg);
	int GetMsgID();
	int GetWidth();
	int GetHeight();
	void InsertDrawer(int nState, UIDrawer * pDrawer);
	void SetElementCapture();//需要父窗口实现对应的消息接口
	void ReleaseElementCapture();//需要父窗口实现对应的消息接口
};

