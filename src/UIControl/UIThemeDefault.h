#pragma once
#include "UITheme.h"
class UIThemeDefault :
	public UITheme
{
public:
	UIThemeDefault();
	virtual ~UIThemeDefault();
public:
	virtual bool Initialize();
};

