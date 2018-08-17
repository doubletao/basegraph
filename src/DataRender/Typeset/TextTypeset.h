#ifndef _BaseGraphTYPESET_INC_
#define _BaseGraphTYPESET_INC_
#include "TextTypesetDataAPI.h"
#include "DataRender\TextDrawer.h"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class CBaseCGObject;
//排版最小尺寸
#define BaseGraphTYPESET_DIMENSION_MIN				4
#define BaseGraphTYPESET_NEEDGRAPHSCALE(x, y)		(fabs((x) - 1.0) > ABS_FLOAT_0 || fabs((y) - 1.0) > ABS_FLOAT_0)
const float FRAMES_PERSEC = 25.0f;
//缺省Project宽高
const int PROJECT_WIDTH = 1920;
const int PROJECT_HEIGHT = 1080;
//帧场信息
const int PROJECT_FIELD = 0;
// 排版器中Tab的最大长度（像素）
const int TYPESET_TABLEN = 20;
//////////////////////////////////////////////////////////////////////////
//排版器接口函数
extern  BaseGraphTypesetInterface g_TypesetInterface;
struct  common_font_def;
//图形空间信息
#define BaseGraphGLYPHSPACEPTs			4				//图形占据位置描述点数
#define BaseGraphGLYPHSPACEPTs_EX		6				//图形占据扩展位置描述点数
typedef struct tag_BaseGraphGlyphSpace
{
	point3f			ptPos;					//Unit位置
	point3f			ptPosOffset;				//Unit位置偏移

	//Bound点次序：
	//横排：				竖排：
	// (0)--(1)--(4)		(0)--(1)
	//  |    |    |			 |    |
	// (3)--(2)--(5)		(3)--(2)
	//              		 |    |
	//						(5)--(4)
	point3f			ptBound[BaseGraphGLYPHSPACEPTs_EX];	//图形占据位置（第4,5点为含字距的扩展位置）
}BaseGraphGlyphSpace, *LPCommonGlyphSpace;
//单条下划线信息
typedef struct tag_BaseGraphUnderLineInfo
{
	int					nCharIndex;					//字符在段中的索引
	point3f				ptPos;						//Unit位置（含位移）

	//Bound点次序：
	// (0)--(1)
	//  |    |
	// (3)--(2)
	point3f				ptBound[BaseGraphGLYPHSPACEPTs];	//图形占据位置
	vectorgraph			vgGraph;					//下划线图形（含缩放）
}BaseGraphUnderLineInfo, *LPCommonUnderLineInfo;
//////////////////////////////////////////////////////////////////////////


struct PageInfo
{
	int nStartIndex;	// 起始字符位置
	int nCharCnt;		// 此页字符数
	int nStartHeight;	// 起始字符实际高度
	int nHeight;		// 本页高度 
	float fStartTime;	// 开始时间
	float fTimeLen;		// 播放时间
	PageInfo()
	{
		nStartIndex = 0;
		nCharCnt = 0;
		nStartHeight = 0;
		nHeight = 0;
		fStartTime = 0;
		fTimeLen = 3 * 2 * FRAMES_PERSEC;		// 每页时间长度（场）
	}
};

typedef std::vector<PageInfo> VectPage;



