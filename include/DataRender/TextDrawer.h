#pragma once
//////////////////////////////////////////////////////////////////////////
//是否兼容历史版本字宽2倍处理
#define SYTEXT_FONTWIDTH_X2

BASEGRAPH_DATARENDER_API LPCTSTR	SYGetDefaultFontFaceName();
BASEGRAPH_DATARENDER_API void		SYSetDefaultFontFaceName(LPCTSTR szFaceName);
BASEGRAPH_DATARENDER_API LPCTSTR	SYGetDefaultChnFontFaceName();
BASEGRAPH_DATARENDER_API void		SYSetDefaultChnFontFaceName(LPCTSTR szFaceName);
BASEGRAPH_DATARENDER_API LPCTSTR	SYGetPredefineSample();
BASEGRAPH_DATARENDER_API void		SYSetPredefineSample(LPCTSTR szSample);

BASEGRAPH_DATARENDER_API BOOL		SYIsHalfAngleSpaceNew();
BASEGRAPH_DATARENDER_API void		SYSetHalfAngleSpaceNew(BOOL bSupport);
BASEGRAPH_DATARENDER_API BOOL		SYIsSupportBidiRender();
BASEGRAPH_DATARENDER_API void		SYSetSupportBidiRender(BOOL bSupport);
BASEGRAPH_DATARENDER_API BOOL		SYIsSupportKorea();
BASEGRAPH_DATARENDER_API void		SYSetSupportKorea(BOOL bSupport);
BASEGRAPH_DATARENDER_API void		SYSetEnableFeedBackFont(BOOL bEnable);
BASEGRAPH_DATARENDER_API BOOL		SYIsEnableFeedBackFont();

extern BASEGRAPH_DATARENDER_API BOOL		SY_bDigitUseLocalFont;		//数字是否使用本地字体
extern BASEGRAPH_DATARENDER_API BOOL		SY_bOnlyUseLocalFont;		//所有字只是用本地字体设置
extern BASEGRAPH_DATARENDER_API int			g_nSmallFontFactor;		//小字加粗系数
extern BASEGRAPH_DATARENDER_API PI_Quality	g_nSmallFontQuality;		//小字抗闪烁质量
extern BASEGRAPH_DATARENDER_API BOOL		g_bSmallFontCalcBorder;	//小字清晰
extern BASEGRAPH_DATARENDER_API int			g_nSBCCaseReplaceSpace;	//全角空格替换成若干半角空格，0不替换。
extern BASEGRAPH_DATARENDER_API BOOL		g_bUseChnTradBoldFunc;

BASEGRAPH_DATARENDER_API void		SYSetTitleWordRefine(BOOL bTileWordRefine);
BASEGRAPH_DATARENDER_API BOOL		SYIsTitleWordRefine();


#define IS_NOT_ASCII_CHAR(ch)	((DWORD)(ch) > 255)
#define SYGETCHNTRADBOLDFUNC	(g_bUseChnTradBoldFunc)
BASEGRAPH_DATARENDER_API void		SYSetChnTradBoldFunc(BOOL bTradBoldFunc);

//标题字转换过来时bUseBorderEditMode默认值
extern BASEGRAPH_DATARENDER_API BOOL g_bWordTransUseBorderEditMode;
BASEGRAPH_DATARENDER_API void		SYWordTransUseBorderEditMode(BOOL bUseBorderEditMode);
//////////////////////////////////////////////////////////////////////////
//单个字形信息
typedef struct tag_SYWordInfo
{
	TCHAR				wChar;					//字符编码
	int					nCharIndex;				//字符在段中的索引
	WORD				wGlyphIndex;			//字形索引
	TCHAR				szFontName[LF_FACESIZE];//实际使用的字体
	//SCRIPT_ANALYSIS关键数据副本
	WORD				fRTL;					//Rendering direction
	//SCRIPT_VISATTR关键数据副本
	WORD				uJustification;
	WORD				fClusterStart;
	WORD				fDiacritic;
	WORD				fZeroWidth;

	//尺寸数据
	float				fDistance;				//字间距
	float				fAdvanceWidth;			//Advance Width
	point2f				ptGOffset;				//字形偏移（变体后字形偏移）
	rectf				rtCell;					//字占位矩形

	bool				bGlyphIndex;			//用Unicode码取字模还是用字形索引取字模
	bool				bVRollGlyph;			//竖向排版时，本字形是经过旋转旋转得来的。
	bool				bVisible;				//可视字符（回车换行符等）
	bool				bHVNest;				//横/竖排嵌套
	int					nChgFlag;				//变化标记（见SYGlyphChg_X定义）
	unsigned long long	uCheckCode;				//校验码（64位，暂时仅使用32位。）
	vectorgraph			vgGraph;				//原始字模图形
}SYWordInfo, *LPSYWordInfo;
typedef tag_SYWordInfo SYWordInfo, *LPSYWordInfo;

//查询字符是否需要旋转
BASEGRAPH_DATARENDER_API BOOL	SYIsNeedRollInVDisplay(TCHAR szCharName);
//字体是否存在
BASEGRAPH_DATARENDER_API BOOL	SYIsFontExist(TCHAR* szFaceName);
//是否是可视字符
BASEGRAPH_DATARENDER_API bool	SYIsVisibleChar(TCHAR ch);
//字符ch是否使用本地字体
BASEGRAPH_DATARENDER_API BOOL	SYIsUseLocalFont(TCHAR ch);
//由SYFont创建HFONT
BASEGRAPH_DATARENDER_API HFONT 	SYFont_CreateFont(LPSYFont pFont, TCHAR ch, LPCTSTR szFont, BOOL bHint);
BASEGRAPH_DATARENDER_API HFONT	SYFont_CreateScaleFont(LPSYFont pFont, TCHAR ch, LPCTSTR szFont, int nScaleW, int nScaleH);

BASEGRAPH_DATARENDER_API void	SYWordInfoInit(LPSYWordInfo pWordInfo);
BASEGRAPH_DATARENDER_API void	SYWordInfoRelease(LPSYWordInfo pWordInfo);

BASEGRAPH_DATARENDER_API BOOL	SYWordInfoFill(HDC hDC,					//如果NULL则函数内部申请
									   LPSYWordInfo pWordInfo,		//WordInfo不持有任何未释放的空间
									   TCHAR* pChar,				//非空时强制使用该Char取字模
									   LPSYFont pFont, 
									   BOOL bHorizontal);


BASEGRAPH_DATARENDER_API LPWSTR	SYCopyToWChar(LPWSTR lpwsz,LPCTSTR str,UINT CodePage=CP_ACP);
BASEGRAPH_DATARENDER_API LPSTR	SYCopyToChar(LPSTR lpsz,LPCTSTR str,UINT CodePage=CP_ACP);
