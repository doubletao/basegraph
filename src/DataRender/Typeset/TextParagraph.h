#ifndef _SYTEXTPARAGRAPH_INC_
#define _SYTEXTPARAGRAPH_INC_
#include "TextUsp.h"
#include "TextTypeset.h"
//////////////////////////////////////////////////////////////////////////
class CBaseTextParagraph;
// CBaseTextLine
class  BASEGRAPH_DATARENDER_API CBaseTextLine
{
public:
	CBaseTextLine(CBaseTextParagraph* pTextParagraph, int nBeginIndex, float fLength);
	~CBaseTextLine();
	//////////////////////////////////////////////////////////////////////////
	// 属性存取
	LPSYText		GetTextObj();
	LPSYTextParagraph	GetParagraphObj();
	inline int	GetWordInfoCount() { return m_nWordInfoCount; }
	inline int	GetWordInfoBeginIndex() { return m_nBeginIndex; }
	inline void	SetWordInfoBeginIndex(int nBeginIndex) { m_nBeginIndex = nBeginIndex; }
	SYSizef*	GetRealSizePtr();
	float		GetBaseLineOffset();
	void		SetLineOffset(point2f ptOffset);
	point2f		GetLineOffset();
	float		GetLineDistance();
	void		SetLineDistance(float fDistance);
	//////////////////////////////////////////////////////////////////////////
	// 排版
	LPSYGlyphSpace	AddWordHorizontal(LPSYWordInfo pWordInfo,		//当前WordInfo
		LPSYWordInfo pLastWordInfo,									//上一次WordInfo
		point2f* pvGraphScale,										//放缩系数
		BOOL bAutoWrap												//自动折行
		);
	void	ResetHorizontal();
	LPSYGlyphSpace	AddWordVertical(LPSYWordInfo pWordInfo,LPSYWordInfo pLastWordInfo, point2f* pvGraphScale, BOOL bAutoWrap);
	void	ResetVertical();
	LPSYGlyphSpace	AddNullWord(LPSYWordInfo pWordInfo);
	void	ScaleNullLineGraph(float fGraphScaleX, float fGraphScaleY);
	void	ZoomUnitAndDistance(float fScaleW, float fScaleH);
	void	CalWordsAndLinePos();
	void	EndAlign(point2f* pvGraphScale);	//两端对齐排版
	//////////////////////////////////////////////////////////////////////////
	// 定位
	BOOL	IsHitTextByPoint(point2f& pt, BOOL bCaptureMouse, BOOL b2DTest);
	BOOL	IsReturnLine();
	//线性排版的简单测试
	int		GetWordInfoPosFromPoint(BOOL& bIsLineEndPos, point2f& pt, 
						WORD wTestLogic			//测试逻辑尺（0x00|限制行宽|XY双向坐标测试|CaptureMouse）
						);
	//非线性排版的复杂测试
	int		GetWordInfoPosFromPointEx(BOOL& bIsLineEndPos, point2f& pt, 
						WORD wTestLogic			//测试逻辑尺（同上）
						);
	BOOL	GetCaretInfoByGlyphIndex(LPSYGlyphSpace pCaretInfo, 
						int nIndex, BOOL bIsLineEndPos, BOOL bLinearTypeset,
						float fCaretHalfWidth	//光标半宽
						);
	BOOL	GetSelSpaceInfoByGlyphIndex(int nBeginIndex, int nEndIndex,BaseARray* pArInfo, BOOL bLinearTypeset);
	BOOL	GetGlyphSpaceInfo(int nIndex, LPSYGlyphSpace pSpaceInfo);
	BOOL	IsGlyphIndexAtLineEnd(int nIndex);
	BOOL	IsGlyphIndexInLine(int nIndex, BOOL bIsLineEndPos);
public:
	BaseARray	m_arGlyphSpace;		//空间信息
	BaseARray	m_arULInfo;			//下划线Info数组
private:
	CBaseTextParagraph*	m_pTextParagraph;		//行所在的段排版对象
	int				m_nBeginIndex;		//行的字形起始索引（相对于段）
	int				m_nWordInfoCount;		//本行字形数
	float			m_fLength;			//行的长度（此长度是由段指定的长度，而非实际长度）
	//以下均为临时计算值
	point2f			m_ptLineOffset;		//行相对于段的偏移
	SYSizef			m_RealSize;			//行实际包围大小（不含行距）
	float			m_fBaseLineOffset;	//基线的偏移量，相对于行
	float			m_fLineDistance;		//行间距（与从属段的行间距值不同，已经过放缩等调整。）
	float			m_fMaxAscent;
	float			m_fMaxDescent;
};
//////////////////////////////////////////////////////////////////////////
// CBaseTextParagraph
class  BASEGRAPH_DATARENDER_API CBaseTextParagraph
{
public:
	CBaseTextParagraph(LPSYText pTextObj, int nParaIndex);
	~CBaseTextParagraph();
	//////////////////////////////////////////////////////////////////////////
	// 属性存取
	inline void		SetParagraphIndex(int nParaIndex) { m_nParaIndex = nParaIndex; }
	inline int		GetParagraphIndex() { return m_nParaIndex; }
	inline void		SetParagraphInfo(LPSYText pTextObj, int nParaIndex)
					{
							m_pTextObj = pTextObj;
							m_nParaIndex = nParaIndex;
					}
	inline void		SetTextObj(LPSYText pTextObj) { m_pTextObj = pTextObj; }
	LPSYText			GetTextObj();
	LPSYTextParagraph	GetParagraphObj();
	BaseARray*			GetLineArray() { return &m_arLine; }
	int				GetLineCount() { return BaseAR_GetCount(&m_arLine); }