//排版器类定义
class  BASEGRAPH_DATARENDER_API CBaseTypeset
{
public:
	enum TypesetLevels
	{
		TTLNone = 0,	//没有发生改变
		TTLOffset,		//位移，无需重新生成字模。
		TTLArrange,		//整理字位置，无需重新生成字模。
		TTLDistance,	//字距等变化，无需重新生成字模，但需要刷新字形位置。
		TTLScaleGraph,	//放缩图形		
		TTLPageChange,	//公告板显示页数改变
		TTLScaleFont,	//放缩字号		
		TTLWordDel,		//删除文字（需要指定哪些字改变了）		
		TTLWordAdd,		//增加文字（需要指定哪些字改变了）
		TTLWordChg,		//部分字改变了（需要指定哪些字改变了）
		TTLReTypeset	//完全重新排版
	};
public:
	CBaseTypeset();
	~CBaseTypeset();
	//锁
	void Lock();
	void Unlock();
	//////////////////////////////////////////////////////////////////////////
	// 初始化、数据释放
	void				Attach(LPCommonText pTextObj);
	void				Attach(LPCommonText pTextObj, LPVOID lpParam);
	void				ReleaseAllTempData();
	//验证临时排版数据合法性
	BOOL				ValidateTypesetPrivateData();
	//////////////////////////////////////////////////////////////////////////
	// 属性存取
	inline LPCommonText	GetTextObj() {return m_pTextObj;}
	inline LPBaseARtTypeset	GetArtWord() {return m_pTextObj->pArtWord;}
	inline LPBaseARtTypeset	GetTypesetPath() {return m_pTextObj->pTypesetPath;}
	LPCTSTR			GetText();
	int				GetTextLength();
	LPCTSTR			GetString(int nStart, int nEnd);
	//获取属性
	void				GetProperty(int nIndex,LPCommonFont& pFont, LPCommonStaticEffect& p2DColor, BaseGraphMaterial*& p3DColor, LPCommonUnderLine& pUnderLine);
	LPCommonTextProp		GetTextProp();
	LPCBaseFont			GetFont(int nIndex);
	void				SetFont(int nIndex, int nCount, LPCBaseFont lpFont, DWORD dwMask = BaseGraphTEXTFONTMASK_ALL);
	void SetFont(LPCBaseFont pFont);
	//通过dwMask隐码，设定单个属性值。nIndex为-1时设置所有字属性
	void				SetFontProperty(int nIndex, int nCount,	const LPVOID lpPropData,const LPVOID lp2ndPropData,DWORD dwMask);
	//改变制式时更新文字的宽高间距和行距
	void				UpdateFontProperty(float fWRate, float fHRate);
	//下划线
	LPCBaseUnderLine		GetUnderLine(int nIndex);
	void				SetUnderLine(int nIndex, int nCount,LPCBaseUnderLine lpUnderLine,DWORD dwMask = BaseGraphTEXTFONTMASK_UNDERLINE_ALL);
	//通过dwMask隐码，设定单个属性值。
	void				SetUnderLineProperty(int nIndex, int nCount,const LPVOID lpPropData,DWORD dwMask);
	//2D色
	LPCBaseStaticEffect	GetTextStaticEffect(int nIndex);
	void				SetTextStaticEffect(int nIndex, int nCount, LPCBaseStaticEffect lpFillColor, DWORD dwMask = BaseGraphTEXTCOLORMASK_ALL);
	//3D色
	BaseGraphMaterial*			GetText3DColor(int nIndex);
	void				SetText3DColor(int nIndex, int nCount, BaseGraphMaterial* p3DColor);
	//子面3D色
	void				SetTextSubFace3DColor(int nIndex, int nCount, int nSubFace, BaseGraphMaterial* p3DColor);
	//上载颜色到Unit
	void				UpLoadColorToUnit(int nIndex, int nCount, DWORD dwUpdate = BaseGraphTEXT_UNIT_AUTO);
	// 各种边框
	void				GetBorderRect(rectf& rtBorder);			//占位符边界
	void				SetBorderRect(rectf rtBorder);
	//rtTypeset:排版时实际使用的矩形;bReCal:是否重新计算
	void				GetTypesetRect(rectf& rtTypeset, BOOL bReCal);	
	void				SetTypesetRect(rectf& rtTypeset);
	//模型边界
	void				GetModelRect(rectf& rtRect, BOOL bIgnoreSpecial);
	BOOL				IsHorizontal();
	void				SetHorizontal(BOOL bHorizontal);
	BOOL				IsLeftInput();
	void				SetLeftInput(BOOL bLeftInput);
	BOOL				IsLineWrap();
	void				SetLineWrap(BOOL bLineWrap);
	BOOL				IsRLC(int nIndex);		//是否是回车换行符
	BOOL				IsFixedFrame();
	BOOL				IsArtWord();			//是否艺术字
	BOOL				IsPathTypeset();		//沿路径排版
	BOOL				IsLinearTypeset();	//是否线性排版（相对于非线性排版而言）

