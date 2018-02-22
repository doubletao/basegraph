#pragma once
#include <vector>
#include <map>
#include <set>
#include "UIDrawer.h"

enum EmUIElementMsg
{
	EmUIElementMsg_BtnClick = WM_USER + 0x00000101,
	EmUIElementMsg_BtnRefresh,
	EmUIElementMsg_SetCapture,//wParam为set和release，lParam为需要capture的element指针
	EmUIElementMsg_OnHScroll,
	EmUIElementMsg_OnVScroll,
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

class BASEGRAPH_UICONTROL_API UIElement
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
	std::vector<UIElement *> m_vecChildElement;//子控件
	std::map<int, std::vector<UIDrawer *>> m_mapDrawer;//绘制管理器
	std::set<UINT> m_setMsgFilter;//消息过滤器
public:
	static bool IsSucceed(EmUIElementMsgRet emMsgRet);
	static bool NeedContinue(EmUIElementMsgRet emMsgRet);
protected:
	virtual void Release();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
	virtual void SetText(CString strText);
	virtual void Refresh();
	virtual void ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual void EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	UIElement();
	virtual ~UIElement();
public:
	virtual void RearrangeChildPos();
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
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
	int GetMsgID();
	int GetWidth();
	int GetHeight();
};