	float	GetRealHeight();
	float	GetRealWidth();
	float	GetHeight();
	float	GetWidth();
	void	SetOffset(point2f ptOffset);
	point2f	GetOffset();
	rectf	GetRect();
	rectf*	GetBorderRect() { return &m_rtBorder; }

	float	GetIndent();
	float	GetSymbolWidth();
	float	GetSymbolHeight();

	BOOL		IsLineWrap();
	BOOL		IsFixedFrame();
	BOOL		IsHorizontal();
	BOOL		IsLeftInput();

	int			GetCharCount();
	int			GetWordInfoCount();
	LPSYWordInfo	GetWordInfo(int nIndex);
	int			GetLineWordInfoCount(int nLineIdx);
	int			GetTrailLineWordInfoBeginIndex();
	SYUnit*		GetUnit(int nWordInfoIdx);
	//////////////////////////////////////////////////////////////////////////
	// 排版
	void	Typeset(BOOL bIsHorizontal);
	void	UpdateWordInfoArray(LPSYText pTextObj);
	void	ZoomUnitAndDistance(float fScaleW, float fScaleH);
	void	OffsetCal(vector3f* pOffset);
	//////////////////////////////////////////////////////////////////////////
	// 定位
	void	ConvertCharPosToLineIdx(int& nLineIdx, int nCharIndex, BOOL bIsLineEndPos);
	BOOL	IsCharPosAtLineEnd(BOOL nCharIndex);
	BOOL	IsHitTextByPoint(point2f& pt, BOOL bCaptureMouse, BOOL bLinearTypeset);
	int		GetCharPosFromPoint(BOOL& bIsLineEndPos, point2f& pt, BOOL bCaptureMouse, BOOL bLinearTypeset);
	int		GetCharPosFromPoint(BOOL& bIsLineEndPos, point2f& pt, int nLineIdx,BOOL bCaptureMouse, BOOL bLinearTypeset);
	BOOL	GetCaretInfoByCharPos(LPSYGlyphSpace pCaretInfo, int nCharIndex, 
						BOOL bIsLineEndPos, 
						BOOL bFullHeight,
						BOOL bLinearTypeset,		//是否线性编辑
						float fCaretHalfWidth		//光标半宽
						);
	BOOL	GetCaretInfoByGlyphIndex(LPSYGlyphSpace pCaretInfo, 
						int nGlyphIndex, 
						BOOL bIsLineEndPos, 
						BOOL bFullHeight, 
						BOOL bLinearTypeset, 
						float fCaretHalfWidth		//光标半宽
						);
	BOOL	GetSelSpaceInfoByCharPos(int nBeginIndex, int nEndIndex, BaseARray* pArInfo, BOOL bLinearTypeset);
	BOOL	GetCharPosSpaceInfo(int nCharIndex, LPSYGlyphSpace pSpaceInfo);
private:
	void	ReleaseTempData();
	void	TypesetHorizontal();
	void	TypesetVertical();
	void	AddBlankLine(CBaseTextLine* pLine);
public:
	SYSCRIPTRESULT	m_uspResult;
	BaseARray			m_arWordInfo;			//字符WordInfo数组
	int				m_nPreAllWordInfoCnt;	//本段之前所有WordInfo元素个数（便于推算Unit）
	int				m_nChgLevel;			//修改等级
private:
	LPSYText		m_pTextObj;			//段所属的SYText
	int				m_nParaIndex;			//段索引
	float			m_fWidth;			//占位符宽度
	float			m_fHeight;
	float			m_fRealWidth;			//字符实际占据的宽度
	float			m_fRealHeight;		//..............高度
	point2f			m_ptOffset;
	rectf			m_rtBorder;			//段占据的矩形
	BaseARray			m_arLine;
};
#endif //_SYTEXTPARAGRAPH_INC_