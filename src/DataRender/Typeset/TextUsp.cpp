#include "StdAfx.h"
#include "TextUsp.h"
#include "DataRender\TextDrawer.h"
#include <algorithm>
#include <functional>
using namespace std;

#pragma comment( lib, "usp10.lib")
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
////////////////////////////////////////////////////////////////////////////////
//前置声明
void		SYFont_Init(LPSYFont pFont);
void		SYText_GetFontProperty(void* pTextProp, LPSYFont pFont, int nIndex);
unsigned long	 SYCRC32Cal(unsigned long ulCRC32, unsigned char *pBuf, unsigned long ulLen);
//////////////////////////////////////////////////////////////////////////
// class CSYUsp
// 不同操作系统字体回退硬编码
LPCTSTR		g_uspFeedBackFontNameDef[] = 
{
	//常用字体
	L"Arial",
	L"宋体",
	L"Microsoft Sans Serif",
	L"Tahoma",
	L"Times New Roman",
	//Vista及以上
	L"Euphemia",				//加拿大音节，北美，包括印纽特语和克里语；
	L"Mongolian Baiti",		//蒙古语，北亚；
	L"Microsoft Himalaya",		//西藏语，中亚；
	L"Plantagenet",			//切罗基语，北美；
	L"Nyala",				//埃塞俄比亚语，非洲，用于阿姆哈拉语和其他语言；
	L"DaunPenh",				//高棉语，东南亚；
	L"MoolBoran",				//...............
	L"DokChampa",				//老挝语，东南亚；
	L"Kalinga",					//奥里雅语，南亚；
	L"Iskoola Pota",			//僧伽罗语，南亚；
	L"Microsoft Yi Baiti",		//彝语，中国；

	//XP SP2
	L"Vrinda",				//孟加拉语，南亚，用于阿萨姆语以及孟加拉语；
	L"Kartika",				//马拉雅拉姆语，南亚；

	//XP
	L"Gautami",				//泰卢固语，南亚；
	L"Tunga",				//埃纳德语，南亚；
	L"Shruti",				//古吉拉特语，南亚；
	L"Raavi",				//旁遮普语，南亚，用于印度的旁遮普语；
	L"Estrangelo Edessa",		//叙利亚语，中东；
	L"MV Boli",				//马尔代夫语，南亚，用于迪贝喜语；

	//Windows 2000
	L"Sylfaen",				//亚美尼亚语，格鲁吉亚语，欧亚，包括北印度语、马拉地语、梵语；
	L"Mangal",				//梵文，南亚；
	L"Latha",				//泰米尔语，南亚；
};
//字体回退列表
CSYUspStrArray			g_uspFeedBackFontName;
//是否允许字体回退功能
BOOL						g_uspEnableFeedBackFont = TRUE;
const SCRIPT_PROPERTIES**	CSYUsp::m_ppScriptProperties = NULL;
int						CSYUsp::m_nMaxScript = 0;
SCRIPT_DIGITSUBSTITUTE		CSYUsp::m_sdsDigitSubstitute;
SCRIPT_CONTROL			CSYUsp::m_scScriptControlTemplate;
SCRIPT_STATE				CSYUsp::m_ssScriptStateTemplate;
CSYUsp::CSYUsp(SYSCRIPTRESULT* pScriptResult)
{
	m_bGenCheckCode = TRUE;
	m_pResult = pScriptResult;
}

CSYUsp::~CSYUsp()
{

}
BOOL CSYUsp::Initialize(LCID dwLcid, BOOL bEnFeedBackFont)
{
	//是否允许字体回退功能
	g_uspEnableFeedBackFont = bEnFeedBackFont;

	//回退字体列表
	g_uspFeedBackFontName.clear();

	wstring strFont;
	//字体回退硬编码
	int nFeedBackCnt = sizeof(g_uspFeedBackFontNameDef) / sizeof(LPCTSTR);
	for (int i = 0; i < nFeedBackCnt; ++i)
	{
		strFont = g_uspFeedBackFontNameDef[i];
		g_uspFeedBackFontName.push_back(strFont);
	}

	strFont = GetDefaultFontFaceName();
	g_uspFeedBackFontName.push_back(strFont);

	HFONT hDefaultGUIFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lfDefaultGUI = {0};
	if (::GetObject(hDefaultGUIFont, sizeof(LOGFONT), &lfDefaultGUI))
	{
		strFont = lfDefaultGUI.lfFaceName;
		g_uspFeedBackFontName.push_back(strFont);
	}

	//语言区
	return RefreshDigitSubstitution(dwLcid);
}

