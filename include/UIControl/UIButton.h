#pragma once
#include "UIElement.h"

class BASEGRAPH_UICONTROL_API UIButton :
	public UIElement
{
	enum
	{
		UIElement_Normal,	//正常
		UIElement_Float,	//划过
		UIElement_Down,		//按下
		UIElement_Xable,	//disable
	};
public:
	UIButton();
	virtual ~UIButton();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
	void OnButtonClickDown();
	void OnButtonClickUp();
	void SetState(int nState, bool bRefresh);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd,
		int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, UIElement * pParent = nullptr, LPCTSTR strText = nullptr, int nFont = 0, COLORREF nFontColor = 0);
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, 
		int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, int nForeImgN, int nForeImgF, int nForeImgD, int nForeImgX, 
		UIElement * pParent = nullptr, LPCTSTR strText = nullptr, int nFont = 0, COLORREF nFontColor = 0);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual void OnMouseHover();
	virtual void OnMouseLeave();
};

