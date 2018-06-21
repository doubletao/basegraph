#pragma once

class UIComboxFontSel : public UICombinedCtrl
{
protected:
	UIStatic * m_pStatic;
	UIComboBox * m_pComboBoxFont;
	//CComboBox * m_pComboBoxFont;
public:
	UIComboxFontSel();
	virtual ~UIComboxFontSel();
public:
	virtual BaseGraph_Common::Value GetValue();
	virtual void SetValue(BaseGraph_Common::Value value, bool bRefresh = true);
	virtual void UpdateValueToUI();
	virtual void UpdateUIToValue();

	virtual bool Create(CString strTitle, int nTitleWidth, int nID, HWND hParentWnd, UIElement * pParent);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void RearrangeChildPos();
};