BOOL CSYUsp::RefreshDigitSubstitution(LCID dwLcid)
{
	HRESULT hr = ::ScriptRecordDigitSubstitution(
		dwLcid, 
		&m_sdsDigitSubstitute
		);
	if (hr != S_OK)
		return FALSE;

	hr = ::ScriptApplyDigitSubstitution(
		&m_sdsDigitSubstitute, 
		&m_scScriptControlTemplate, 
		&m_ssScriptStateTemplate
		);
	return hr == S_OK;
}

CSYUspStrArray* CSYUsp::GetFeedBackFontFonts()
{
	return &g_uspFeedBackFontName;
}

const SCRIPT_PROPERTIES* CSYUsp::GetScriptProperty(WORD wScript)
{
	if (m_ppScriptProperties == NULL && m_nMaxScript == 0)
	{
		HRESULT hr = ::ScriptGetProperties(&m_ppScriptProperties, &m_nMaxScript);
		if (hr != 0)
		{
			m_nMaxScript = -1;
			return NULL;
		}
	}

	if (m_nMaxScript <= 0 || wScript >= (UINT)m_nMaxScript)
		return NULL;

	return m_ppScriptProperties != NULL ? m_ppScriptProperties[wScript] : NULL;
}

SYSCRIPTRESULT* CSYUsp::Attach(SYSCRIPTRESULT* pScriptResult)
{
	SYSCRIPTRESULT* pOld = m_pResult;
	m_pResult = pScriptResult;
	return pOld;
}
BOOL CSYUsp::Analyse(WCHAR* pwcChars,int nCharStart, int nCharsCnt, void* pTextProp,BOOL bLToR)
{
	if (m_pResult == NULL)
		return FALSE;

	//// 清空结果
	m_pResult->Empty();

	memcpy(&m_scScriptControl, &m_scScriptControlTemplate, sizeof(SCRIPT_CONTROL));
	memcpy(&m_ssScriptState, &m_ssScriptStateTemplate, sizeof(SCRIPT_STATE));
	m_ssScriptState.uBidiLevel = bLToR ? 0 : 1;		// 0: LTR; 1:RTL.

	HRESULT hr;
	//// 生成Item
	CSYUspItemArray& aryItem = m_pResult->m_aryScriptItem;
	hr = ScriptItemize(pwcChars + nCharStart, nCharsCnt);
	if (hr != S_OK || aryItem.size() <= 1)
	{
		return FALSE;
	}

	//// 将Item细分成Run
	ProduceRunsWithItems(pwcChars, nCharStart, pTextProp, FALSE);

	HWND hDesktopWnd = ::GetDesktopWindow();
	HDC hDC = ::GetDC(hDesktopWnd);

	CSYUspRunPtrArray& aryRun = m_pResult->m_aryRunPtr;
	int nRunCnt = (int)aryRun.size();

	CSYUspByteArray aryLevel;
	aryLevel.resize(nRunCnt);

	CSYUspStrArray* parFeedBackFont = nullptr;
	int nFeedBackFontCnt = 0;
	if (g_uspEnableFeedBackFont)
	{
		parFeedBackFont = GetFeedBackFontFonts();
		nFeedBackFontCnt = (int)parFeedBackFont->size();
	}

	SYFont objFont;
	SYFont_Init(&objFont);
	for (int nRunIndex = 0; nRunIndex < nRunCnt; ++nRunIndex)
	{
		SYSCRIPTRUN* psrRun = aryRun[nRunIndex];
		aryLevel[nRunIndex] = (BYTE)psrRun->m_RunAnalysis.s.uBidiLevel;

		//在完整字符串中的位置
		int nRealCharPos = psrRun->m_nRunCharPos + nCharStart;
		SYText_GetFontProperty(pTextProp, &objFont, nRealCharPos);

		WCHAR chStart = pwcChars[nRealCharPos];
		HFONT hRunFont = NULL, hOldFont = NULL;
		hRunFont = SYFont_CreateFont(&objFont, chStart, NULL, TRUE);
		if (hRunFont != NULL)
			hOldFont = (HFONT)::SelectObject(hDC, hRunFont);

		HRESULT hr = ScriptGetCMap(
			hDC, 
			psrRun, 
			pwcChars + nRealCharPos, 
			psrRun->m_nRunCharLen
			);
		int i = 0;
		for (i = 0; hr == S_FALSE && i < nFeedBackFontCnt; ++i)
		{
			if (hOldFont != NULL)
			{
				::SelectObject(hDC, hOldFont);
				hOldFont = NULL;
			}
			if (hRunFont != NULL)
				::DeleteObject(hRunFont);

			hRunFont = SYFont_CreateFont(&objFont, chStart, (*parFeedBackFont)[i].c_str(), TRUE);
			if (hRunFont != NULL)
				hOldFont = (HFONT)::SelectObject(hDC, hRunFont);
			if (psrRun->m_ScriptCache != NULL)
			{
				::ScriptFreeCache(&psrRun->m_ScriptCache);
				psrRun->m_ScriptCache = NULL;
			}

			hr = ScriptGetCMap(
				hDC, 
				psrRun, 
				pwcChars + nRealCharPos, 
				psrRun->m_nRunCharLen
				);
			if (hr != S_FALSE)
			{
				const TCHAR* pFBFont = (*parFeedBackFont)[i].c_str();
				int nFBLen = (int)(_tcslen(pFBFont) + 1);
				memcpy(psrRun->m_szFontName, pFBFont, sizeof(TCHAR) * nFBLen);
			}
		}

		if (hr == FALSE && i == nFeedBackFontCnt && i > 0)
		{
			if (hOldFont != NULL)
			{
				::SelectObject(hDC, hOldFont);
				hOldFont = NULL;
			}
			if (hRunFont != NULL)
				::DeleteObject(hRunFont);

			hRunFont = SYFont_CreateFont(&objFont, chStart, psrRun->m_szFontName, TRUE);
			if (hRunFont != NULL)
				hOldFont = (HFONT)::SelectObject(hDC, hRunFont);
			if (psrRun->m_ScriptCache != NULL)
			{
				::ScriptFreeCache(&psrRun->m_ScriptCache);
				psrRun->m_ScriptCache = NULL;
			}
		}

		GenerateClusterGlyph(pwcChars, nCharStart, &objFont, hDC, psrRun);

		if (hOldFont != NULL)
			::SelectObject(hDC, hOldFont);
		if (hRunFont != NULL)
			::DeleteObject(hRunFont);
	}

	//// 获得字形布局参数
	m_pResult->m_aryVis2Log.resize(nRunCnt);
	m_pResult->m_aryLog2Vis.resize(nRunCnt);
	hr = ::ScriptLayout(
		nRunCnt,
		&aryLevel[0],
		&m_pResult->m_aryVis2Log[0], 
		&m_pResult->m_aryLog2Vis[0]
		);

	if (hr != S_OK)
	{
		m_pResult->m_aryVis2Log.clear();
		m_pResult->m_aryLog2Vis.clear();
	}

	//// 格式化信息
	CSYUspLogAttrArray& aryLogAttr = m_pResult->m_aryLogAttr;
	aryLogAttr.resize(nCharsCnt);
	int nItemCount = (int)aryItem.size();
	for (int nItemIdx=0; nItemIdx < nItemCount-1; ++nItemIdx)
	{
		SCRIPT_ITEM* pScriptItem = &(aryItem[nItemIdx]);
		::ScriptBreak(
			pwcChars + pScriptItem->iCharPos + nCharStart, 
			aryItem[nItemIdx + 1].iCharPos - pScriptItem->iCharPos, 
			&pScriptItem->a, 
			&aryLogAttr[0] + pScriptItem->iCharPos
			);
	}

	if (hDC != NULL)
		::ReleaseDC(hDesktopWnd, hDC);
	return TRUE;
}

