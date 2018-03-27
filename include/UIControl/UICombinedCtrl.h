#pragma once
#include "UIControl\UIElement.h"

class UICombinedCtrl
	: public UIElement
{
protected:
	BaseGraph_Common::Value m_value;
public:
	virtual BaseGraph_Common::Value GetValue() = 0;
	virtual void SetValue(BaseGraph_Common::Value value, bool bRefresh = true) = 0;
	virtual void UpdateValueToUI() = 0;
	virtual void UpdateUIToValue() = 0;
};
