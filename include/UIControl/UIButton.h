#pragma once
#include "UIElement.h"

class BASEGRAPH_UICONTROL_API UIButton :
	public UIElement
{
public:
	enum
	{
		UIElement_BtnNormal,	//正常
		UIElement_BtnFloat,	//划过
		UIElement_BtnDown,		//按下
		UIElement_BtnXable,	//disable
	};
public:
	UIButton();
	virtual ~UIButton();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
public:
	void SetState(int nState, bool bRefresh);
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd,
		int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, UIElement * pParent = nullptr, LPCTSTR strText = nullptr, int nFont = 0, COLORREF nFontColor = 0);
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, 
		int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, int nForeImgN, int nForeImgF, int nForeImgD, int nForeImgX, 
		UIElement * pParent = nullptr, LPCTSTR strText = nullptr, int nFont = 0, COLORREF nFontColor = 0);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual void OnButtonClickDown();
	virtual void OnButtonClickUp();
	virtual void OnMouseHover();
	virtual void OnMouseLeave();
	virtual void SetCheck(bool bCheck);
	virtual bool GetCheck();
};