HRESULT CSYUsp::ScriptItemize(WCHAR* pwcChars, 
							  int nChars)
{
	CSYUspItemArray& aryItem = m_pResult->m_aryScriptItem;
	if (aryItem.size() <= 2)
		aryItem.resize(2);

	int nItems = 0;
	HRESULT hr = ::ScriptItemize(
		pwcChars, 
		nChars, 
		(int)aryItem.size(), 
		&m_scScriptControl, 
		&m_ssScriptState, 
		&aryItem[0], 
		&nItems
		);
	while (hr == E_OUTOFMEMORY && aryItem.size() <= nChars)
	{
		aryItem.resize(max(aryItem.size() * 2, nItems + 1));
		hr = ::ScriptItemize(
			pwcChars, 
			nChars, 
			(int)aryItem.size(), 
			&m_scScriptControl, 
			&m_ssScriptState, 
			&aryItem[0], 
			&nItems
			);
	}
	if (hr == S_OK)
	{
		aryItem.resize(nItems + 1);
	}
	else
	{
		aryItem.clear();
	}
	return hr;
}

HRESULT CSYUsp::ScriptGetCMap(HDC hDC, SYSCRIPTRUN* psrRun, WCHAR *pwcChars, int nChars)
{
	SCRIPT_FONTPROPERTIES sfp;
	ZeroMemory(&sfp, sizeof(SCRIPT_FONTPROPERTIES));
	sfp.cBytes = sizeof(SCRIPT_FONTPROPERTIES);
	::ScriptGetFontProperties(hDC, &psrRun->m_ScriptCache, &sfp);

	CSYUspWordArray& aryGlyphs = psrRun->m_aryCharGlyphs;
	aryGlyphs.resize(nChars);
	for (int i = 0; i < nChars; ++i)
		aryGlyphs[i] = sfp.wgDefault;

	HRESULT hr = ::ScriptGetCMap(
		hDC, 
		&psrRun->m_ScriptCache, 
		pwcChars, 
		nChars, 
		0, 
		&aryGlyphs[0]
		);
	if (hr == S_OK)
	{
		return hr;
	}
	else
	{
		int nMissing = 0;
		int nGlyphCnt = (int)aryGlyphs.size();
		for (int i = 0; i < nGlyphCnt; ++i)
		{
			DWORD dwCharGlyph = aryGlyphs[i];
			if (dwCharGlyph == sfp.wgDefault
				|| dwCharGlyph == sfp.wgInvalid)
				++ nMissing;
		}
		if (nMissing < nChars / 2)
			return S_OK;
		aryGlyphs.clear();
	}
	return hr;
}

