#include "stdafx.h"
#include "ThemeManager.h"


ThemeManager::ThemeManager()
	: m_nCurrentTheme(-1)
{
}


ThemeManager::~ThemeManager()
{
	for (size_t i = 0; i < m_vecTheme.size(); i++)
	{
		UITheme * pTheme = m_vecTheme[i];
		delete pTheme;
		pTheme = nullptr;
	}
	m_vecTheme.clear();
}

UITheme * ThemeManager::GetCurTheme()
{
	if (m_nCurrentTheme > -1 && m_vecTheme.size() > m_nCurrentTheme
		&& m_vecTheme[m_nCurrentTheme])
	{
		m_vecTheme[m_nCurrentTheme]->Initialize();
		return m_vecTheme[m_nCurrentTheme];
	}
	return nullptr;
}

void ThemeManager::InitialThemeList()
{
	//TODO:主题列表的创建在这里
}

ThemeManager & ThemeManager::Instance()
{
	static ThemeManager inst;
	return inst;
}

bool ThemeManager::SwitchTheme(int nTheme)
{
	if (nTheme > -1 && m_vecTheme.size() > nTheme)
	{
		UITheme * pTheme = nullptr;
		if (m_nCurrentTheme > -1 && m_vecTheme.size() > m_nCurrentTheme
			&& m_vecTheme[m_nCurrentTheme])
		{
			m_vecTheme[m_nCurrentTheme]->Release();
		}
		m_nCurrentTheme = nTheme;
		return true;
	}
	return false;
}

LOGFONT ThemeManager::GetFont(int nFontID)
{
	UITheme * pTheme = GetCurTheme();
	if (pTheme)
	{
		return pTheme->GetFont(nFontID);
	}
	return LOGFONT();
}

COLORREF ThemeManager::GetColor(int nColorID)
{
	UITheme * pTheme = GetCurTheme();
	if (pTheme)
	{
		return pTheme->GetColor(nColorID);
	}
	return 0;
}

CImage * ThemeManager::GetImage(int nImgID)
{
	UITheme * pTheme = GetCurTheme();
	if (pTheme)
	{
		return pTheme->GetImage(nImgID);
	}
	return nullptr;
}
