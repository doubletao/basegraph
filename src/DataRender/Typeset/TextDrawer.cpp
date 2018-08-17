#include "StdAfx.h"
#include "DataRender\AlgorithmRip.h"
#include "DataRender\DataRenderUtityFunc.h"
#include "DataRender\TextDrawer.h"
#include "DataRender\TextDrawer.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
using namespace std;

//////////////////////////////////////////////////////////////////////////
//缺省字体、缺省中文字体、预制用字符。
BASEGRAPH_DATARENDER_API TCHAR			g_strDefFontFacename[LF_FACESIZE] = _T("Arial");
BASEGRAPH_DATARENDER_API TCHAR			g_strDefChnFontFacename[LF_FACESIZE] = _T("宋体");
BASEGRAPH_DATARENDER_API TCHAR			g_strPredefSample[8] = _T("A");
LPCTSTR	SYGetDefaultFontFaceName()
{
	return g_strDefFontFacename;
}

void	SYSetDefaultFontFaceName(LPCTSTR szFaceName)
{
	memcpy(g_strDefFontFacename, szFaceName, sizeof(TCHAR) * (_tcslen(szFaceName) + 1));
}

LPCTSTR	SYGetDefaultChnFontFaceName()
{
	return g_strDefChnFontFacename;
}

void	SYSetDefaultChnFontFaceName(LPCTSTR szFaceName)
{
	memcpy(g_strDefChnFontFacename, szFaceName, sizeof(TCHAR) * (_tcslen(szFaceName) + 1));
}

LPCTSTR	SYGetPredefineSample()
{
	return g_strPredefSample;
}

void	SYSetPredefineSample(LPCTSTR szSample)
{
	memcpy(g_strPredefSample, szSample, sizeof(TCHAR) * 2);
}


//兼容旧版华康台湾半角空格，On时，兼容老版，但是算法不正确；OFF时，不兼容老版，但是算法正确。
BASEGRAPH_DATARENDER_API BOOL			g_bHalfAngleSpaceNew = TRUE;
//双向渲染算法。设定是否启动阿拉伯语系输入法，“OFF”为不启动，“ON”为启动。
//在阿拉伯语系输入状态时，各个文本编辑器都需要右打方式，且使用阿拉伯语系的字体。
BASEGRAPH_DATARENDER_API BOOL			g_bSupportBidiRender = FALSE;
//设定是否使用韩文输入法，“ON”为使用，“OFF”为不使用。
BASEGRAPH_DATARENDER_API BOOL			g_bSupportKorea = FALSE;
//是否允许字体回退功能
BASEGRAPH_DATARENDER_API BOOL			g_bEnableFeedBackFont = FALSE;
BOOL	SYIsHalfAngleSpaceNew()
{
	return g_bHalfAngleSpaceNew;
}

void	SYSetHalfAngleSpaceNew(BOOL bSupport)
{
	g_bHalfAngleSpaceNew = bSupport;
}

BOOL	SYIsSupportBidiRender()
{
	return g_bSupportBidiRender;
}

void	SYSetSupportBidiRender(BOOL bSupport)
{
	g_bSupportBidiRender = bSupport;
}

BOOL	SYIsSupportKorea()
{
	return g_bSupportKorea;
}

void	SYSetSupportKorea(BOOL bSupport)
{
	g_bSupportKorea = bSupport;
}

void	SYSetEnableFeedBackFont(BOOL bEnable)
{
	g_bEnableFeedBackFont = bEnable;
}

BOOL	SYIsEnableFeedBackFont()
{
	return g_bEnableFeedBackFont;
}

BOOL			Common_bDigitUseLocalFont = TRUE;				//数字是否使用本地字体
BOOL			Common_bOnlyUseLocalFont = FALSE;				//所有字只是用本地字体设置
//是否启用小字优化
int			g_nSmallFontFactor = 3;					//小字加粗系数
PI_Quality	g_nSmallFontQuality = PI_Quality_LOW;		//小字抗闪烁质量
BOOL			g_bSmallFontCalcBorder = FALSE;				//小字清晰
int			g_nSBCCaseReplaceSpace = 0;				//全角空格替换成若干半角空格，0不替换。
BOOL			g_bUseTitleWordRefine = TRUE;

