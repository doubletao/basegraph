#pragma once

class UICheckColorButton : public UICombinedCtrl
{
protected:
	UICheckBtn * m_pCheckButton;
	UIColorButton * m_pColorButton;
public:
	UICheckColorButton();
	virtual ~UICheckColorButton();
public:
	virtual BaseGraph_Common::Value GetValue();
	virtual void SetValue(BaseGraph_Common::Value value, bool bRefresh = true);
	virtual void UpdateValueToUI();
	virtual void UpdateUIToValue();

	virtual bool Create(int nID, HWND hParentWnd, UIElement * pParent);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void RearrangeChildPos();
};

