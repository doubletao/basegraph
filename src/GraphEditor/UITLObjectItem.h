#pragma once

enum UITLObjectActionBarType
{
	ActionBarType_Err,
	ActionBarType_Normal,//普通
	ActionBarType_Sel,//选中
	ActionBarType_Group,//组普通
	ActionBarType_GroupFloat,//组选中
	ActionBarType_Lock,//锁定
};

class UITLObjectItem
	: public UIElement
{
protected:
	CString m_strObjID;
	bool m_bExpand;
public:
	UITLObjectItem();
	~UITLObjectItem();
public:
	bool GetExpand();
	void SetExpand(bool bExpand);
	CString GetObjID();
	void SetObjID(CString strID);
};