HRESULT CSYUsp::ScriptShape(HDC hDC, SYSCRIPTRUN* psrRun, WCHAR *pwcChars, int nChars, int* pnGlyphs)
{
	CSYUspWordArray& aryGlyphs = psrRun->m_aryCharGlyphs;
	CSYUspWordArray& aryLogCluster = psrRun->m_aryLogicalCluster;
	CSYUspVisAttrArray& aryVisAttr = psrRun->m_aryVisAttribute;

	int nTryCount = (int)(nChars * 1.5 + 16), nTryInc = nChars / 2;
	if (aryGlyphs.size() < nTryCount)
		aryGlyphs.resize(nTryCount);

	aryLogCluster.resize(nChars);
	if (aryVisAttr.size() < nTryCount)
		aryVisAttr.resize(nTryCount);

	int nGlyphs = 0;
	HRESULT hr = ::ScriptShape(
		hDC, 
		&psrRun->m_ScriptCache, 
		pwcChars, 
		nChars, 
		(int)aryGlyphs.size(), 
		&psrRun->m_RunAnalysis, 
		&aryGlyphs[0], 
		&aryLogCluster[0], 
		&aryVisAttr[0], 
		&nGlyphs
		);
	for (nTryCount += nTryInc; hr == E_OUTOFMEMORY; nTryCount += nTryInc)
	{
		if (aryGlyphs.size() < nTryCount)
			aryGlyphs.resize(nTryCount);
		if (aryVisAttr.size() < nTryCount)
			aryVisAttr.resize(nTryCount);
		hr = ::ScriptShape(
			hDC, 
			&psrRun->m_ScriptCache, 
			pwcChars, 
			nChars, 
			(int)aryGlyphs.size(), 
			&psrRun->m_RunAnalysis, 
			&aryGlyphs[0], 
			&aryLogCluster[0], 
			&aryVisAttr[0], 
			&nGlyphs
			);
	}

	if (hr == S_OK)
	{
		if (!psrRun->m_RunAnalysis.fNoGlyphIndex)
		{
			aryGlyphs.resize(nGlyphs);
			aryVisAttr.resize(nGlyphs);
		}

		psrRun->m_aryAdvanceWidth.resize(nGlyphs);
		psrRun->m_aryGOffset.resize(nGlyphs);
		ABC objRunABC;
		hr = ::ScriptPlace(
			hDC, 
			&psrRun->m_ScriptCache, 
			&aryGlyphs[0], 
			nGlyphs, 
			&aryVisAttr[0], 
			&psrRun->m_RunAnalysis, 
			&psrRun->m_aryAdvanceWidth[0], 
			&psrRun->m_aryGOffset[0], 
			&objRunABC
			);

		if (pnGlyphs != NULL)
			*pnGlyphs = (int)aryGlyphs.size();
	}
	else
	{
		aryLogCluster.clear();
		aryGlyphs.clear();
		aryVisAttr.clear();
		psrRun->m_aryAdvanceWidth.clear();
		psrRun->m_aryGOffset.clear();
	}

	return hr;
}

