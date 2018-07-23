#pragma once
class UIPropertyViewElement
	: public UIElement
{
protected:
	int m_nParamIDBegin;
	int m_nParamIDEnd;
	CString m_strObjID;//属性窗的当前绑定物件（可能为空）
	BaseGraph_DataBase::ObjectType m_emObjType;
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
	virtual void EraseChild(UIElement * pChild);
	virtual void EraseChild(std::set<UIElement *> setChildPointer);
	virtual void EraseAllChild();
public:
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	bool Create(int nX, int nY, int nWidth, int nHeight, int m_nID, HWND hParentWnd, UIElement * pParent, int nParamIDBegin, int nParamIDEnd);
	bool BindCreateType(BaseGraph_DataBase::ObjectType emType);
	bool BindObject(CString strObjID);//将属性窗绑定到指定物件
	virtual void RefreshFromID(CString strObjID);
	virtual void RefreshFromType(BaseGraph_DataBase::ObjectType emType);
	void SetValue(const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam, bool bRefresh = true);
	void SetValue(BaseGraph_DataBase::ParamValueDef paramValueDef, Value value, bool bRefresh = true);
	void UpdateToObject(BaseGraph_DataBase::ParamValueDef paramValueDef, Value value);//更新属性到物件上
	std::map<BaseGraph_DataBase::ParamValueDef, Value> GetValue();
	Value GetValue(BaseGraph_DataBase::ParamValueDef paramValueDef);
	void UpdateDataToUI();
	void UpdateUIToData();
	virtual void SetVisible(bool bVisible, bool bRefresh = true);
};
