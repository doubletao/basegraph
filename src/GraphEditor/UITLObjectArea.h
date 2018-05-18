#pragma once
#include "UITLObjectItem.h"
#include "UITLObjectLeftArea.h"
#include "UITLObjectRightArea.h"

enum ObjectAreaHittestType
{
	ObjectArea_HitNull,
	ObjectArea_HitItem,
	ObjectArea_HitBar,
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
protected:
	virtual void RearrangeChildPos();
	UITLObjectItem * CreateObjectItem(CString strID);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
public:
	virtual void OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom);
	ObjectAreaHittestType ObjectHittest(CPoint ptHit, CString * pStrID = nullptr);
	ObjectAreaHittestType ObjectHittest(int x, int y, CString * pStrID = nullptr);
	ObjectAreaHittestType ObjectHittest(LPARAM lParam, CString * pStrID = nullptr);

	virtual void DrawElement(CDC * pDC);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement);

	bool SetScrollInfo(bool bRefresh = true);
	bool SetScrollXPos(int nXPos, bool bRefresh = true);
	bool SetScrollYPos(int nYPos, bool bRefresh = true);
	int GetScrollXPos();
	int GetScrollYPos();

	void SetVScrollHeight(int nHeight = 0, bool bReset = false);
	int GetVScrollHeight();

	void AdjustScrollZoom(float fScrollPercent);//按比例缩放frame总长使得横向滚动条满屏
	float GetCurrentPercent();
	std::map<CString, UITLObjectItem *> & GetObjDrawerInfo();
	UITLObjectItem * GetObjDrawerInfo(CString strID);
	bool  DeleteObjDrawerInfo(CString strID);//TODO: 目前为空,需要实现
};