int	CSYUsp::TranslateGlyphToChar(SYSCRIPTRUN* psrRun, int nGlyph)
{
	int nCharCount = (int)psrRun->m_aryLogicalCluster.size();
	int nMin = 0;

	if (!psrRun->m_RunAnalysis.fRTL)
	{
		WORD* pFirst = &psrRun->m_aryLogicalCluster[0];
		WORD* pLast = pFirst + psrRun->m_aryLogicalCluster.size();
		WORD* pLower = lower_bound(pFirst, pLast, nGlyph);
		WORD* pUpper = upper_bound(pFirst, pLast, nGlyph);


		if (pLower == pUpper && pUpper > pFirst)
		{
			-- pLower;
			while (pLower > pFirst && *(pLower - 1) == *pLower)
				-- pLower;
		}

		nMin = (int)(min(pLower - pFirst + nGlyph, pUpper - pFirst - 1));
	}
	else
	{
		WORD* pFirst = &psrRun->m_aryLogicalCluster[0];
		WORD* pLast = pFirst + psrRun->m_aryLogicalCluster.size();
		WORD* pLower = lower_bound(pFirst, pLast, nGlyph, greater<WORD>());
		WORD* pUpper = upper_bound(pFirst, pLast, nGlyph, greater<WORD>());

		if (pLower == pUpper)
		{
			while (pUpper < pLast && *pUpper == *pLower)
				++ pUpper;
		}

		nMin = (int)(min(pLower - pFirst + nGlyph, pUpper - pFirst - 1));
	}
	return max(nMin, 0) + psrRun->m_nRunCharPos;
}

BOOL CSYUsp::ScriptIsComplex(const WCHAR *pwcChars, 
							 int nCharStart,
							 int nChars)
{
	HRESULT hr = ::ScriptIsComplex(pwcChars + nCharStart, 
		nChars, 
		SIC_COMPLEX | SIC_ASCIIDIGIT | SIC_NEUTRAL);
	return hr == S_OK;
}

int	CSYUsp::ScriptCPToGX(SYSCRIPTRESULT& usResult, 
						 int nCharXPos, 
						 int nRunIndex/* = -1*/)
{
	int nRunCount = (int)usResult.m_aryRunPtr.size();
	if (nRunCount == 0)
		return 0;

	if (nRunIndex >= 0 && nRunIndex < nRunCount)	// 检查 nRunIndex 是否有效
	{
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunIndex];
		if (nCharXPos < psrRunInfo->m_nRunCharPos || 
			nCharXPos > psrRunInfo->m_nRunCharPos + psrRunInfo->m_nRunCharLen)
			nRunIndex = -1;
	}

	if (nRunIndex < 0 || nRunIndex >= nRunCount)	// 查找 nCharXPos 所在 Run
	{
		for (nRunIndex = 0; nRunIndex < nRunCount; ++nRunIndex)
		{
			SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunIndex];
			if (nCharXPos >= psrRunInfo->m_nRunCharPos && 
				nCharXPos <= psrRunInfo->m_nRunCharPos + psrRunInfo->m_nRunCharLen)
				break;
		}
	}

	int nGlyphPos = 0;
	if (nRunIndex >= 0 && nRunIndex < nRunCount)
	{
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunIndex];

		nCharXPos -= psrRunInfo->m_nRunCharPos;
		if (psrRunInfo->m_RunAnalysis.fRTL)
		{
			if (nCharXPos < psrRunInfo->m_nRunCharLen)
				nGlyphPos = psrRunInfo->m_aryLogicalCluster[nCharXPos] + 1;
		}
		else
		{
			if (nCharXPos < psrRunInfo->m_nRunCharLen)
				nGlyphPos = psrRunInfo->m_aryLogicalCluster[nCharXPos];
			else
				nGlyphPos = (int)psrRunInfo->m_aryCharIndex.size();
		}

		nRunCount = usResult.m_aryLog2Vis[nRunIndex];		// logical -> visual
	}
	else
	{
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunCount - 1];
		if (!psrRunInfo->m_RunAnalysis.fRTL)
			nGlyphPos = (int)psrRunInfo->m_aryCharIndex.size();

		nRunCount = usResult.m_aryLog2Vis[nRunCount - 1];	// logical -> visual
	}
	for (int n = 0; n < nRunCount; ++n)
	{
		nRunIndex = usResult.m_aryVis2Log[n];				// visual -> logical
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunIndex];
		nGlyphPos += (int)psrRunInfo->m_aryCharIndex.size();
	}
	return nGlyphPos;
}

