#pragma once
#include "UITLObjectItem.h"
#include "UITLObjectLeftArea.h"
#include "UITLObjectRightArea.h"

enum ObjectAreaHittestType
{
	ObjectArea_HitNull,
	ObjectArea_HitItem,//点中了物件（包含物件树和物件bar在内的整个物件）
	ObjectArea_HitItemBarLeftEnd,//点中了物件bar的左端点
	ObjectArea_HitItemBarMiddle,//点中了物件bar中间
	ObjectArea_HitItemBarRightEnd,//点中的物件bar的右端点
};

enum ObjectBarMoveState
{
	MoveState_Null,//默认空白值
	MoveState_ActionBarMove,//物件action被移动
	MoveState_ActionLeftEndMove,//物件action左端点被移动
	MoveState_ActionRightMove,//物件action右端点被移动
};

class UITLObjectArea :
	public UIElement
{
public:
	UITLObjectArea();
	virtual ~UITLObjectArea();
protected:
	std::map<CString, UITLObjectItem *> m_mapObjDrawerInfo;//绘制信息的集合
	UITLObjectLeftArea * m_pLeftArea;
	UITLObjectRightArea * m_pRightArea;
	int m_nVScrollHeight;//记录当前最大y值,用于计算垂直滚动条
	int m_nItemBarEndAreaWidth;//action bar两端的敏感区域宽度，进入此区域后视为调整action端点的行为
	ObjectBarMoveState m_emBarMoveState;//当前操作状态
	CPoint m_ptBegin;//鼠标点下的点
	CString m_strHitObjectID;//当前点中的物件
	bool m_bNeedRefresh;
protected:
	virtual void RearrangeChildPos();
	UITLObjectItem * CreateObjectItem(CString strID);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(ObjectAreaHittestType emHittestType);
public:
	virtual void OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom);
	ObjectAreaHittestType ObjectHittest(CPoint ptHit, CString * pStrID = nullptr);
	ObjectAreaHittestType ObjectHittest(int x, int y, CString * pStrID = nullptr);
	ObjectAreaHittestType ObjectHittest(LPARAM lParam, CString * pStrID = nullptr);

	virtual void DrawElement(CDC * pDC);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement);

	bool SetScrollInfo(bool bRefresh = true);
	bool SetScrollXPos(int nXPos, bool bRefresh = true);
	bool SetScrollYPos(int nYPos, bool bRefresh = true);
	int GetScrollXPos();
	int GetScrollYPos();

	void SetVScrollHeight(int nHeight = 0, bool bReset = false);
	int GetVScrollHeight();

	void AdjustScrollZoom(float fScrollPercent);//按比例缩放frame总长使得横向滚动条满屏
	void OnScrollFlexed();//滚动条发生了伸缩变换
	void AdjustScrollZoomKeepPixPerFrame();//保持当前像素与帧数比值
	float GetCurrentPercent();
	std::map<CString, UITLObjectItem *> & GetObjDrawerInfo();
	UITLObjectItem * GetObjDrawerInfo(CString strID);
	bool  DeleteObjDrawerInfo(CString strID);
};
