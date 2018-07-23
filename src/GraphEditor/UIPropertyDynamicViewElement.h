#pragma once
#include "UIPropertyViewElement.h"
/*
 * 动态属性窗，和普通属性窗的差别在于，内部控件在绑定的物件变动时可能会重新创建
 */

class UIPropertyDynamicViewElement
	: public UIPropertyViewElement
{
public:
	UIPropertyDynamicViewElement();
	virtual ~UIPropertyDynamicViewElement();
public:
	virtual void RefreshFromID(CString strObjID);
	virtual void RefreshFromType(BaseGraph_DataBase::ObjectType emType);
};
