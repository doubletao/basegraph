////////////////////////////////////////////////////////////////////////////////
// 附注：
// 1、一个Char对应Unicode Point（代码点）；
// 2、Glyph为字形，Glyph Index为字形索引；
// 3、tagSYSCRIPTRUN 的CharIndex是在整个字符串中的索引位置；
// 4、tagSYSCRIPTRUN 的m_nRunCharPos是一段字符中的索引位置；
// 5、“logical”关键字通常用来表示Unicode顺序；
// 6、“Visuable”关键字通常用来表示显示（可视）顺序；
////////////////////////////////////////////////////////////////////////////////
#ifndef __SYUNISCRIBE_H__
#define __SYUNISCRIBE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Usp10.h>
#include <string>
#include <vector>
#include "DataRender\DataRenderDataDef.h"
//////////////////////////////////////////////////////////////////////////
using std::string;
using std::vector;
using std::allocator;
using std::wstring;
typedef unsigned long long	mtusp_code;
#define Common_USE_UNISCRIBE	//使用多语言处理器
//消除C4251警告
template class  allocator<BYTE>;
template class  vector<BYTE, allocator<BYTE>>;
template class  allocator<int>;
template class  vector<int, allocator<int>>;
template class  allocator<WORD>;
template class  vector<WORD, allocator<WORD>>;
template class  allocator<mtusp_code>;
template class  vector<mtusp_code, allocator<mtusp_code>>;
template class  allocator<wstring>;
template class  vector<wstring, allocator<wstring>>;
template class  allocator<SCRIPT_ITEM>;
template class  vector<SCRIPT_ITEM, allocator<SCRIPT_ITEM>>;
template class  allocator<SCRIPT_VISATTR>;
template class  vector<SCRIPT_VISATTR, allocator<SCRIPT_VISATTR>>;
template class  allocator<SCRIPT_LOGATTR>;
template class  vector<SCRIPT_LOGATTR, allocator<SCRIPT_LOGATTR>>;
template class  allocator<GOFFSET>;
template class  vector<GOFFSET, allocator<GOFFSET>>;

typedef vector<BYTE>			CBaseUspByteArray;
typedef vector<int>				CBaseUspIntArray;
typedef vector<WORD>			CBaseUspWordArray;
typedef vector<mtusp_code>		CBaseUspCheckCodeArray;
typedef vector<wstring>			CBaseUspStrArray;
typedef vector<SCRIPT_ITEM>		CBaseUspItemArray;
typedef vector<SCRIPT_VISATTR>	CBaseUspVisAttrArray;
typedef vector<SCRIPT_LOGATTR>	CBaseUspLogAttrArray;
typedef vector<GOFFSET>			CBaseUspGOffsetArray;
//////////////////////////////////////////////////////////////////////////
// tagSYSCRIPTRUN - Run单元
struct  tagSYSCRIPTRUN
{
	int				m_nRunCharPos;	// char index in paragraph
	int				m_nRunCharLen;
	SCRIPT_ANALYSIS	m_RunAnalysis;
	TCHAR			m_szFontName[LF_FACESIZE];//font name used actually
	SCRIPT_CACHE	m_ScriptCache;
	CBaseUspIntArray	m_aryCharIndex;		// char index in paragraph
	CBaseUspWordArray	m_aryCharGlyphs;		// glyph shaped information
	CBaseUspWordArray	m_aryLogicalCluster;	// logical cluster information
	CBaseUspWordArray	m_aryNoGlyphIndex;
	CBaseUspVisAttrArray	m_aryVisAttribute;// glyph shaped visual attribute

	CBaseUspIntArray		m_aryAdvanceWidth;		// advance width
	CBaseUspGOffsetArray	m_aryGOffset;			// glyph offset
	CBaseUspCheckCodeArray	m_aryCheckCode;		// ckeck code

	tagSYSCRIPTRUN() : m_nRunCharPos(0), m_nRunCharLen(0), m_ScriptCache(NULL)
	{
	}
	~tagSYSCRIPTRUN()
	{
		if (m_ScriptCache != NULL)
			::ScriptFreeCache(&m_ScriptCache);
	}

	tagSYSCRIPTRUN(const tagSYSCRIPTRUN& other)
	{
		*this = other;
	}

