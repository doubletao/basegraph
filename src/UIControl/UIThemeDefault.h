#pragma once
#include "UITheme.h"
class UIThemeDefault :
	public UITheme
{
public:
	UIThemeDefault();
	virtual ~UIThemeDefault();
public:
	virtual CString GetThemePath();
	virtual bool Initialize();
};