	// 公告板翻页
	BOOL				IsPageMode();
	void				SetPageMode(BOOL bPageMode);
	BOOL				IsShowAllPage();
	void				SetShowPage(BOOL bShowAllPage, BOOL bReTypeset = TRUE);
	UINT				GetCurPageIndex();
	void				SetCurPageIndex(int iCurPageIndex);
 	VectPage*			GetPageInfo();
	void				SetPageInfo(VectPage* pPageInfo);

	//纵向对齐方式
	E_TextAlignVeritical	GetVerticalAlign();
	void				SetVerticalAlign(E_TextAlignVeritical eAlignVertical,int nLevel = CBaseTypeset::TTLNone);
	//////////////////////////////////////////////////////////////////////////
	// 段落
	//获得排版临时段数组
	BaseARray*		GetDynParagraphArray();
	LPVOID			GetDynParagraph(int nParaIdx);
	//获得指定段的字形个数
	int				GetParagraphWordInfoCount(int nParaIdx);
	//获得指定段的字形数组
	BaseARray*		GetParagraphWordInfoArray(int nParaIdx);
	//获得指定段指定行的字形个数
	int				GetParagraphLineWordInfoCount(int nParaIdx, int nLineIdx);
	//获得指定段尾行的字形开始索引
	int				GetParagraphTrailLineWordInfoBeginIndex(int nParaIdx);
	int				GetParagraphIndexByCharPos(int nIndex);
	BaseARray*		GetTextParagraphArray();
	LPCommonTextParagraph	GetParagraph(int nIndex);
	int				GetParagraphCharCount(int nIndex);
	//段的横向对齐方式
	E_TextAlignHorizontal	GetParagraphHorAlign(int nParagIdx);
	void			SetParagraphHorAlign(E_TextAlignHorizontal eAlign, 
							int nParagIdx = -1,				//起始段索引，-1则修改所有段。
							int nCount = 1,					//欲修改的段数
							int nLevel = CBaseTypeset::TTLNone	//排版
							);
	//段的行间距
	float			GetParagraphDistance(int nParagIdx);
	void			SetParagraphDistance(float fDistance, 
							int nParagIdx = -1,				//起始段索引，-1则修改所有段。
							int nCount = 1,					//欲修改的段数
							int nLevel = CBaseTypeset::TTLNone	//排版
							);
	//////////////////////////////////////////////////////////////////////////
	// 排版
	void			Typeset(int nLevel = CBaseTypeset::TTLArrange, int nIndex=-1, int nCount=-1);
	//局部排版
	void			TypesetPartly(int nLevel = CBaseTypeset::TTLArrange);