	tagSYSCRIPTRUN& operator =(const tagSYSCRIPTRUN& other)
	{
		if (this == &other)
			return *this;

		m_nRunCharPos	= other.m_nRunCharPos;
		m_nRunCharLen	= other.m_nRunCharLen;
		m_RunAnalysis	= other.m_RunAnalysis;
		memcpy(m_szFontName, other.m_szFontName, sizeof(TCHAR) * LF_FACESIZE);
		m_ScriptCache	= other.m_ScriptCache;

		m_aryCharIndex.assign(other.m_aryCharIndex.begin(), other.m_aryCharIndex.end());
		m_aryCharGlyphs.assign(other.m_aryCharGlyphs.begin(), other.m_aryCharGlyphs.end());
		m_aryLogicalCluster.assign(other.m_aryLogicalCluster.begin(), other.m_aryLogicalCluster.end());
		m_aryNoGlyphIndex.assign(other.m_aryNoGlyphIndex.begin(), other.m_aryNoGlyphIndex.end());
		m_aryVisAttribute.assign(other.m_aryVisAttribute.begin(), other.m_aryVisAttribute.end());
		m_aryAdvanceWidth.assign(other.m_aryAdvanceWidth.begin(), other.m_aryAdvanceWidth.end());
		m_aryGOffset.assign(other.m_aryGOffset.begin(), other.m_aryGOffset.end());
		return *this;
	}
};
typedef tagSYSCRIPTRUN SYSCRIPTRUN;
template class  allocator<SYSCRIPTRUN*>;
template class  vector<SYSCRIPTRUN*, allocator<SYSCRIPTRUN*>>;
typedef vector<SYSCRIPTRUN*>	CBaseUspRunPtrArray;
//////////////////////////////////////////////////////////////////////////
// tagSYSCRIPTRESULT - 数据处理结果
struct  tagSYSCRIPTRESULT
{
	// Item 临时数据
	CBaseUspItemArray		m_aryScriptItem;
	// 结果数据
	CBaseUspRunPtrArray	m_aryRunPtr;			// logical run ptr
	CBaseUspIntArray		m_aryVis2Log;			// visual order run indexes
	CBaseUspIntArray		m_aryLog2Vis;			// visual run positions
	CBaseUspLogAttrArray	m_aryLogAttr;
	tagSYSCRIPTRESULT()
	{
	}
	~tagSYSCRIPTRESULT()
	{
		int nRunCount = (int)m_aryRunPtr.size();
		for (int i = 0; i < nRunCount; ++ i)
			delete m_aryRunPtr[i];
	}
	tagSYSCRIPTRESULT(const tagSYSCRIPTRESULT& other)
	{
		*this = other;
	}
	tagSYSCRIPTRESULT& operator =(const tagSYSCRIPTRESULT& other)
	{
		if (this == &other)
			return *this;
		m_aryRunPtr.assign(other.m_aryRunPtr.begin(), other.m_aryRunPtr.end());
		m_aryVis2Log.assign(other.m_aryVis2Log.begin(), other.m_aryVis2Log.end());
		m_aryLog2Vis.assign(other.m_aryLog2Vis.begin(), other.m_aryLog2Vis.end());
		m_aryLogAttr.assign(other.m_aryLogAttr.begin(), other.m_aryLogAttr.end());
		return *this;
	}
	void Empty()
	{
		int nRunCount = (int)m_aryRunPtr.size();
		for (int i = 0; i < nRunCount; ++ i)
			delete m_aryRunPtr[i];

		m_aryRunPtr.clear();
		m_aryVis2Log.clear();
		m_aryLog2Vis.clear();
		m_aryLogAttr.clear();
	}
};
typedef tagSYSCRIPTRESULT SYSCRIPTRESULT, *LPCommonSCRIPTRESULT;
//////////////////////////////////////////////////////////////////////////
// CBaseUsp - 多语言处理器
class  CBaseUsp
{
public:
	CBaseUsp(SYSCRIPTRESULT* pScriptResult);
	virtual ~CBaseUsp();
	//Initialize
	static BOOL				Initialize(LCID dwLcid = LOCALE_USER_DEFAULT, BOOL bEnFeedBackFont = TRUE);
	// Call it while init or response the WM_SETTINGCHANGE msg.
	static BOOL				RefreshDigitSubstitution(LCID dwLcid = LOCALE_USER_DEFAULT);
	CBaseUspStrArray*			GetFeedBackFontFonts();

	const SCRIPT_PROPERTIES* GetScriptProperty(IN WORD wScript);
	int						GetScriptMaxCount(){ return m_nMaxScript; }

	const SCRIPT_CONTROL*	GetScriptControl(){ return &m_scScriptControl; }
	const SCRIPT_STATE*		GetScriptState(){ return &m_ssScriptState; }

