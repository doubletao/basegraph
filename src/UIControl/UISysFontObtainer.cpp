#include "stdafx.h"
#include "UISysFontObtainer.h"


UISysFontObtainer::UISysFontObtainer()
{
}


UISysFontObtainer::~UISysFontObtainer()
{
}

void UISysFontObtainer::LoadFontList()
{
	CDC memDC;
	memDC.CreateCompatibleDC(NULL);
	EnumFontFamilies(memDC.m_hDC, NULL, FontEngEnumProc, (LPARAM)this);
}

int UISysFontObtainer::FontEngEnumProc(const LOGFONT * pLogFont, const TEXTMETRIC * pTextMetric, DWORD type, LPARAM pDestLogFont)
{
	UISysFontObtainer * pThis = (UISysFontObtainer *)pDestLogFont;
	if (pThis)
	{
		if ((type != 0) && ((type & RASTER_FONTTYPE) == 0) && pLogFont)
		{
			if (pLogFont->lfFaceName[0] != _T('@'))
			{
				pThis->m_lstFontName.push_back(pLogFont->lfFaceName);
			}
		}
	}
	return true;
}

UISysFontObtainer & UISysFontObtainer::Instance()
{
	static UISysFontObtainer inst;
	return inst;
}

const std::list<CString>& UISysFontObtainer::GetFontList()
{
	if (m_lstFontName.empty())
	{
		LoadFontList();
	}
	return m_lstFontName;
}
