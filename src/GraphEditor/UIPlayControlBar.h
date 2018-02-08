#pragma once
#include "GraphEditorCommonDef.h"

class UIPlayControlBar :
	public UICheckBtnBar
{
public:
	UIPlayControlBar();
	virtual ~UIPlayControlBar();
protected:
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
};

