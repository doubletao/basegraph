#pragma once
class UIPropertyViewElement
	: public UIElement
{
protected:
	int m_nParamIDBegin;
	int m_nParamIDEnd;
	std::map<BaseGraph_DataBase::ParamValueDef, Value> m_mapParam;
	//这里用一个冗余map做一个双向查找键值对，确保value类型和控件能够快速查找
	std::map<BaseGraph_DataBase::ParamValueDef, UICombinedCtrl *> m_mapValueControl;
	std::map<UICombinedCtrl *, BaseGraph_DataBase::ParamValueDef> m_mapControlValue;
public:
	UIPropertyViewElement();
	virtual ~UIPropertyViewElement();
protected:
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
public:
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd, UIElement * pParent, int nParamIDBegin, int nParamIDEnd);
	void SetValue(const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam, bool bRefresh = true);
	void SetValue(BaseGraph_DataBase::ParamValueDef paramValueDef, Value value, bool bRefresh = true);
	std::map<BaseGraph_DataBase::ParamValueDef, Value> GetValue();
	Value GetValue(BaseGraph_DataBase::ParamValueDef paramValueDef);
	void UpdateDataToUI();
	void UpdateUIToData();
};