int	CSYUsp::ScriptGXToCP(SYSCRIPTRESULT& usResult, 
						 int nGlyphXPos, 
						 int nRunIndex/* = -1*/)
{
	int nRunCount = (int)usResult.m_aryRunPtr.size();
	if (nRunCount == 0)
		return 0;

	int nVisRunIndex = 0;
	int nGlyphCount  = 0;
	for (; nVisRunIndex < nRunCount; ++nVisRunIndex)
	{
		int nLogRunIndex = usResult.m_aryVis2Log[nVisRunIndex];
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nLogRunIndex];
		if (nLogRunIndex == nRunIndex)
		{
			if (nGlyphXPos >= nGlyphCount && nGlyphXPos <= nGlyphCount + 
				psrRunInfo->m_aryCharIndex.size())
				break;
		}
		else
		{
			if (nGlyphXPos >= nGlyphCount && nGlyphXPos < nGlyphCount + 
				psrRunInfo->m_aryCharIndex.size())
				break;
		}
		nGlyphCount += (int)psrRunInfo->m_aryCharIndex.size();
	}

	nGlyphXPos -= nGlyphCount;

	if (nVisRunIndex < nRunCount)
	{
		int nLogRunIndex = usResult.m_aryVis2Log[nVisRunIndex];
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nLogRunIndex];

		int n = 0;
		int nCharCount = (int)psrRunInfo->m_aryLogicalCluster.size();
		if (!psrRunInfo->m_RunAnalysis.fRTL)
		{
			for (; n < nCharCount; ++n)
			{
				if (nGlyphXPos <= psrRunInfo->m_aryLogicalCluster[n])
					break;
			}
		}
		else
		{
			for (; n < nCharCount; ++n)
			{
				if (nGlyphXPos > psrRunInfo->m_aryLogicalCluster[n])
					break;
			}
		}
		return psrRunInfo->m_nRunCharPos + n;
	}
	else
	{
		int nLogRunIndex = usResult.m_aryVis2Log[nRunCount - 1];
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nLogRunIndex];
		if (psrRunInfo->m_RunAnalysis.fRTL)
			return psrRunInfo->m_nRunCharPos;
		else
			return psrRunInfo->m_nRunCharPos + psrRunInfo->m_nRunCharLen;
	}
}

int CSYUsp::ScriptGPToCI(SYSCRIPTRESULT& usResult, int nGlyphPos)
{
	int nRunCount = (int)usResult.m_aryRunPtr.size();
	int i = 0;
	int nGlyphCount = 0;
	for (; i < nRunCount; ++i)
	{
		int nRunIndex = usResult.m_aryVis2Log[i];
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunIndex];
		if (nGlyphPos >= nGlyphCount && nGlyphPos < nGlyphCount + 
			psrRunInfo->m_aryCharIndex.size())
			break;
		nGlyphCount += (int)psrRunInfo->m_aryCharIndex.size();
	}

	nGlyphPos -= nGlyphCount;

	int nRunIndex = usResult.m_aryVis2Log[i];
	return TranslateGlyphToChar(usResult.m_aryRunPtr[nRunIndex], nGlyphPos);
}

int CSYUsp::ScriptGetRunByGlyph(SYSCRIPTRESULT& usResult, int nGlyph)
{
	int nRunCount = (int)usResult.m_aryRunPtr.size();
	for (int i = 0, nGlyphCount = 0; i < nRunCount; ++i)
	{
		int nRunIndex = usResult.m_aryVis2Log[i];
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[nRunIndex];
		if (nGlyph >= nGlyphCount && 
			nGlyph < nGlyphCount + psrRunInfo->m_aryCharIndex.size())
			return nRunIndex;
		nGlyphCount += (int)psrRunInfo->m_aryCharIndex.size();
	}
	return nRunCount;
}

