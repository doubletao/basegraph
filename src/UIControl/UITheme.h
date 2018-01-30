#pragma once
#include <atlimage.h>
#include <map>

class UITheme
{
private:
	std::map<int, LOGFONT> m_mapFontList;
	std::map<int, COLORREF> m_mapColorList;
	std::map<int, CImage * > m_mapImageList;
	LOGFONT m_DefaultLogFont;
	COLORREF m_DefaultColor;
	CImage m_DefaultImage;
public:
	UITheme();
	~UITheme();
public:
	virtual bool Initialize() = 0;
	virtual bool Release();
	virtual LOGFONT GetFont(int nFontID);
	virtual COLORREF GetColor(int nColorID);
	virtual CImage * GetImage(int nImgID);
};