	int				DeleteString(int nIndex, int nCount);
	int				InsertChar(int nIndex, TCHAR tChar);
	int				InsertString(int nIndex, LPCTSTR szText);
	void			ReplaceString(LPCTSTR szText, BOOL bKeepProperty);
	BOOL			InsertTabChar(int nBeginIndexm, int nEndIndex, BOOL bPressShift, BOOL bPressCtrl);
	BOOL			CopyStringToClipboard(int nStart, int nEnd);
	LPCTSTR			GetStringFromClipboard();
	//字体按比例缩放
	void			FontZoom(int nIndex, int nCount, float fZoomRate);
	BOOL			AdjustTypesetRealRect(rectf& rtReal);
	void			FixedFrameCal();						//固定字框计算
	//计算结果进行偏移
	void			OffsetCal(vector3f* pOffset);
	void			GenerateWordInfo(LPCommonWordInfo pWordInfo, TCHAR ch, int nFontIndex);
	LPCommonWordInfo	CreateDefaultWordInfo(TCHAR ch);
	//设置段排版等级
	void			SetParagraphChgLevel(int nLevel, int nParaIdx, BOOL bReset);
	//////////////////////////////////////////////////////////////////////////
	// Unit
	BaseGraphUnit*			GetUnit(int nIndex);
	BaseGraphUnit*			Get3DUnit(int nIndex);
	static void		CreateUnit(BaseGraphUnit* pUnit, LPCommonWordInfo pWordInfo, const static_effect* pFillColor);
	BaseGraphUnit*			Create3DUnitUse2DUnit(BaseGraphUnit* p2DUnit, BaseGraphUnit* pTempl3DUnit);
	BaseGraphUnit*			Create3DUnitUse2DUnit(BaseGraphUnit* p2DUnit, BaseGraphMaterial** p3DColor);
	void			Modify2DUnitTo3DUnit(BaseGraphUnit* p2DUnit, BaseGraphUnit* p3DUnit, BaseGraphMaterial** p3DColor);
	//////////////////////////////////////////////////////////////////////////
	// 定位
	// 击中测试
	BOOL			HitTestText(point2f& point,				//[IN]测试点
								BOOL bCaptureMouse,			//[IN]测试点是否允许出编辑框
								BOOL& bHorizontal				//[OUT]是否横排文字
								);
	// 轻量级击中测试，仅关心是否击中，不关心具体击中位置。
	BOOL			IsHitTextByPoint(point2f& pt,BOOL bCaptureMouse);
	// 细节击中测试，可以知道具体击中位置。//在所有段落中查找,bCaptureMouse 表示是否对在框外的点进行击中，
	int				GetCharPosFromPoint(BOOL& bIsLineEndPos, point2f& pt, BOOL bCaptureMouse);
	//仅在指定段落查找
	int				GetCharPosFromPoint(BOOL& bIsLineEndPos, point2f& pt, int nParaIdx, int nLineIdx, BOOL bCaptureMouse);
	//字符位置转换成段行索引
	void			ConvertCharPosToParaLineIdx(int& nParaIdx, int& nLineIdx, int nCharIndex, BOOL bIsLineEndPos);
	//取上一行/下一行
	void			GetParaLineIdxOfNextOrPreLine(int& nParaIdx, int& nLineIdx, BOOL bNextLine);
	BOOL			IsCharPosAtLineEnd(BOOL nCharIndex);	//字符对应的图形是否在行尾
	//构造光标图形
	BOOL			GetCaretGraphByCharPos(int nCharIndex, BOOL bIsLineEndPos, BOOL bFullHeight, vectorgraph* pGraph);
	//构造选区图形
	BOOL			GetSelGraphByCharPos(	int nBeginIndex, int nEndIndex, vectorgraph* pGraph);
	//构造选区空格占位图形
	BOOL			GetSelSpaceGraphByCharPos(int nBeginIndex, int nEndIndex, vectorgraph* pGraph);
	//光标信息//fCaretHalfWidth:光标半宽
	BOOL			GetCaretInfoByCharPos(	LPCommonGlyphSpace pCaretInfo, int nIndex, BOOL bIsLineEndPos, BOOL bFullHeight, float fCaretHalfWidth	);
	BOOL			GetCaretInfoByGlyphIndex(				//光标信息
								LPCommonGlyphSpace pCaretInfo, 
								int nParaIdx,				//段索引
								int nIndex,				//段内索引
								BOOL bIsLineEndPos, 
								BOOL bFullHeight, 
								float fCaretHalfWidth		//光标半宽
								);
	//选区空间信息
	BOOL			GetSelSpaceInfoByCharPos(int nBeginIndex, int nEndIndex, BaseARray* pArInfo);
	//选区空格空间信息
	BOOL			GetSelSpaceSpaceInfoByCharPos(int nBeginIndex, int nEndIndex, BaseARray* pArInfo);
	int				GetEndCaretCharPos();	//结尾光标位置
	//字符在标题字中的空间位置
	BOOL			GetCharPosSpaceInfo(int nCharIndex, LPCommonGlyphSpace pSpaceInfo);
	BOOL			SelectAllChar(int& nBeginIndex, int& nEndIndex);
	BOOL			ProcessDirectionKey(UINT uKey, int& nBeginIndex, int& nEndIndex,BOOL& bIsLineEndPos,BOOL bPressShift, BOOL bPressCtrl, LPVOID lpParam);
	BOOL			MoveCaretNext(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl);
	BOOL			MoveCaretPre(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl);
	BOOL			MoveCaretNextLine(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl);
	BOOL			MoveCaretPreLine(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl);
	BOOL			MoveCaretToHome(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl);
	BOOL			MoveCaretToEnd(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl);
protected:
	//////////////////////////////////////////////////////////////////////////
	// 排版
	void			ArtWordForm();						//艺术字排版
	void			PathTypesetForm();					//沿路径排版
	void			CalTypesetBorder();					//计算排版时实际使用的矩形
	void			SplitParagraph();
	void			GenerateWordInfo();
	void			GenerateParagraphInfo();
	void			ReleaseParagraph();
	void			SetParagraphPos();
	int				GenerateUnderLineInfo(BOOL bAllUnit = TRUE);
	void			UpdateUnderLineUnit(int nUnderLineCnt, BOOL bAllUnit = TRUE);
	void			UpdatePageInfo();
	void			UpdateUnitPos(int nUnderLineCnt, BOOL bAllUnit = TRUE);
	int				MakeSpecialEffect();				//特殊效果
	BOOL			IsNeedUpdateUnitArray();
	//////////////////////////////////////////////////////////////////////////
	// Unit
	void			Update2DUnitArray(int nLevel, BOOL bAllUnit = TRUE);
	void			Update3DUnitArray(int nLevel, BOOL bAllUnit = TRUE);
	int				GetUnitCount();
	int				Get3DUnitCount();
	void			AddUnit(BaseGraphUnit* pUnit);
	void			InsertUnit(BaseGraphUnit* pUnit, int nIndex, BaseGraphUnit* pTemp3DUnit);
	void			DeleteUnit(int nIndex, int nCount);
	void			DeleteAllUnit(WORD wdUnitType);
	void			ReleaseUnit(int nIndex, int nCount);
	void			ReleaseAllUnit();
	void			MP_OnDeleteString(int nIndex, int nCount, BOOL bWholeRender);
	void			MP_OnInsertString(int nIndex, int nCount);
	//////////////////////////////////////////////////////////////////////////
	// 同步Env
	void			OnLock();
	void			OnUnLock();
	void			OnAddUnit(BOOL bKeepBalance);
	void			OnInsertUnit(int nIndex, int nCount, BOOL bKeepBalance);
	void			OnDeleteUnit(int nIndex, int nCount, BOOL bKeepBalance);
	void			OnDeleteAllUnit();
	void			OnRefreshUnit(int nIndex, int nCount);
	void			OnRefreshAllUnit(int nLevel);

protected:
	LPCommonText			m_pTextObj;		//文本数据
	LPCommonTypesetPrivate	m_pTypesetPrivate;//排版中间数据
	int					m_chgIndex;	//变化开始位置
	int					m_chgUnitIndex;
	int					m_chgCnt;	//变化数量
	BOOL				m_bInsertReturn;	// 是否插入回车

