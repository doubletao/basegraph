#pragma once
#include "GraphEditorCommonDef.h"

class UIActionBar :
	public UICheckBtnBar
{
public:
	UIActionBar();
	virtual ~UIActionBar();
protected:
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
public:
	void UpdateStatus();
};

