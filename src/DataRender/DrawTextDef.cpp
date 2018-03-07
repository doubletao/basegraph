#include "stdafx.h"
#include "DataRender\DrawTextDef.h"

//////////////////////////////////////////////////////////////////////////
//缺省字体、缺省中文字体、预制用字符。
BASEGRAPH_DATARENDER_API TCHAR			g_strDefFontFacename[LF_FACESIZE] = _T("Arial");
BASEGRAPH_DATARENDER_API TCHAR			g_strDefChnFontFacename[LF_FACESIZE] = _T("宋体");
BASEGRAPH_DATARENDER_API TCHAR			g_strPredefSample[8] = _T("A");
LPCTSTR	GetDefaultFontFaceName()
{
	return g_strDefFontFacename;
}

void	SetDefaultFontFaceName(LPCTSTR szFaceName)
{
	memcpy(g_strDefFontFacename, szFaceName, sizeof(TCHAR) * (_tcslen(szFaceName) + 1));
}

LPCTSTR	GetDefaultChnFontFaceName()
{
	return g_strDefChnFontFacename;
}

void	SetDefaultChnFontFaceName(LPCTSTR szFaceName)
{
	memcpy(g_strDefChnFontFacename, szFaceName, sizeof(TCHAR) * (_tcslen(szFaceName) + 1));
}

LPCTSTR	GetPredefineSample()
{
	return g_strPredefSample;
}

void	SetPredefineSample(LPCTSTR szSample)
{
	memcpy(g_strPredefSample, szSample, sizeof(TCHAR) * 2);
}

BASEGRAPH_DATARENDER_API void sy_text_prop_def::Reset()
{
	memset(this, 0, sizeof(sy_text_prop_def));
}

BASEGRAPH_DATARENDER_API void sy_font_def::Reset()
{
	lfHeight = 0;
	lfWidth = 0;
	lfEscapement = 0;				//旋转
	lfOrientation = 0;
	lfWeight = 0;
	lfItalic = 0;
	lfUnderline = 0;
	lfStrikeOut = 0;
	lfCharSet = 0;
	lfOutPrecision = 0;
	lfClipPrecision = 0;
	lfQuality = 0;
	lfPitchAndFamily = 0;
	memset(lfFaceName, 0, LF_FACESIZE * sizeof(TCHAR));
	dwVersion = 0;					//版本号
	lfDistance = 0;					//字间距
	bBold = 0;						//是否加粗（lfWeight会重新调整）
	lpParam = 0;
	memset(lfFaceNameEng, 0, LF_FACESIZE * sizeof(TCHAR));	//英文字体
	lfVEmbeding = 0;				//（横排版中）嵌入竖排版
	lfHEmbeding = 0;				//（竖排版中）嵌入横排版
	lfVEmbedingAlign = 0;			//（横排版中）嵌入竖排版的对齐方式
	lfHEmbedingAlign = 0;			//（竖排版中）嵌入横排版的对齐方式
	lfVGRotate = 0;					//竖排旋转字形
	lfScript = 0;					//脚注
	lfScriptOffset = 0;				//自定义脚注偏移
}

BASEGRAPH_DATARENDER_API void sy_art_typeset_def::Reset()
{
	memset(this, 0, sizeof(sy_art_typeset_def));
}

BASEGRAPH_DATARENDER_API void sy_text_def::Reset()
{
	memset(this, 0, sizeof(sy_text_def));
	dEndTime = 1;
}

BASEGRAPH_DATARENDER_API void sy_textsymbol_def::Reset()
{
	memset(&seFillColor, 0, sizeof(static_effect));
	memset(&rtSize, 0, sizeof(rectf));
	fScale = 0;
}

BASEGRAPH_DATARENDER_API void sy_textparagraph_def::Reset()
{
	memset(this, 0, sizeof(sy_textparagraph_def));
}

BASEGRAPH_DATARENDER_API void sy_under_line_def::Reset()
{
	eType = UNDERLINE_NULL;					//线类型
	fDistance = 0;				//线相对于基线的偏移
	fHeight = 0;
	se2DColor.renderline = 0;
	se2DColor.fwidth = 0;
	se2DColor.index = 0;
}