int CSYUsp::ScriptGetRunByChar(SYSCRIPTRESULT& usResult, int nChar)
{
	int nRunCount = (int)usResult.m_aryRunPtr.size();
	for (int i = 0; i < nRunCount; ++i)
	{
		SYSCRIPTRUN* psrRunInfo = usResult.m_aryRunPtr[i];
		if (nChar >= psrRunInfo->m_nRunCharPos && 
			nChar < psrRunInfo->m_nRunCharPos + psrRunInfo->m_nRunCharLen)
			return i;
	}
	return nRunCount;
}

void CSYUsp::ProduceRunsWithItems(WCHAR* pwcChars, 
								  int nCharStart, 
								  void* pTextProp, 
								  BOOL bTreatColor)
{
	SYFont objStartFont;
	SYFont objCurFont;
	SYFont_Init(&objStartFont);
	SYFont_Init(&objCurFont);

	CSYUspItemArray& aryItem = m_pResult->m_aryScriptItem;
	CSYUspRunPtrArray& aryRun = m_pResult->m_aryRunPtr;
	int nItemCount = (int)aryItem.size();
	for (int nItemIndex = 0; nItemIndex < nItemCount - 1; ++nItemIndex)
	{
		SCRIPT_ITEM* pScriptItem = &aryItem[nItemIndex];
		int nChar = pScriptItem->iCharPos;
		int nStart = nChar;
		for (; nChar < aryItem[nItemIndex + 1].iCharPos; ++nChar)
		{
			if (nChar <= nStart)
				continue;

			SYText_GetFontProperty(pTextProp, &objStartFont, nStart + nCharStart);
			SYText_GetFontProperty(pTextProp, &objCurFont, nChar + nCharStart);

			LPCTSTR lpstrStartFontName = GetWordTextsFontName(
				pwcChars[nStart + nCharStart], 
				objStartFont.lfFaceName, 
				objStartFont.lfFaceNameEng);
			LPCTSTR lpstrCurFontName = GetWordTextsFontName(
				pwcChars[nChar + nCharStart], 
				objCurFont.lfFaceName, 
				objCurFont.lfFaceNameEng);

			if (lstrcmp(lpstrStartFontName, lpstrCurFontName) != 0 || 
				objStartFont.lfHeight != objCurFont.lfHeight || 
				objStartFont.lfWidth != objCurFont.lfWidth || 
				objStartFont.lfItalic != objCurFont.lfItalic || 
				objStartFont.bBold != objCurFont.bBold)
			{
				SYSCRIPTRUN* psrRun = new SYSCRIPTRUN;
				psrRun->m_nRunCharPos = nStart;
				psrRun->m_nRunCharLen = nChar - nStart;
				psrRun->m_RunAnalysis = pScriptItem->a;
				memcpy(psrRun->m_szFontName, lpstrStartFontName, sizeof(TCHAR) * LF_FACESIZE);
				aryRun.push_back(psrRun);
				nStart = nChar;
			}
		}

		if (nChar > nStart)
		{
			SYText_GetFontProperty(pTextProp, &objStartFont, nStart + nStart);
			LPCTSTR lpstrStartFontName = GetWordTextsFontName(
				pwcChars[nStart + nCharStart], 
				objStartFont.lfFaceName, 
				objStartFont.lfFaceNameEng);

			SYSCRIPTRUN* psrRun = new SYSCRIPTRUN;
			psrRun->m_nRunCharPos = nStart;
			psrRun->m_nRunCharLen = nChar - nStart;
			psrRun->m_RunAnalysis = pScriptItem->a;
			memcpy(psrRun->m_szFontName, lpstrStartFontName, sizeof(TCHAR) * LF_FACESIZE);
			aryRun.push_back(psrRun);
		}
	}
}

LPCTSTR CSYUsp::GetWordTextsFontName(TCHAR tch, LPCTSTR lpszFontName, LPCTSTR lpszFontNameEng)
{
	LPCTSTR lpstrFontName = lpszFontName;
	if (!SY_bOnlyUseLocalFont)
	{
		if (iswascii(tch) && !iswspace(tch))
		{
			if (!SY_bDigitUseLocalFont || !iswdigit(tch))
				lpstrFontName = lpszFontNameEng;
		}
	}

	return lpstrFontName;
}

