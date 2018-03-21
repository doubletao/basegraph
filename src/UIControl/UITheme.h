#pragma once
#include <atlimage.h>
#include <map>

class UITheme
{
protected:
	std::map<int, LOGFONT> m_mapFontList;
	std::map<int, COLORREF> m_mapColorList;
	std::map<int, CImage * > m_mapImageList;
	std::map<int, int> m_mapIntValue;
	std::map<int, CString> m_mapCStringValue;
	LOGFONT m_DefaultLogFont;
	COLORREF m_DefaultColor;
	CImage m_DefaultImage;
	int m_nDefaultInt;
public:
	UITheme();
	virtual ~UITheme();
public:
	virtual CString GetThemePath();
	virtual bool Initialize() = 0;
	virtual bool Release();
	virtual LOGFONT GetFont(int nFontID);
	virtual COLORREF GetColor(int nColorID);
	virtual CImage * GetImage(int nImgID);
	virtual int GetIntValue(int nIntID);
	virtual CString GetCStringValue(int nCStringID);
};