	void					SetNeedCheckCode(BOOL bNeed) { m_bGenCheckCode = bNeed; }
	BOOL					IsNeedCheckCode() { return m_bGenCheckCode; }
	// Attach result data, return old data.
	SYSCRIPTRESULT*	Attach(IN	SYSCRIPTRESULT*	pScriptResult);
	BOOL			Analyse(IN	WCHAR*	pwcChars,IN	int	nCharStart, IN	int	nChars, IN	void*	pTextProp,IN	BOOL		bLToR);
	// Return : succeeds - S_OK, fails - E_INVALIDARG or other error
	HRESULT			ScriptItemize(IN	WCHAR*	pwcChars, IN	int	nChars);
	// Return : succeeds - S_OK, fails - E_HANDLE, S_FALSE
	HRESULT			ScriptGetCMap(IN	HDC	hDC, IN OUT	SYSCRIPTRUN*	psrRun, IN	WCHAR*	pwcChars,IN	int	nChars);
	// Return : succeeds - S_OK, fails - other error
	HRESULT			ScriptShape(IN	HDC	hDC, IN OUT	SYSCRIPTRUN*	psrRun, IN	WCHAR*	pwcChars, IN	int	nChars, OUT	int*	pnGlyphs);
	static int		TranslateGlyphToChar(IN	SYSCRIPTRUN*	psrRun, IN	int	nGlyph);
	//  Determines whether a Unicode string requires complex script processing
	static BOOL		ScriptIsComplex(IN	const WCHAR	*pwcChars, IN	int	nCharStart,IN	int	nChars);
	// Translate char position to glyph X position
	// Second parameter : nCharXPos >= 0 && nCharXPos <= char count in line
	// Return : nGlyphXPos >= 0 && nGlyphXPos <= glyph count in line
	static int		ScriptCPToGX(IN	SYSCRIPTRESULT&	usResult, IN	int	nCharXPos, IN	int	nRunIndex = -1);
	// Translate glyph X position to char position
	// Second parameter : nGlyphXPos >= 0 && nGlyphXPos <= glyph count in line
	// Return : nCharXPos >= 0 && nCharXPos <= char count in line
	static int		ScriptGXToCP(IN	SYSCRIPTRESULT&	usResult, IN	int	nGlyphXPos, IN	int	nRunIndex = -1);
	// Second parameter : nGlyphPos >= 0 && nGlyphPos < glyph count in line
	// Return : nCharIndex >= 0 && nCharIndex < char count in line
	static int		ScriptGPToCI(IN	SYSCRIPTRESULT&	usResult, IN	int	nGlyphPos);
	// Second parameter : nGlyph >= 0 && nGlyph < glyph count in line
	// Return : nRunIndex >= 0 && nRunIndex < run count in line
	static int		ScriptGetRunByGlyph(IN	SYSCRIPTRESULT&	usResult, IN	int	nGlyph);
	// Second parameter : nChar >= 0 && nChar < char count in line
	// Return : nRunIndex >= 0 && nRunIndex < run count in line
	static int		ScriptGetRunByChar(IN	SYSCRIPTRESULT&	usResult, IN	int	nChar);
protected:
	void			ProduceRunsWithItems(IN	WCHAR*	pwcChars, IN	int	nCharStart, IN	void*	pTextProp, IN	 BOOL	bTreatColor);
	LPCTSTR			GetWordTextsFontName(IN	TCHAR	tch, IN	LPCTSTR	lpszFontName, IN	LPCTSTR	lpszFontNameEng);
	HRESULT			GenerateClusterGlyph(IN	WCHAR*	pwcChars,IN	int	nCharStart, IN	SYFont*	pFont, IN	HDC	hDC, IN OUT	SYSCRIPTRUN*	psrRun);
protected:
	// Array of pointers to properties.
	static const SCRIPT_PROPERTIES**	m_ppScriptProperties;
	static int							m_nMaxScript;		// Max Script count
	static SCRIPT_DIGITSUBSTITUTE		m_sdsDigitSubstitute;
	static SCRIPT_CONTROL				m_scScriptControlTemplate;
	static SCRIPT_STATE					m_ssScriptStateTemplate;
	SCRIPT_CONTROL						m_scScriptControl;
	SCRIPT_STATE						m_ssScriptState;
	BOOL								m_bGenCheckCode;	// Need check code
	//数组处理结果必须由外部指定
	SYSCRIPTRESULT*						m_pResult;
};

////////////////////////////////////////////////////////////////////////////////
// 中英文后置标点定义
#define SYPOSTPUNCTUATION	_T("!),.:;?]}¨·ˇˉ―‖’”…∶、。〃々〉》」』】〕〗！＂＇），．：；？］｀｜｝～￠︶︸︺︼︾﹀﹂﹄﹐﹑﹒﹔﹕﹖﹗﹚﹜﹞")
// 中英文前置标点定义
#define SYPREVPUNCTUATION	_T("([{·‘“〈《「『【〔〖（．［｛￡￥︵︷︹︻︽︿﹁﹃﹙﹛﹝")

////////////////////////////////////////////////////////////////////////////////
#endif	// !define(__SYUNISCRIBE_H__)
