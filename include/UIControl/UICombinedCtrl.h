#pragma once
#include "UIControl\UIElement.h"

class BASEGRAPH_UICONTROL_API UICombinedCtrl
	: public UIElement
{
protected:
	BaseGraph_Common::Value m_value;
public:
	virtual BaseGraph_Common::Value GetValue() = 0;
	virtual void SetValue(BaseGraph_Common::Value value, bool bRefresh = true) = 0;
	virtual void UpdateValueToUI() = 0;
	virtual void UpdateUIToValue() = 0;
	virtual void NotifyValueChg();//发出数据变动的消息给上层
	virtual void SetVisible(bool bVisible, bool bRefresh = true);
};
