#pragma once
#include <vector>
#include <map>
#include <set>
#include "UIDrawer.h"

enum EmUIElementMsg
{
	EmUIElementMsg_BtnClick = WM_USER + 0x00000101,
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
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	void Refresh();
public:
	UIElement();
	virtual ~UIElement();
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual inline bool HitTest(CPoint point);
	virtual inline bool HitTest(int nX, int nY);
	virtual inline bool HitTest(LPARAM lParam);
	virtual inline CRect GetRect();
	virtual inline void MoveTo(CRect rtNew);
	virtual inline void MoveTo(int nX, int nY);
	virtual void DrawElement(CDC * pDC, int nX = 0, int nY = 0);
	virtual void OnMouseHover();
	virtual void OnMouseLeave();
};

