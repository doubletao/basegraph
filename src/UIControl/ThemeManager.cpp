#include "stdafx.h"
#include "ThemeManager.h"
#include "UIThemeDefault.h"
#include <assert.h>

ThemeManager::ThemeManager()
	: m_nCurrentTheme(-1)
{
	InitialThemeList();
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
		return m_vecTheme[m_nCurrentTheme];
	}
	return nullptr;
}

void ThemeManager::InitialThemeList()
{
	UITheme * pTheme = nullptr;

	do
	{
		pTheme = new UIThemeDefault;
		if (pTheme)
		{
			pTheme->Initialize();
			m_vecTheme.push_back(pTheme);
		}
	} while (false);

	if (m_vecTheme.size() > 0)
	{
		m_nCurrentTheme = 0;
	}
}

ThemeManager & ThemeManager::Instance()
{
	static ThemeManager inst;
	return inst;
}

bool ThemeManager::SwitchTheme(int nTheme)
{
	auto CheckThemeIndex = [&](int nIndex)->bool
	{
		if (nIndex > -1 && m_vecTheme.size() > nIndex
			&& m_vecTheme[nIndex])
		{
			return true;
		}
		return false;
	};
	//处于节约资源的考虑，主题的切换动作会释放掉当前主题，创建新的主题，保证同一时刻内存中只有一个主题
	if (CheckThemeIndex(nTheme))
	{
		if (CheckThemeIndex(m_nCurrentTheme))
		{
			UITheme *& pThemeCur = m_vecTheme[m_nCurrentTheme];
			pThemeCur->Release();
		}
		UITheme *& pThemeNew = m_vecTheme[nTheme];
		pThemeNew->Initialize();
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
	assert(pTheme);
	if (pTheme)
	{
		return pTheme->GetImage(nImgID);
	}
	return nullptr;
}

int ThemeManager::GetIntValue(int nIntID)
{
	UITheme * pTheme = GetCurTheme();
	if (pTheme)
	{
		return pTheme->GetIntValue(nIntID);
	}
	return 0;
}