//////////////////////////////////////////////////////////////////////////
void  SYSetTitleWordRefine(BOOL bTitleWordRefine)
{
	g_bUseTitleWordRefine = bTitleWordRefine;
}

BOOL  SYIsTitleWordRefine()
{
	return g_bUseTitleWordRefine;
}

//是否使用繁体系统下字体加粗特殊处理
BOOL		g_bUseChnTradBoldFunc = FALSE;
void SYSetChnTradBoldFunc(BOOL bTradBoldFunc)
{
	g_bUseChnTradBoldFunc = bTradBoldFunc;
}

BOOL SYGetChnTradBoldFunc()
{
	return g_bUseChnTradBoldFunc;
}

BASEGRAPH_DATARENDER_API BOOL			g_bWordTransUseBorderEditMode = FALSE;
void SYWordTransUseBorderEditMode(BOOL bUseBorderEditMode)
{
	g_bWordTransUseBorderEditMode = bUseBorderEditMode;
}
//////////////////////////////////////////////////////////////////////////
//字符旋转检索器
class VRollCharFinder
{
private:
	BOOL bValid;
	BOOL bUnicode;
	vector<WORD> aryWord;
	UINT  nWordSize;
	CString strVersion;
	CString strLanguage;
	BOOL ReadDataFromFile();
public:
	VRollCharFinder()
	{
		bValid = FALSE;
#ifdef _UNICODE
		bUnicode = TRUE;
#else
		bUnicode = FALSE;
#endif
		aryWord.clear();
		nWordSize = 0;
		bValid = ReadDataFromFile();
	}
	~VRollCharFinder()
	{

	}
	BOOL IsValid(){return bValid;}
	bool FindChar(WORD wChar);
	static int FindCharSkipTrailByte(const CString& str, TCHAR ch, int nStart = 0); 
};

int VRollCharFinder::FindCharSkipTrailByte(const CString& str, TCHAR ch, int nStart)
{
#ifdef _UNICODE
	return str.Find(ch, nStart);
#else
	//as in mbcs if we find a char which is the trail byte , that is not what we want
	//we should go on to find another
	int n = str.Find(ch, nStart);
	int nLen = str.GetLength();
	const unsigned char * lpsz = (const unsigned char *)(LPCSTR)str;
	while(n!= -1)
	{
		if(0 == _ismbstrail (lpsz, lpsz + n))//Not a trail byte 
		{
			return n;
		}
		//if ch is trail byte of an MBCS string, find next 
		if(n == nLen - 1)
			return -1;
		else 
			n = str.Find(ch, n + 1);
	}
	return n;
#endif
}

bool VRollcompare( const WORD arg1, const WORD arg2 )
{
	return ( arg1 < arg2 );
}

