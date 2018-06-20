#pragma once
#include <list>

class UISysFontObtainer
{
private:
	UISysFontObtainer();
	~UISysFontObtainer();
protected:
	std::list<CString> m_lstFontName;
protected:
	void LoadFontList();
	static int CALLBACK FontEngEnumProc(const LOGFONT *pLogFont, const TEXTMETRIC *pTextMetric, DWORD type, LPARAM pDestLogFont);
public:
	static UISysFontObtainer & Instance();
	const std::list<CString> & GetFontList();
};

