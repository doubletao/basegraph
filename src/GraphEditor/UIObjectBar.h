#pragma once
#include "GraphEditorCommonDef.h"

class UIObjectBar : public UICheckBtnBar
{
public:
	UIObjectBar();
	virtual ~UIObjectBar();
protected:
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
public:
	void UpdateStatus();
};