BOOL VRollCharFinder::ReadDataFromFile()
{
	assert(false);
	return FALSE;
#if 0
	BOOL bHeaderRead = FALSE;
	CString strLine;
	TCHAR chRangeSeperator = _T('-');
	TCHAR chSeperator = _T(',');
	BOOL bSpaceInclude = TRUE;
	TCHAR chNumTag = _T('\\');
	aryWord.clear();
	nWordSize = 0;

	CString strFile = SYPath_GetPath(Common_PATH_SYS);
	strFile.TrimRight(_T("\\"));
	strFile += _T("\\VerticalRoll.Dat");
	fstream file(strFile.c_str());
	if (!file)
		return FALSE;

	BOOL bRangeSeperatorInclude = FALSE;
	BOOL bSeperatorInclude = FALSE;
	BOOL bNumTagInclude = FALSE;

	int nWordIndex = 0;
	try
	{
		char chBuf[512];
		wchar_t pwBuf[512];
		while(!file.eof())
		{
			file.getline(chBuf, 512);
			MultiByteToWideChar(CP_ACP, 0, chBuf, -1, pwBuf, 512);
			strLine = pwBuf;

			strLine.TrimLeft();;
			strLine.TrimRight();
			if(strLine.IsEmpty() || strLine.Left(2) == _T("//"))
				continue;
			int nLineLength = strLine.GetLength(); 
			if(!bHeaderRead)
			{
				if(strLine.Find(_T("Version")) == 0)
				{
					strVersion = strLine.Mid(7);
					strVersion.TrimLeft();
					strVersion.TrimRight();
				}
				else if(strLine.Find(_T("Language")) == 0)
				{
					strLanguage = strLine.Mid(8);
					strLanguage.TrimLeft();
					strLanguage.TrimRight();
				}
				else if(strLine.Find(_T("Type")) == 0)
				{
					int n = FindCharSkipTrailByte(strLine, _T('='));
					if(n != - 1 && n < nLineLength - 1)
					{
						CString str = strLine.Mid(n + 1);
						str.TrimLeft();
						str.TrimRight();
						if(str == _T("0"))
							bUnicode = FALSE;
						if(str == _T("1"))
							bUnicode = TRUE;

					}
				}
				else if(strLine.Find(_T("Space")) == 0)
				{
					int n = FindCharSkipTrailByte(strLine, _T('='));
					if(n != -1 && n < nLineLength - 1)
					{
						CString str = strLine.Mid(n + 1);
						str.TrimLeft();
						str.TrimRight();
						if(str == _T("0"))
							bSpaceInclude = FALSE;
						if(str == _T("1"))
							bSpaceInclude = TRUE;

					}
				}
				else if(strLine.Find(_T("RangeSeperator")) == 0)
				{
					int n = FindCharSkipTrailByte(strLine, _T('='));
					if(n != -1 && n < nLineLength - 1)
					{
						CString strRangeSeperator = strLine.Mid(n + 1);
						strRangeSeperator.TrimLeft();
						strRangeSeperator.TrimRight();
						int nLen = strRangeSeperator.GetLength();
						if( nLen > 1)
						{
							chRangeSeperator = strRangeSeperator[0];
						}
						if(nLen > 2)
						{
							if(strRangeSeperator[nLen - 1] == _T('1'))
								bRangeSeperatorInclude = TRUE;
						}
					}
				}
				else if(strLine.Find(_T("Seperator")) == 0)
				{
					int n = FindCharSkipTrailByte(strLine, _T('=')); 
					if(n != -1 && n < nLineLength - 1 )
					{
						CString strSeperator = strLine.Mid(n + 1);
						strSeperator.TrimLeft();
						strSeperator.TrimRight();
						int nLen = strSeperator.GetLength();
						if( nLen > 1)
						{
							chSeperator = strSeperator[0];
						}
						if(nLen > 2)
						{
							if(strSeperator[nLen - 1] == _T('1'))
								bSeperatorInclude = TRUE;

						}
					}
				}
				else if(strLine.Find(_T("NumTag")) == 0)
				{
					int n = FindCharSkipTrailByte(strLine, _T('=')); 
					if(n != -1 && n < nLineLength - 1)
					{
						CString str = strLine.Mid(n + 1);
						str.TrimLeft();
						str.TrimRight();
						int nLen = str.GetLength();
						if( nLen > 1)
						{
							chNumTag = str[0];
						}
						if(nLen > 2)
						{
							if(str[nLen - 1] == _T('1'))
								bNumTagInclude = TRUE;
						}
					}
				}
				else if(strLine.Find(_T("Data_Begin")) == 0)
				{
					bHeaderRead = TRUE;
				}
			}
			else
			{
				//caculate nSize
				CString strRemain;
				CString strSub = strLine;;
				int nSeperator = FindCharSkipTrailByte(strLine, chSeperator); 
				if(nSeperator != -1) 
				{
					strSub = strLine.Left(nSeperator);
					if(nSeperator < strLine.GetLength() - 1)
						strRemain = strLine.Mid(nSeperator + 1);
				}

				while(1)
				{
					strSub.TrimLeft();
					strSub.TrimRight();
					if(!strSub.empty())
					{
						int n = FindCharSkipTrailByte(strSub, chRangeSeperator);
						if( n > 0 && n < strSub.GetLength() - 1)
						{
							CString strLeft = strSub.Left(n);
							CString strRight = strSub.Mid(n + 1);
							strLeft.TrimLeft();
							strRight.TrimRight();
							WORD wLeft = 0;
							WORD wRight = 0;
							if(FindCharSkipTrailByte(strLeft, chNumTag) == 0)
							{
								strLeft = strLeft.Mid(1);
								int nBase = 10;
								LPTSTR strStop;
								if(FindCharSkipTrailByte(strLeft, _T('X')) != -1 
									|| FindCharSkipTrailByte(strLeft, _T('x')) != -1)
									nBase = 16;
								wLeft = (WORD)_tcstoul(strLeft.c_str(), &strStop, nBase);
							}
							else
							{
								if(bUnicode)
								{
									if(strLeft.GetLength() == 1)
										wLeft = strLeft[0];

								}
								else
								{

									if(strLeft.GetLength() == 1)
										wLeft = strLeft[0];
									if(strLeft.GetLength() == 2)
										//for mbcs the lead byte is the lower byte.
										//as in ascii char the lead byte is NA
										//if we make a word from mbcs char we should
										//make an ascii char "as it is", that is,
										//let the only one char (byte) to be the low
										//byte, while as to mbcs char we should let 
										//the lead byte ( strLeft[0] ) to be the high byte.
										//so...
										wLeft = MAKEWORD(strLeft[1], strLeft[0]);
								}
							}
							if(FindCharSkipTrailByte(strRight, chNumTag) == 0)
							{
								strRight = strRight.Mid(1);
								int nBase = 10;
								LPTSTR strStop;
								if(strRight.Find(_T('X')) != -1 || strRight.Find(_T('x')) != -1)
									nBase = 16;
								wRight = (WORD)_tcstoul(strRight.c_str(), &strStop, nBase);
							}
							else
							{
								if(bUnicode)
								{
									if(strRight.GetLength() == 1)
										wRight = strRight[0];
								}
								else
								{
									if(strRight.GetLength() == 1)
										wRight = strRight[0];
									if(strRight.GetLength() == 2)
										wRight = MAKEWORD(strRight[1], strRight[0]);
								}
							}
							if(wLeft && wRight)
							{
								if(wLeft > wRight)
								{
									WORD w = wRight;
									wRight = wLeft;
									wLeft = w;
								}
								UINT nSize = wRight - wLeft + 1;
								for(UINT u = 0; u < nSize; u++)
									aryWord.push_back(wLeft + u);
							}
						}
						else
						{
							WORD w = 0;
							if(FindCharSkipTrailByte(strSub, chNumTag) == 0)
							{
								strSub = strSub.Mid(1);
								int nBase = 10;
								LPTSTR strStop;
								if(FindCharSkipTrailByte(strSub, _T('X')) != -1 
									|| FindCharSkipTrailByte(strSub, _T('x')) != -1)
									nBase = 16;
								w = (WORD)_tcstoul(strSub.c_str(), &strStop, nBase);
							}
							else
							{
								if(bUnicode)
								{
									if(strSub.GetLength() == 1)
										w = strSub[0];
								}
								else
								{
									if(strSub.GetLength() == 1)
										w = strSub[0];
									if(strSub.GetLength() == 2)
										w = MAKEWORD(strSub[1], strSub[0]);
								}
							}
							if(w)
							{
								aryWord.push_back(w);
							}
						}

					}

					if(strRemain.IsEmpty())
						break;
					nSeperator = FindCharSkipTrailByte(strRemain, chSeperator); 
					if(nSeperator != -1) 
					{
						strSub = strRemain.Left(nSeperator);
						if(nSeperator < strRemain.GetLength() - 1)
							strRemain = strRemain.Mid(nSeperator + 1);
						else
							strRemain.Empty();
					}
					else
					{
						strSub = strRemain;
						strRemain.Empty();
					}

				}
			}
		}
	}
	catch(...)
	{
		file.close();
		return FALSE;
	}
	file.close();
	if(bSpaceInclude)
	{
		aryWord.push_back(0x20);
	}
	if(bSeperatorInclude)
	{
		aryWord.push_back(chSeperator);
	}
	if(bNumTagInclude)
	{
		aryWord.push_back(chNumTag);
	}
	if(bRangeSeperatorInclude)
	{
		aryWord.push_back(chRangeSeperator);
	}
	nWordSize = (int)aryWord.size();
	//Sort it
	sort(aryWord.begin(), aryWord.end(), VRollcompare);
	return TRUE;
#endif
}