HRESULT CSYUsp::GenerateClusterGlyph(WCHAR* pwcChars, int nCharStart, SYFont* pFont, HDC hDC, SYSCRIPTRUN* psrRun)
{
	SCRIPT_FONTPROPERTIES sfp;
	ZeroMemory(&sfp, sizeof(SCRIPT_FONTPROPERTIES));
	sfp.cBytes = sizeof(SCRIPT_FONTPROPERTIES);
	::ScriptGetFontProperties(hDC, &psrRun->m_ScriptCache, &sfp);

	const WORD* pwOutGlyphs = NULL;
	int nGlyphCount = 0;
	HRESULT hr = ScriptShape(
		hDC, 
		psrRun, 
		pwcChars + psrRun->m_nRunCharPos + nCharStart, 
		psrRun->m_nRunCharLen, 
		&nGlyphCount
		);

	DWORD wCharGlyph;
	int nTemp;
	if (hr != S_OK || psrRun->m_RunAnalysis.fNoGlyphIndex)
	{
		nGlyphCount = psrRun->m_nRunCharLen;

		psrRun->m_aryCharIndex.resize(nGlyphCount);
		psrRun->m_aryCharGlyphs.resize(nGlyphCount);
		psrRun->m_aryLogicalCluster.resize(nGlyphCount);
		psrRun->m_aryVisAttribute.resize(nGlyphCount);
		psrRun->m_aryNoGlyphIndex.resize(nGlyphCount);
		psrRun->m_aryAdvanceWidth.resize(nGlyphCount);
		psrRun->m_aryGOffset.resize(nGlyphCount);
		psrRun->m_aryCheckCode.resize(nGlyphCount);

		SCRIPT_VISATTR sVisAttr;
		ZeroMemory(&sVisAttr, sizeof(SCRIPT_VISATTR));
		sVisAttr.fClusterStart = 1;

		for (int n = 0; n < nGlyphCount; ++n)
		{
			int nCharPos = psrRun->m_nRunCharPos + n;
			psrRun->m_aryCharIndex[n] = nCharPos;

			wCharGlyph = sfp.wgDefault;
			psrRun->m_aryCharGlyphs[n] = (WORD)wCharGlyph;
			if (psrRun->m_RunAnalysis.fRTL)
				psrRun->m_aryLogicalCluster[n] = nGlyphCount - 1 - n;
			else
				psrRun->m_aryLogicalCluster[n] = n;

			psrRun->m_aryVisAttribute[n] = sVisAttr;
			psrRun->m_aryNoGlyphIndex[n] = TRUE;

			//生成校验码
			if (m_bGenCheckCode)
			{
				unsigned long ullCRC = 0;
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->lfWidth), sizeof(float));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->lfHeight), sizeof(float));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->lfItalic), sizeof(LONG));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->bBold), sizeof(BYTE));
				nTemp = (int)(_tcslen(psrRun->m_szFontName) * sizeof(TCHAR));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(psrRun->m_szFontName), nTemp);
				
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&wCharGlyph), sizeof(WORD));
				WCHAR wCh = pwcChars[nCharStart + nCharPos];
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&wCh), sizeof(WCHAR));

				psrRun->m_aryCheckCode[n] = (mtusp_code)ullCRC;
			}
		}
	}
	else
	{
		psrRun->m_aryCharIndex.resize(nGlyphCount);
		psrRun->m_aryNoGlyphIndex.resize(nGlyphCount);
		psrRun->m_aryCheckCode.resize(nGlyphCount);

		for (int n = 0; n < nGlyphCount; ++n)
		{
			int nCharPos = TranslateGlyphToChar(psrRun, n);
			psrRun->m_aryCharIndex[n] = nCharPos;

			wCharGlyph = psrRun->m_aryCharGlyphs[n];
			if (wCharGlyph == sfp.wgDefault
				|| wCharGlyph == sfp.wgInvalid)
				psrRun->m_aryNoGlyphIndex[n] = TRUE;
			else
				psrRun->m_aryNoGlyphIndex[n] = FALSE;

			//生成校验码
			if (m_bGenCheckCode)
			{
				unsigned long ullCRC = 0;
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->lfWidth), sizeof(float));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->lfHeight), sizeof(float));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->lfItalic), sizeof(LONG));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&pFont->bBold), sizeof(BYTE));
				nTemp = (int)(_tcslen(psrRun->m_szFontName) * sizeof(TCHAR));
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(psrRun->m_szFontName), nTemp);

				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&wCharGlyph), sizeof(WORD));
				WCHAR wCh = pwcChars[nCharStart + nCharPos];
				ullCRC = SYCRC32Cal(ullCRC, (unsigned char*)(&wCh), sizeof(WCHAR));

				psrRun->m_aryCheckCode[n] = (mtusp_code)ullCRC;
			}
		}
	}

	return hr;
}
