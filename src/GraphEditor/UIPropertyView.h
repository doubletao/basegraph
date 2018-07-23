#pragma once
#include "UIPropertyTabBar.h"
#include "UIPropertyViewElement.h"

class UIPropertyView :
	public UIElement
{
protected:
	UIPropertyTabBar * m_pPropertyTabBar;
	CString m_strObjID;//当前属性窗绑定物件
	UIElementMsgDef m_emCurrentTab;

	UIPropertyViewElement * m_pColorElement;
	UIPropertyViewElement * m_pObjectElement;
	UIPropertyViewElement * m_pSpaceElement;
	UIPropertyViewElement * m_pFxElement;
	UIPropertyViewElement * m_pTextElement;
public:
	UIPropertyView();
	virtual ~UIPropertyView();
protected:
	virtual void Release();
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr);//初始化工作
	virtual void RearrangeChildPos();
	virtual void InitializeObject();
	CString GetCurSelObjID();
public:
	virtual void OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom);
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void OnBtnPropertyTabColor();
	void OnBtnPropertyTabObject();
	void OnBtnPropertyTabSpace();
	void OnBtnPropertyTabFx();
	void OnBtnPropertyTabText();
	void UpdateStatus();
	std::map<BaseGraph_DataBase::ParamValueDef, Value> GetCurrentParamValue();//拿到当前所有属性窗的属性集合
};