	BOOL				m_bShowAllPage;
	VectPage*			m_pVecPageInfo;

	//属性临时交换数据
	BaseGraphTextProp			m_TextProp;
	BaseGraphFont				m_font;			//字体
	BaseGraphStaticEffect		m_fillColor;		//2D色
	BaseGraphMaterial			m_3DColor;		//3D色
	BaseGraphUnderLine			m_underLine;		//下划线

	//排版器的锁
	CRITICAL_SECTION	m_TypesetLock;
#ifdef _DEBUG
	int					m_nLockCount;
#endif
};
//////////////////////////////////////////////////////////////////////////
// API
//获取指定段的字符个数
 int		BaseGraphTypeset_GetParagraphCharCount(LPCommonText pTextObj, int nParagIdx);
//通过WordInfo获得Unit
 BaseGraphUnit*	BaseGraphTypeset_GetUnitByWordInfo(LPCommonText pTextObj, int nWordInfoIdx);
//当前排版实际使用的矩形
 void		BaseGraphTypeset_GetTypesetRect(LPCommonText pTextObj, rectf& rtTypeset, BOOL bReCal);
//获得段落数组
BASEGRAPH_DATARENDER_API BaseARray*	BaseGraphTypeset_GetTextParagraphArray(LPCommonText pTextObj);
//用Char生成WordInfo
 void		BaseGraphTypeset_GenerateWordInfo(LPCommonText pTextObj, LPCommonWordInfo pWordInfo, TCHAR ch, int nFontIndex);
 //////////////////////////////////////////////////////////////////////////
#endif	//_BaseGraphTYPESET_INC_
