#pragma once
#include <list>

class UISysFontObtainer
{
private:
	UISysFontObtainer();
	~UISysFontObtainer();
protected:
	std::list<CString> m_lstEngFontName;
	std::list<CString> m_lstChnFontName;
protected:
	void LoadFontList();
	static int CALLBACK FontEnumProc(const LOGFONT *pLogFont, const TEXTMETRIC *pTextMetric, DWORD type, LPARAM pDestLogFont);
public:
	static UISysFontObtainer & Instance();
	const std::list<CString> & GetEngFontList();
	const std::list<CString> & GetChnFontList();
};