bool VRollCharFinder::FindChar(WORD wChar)
{
	if(nWordSize == 0)
		return false;

	return binary_search(aryWord.begin(), aryWord.end(), wChar);
}

BASEGRAPH_DATARENDER_API BOOL SYIsNeedRollInVDisplay(TCHAR szCharName)
{
	//here we only support dbcs and unicode so we use a word
	//djt 可以从配置文件读取竖向排版时需要旋转的的字符
	//static VRollCharFinder VRollChar;
	//if(VRollChar.IsValid())
	//{
	//	WORD w = (WORD)szCharName;
	//	return VRollChar.FindChar(w);
	//}
	if ((szCharName >= 0x30 && szCharName <=0x39)||	// 0-9
		(szCharName >= 0x40 && szCharName <=0x5A)||	// A-Z
		(szCharName >= 0x61 && szCharName <=0x7A)||	// a-z
		(szCharName >= 0x23 && szCharName <=0x2A)||	// #-*
		(szCharName >= 0x3C && szCharName <=0x3E)||	// <->
		szCharName == 0x7B || szCharName == 0x7D ||  // {}
		szCharName == 0x5B || szCharName == 0x5D ||  // []
		szCharName == _T('‘') || szCharName == _T('’') ||
		szCharName == _T('“') || szCharName == _T('”') ||  
		szCharName == _T('［') || szCharName == _T('］') ||  
		szCharName == _T('（') || szCharName == _T('）') ||  
		szCharName == _T('｛') || szCharName == _T('｝') ||
		szCharName == _T('〖') || szCharName == _T('〗') ||
		szCharName == _T('「') || szCharName == _T('」') ||
		szCharName == _T('『') || szCharName == _T('』') ||
		szCharName == _T('《') || szCharName == _T('》') ||
		szCharName == _T('【') || szCharName == _T('】') ||
		szCharName == _T(' ')
		)
	{
		return TRUE;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
bool SYIsVisibleChar(TCHAR ch)
{
	if (ch != _T('\n') && ch != _T('\r'))
		return true;

	return false;
}

BOOL SYIsUseLocalFont(TCHAR ch)
{
	if (!Common_bOnlyUseLocalFont)
	{
		if (iswascii(ch) && !iswspace(ch))
		{
			if (!Common_bDigitUseLocalFont || !iswdigit(ch))
				return FALSE;
		}
	}
	return TRUE;
}

HFONT SYFont_CreateFont(LPCommonFont pFont, TCHAR ch, LPCTSTR szFont, BOOL bHint)
{
	//逻辑字高与字号转换关系
	//LONG lFontSize = pFont->lfHeight * 72.0 / GetDeviceCaps(hDC, LOGPIXELSY) + 0.5 ;

	HFONT hFont = NULL;
	int nWidth = (int)(pFont->lfWidth);
	int nEscapement = (int)pFont->lfEscapement;

	//字高用负值取字模（沿用历史定义）
	int nHeight = (int)(pFont->lfHeight*2);
	if (nHeight > 0)
		nHeight = -nHeight;

	//加粗特殊处理（加粗值沿用历史定义）
	int nWeight;
	BYTE bBold = pFont->bBold;
	if (bBold)
	{
		if (SYGETCHNTRADBOLDFUNC && IS_NOT_ASCII_CHAR(ch))
			nWeight = FW_MEDIUM;
		else
			nWeight = FW_HEAVY;
	}
	else
	{
		nWeight = FW_MEDIUM;
	}

	LPCTSTR pFontName;
	//该字符是否使用本地字体
	if (szFont != NULL)
	{
		pFontName = szFont;
	}
	else
	{
		if (SYIsUseLocalFont(ch))
			pFontName = pFont->lfFaceName;
		else
			pFontName = pFont->lfFaceNameEng;
	}

	hFont = ::CreateFont(
		nHeight, 
		nWidth, 
		0, 
		0, 
		nWeight, 
		pFont->lfItalic, 
		0, 
		0, 
		DEFAULT_CHARSET, 
		OUT_TT_PRECIS, 
		CLIP_TT_ALWAYS, 
		ANTIALIASED_QUALITY, 
		DEFAULT_PITCH, 
		pFontName
		);

	return hFont;
}

HFONT SYFont_CreateScaleFont(LPCommonFont pFont, TCHAR ch, LPCTSTR szFont, int nScaleW, int nScaleH)
{
	HFONT hFont = NULL;
	int nWidth = (int)(pFont->lfWidth);
	int nEscapement = (int)pFont->lfEscapement;

	//字高用负值取字模（沿用历史定义）
	int nHeight = (int)(pFont->lfHeight*2);
	if (nHeight > 0)
		nHeight = -nHeight;

	//加粗特殊处理（加粗值沿用历史定义）
	int nWeight;
	BYTE bBold = pFont->bBold;
	if (bBold)
	{
		if (SYGETCHNTRADBOLDFUNC && IS_NOT_ASCII_CHAR(ch))
			nWeight = FW_MEDIUM;
		else
			nWeight = FW_HEAVY;
	}
	else
	{
		nWeight = FW_MEDIUM;
	}

	LPCTSTR pFontName;
	//该字符是否使用本地字体
	if (szFont != NULL)
	{
		pFontName = szFont;
	}
	else
	{
		if (SYIsUseLocalFont(ch))
			pFontName = pFont->lfFaceName;
		else
			pFontName = pFont->lfFaceNameEng;
	}

	hFont = ::CreateFont(
		nHeight * nScaleH, 
		nWidth * nScaleW, 
		0, 
		0, 
		nWeight, 
		pFont->lfItalic, 
		0, 
		0, 
		DEFAULT_CHARSET, 
		OUT_TT_PRECIS, 
		CLIP_TT_ALWAYS, 
		ANTIALIASED_QUALITY, 
		DEFAULT_PITCH, 
		pFontName
		);

	return hFont;
}

typedef struct tag_FontNameCompare
{
	TCHAR	szFaceName[LF_FACESIZE];
	BOOL	bSame;
}FontNameCompare, *LPFontNameCompare;

int CALLBACK FontExistanceTest(
							   ENUMLOGFONTEX *lpelfe,    // logical-font data
							   NEWTEXTMETRICEX *lpntme,  // physical-font data
							   DWORD FontType,           // type of font
							   LPARAM lParam             // application-defined data
							   )
{
	LPFontNameCompare pCompareData = (LPFontNameCompare)lParam;
	if (_tcscmp(pCompareData->szFaceName, lpelfe->elfFullName) == 0)
	{
		pCompareData->bSame = TRUE;
		return 0;
	}
	else
	{
		pCompareData->bSame = FALSE;
		return 1;
	}
}

//检验某种字体是否有对应的@类型的字体族
BOOL SYFontHasVerticalStyle(TCHAR* szFaceName)
{
	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetDC(hWnd);

	FontNameCompare compareData;
	compareData.bSame = FALSE;
	compareData.szFaceName[0] = _T('@');
	_tcscpy_s(compareData.szFaceName + 1, LF_FACESIZE - 1, szFaceName);

	LOGFONT font;
	memset(&font, 0, sizeof(LOGFONT));
	font.lfCharSet = DEFAULT_CHARSET;
	font.lfFaceName[0] = _T('\0');

	EnumFontFamiliesEx(hDC, &font, (FONTENUMPROC)FontExistanceTest, (LPARAM)&compareData, 0);

	ReleaseDC(hWnd, hDC);

	if (compareData.bSame)
		return TRUE;
	else
		return FALSE;
}

//指定的字体是否存在
BOOL SYIsFontExist(TCHAR* szFaceName)
{
	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetDC(hWnd);

	FontNameCompare compareData;
	compareData.bSame = FALSE;
	_tcscpy_s(compareData.szFaceName, LF_FACESIZE, szFaceName);

	LOGFONT font;
	memset(&font, 0, sizeof(LOGFONT));
	font.lfCharSet = DEFAULT_CHARSET;
	font.lfFaceName[0] = _T('\0');

	EnumFontFamiliesEx(hDC, &font, (FONTENUMPROC)FontExistanceTest, (LPARAM)&compareData, 0);

	ReleaseDC(hWnd, hDC);

	if (compareData.bSame)
		return TRUE;
	else
		return FALSE;
}

static BOOL AdjustCharWidth(HDC hDC, 
							float& fA, 
							float& fB, 
							float& fC, 
							TCHAR wChar, 
							HFONT hFont)
{
	if(wChar == _T(' '))
	{
		int nW;
		::GetCharWidth(hDC, 0, 0, &nW);
		if(nW == 0)
		{
			HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
			::GetCharWidth(hDC, 0, 0, &nW);
			::SelectObject(hDC, hOldFont);
		}

		int nWidth = (int)((fA + fB + fC) * 16);
		int nNewWidth = (nW/2* nWidth-1) / nWidth;
		float fRatio = (float)nNewWidth / nWidth;
		fA *= fRatio;
		fB *= fRatio;
		fC *= fRatio;
	}

	return TRUE;
}

void SYWordInfoInit(LPCommonWordInfo pWordInfo)
{
	memset(pWordInfo, 0, sizeof(SYWordInfo));
	pWordInfo->bVisible = true;
}

void SYWordInfoRelease(LPCommonWordInfo pWordInfo)
{
	graph_release(&pWordInfo->vgGraph);
}

BOOL SYWordInfoFill(HDC hDC, 
					LPCommonWordInfo pWordInfo, 
					TCHAR* pChar, 
					LPCommonFont pFont, 
					BOOL bHorizontal)
{
	//1、复杂文字排版时，A、B、C、Ascent以及Descent这些信息是不足以作为排版依据的，
	//   因此，本算法将这些信息进行了转换，调用者无需关心。
	//2、调用者将字形视作方格进行排版即可。
	HDC hWndDC;
	HWND hWnd = NULL;
	if (hDC != NULL)
	{
		hWndDC = hDC;
	}
	else
	{
		hWnd = GetDesktopWindow();
		hWndDC = GetDC(hWnd);
		SetGraphicsMode(hWndDC, GM_ADVANCED);
		SetMapMode(hWndDC, MM_TEXT);
	}

	TCHAR wChar;
	wChar = pChar == NULL ? pWordInfo->wChar : *pChar;
	//是否竖向旋转图形
	BOOL bVRollGlyph = FALSE;
	if (!bHorizontal)
	{
		if (pFont->lfVGRotate == SYVGROT_NODEF)
			bVRollGlyph = SYIsNeedRollInVDisplay(wChar);
		else
			bVRollGlyph = (pFont->lfVGRotate == SYVGROT_ROT);
	}
	pWordInfo->bVRollGlyph = bVRollGlyph;

	//是否使用字形索引
	BOOL bGlyphIndex = pWordInfo->bVisible ? pWordInfo->bGlyphIndex : FALSE;
	bGlyphIndex = TRUE;

	HFONT hFont;
	hFont = SYFont_CreateFont(pFont, wChar, bGlyphIndex ? pWordInfo->szFontName : NULL, FALSE);
	if (hFont == NULL)
		return FALSE;
	HFONT hOldFont = (HFONT)SelectObject(hWndDC, hFont);

	TEXTMETRIC MetRic;
	if (!GetTextMetrics(hWndDC, &MetRic))
		return FALSE;

	rectf* pRrCell = &pWordInfo->rtCell;

	if (pWordInfo->bVisible)
	{
		GLYPHMETRICS glyphm;
		memset(&glyphm, 0, sizeof(GLYPHMETRICS));
		MAT2 mat;
		memset(&mat, 0, sizeof(MAT2));
		mat.eM11.value = 1;
		mat.eM22.value = 1;

		int nNeedByte, nNeedByteReal;
		if (bGlyphIndex)
			nNeedByte = GetGlyphOutline(hWndDC, pWordInfo->wGlyphIndex, GGO_NATIVE | GGO_GLYPH_INDEX, &glyphm, 0, NULL, &mat);
		else
			nNeedByte = GetGlyphOutline(hWndDC, wChar, GGO_NATIVE, &glyphm, 0, NULL, &mat);

		TTPOLYGONHEADER* pPolyHead = NULL;
		if (nNeedByte > 0)
		{
			pPolyHead = (TTPOLYGONHEADER*)(new BYTE[nNeedByte]);
			if (bGlyphIndex)
				nNeedByteReal = GetGlyphOutline(hWndDC, pWordInfo->wGlyphIndex, GGO_NATIVE | GGO_GLYPH_INDEX, &glyphm, nNeedByte, pPolyHead, &mat);
			else
				nNeedByteReal = GetGlyphOutline(hWndDC, wChar, GGO_NATIVE, &glyphm, nNeedByte, pPolyHead, &mat);

			SYVectorGraph_AssignTTPOLYGONHEADER(&pWordInfo->vgGraph, pPolyHead, nNeedByteReal);
			delete[] pPolyHead;
			pPolyHead = NULL;

			//竖排图形旋转
			if (bVRollGlyph || pFont->lfEscapement != 0)
			{
				matrix4f matVRotate;
				matrix4f_identity(&matVRotate);
				float fAngleZ = 0;
				if (bVRollGlyph)
					fAngleZ += PAIF/2;
				if (pFont->lfEscapement != 0)
					fAngleZ -= pFont->lfEscapement*PAIF/1800;
				matrix4f_fromeuler_lyxz(&matVRotate, 0, 0, fAngleZ);
				vg_rotate_matrix(&pWordInfo->vgGraph, &matVRotate);
			}

			//变体位移
			if (pWordInfo->ptGOffset.x != 0 || pWordInfo->ptGOffset.y != 0)
			{
				if (bVRollGlyph)
				{
					vg_offset(&pWordInfo->vgGraph, pWordInfo->ptGOffset.y, pWordInfo->ptGOffset.x, 0);
				}
				else
				{
					vg_offset(&pWordInfo->vgGraph, pWordInfo->ptGOffset.x, -pWordInfo->ptGOffset.y, 0);
				}
			}
		}

		if (bVRollGlyph)
		{
			pRrCell->left   = -(float)MetRic.tmDescent;
			pRrCell->top    = 0;
			pRrCell->right  = (float)MetRic.tmAscent;
			pRrCell->bottom = glyphm.gmCellIncX;
		}
		else
		{
			pRrCell->left   = 0;
			pRrCell->top    = -(float)MetRic.tmAscent;
			pRrCell->right  = glyphm.gmCellIncX;
			pRrCell->bottom = (float)MetRic.tmDescent;
		}

		if (nNeedByte <= 0)
			pWordInfo->vgGraph.bound = *pRrCell;
	}
	else
	{
		if (bHorizontal)
		{
			pRrCell->left   = 0;
			pRrCell->top    = -(float)MetRic.tmAscent;
			pRrCell->right  = 0;
			pRrCell->bottom = (float)MetRic.tmDescent;
		}
		else
		{
			pRrCell->left   = -(float)MetRic.tmDescent;
			pRrCell->top    = 0;
			pRrCell->right  = (float)MetRic.tmAscent;
			pRrCell->bottom = 0;
		}
	}

	SelectObject(hWndDC, hOldFont);
	DeleteObject(hFont);
	if (hDC == NULL)
		ReleaseDC(hWnd, hWndDC);

	return TRUE;
}

LPWSTR CommonCopyToWChar(LPWSTR lpwsz,LPCTSTR str,UINT CodePage)
{
#ifdef _UNICODE
	rsize_t n = (rsize_t)wcslen(str);
	wcscpy_s(lpwsz, n, str);
	return lpwsz;
#else
	int n = strlen(str);
	ZeroMemory(lpwsz, (n+1)*2);
	MultiByteToWideChar(CodePage, NULL, str, n, lpwsz, n+1);
	return lpwsz;
#endif
}

LPSTR CommonCopyToChar(LPSTR lpsz,LPCTSTR str,UINT CodePage)
{
#ifdef _UNICODE
	int n=(int)wcslen(str);
	ZeroMemory(lpsz, n*2+1);
	WideCharToMultiByte(CodePage, NULL, str, n, lpsz, n*2+1, NULL, NULL);
	return lpsz;
#else
	return strcpy(lpsz, str);
#endif

}
