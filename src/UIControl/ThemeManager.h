#pragma once
#include <vector>
#include "UITheme.h"

class ThemeManager
{
private:
	int m_nCurrentTheme;
	std::vector<UITheme *> m_vecTheme;
private:
	ThemeManager();
	~ThemeManager();
protected:
	UITheme * GetCurTheme();
	void InitialThemeList();
public:
	static ThemeManager & Instance();
	bool SwitchTheme(int nTheme);
	LOGFONT GetFont(int nFontID);
	COLORREF GetColor(int nColorID);
	CImage * GetImage(int nImgID);
	int GetIntValue(int nIntID);
};

