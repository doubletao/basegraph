#include "stdafx.h"
#include "UISysFontObtainer.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
	EnumFontFamilies(memDC.m_hDC, NULL, FontEnumProc, (LPARAM)this);
}

int UISysFontObtainer::FontEnumProc(const LOGFONT * pLogFont, const TEXTMETRIC * pTextMetric, DWORD type, LPARAM pDestLogFont)
{
	UISysFontObtainer * pThis = (UISysFontObtainer *)pDestLogFont;
	if (pThis)
	{
		if (type != 0 && (type & RASTER_FONTTYPE) == 0)
		{
			if ((pLogFont->lfFaceName)[0] != _T('@'))
			{
				pThis->m_lstEngFontName.push_back(pLogFont->lfFaceName);
			}
		}
		if (type != 0 && (type & RASTER_FONTTYPE) == 0)
		{
			if ((pLogFont->lfFaceName)[0] != _T('@'))
			{
				pThis->m_lstChnFontName.push_back(pLogFont->lfFaceName);
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

const std::list<CString>& UISysFontObtainer::GetEngFontList()
{
	if (m_lstEngFontName.empty())
	{
		LoadFontList();
	}
	return m_lstEngFontName;
}

const std::list<CString>& UISysFontObtainer::GetChnFontList()
{
	if (m_lstChnFontName.empty())
	{
		LoadFontList();
	}
	return m_lstChnFontName;
}
