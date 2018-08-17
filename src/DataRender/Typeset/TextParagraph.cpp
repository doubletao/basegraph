#include "stdafx.h"
#include "TextParagraph.h"
#include "TextTypeset.h"
#include "TextTypesetDataAPI.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////
// CBaseTextLine
CBaseTextLine::CBaseTextLine(CBaseTextParagraph* pTextParagraph, int nBeginIndex, float fLength)
{
	BaseAR_Init(&m_arGlyphSpace);
	BaseAR_Init(&m_arULInfo);

	m_pTextParagraph     = pTextParagraph;
	m_nBeginIndex        = nBeginIndex;
	m_nWordInfoCount     = 0;

	m_fLength            = fLength;
	m_ptLineOffset.x     = 0;
	m_ptLineOffset.y     = 0;
	m_RealSize.cx        = 0;
	m_RealSize.cy        = 0;
	m_fBaseLineOffset    = 0;
	m_fLineDistance      = 0;
	m_fMaxAscent         = 0;
	m_fMaxDescent        = 0;
}

CBaseTextLine::~CBaseTextLine()
{
	BaseGraphGlyphSpace_ReleaseAll(m_arGlyphSpace);
	//下划线
	int nCnt;
	nCnt = BaseAR_GetCount(&m_arULInfo);
	for (int i = 0; i < nCnt; ++i)
	{
		LPCommonUnderLineInfo pULInfo = (LPCommonUnderLineInfo)BaseAR_GetAt(&m_arULInfo, i);
		delete pULInfo;
	}
	BaseAR_RemoveAll(&m_arULInfo);
}

LPCommonText CBaseTextLine::GetTextObj()
{
	return m_pTextParagraph->GetTextObj();
}

LPCommonTextParagraph CBaseTextLine::GetParagraphObj()
{
	return m_pTextParagraph->GetParagraphObj();
}

BaseGraphSizef* CBaseTextLine::GetRealSizePtr()
{
	return &m_RealSize;
}

float CBaseTextLine::GetBaseLineOffset()
{
	return m_fBaseLineOffset;
}

void CBaseTextLine::SetLineOffset(point2f ptOffset)
{
	m_ptLineOffset = ptOffset;
}

point2f CBaseTextLine::GetLineOffset()
{
	return m_ptLineOffset;
}

float CBaseTextLine::GetLineDistance()
{
	return m_fLineDistance;
}

void CBaseTextLine::SetLineDistance(float fDistance)
{
	m_fLineDistance = fDistance;
}

LPCommonGlyphSpace CBaseTextLine::AddWordHorizontal(LPCommonWordInfo pWordInfo, 
											  LPCommonWordInfo pLastWordInfo, 
											  point2f* pvGraphScale, 
											  BOOL bAutoWrap)
{
	//行长度增量
	//可视且非零宽度字符或者注音符，则累算宽度。
	//此乃经验算法，尚未获得资料说明ZWJ、NZWJ和Diacritic之间的明确关系。
	float fIncrement;
	float fABC;
	float fDistance = 0;
	if (pWordInfo->bVisible && (!pWordInfo->fZeroWidth || pWordInfo->fDiacritic))
	{
		fABC = pWordInfo->rtCell.right - pWordInfo->rtCell.left;
		fABC *= pvGraphScale->x;
		//本行上一个字形，字距仅允许附加在簇之间。
		if (pLastWordInfo != NULL)
		{
			//左打文字，簇分界依据是当前字；右打文字，簇分解依据是上一个字。
			//字距始终使用上一个字的值。
			if (pLastWordInfo->fClusterStart)
				fDistance = pLastWordInfo->fDistance * pvGraphScale->x;
		}
		fIncrement = fABC + fDistance;
	}
	else if (pWordInfo->bVisible && pWordInfo->wChar == VK_TAB)
	{
		// 此Tab宽度为上一个字符结尾到下一个Tab位置
		int nTabWidth = TYPESET_TABLEN;
		int nCurLineWidth = m_RealSize.cx;
		int nTabIndex = nCurLineWidth / nTabWidth;
		nTabIndex++;

		fABC = nTabWidth * nTabIndex - nCurLineWidth;
		fABC *= pvGraphScale->x;
		// 根据上下文确定长度
		if (pLastWordInfo != NULL)
		{
			//左打文字，簇分界依据是当前字；右打文字，簇分解依据是上一个字。
			//字距始终使用上一个字的值。
			if (pLastWordInfo->fClusterStart)
				fDistance = pLastWordInfo->fDistance * pvGraphScale->x;
		}
		fIncrement = fABC + fDistance;
	}
	else
	{
		fABC = 0;
		fIncrement = 0;
	}

	LPCommonGlyphSpace pSpaceInfo;
	//当前实际行宽
	float fLineRealW = m_RealSize.cx;
	//试算的新行宽（试算值可能不合理，所有不一定使用。）
	float fLineNewW = fLineRealW + fIncrement;
	float fTemp;

	if (!bAutoWrap || 
		fLineNewW <= m_fLength || fLineRealW <= ABS_FLOAT_0 || fIncrement <= ABS_FLOAT_0)
	{
		m_nWordInfoCount++;
		pSpaceInfo = new BaseGraphGlyphSpace;
		memset(pSpaceInfo, 0, sizeof(BaseGraphGlyphSpace));

		//增量禁止倒流
		if (fLineNewW >= fLineRealW)
		{
			fTemp = fLineRealW + fDistance;
			pSpaceInfo->ptPos.x = fTemp;
			pSpaceInfo->ptBound[0].x = fTemp;
			pSpaceInfo->ptBound[3].x = fTemp;

			m_RealSize.cx = fLineNewW;
			pSpaceInfo->ptBound[1].x = fLineNewW;
			pSpaceInfo->ptBound[2].x = fLineNewW;
		}
		else
		{
			pSpaceInfo->ptPos.x = fLineRealW;
			pSpaceInfo->ptBound[0].x = fLineRealW;
			pSpaceInfo->ptBound[3].x = fLineRealW;

			fTemp = fLineRealW + fABC;
			pSpaceInfo->ptBound[1].x = fTemp;
			pSpaceInfo->ptBound[2].x = fTemp;
		}

		fTemp = -pWordInfo->rtCell.top;
		m_fMaxAscent = max(m_fMaxAscent, fTemp);
		fTemp = pWordInfo->rtCell.bottom;
		m_fMaxDescent = max(m_fMaxDescent, fTemp);

		BaseAR_Add(&m_arGlyphSpace, pSpaceInfo);
		return pSpaceInfo;
	}
	return NULL;
}

// 将左打加入的行元素位置颠倒过来
void CBaseTextLine::ResetHorizontal()
{
	BaseARray	arGlyphSpace;
	BaseAR_Init(&arGlyphSpace);
	int spaceCnt = BaseAR_GetCount(&m_arGlyphSpace);
	BaseGraphGlyphSpace* pLastSpace = NULL;
	float fLastXPos = 0;
	float fDistance = 0;
	for (int i=spaceCnt-1; i>=0; i--)
	{
		BaseGraphGlyphSpace* pSpaceInfo = (BaseGraphGlyphSpace*)BaseAR_GetAt(&m_arGlyphSpace, i);
		if (i >= 1)
		{
			BaseGraphGlyphSpace* pLastSpace = (BaseGraphGlyphSpace*)BaseAR_GetAt(&m_arGlyphSpace, i-1);
			fDistance = pSpaceInfo->ptBound[0].x - pLastSpace->ptBound[1].x;
		}
		int wordWidth = pSpaceInfo->ptBound[1].x - pSpaceInfo->ptBound[0].x;

		pSpaceInfo->ptPos.x = fLastXPos;
		pSpaceInfo->ptBound[0].x = fLastXPos;
		pSpaceInfo->ptBound[3].x = fLastXPos;
		pSpaceInfo->ptBound[1].x = fLastXPos + wordWidth;
		pSpaceInfo->ptBound[2].x = fLastXPos + wordWidth;

		fLastXPos += wordWidth;
		fLastXPos += fDistance;
		BaseAR_Add(&arGlyphSpace, pSpaceInfo);
	}

	for (int i=0; i<spaceCnt; i++)
	{
		BaseGraphGlyphSpace* pSpace = (BaseGraphGlyphSpace*)BaseAR_GetAt(&arGlyphSpace, i);
		BaseAR_SetAt(&m_arGlyphSpace, i, pSpace);
	}
}

LPCommonGlyphSpace CBaseTextLine::AddWordVertical(LPCommonWordInfo pWordInfo, 
											LPCommonWordInfo pLastWordInfo, 
											point2f* pvGraphScale, 
											BOOL bAutoWrap)
{
	float fIncrement;
	float fABC;
	float fDistance = 0;
	if (pWordInfo->bVisible && (!pWordInfo->fZeroWidth || pWordInfo->fDiacritic))
	{
		fABC = pWordInfo->rtCell.bottom - pWordInfo->rtCell.top;
		fABC *= pvGraphScale->y;
		if (pLastWordInfo != NULL)
		{
			if (pLastWordInfo->fClusterStart)
				fDistance = pLastWordInfo->fDistance * pvGraphScale->y;
		}
		fIncrement = fABC + fDistance;
	}
	else if (pWordInfo->bVisible && pWordInfo->wChar == VK_TAB)
	{
		// 此Tab宽度为上一个字符结尾到下一个Tab位置
		int nTabWidth = TYPESET_TABLEN;
		int nCurLineWidth = m_RealSize.cy;
		int nTabIndex = nCurLineWidth / nTabWidth;
		nTabIndex++;

		fABC = nTabWidth * nTabIndex - nCurLineWidth;
		fABC *= pvGraphScale->y;
		// 根据上下文确定长度
		if (pLastWordInfo != NULL)
		{
			//左打文字，簇分界依据是当前字；右打文字，簇分解依据是上一个字。
			//字距始终使用上一个字的值。
			if (pLastWordInfo->fClusterStart)
				fDistance = pLastWordInfo->fDistance * pvGraphScale->y;
		}
		fIncrement = fABC + fDistance;
	}
	else
	{
		fABC = 0;
		fIncrement = 0;
	}

	LPCommonGlyphSpace pSpaceInfo;
	float fLineRealW = m_RealSize.cy;
	float fLineNewW = fLineRealW + fIncrement;
	float fTemp;

	if (!bAutoWrap || 
		fLineNewW <= m_fLength || fLineRealW <= ABS_FLOAT_0 || fIncrement <= ABS_FLOAT_0)
	{
		m_nWordInfoCount++;
		pSpaceInfo = new BaseGraphGlyphSpace;
		memset(pSpaceInfo, 0, sizeof(BaseGraphGlyphSpace));

		//增量禁止倒流
		if (fLineNewW >= fLineRealW)
		{
			fTemp = fLineRealW + fDistance;
			pSpaceInfo->ptPos.y = fTemp;
			pSpaceInfo->ptBound[0].y = fTemp;
			pSpaceInfo->ptBound[1].y = fTemp;

			m_RealSize.cy = fLineNewW;
			pSpaceInfo->ptBound[2].y = fLineNewW;
			pSpaceInfo->ptBound[3].y = fLineNewW;
		}
		else
		{
			pSpaceInfo->ptPos.y = fLineRealW;
			pSpaceInfo->ptBound[0].y = fLineRealW;
			pSpaceInfo->ptBound[1].y = fLineRealW;

			fTemp = fLineRealW + fABC;
			pSpaceInfo->ptBound[2].y = fTemp;
			pSpaceInfo->ptBound[3].y = fTemp;
		}

 		if (pWordInfo->wChar != _T(' '))
		{
			fTemp = pWordInfo->rtCell.right;
			m_fMaxAscent = max(m_fMaxAscent, fTemp);
			fTemp = -pWordInfo->rtCell.left;
			m_fMaxDescent = max(m_fMaxDescent, fTemp);
		}
		else
		{
			// 竖排时空格会影响宽度
			fTemp = pWordInfo->rtCell.right * 0.5;
			m_fMaxAscent = max(m_fMaxAscent, fTemp);
			m_fMaxDescent = max(m_fMaxDescent, 0);
		}

		BaseAR_Add(&m_arGlyphSpace, pSpaceInfo);
		return pSpaceInfo;
	}
	return NULL;
}

// 将左打加入的行元素位置颠倒过来
void CBaseTextLine::ResetVertical()
{
	BaseARray	arGlyphSpace;
	BaseAR_Init(&arGlyphSpace);
	int spaceCnt = BaseAR_GetCount(&m_arGlyphSpace);
	BaseGraphGlyphSpace* pLastSpace = NULL;
	float fLastYPos = 0;
	for (int i=spaceCnt-1; i>=0; i--)
	{
		BaseGraphGlyphSpace* pSpaceInfo = (BaseGraphGlyphSpace*)BaseAR_GetAt(&m_arGlyphSpace, i);
		int wordHeight = pSpaceInfo->ptBound[3].y - pSpaceInfo->ptBound[0].y;

		pSpaceInfo->ptPos.y = fLastYPos;
		pSpaceInfo->ptBound[0].y = fLastYPos;
		pSpaceInfo->ptBound[1].y = fLastYPos;
		pSpaceInfo->ptBound[2].y = fLastYPos + wordHeight;
		pSpaceInfo->ptBound[3].y = fLastYPos + wordHeight;

		fLastYPos += wordHeight;
		BaseAR_Add(&arGlyphSpace, pSpaceInfo);
	}

	for (int i=0; i<spaceCnt; i++)
	{
		BaseGraphGlyphSpace* pSpace = (BaseGraphGlyphSpace*)BaseAR_GetAt(&arGlyphSpace, i);
		BaseAR_SetAt(&m_arGlyphSpace, i, pSpace);
	}
}

LPCommonGlyphSpace CBaseTextLine::AddNullWord(LPCommonWordInfo pWordInfo)
{
	//添加“空字”，只是记录字的参数信息。
	LPCommonGlyphSpace pSpaceInfo = new BaseGraphGlyphSpace;
	memset(pSpaceInfo, 0, sizeof(BaseGraphGlyphSpace));
	BaseAR_Add(&m_arGlyphSpace, pSpaceInfo);
	float fTempAD;
	if (m_pTextParagraph->IsHorizontal())
	{
		fTempAD = -pWordInfo->rtCell.top;
		m_fMaxAscent = max(m_fMaxAscent, fTempAD);
		fTempAD = pWordInfo->rtCell.bottom;
		m_fMaxDescent = max(m_fMaxDescent, fTempAD);
	}
	else
	{
		fTempAD = pWordInfo->rtCell.right;
		m_fMaxAscent = max(m_fMaxAscent, fTempAD);
		fTempAD = -pWordInfo->rtCell.left;
		m_fMaxDescent = max(m_fMaxDescent, fTempAD);
	}
	return pSpaceInfo;
}

void CBaseTextLine::ScaleNullLineGraph(float fGraphScaleX, float fGraphScaleY)
{
	//缩放带来的调整
	LPCommonGlyphSpace pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, 0);
	float fTemp;
	if (m_pTextParagraph->IsHorizontal())
	{
		m_fLineDistance *= fGraphScaleY;
		fTemp = (m_fMaxAscent + m_fMaxDescent) * fGraphScaleY;
		pSpaceInfo->ptBound[3].y = fTemp;
		pSpaceInfo->ptBound[2].y = fTemp;
		pSpaceInfo->ptBound[5].y = fTemp;
	}
	else
	{
		m_fLineDistance *= fGraphScaleX;
		fTemp = (m_fMaxAscent + m_fMaxDescent) * fGraphScaleX;
		pSpaceInfo->ptBound[1].x = fTemp;   
		pSpaceInfo->ptBound[2].x = fTemp;
		pSpaceInfo->ptBound[4].x = fTemp;
	}
}

void CBaseTextLine::ZoomUnitAndDistance(float fScaleW, float fScaleH)
{
	if (m_pTextParagraph->IsHorizontal())
	{
		m_fLineDistance *= fScaleH;
		m_ptLineOffset.y *= fScaleH;
		m_fMaxAscent *= fScaleH;
		m_fBaseLineOffset = m_fMaxAscent;
	}
	else
	{
		m_fLineDistance *= fScaleW;
		m_ptLineOffset.x *= fScaleW;
		m_fMaxAscent *= fScaleW;
		m_fBaseLineOffset = m_fMaxAscent;
	}

	LPCommonGlyphSpace pSpaceInfo;
	for (int i = 0; i < m_nWordInfoCount; ++i)
	{
		//缩放图形
		BaseGraphUnit* pUnit = m_pTextParagraph->GetUnit(m_nBeginIndex + i);
		vg_zoom(&(pUnit->u.u2d.graph), fScaleW, fScaleH, 1, 0, 0, 0);

		//位置
		pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
		pSpaceInfo->ptPos.x *= fScaleW;
		pSpaceInfo->ptPos.y *= fScaleH;
		pSpaceInfo->ptPosOffset.x *= fScaleW;
		pSpaceInfo->ptPosOffset.y *= fScaleH;
		for (int j = 0; j < BaseGraphGLYPHSPACEPTs_EX; ++j)
		{
			pSpaceInfo->ptBound[j].x *= fScaleW;
			pSpaceInfo->ptBound[j].y *= fScaleH;
		}
	}

	//最后一个
	pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, m_nWordInfoCount);
	pSpaceInfo->ptPos.x *= fScaleW;
	pSpaceInfo->ptPos.y *= fScaleH;
	pSpaceInfo->ptPosOffset.x *= fScaleW;
	pSpaceInfo->ptPosOffset.y *= fScaleH;
	for (int j = 0; j < BaseGraphGLYPHSPACEPTs_EX; ++j)
	{
		pSpaceInfo->ptBound[j].x *= fScaleW;
		pSpaceInfo->ptBound[j].y *= fScaleH;
	}

	m_RealSize.cx *= fScaleW;
	m_RealSize.cy *= fScaleH;
}

void CBaseTextLine::CalWordsAndLinePos()
{
	//允许存在空行
	int nSpaceCnt = BaseAR_GetCount(&m_arGlyphSpace);
	if (nSpaceCnt < 1)
		return;

	LPCommonText pTextObj = GetTextObj();
	BOOL bHorizontal = m_pTextParagraph->IsHorizontal();
	float fGraphScale = bHorizontal ? pTextObj->vGraphScale.x : pTextObj->vGraphScale.y;

	float fTemp;
	float fLineH;
	if (bHorizontal)
	{
		m_fMaxAscent  *= pTextObj->vGraphScale.y;
		m_fMaxDescent *= pTextObj->vGraphScale.y;
		fLineH = m_fMaxAscent + m_fMaxDescent;
		//行高
		m_RealSize.cy = fLineH;
	}
	else
	{
		m_fMaxAscent  *= pTextObj->vGraphScale.x;
		m_fMaxDescent *= pTextObj->vGraphScale.x;
		fLineH = m_fMaxAscent + m_fMaxDescent;
		m_RealSize.cx = fLineH;
	}
	//基线偏移
	m_fBaseLineOffset = m_fMaxAscent;

	LPCommonGlyphSpace pSrcSpaceInfo;
	LPCommonGlyphSpace pDesSpaceInfo;
	LPCommonWordInfo pWordInfo;
	if (bHorizontal)
	{
		for (int i = 0; i < m_nWordInfoCount; ++i)
		{
			pDesSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			pDesSpaceInfo->ptPos.y = m_fBaseLineOffset;
			//扩展位置取下一字形的左侧
			if (i+1 < nSpaceCnt)
			{
				pSrcSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i+1);
				pDesSpaceInfo->ptBound[4].x = pSrcSpaceInfo->ptBound[0].x;
				pDesSpaceInfo->ptBound[5].x = pSrcSpaceInfo->ptBound[3].x;
			}
			else
			{
				pDesSpaceInfo->ptBound[4].x = pDesSpaceInfo->ptBound[1].x;
				pDesSpaceInfo->ptBound[5].x = pDesSpaceInfo->ptBound[2].x;
			}
			//下边界
			pDesSpaceInfo->ptBound[2].y = fLineH;
			pDesSpaceInfo->ptBound[3].y = fLineH;
			pDesSpaceInfo->ptBound[5].y = fLineH;
		}
	}
	else
	{
		for (int i = 0; i < m_nWordInfoCount; ++i)
		{
			pWordInfo = m_pTextParagraph->GetWordInfo(m_nBeginIndex + i);
			pDesSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			pDesSpaceInfo->ptPos.x = fLineH - m_fBaseLineOffset;

			//非旋转Y方向偏移增量
			if (!pWordInfo->bVRollGlyph)
			{
				pDesSpaceInfo->ptPosOffset.y = (-pWordInfo->rtCell.top)*fGraphScale;
			}

			//扩展位置取下一字形的上侧
			if (i+1 < nSpaceCnt)
			{
				pSrcSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i+1);
				pDesSpaceInfo->ptBound[4].y = pSrcSpaceInfo->ptBound[1].y;
				pDesSpaceInfo->ptBound[5].y = pSrcSpaceInfo->ptBound[0].y;
			}
			else
			{
				pDesSpaceInfo->ptBound[4].y = pDesSpaceInfo->ptBound[2].y;
				pDesSpaceInfo->ptBound[5].y = pDesSpaceInfo->ptBound[3].y;
			}
			//右边界
			pDesSpaceInfo->ptBound[1].x = fLineH;
			pDesSpaceInfo->ptBound[2].x = fLineH;
			pDesSpaceInfo->ptBound[4].x = fLineH;
		}
	}

	//在尾部追加一个
	pSrcSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nSpaceCnt-1);
	pDesSpaceInfo = new BaseGraphGlyphSpace;
	memcpy(pDesSpaceInfo, pSrcSpaceInfo, sizeof(BaseGraphGlyphSpace));
	BaseAR_Add(&m_arGlyphSpace, pDesSpaceInfo);
	if (bHorizontal)
	{
		//X坐标全部用最后一个字形扩展位置填充
		fTemp = pSrcSpaceInfo->ptBound[4].x;
		pDesSpaceInfo->ptBound[0].x = fTemp;
		pDesSpaceInfo->ptBound[1].x = fTemp;
		pDesSpaceInfo->ptBound[2].x = fTemp;
		pDesSpaceInfo->ptBound[3].x = fTemp;
		pDesSpaceInfo->ptBound[4].x = fTemp;
		pDesSpaceInfo->ptBound[5].x = fTemp;
		pDesSpaceInfo->ptPos.x = fTemp;
	}
	else
	{
		//Y坐标全部用最后一个字形扩展位置填充
		fTemp = pSrcSpaceInfo->ptBound[4].y;
		pDesSpaceInfo->ptBound[0].y = fTemp;
		pDesSpaceInfo->ptBound[1].y = fTemp;
		pDesSpaceInfo->ptBound[2].y = fTemp;
		pDesSpaceInfo->ptBound[3].y = fTemp;
		pDesSpaceInfo->ptBound[4].y = fTemp;
		pDesSpaceInfo->ptBound[5].y = fTemp;
		pDesSpaceInfo->ptPos.y = fTemp;
	}
}

void CBaseTextLine::EndAlign(point2f* pvGraphScale)
{
	//两端对齐排版
	float fRealLength = 0.f;
	float fLastCharABC = 0.f;
	int nSearchI = -1;

	LPCommonGlyphSpace pGlyphSpace;
	BOOL bHorizontal = m_pTextParagraph->IsHorizontal();
	if (bHorizontal)
	{
		for (int i=m_nWordInfoCount-1; i>=0; --i)
		{
			LPCommonWordInfo pWordInfo = m_pTextParagraph->GetWordInfo(m_nBeginIndex+i);
			if (pWordInfo->bVisible)
			{
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
				fRealLength = pGlyphSpace->ptPos.x;
				fLastCharABC = pWordInfo->rtCell.right - pWordInfo->rtCell.left;
				fLastCharABC *= pvGraphScale->x;
				nSearchI = i;
				break;
			}
		}
	}
	else
	{
		for (int i=m_nWordInfoCount-1; i>=0; --i)
		{
			LPCommonWordInfo pWordInfo = m_pTextParagraph->GetWordInfo(m_nBeginIndex+i);
			if (pWordInfo->bVisible)
			{
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
				fRealLength = pGlyphSpace->ptPos.y;
				fLastCharABC = pWordInfo->rtCell.bottom - pWordInfo->rtCell.top;
				fLastCharABC *= pvGraphScale->y;
				nSearchI = i;
				break;
			}
		}
	}

	if (fRealLength < ABS_FLOAT_0)
		return;

	//计算一个缩放比例
	float fScale;
	float fPosOffset;
	fScale = (m_fLength - fLastCharABC) / fRealLength;
	if (bHorizontal)
	{
		for (int i = 0; i <= nSearchI; i++)
		{
			pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			fPosOffset = pGlyphSpace->ptPos.x;
			pGlyphSpace->ptPos.x *= fScale;
			fPosOffset = pGlyphSpace->ptPos.x - fPosOffset;
			for (int j = 0; j < BaseGraphGLYPHSPACEPTs_EX; ++j)
			{
				pGlyphSpace->ptBound[j].x += fPosOffset;
			}
		}
		//尾部的不可见字符没有比例关系
		LPCommonGlyphSpace pSearchGS = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nSearchI);
		for (int i = nSearchI+1; i <= m_nWordInfoCount; i++)
		{
			pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			pGlyphSpace->ptPos.x = pSearchGS->ptPos.x + fLastCharABC;
			pGlyphSpace->ptBound[0].x = pSearchGS->ptBound[1].x;
			pGlyphSpace->ptBound[3].x = pSearchGS->ptBound[2].x;
			pGlyphSpace->ptBound[1].x = pSearchGS->ptBound[1].x;
			pGlyphSpace->ptBound[2].x = pSearchGS->ptBound[2].x;
			pGlyphSpace->ptBound[4].x = pGlyphSpace->ptBound[1].x;
			pGlyphSpace->ptBound[5].x = pGlyphSpace->ptBound[2].x;
		}

		//修正行实际尺寸
		pSearchGS = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, m_nWordInfoCount);
		m_RealSize.cx = pSearchGS->ptPos.x;
	}
	else
	{
		for (int i = 0; i <= nSearchI; i++)
		{
			pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			fPosOffset = pGlyphSpace->ptPos.y;
			pGlyphSpace->ptPos.y *= fScale;
			fPosOffset = pGlyphSpace->ptPos.y - fPosOffset;
			for (int j = 0; j < BaseGraphGLYPHSPACEPTs_EX; ++j)
			{
				pGlyphSpace->ptBound[j].y += fPosOffset;
			}
		}

		LPCommonGlyphSpace pSearchGS = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nSearchI);
		for (int i = nSearchI+1; i <= m_nWordInfoCount; i++)
		{
			pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			pGlyphSpace->ptPos.y = pSearchGS->ptPos.y + fLastCharABC;
			pGlyphSpace->ptBound[0].y = pSearchGS->ptBound[3].y;
			pGlyphSpace->ptBound[1].y = pSearchGS->ptBound[2].y;
			pGlyphSpace->ptBound[3].y = pSearchGS->ptBound[3].y;
			pGlyphSpace->ptBound[2].y = pSearchGS->ptBound[2].y;
			pGlyphSpace->ptBound[5].y = pGlyphSpace->ptBound[3].y;
			pGlyphSpace->ptBound[4].y = pGlyphSpace->ptBound[2].y;
		}

		pSearchGS = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, m_nWordInfoCount);
		m_RealSize.cy = pSearchGS->ptPos.y;
	}
}

BOOL CBaseTextLine::IsHitTextByPoint(point2f& pt, BOOL bCaptureMouse, BOOL b2DTest)
{
	rectf rtLine;
	float fLength;

	LPCommonText pTextObj = m_pTextParagraph->GetTextObj();
	BOOL bOtherCor;		//另一个坐标是否已经满足条件
	if (pTextObj->bHorizontal)
	{
		if (bCaptureMouse)
			fLength = max(m_fLength, m_RealSize.cx);
		else
			fLength = m_fLength;

		BaseRectf_SetRect(&rtLine, 0, m_ptLineOffset.y, fLength, m_ptLineOffset.y + m_RealSize.cy + m_fLineDistance);
		if (b2DTest)
		{
			if (pt.y >= rtLine.top && pt.y <= rtLine.bottom)
				bOtherCor = TRUE;
			else
				bOtherCor = FALSE;
		}
		else
		{
			bOtherCor = TRUE;
		}
		if (bOtherCor && pt.x >= rtLine.left && pt.x <= rtLine.right)
		{
			return TRUE;
		}
	}
	else
	{
		if (bCaptureMouse)
			fLength = max(m_fLength, m_RealSize.cy);
		else
			fLength = m_fLength;

		if (pTextObj->bLeftInput)
			BaseRectf_SetRect(&rtLine, m_ptLineOffset.x, 0, m_ptLineOffset.x + m_RealSize.cx + m_fLineDistance, fLength);
		else
			BaseRectf_SetRect(&rtLine, m_ptLineOffset.x - m_fLineDistance, 0, m_ptLineOffset.x + m_RealSize.cx, fLength);

		if (b2DTest)
		{
			if (pt.x >= rtLine.left && pt.x <= rtLine.right)
				bOtherCor = TRUE;
			else
				bOtherCor = FALSE;
		}
		else
		{
			bOtherCor = TRUE;
		}

		if (bOtherCor && pt.y >= rtLine.top && pt.y <= rtLine.bottom)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBaseTextLine::IsReturnLine()
{
	LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextParagraph->GetTextObj()->arParagraph, m_pTextParagraph->GetParagraphIndex());
	int startPos = pParagObj->nBeginIndex + m_nBeginIndex;
	CString strText = m_pTextParagraph->GetTextObj()->szText;
	strText = strText.Mid(startPos, m_nWordInfoCount);
	int findRet = strText.Find(_T("\r\n"));
	if (findRet != -1)
	{
		return TRUE;
	}
	return FALSE;
}

int CBaseTextLine::GetWordInfoPosFromPoint(BOOL& bIsLineEndPos, 
										 point2f& pt, 
										 WORD wTestLogic)
{
	//反馈调用者，是否击中的是行最后位置。
	bIsLineEndPos = FALSE;

	rectf rtLine;
	int nHitI = -1;
	float fLength;

	LPCommonText pTextObj = m_pTextParagraph->GetTextObj();
	BOOL b2DTest = wTestLogic & 0x0002;
	BOOL bCheckLength = wTestLogic & 0x0004;
	BOOL bOtherCor;		//另一个坐标是否已经满足条件
	if (pTextObj->bHorizontal)
	{
		if (wTestLogic & 0x0001)
			fLength = max(m_fLength, m_RealSize.cx);
		else
			fLength = m_fLength;
		BaseRectf_SetRect(&rtLine, 0, m_ptLineOffset.y, fLength, m_ptLineOffset.y + m_RealSize.cy + m_fLineDistance);
		if (b2DTest)
		{
			if (pt.y >= rtLine.top && pt.y <= rtLine.bottom)
				bOtherCor = TRUE;
			else
				bOtherCor = FALSE;
		}
		else
		{
			bOtherCor = TRUE;
		}
		if (bOtherCor && ((!bCheckLength) || (pt.x >= rtLine.left && pt.x <= rtLine.right)))
		{
			if (m_nWordInfoCount < 1)
				return 0;

			rectf rtWord;
			float fLeft, fCenter, fRight;
			float fTop = rtLine.top;
			float fBottom = rtLine.bottom;

			for (int i = 0; i <= m_nWordInfoCount; i++)
			{
				LPCommonGlyphSpace pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
				float fWordOffset = pSpaceInfo->ptBound[0].x;
				fCenter = m_ptLineOffset.x + fWordOffset;
				if (i > 0)
				{
					pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i-1);
					fLeft = fCenter - (fWordOffset - pSpaceInfo->ptBound[0].x) / 2;
				}
				else
				{
					fLeft = fCenter;
				}

				if (i < m_nWordInfoCount)
				{
					pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i+1);
					fRight = fCenter + (pSpaceInfo->ptBound[0].x - fWordOffset) / 2;
				}
				else
				{
					fRight = fCenter;
				}

				BaseRectf_SetRect(&rtWord, fLeft - 1, fTop - 1, fRight + 1, fBottom + 1);
				if (b2DTest)
				{
					if (pt.y >= rtWord.top && pt.y <= rtWord.bottom)
						bOtherCor = TRUE;
					else
						bOtherCor = FALSE;
				}
				else
				{
					bOtherCor = TRUE;
				}
				if (bOtherCor && pt.x >= rtWord.left && pt.x <= rtWord.right)
				{
					nHitI = i;
					break;
				}
			}

			//如果未找到，需要确定是最左端还是最右端。
			if (nHitI < 0)
			{
				if (pt.x >= 0 && pt.x <= m_ptLineOffset.x)
					nHitI = 0;
				else
					nHitI = m_nWordInfoCount;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		if (wTestLogic & 0x0001)
			fLength = max(m_fLength, m_RealSize.cy);
		else
			fLength = m_fLength;
		if (pTextObj->bLeftInput)
			BaseRectf_SetRect(&rtLine, m_ptLineOffset.x, 0, m_ptLineOffset.x + m_RealSize.cx + m_fLineDistance, fLength);
		else
			BaseRectf_SetRect(&rtLine, m_ptLineOffset.x - m_fLineDistance, 0, m_ptLineOffset.x + m_RealSize.cx, fLength);

		if (b2DTest)
		{
			if (pt.x >= rtLine.left && pt.x <= rtLine.right)
				bOtherCor = TRUE;
			else
				bOtherCor = FALSE;
		}
		else
		{
			bOtherCor = TRUE;
		}

		if (bOtherCor && ((!bCheckLength) || (pt.y >= rtLine.top && pt.y <= rtLine.bottom)))
		{
			if (m_nWordInfoCount < 1)
				return 0;

			rectf rtWord;
			float fTop, fCenter, fBottom;
			float fLeft = rtLine.left;
			float fRight = rtLine.right;

			//左打排版
			for (int i = 0; i <= m_nWordInfoCount; i++)
			{
				LPCommonGlyphSpace pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
				float fWordOffset = pSpaceInfo->ptBound[0].y;
				fCenter = m_ptLineOffset.y + fWordOffset;
				if (i > 0)
				{
					pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i-1);
					fTop = fCenter - (fWordOffset - pSpaceInfo->ptBound[0].y) / 2;
				}
				else
					fTop = fCenter;

				if (i < m_nWordInfoCount)
				{
					pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i+1);
					fBottom = fCenter + (pSpaceInfo->ptBound[0].y - fWordOffset) / 2;
				}
				else
				{
					fBottom = fCenter;
				}

				BaseRectf_SetRect(&rtWord, fLeft - 1, fTop - 1, fRight + 1, fBottom + 1);
				if (b2DTest)
				{
					if (pt.x >= rtWord.left && pt.x <= rtWord.right)
						bOtherCor = TRUE;
					else
						bOtherCor = FALSE;
				}
				else
				{
					bOtherCor = TRUE;
				}
				if (bOtherCor && pt.y >= rtWord.top && pt.y <= rtWord.bottom)
				{
					nHitI = i;
					break;
				}
			}

			//如果未找到，需要确定是最左端还是最右端。
			if (nHitI < 0)
			{
				if (pt.y >= 0 && pt.y <= m_ptLineOffset.y)
					nHitI = 0;
				else
					nHitI = m_nWordInfoCount;
			}
		}
		else
		{
			return -1;
		}
	}

	bIsLineEndPos = (nHitI >= m_nWordInfoCount);
	return nHitI;
}

int CBaseTextLine::GetWordInfoPosFromPointEx(BOOL& bIsLineEndPos, 
										   point2f& pt, 
										   WORD wTestLogic)
{
	//测试点前后两个空间位置各（近似）取一半，
	//构成的不规则矩形作为测试区。
	int nHitI = -1;
	point2f ptTest = pt;

	point2f ptSpace[4];
	//ptSpace 4点次序：
	// (0)--(1)
	//  |    |
	// (3)--(2)

	BOOL b2DTest = wTestLogic & 0x0002;
	LPCommonGlyphSpace pCurSpace;
	LPCommonGlyphSpace pPreSpace;
	ptTest.x -= m_ptLineOffset.x;
	ptTest.y -= m_ptLineOffset.y;

	LPCommonText pTextObj = m_pTextParagraph->GetTextObj();
	if (pTextObj->bHorizontal)
	{
		for (int i = 0; i <= m_nWordInfoCount; ++i)
		{
			pCurSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			if (i > 0)
			{
				pPreSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i-1);
				ptSpace[0].x = (pPreSpace->ptBound[0].x + pPreSpace->ptBound[1].x) / 2.0f;
				ptSpace[0].y = (pPreSpace->ptBound[0].y + pPreSpace->ptBound[1].y) / 2.0f;
				ptSpace[3].x = (pPreSpace->ptBound[2].x + pPreSpace->ptBound[3].x) / 2.0f;
				ptSpace[3].y = (pPreSpace->ptBound[2].y + pPreSpace->ptBound[3].y) / 2.0f;
				ptSpace[1].x = (pCurSpace->ptBound[0].x + pCurSpace->ptBound[1].x) / 2.0f;
				ptSpace[1].y = (pCurSpace->ptBound[0].y + pCurSpace->ptBound[1].y) / 2.0f;
				ptSpace[2].x = (pCurSpace->ptBound[2].x + pCurSpace->ptBound[3].x) / 2.0f;
				ptSpace[2].y = (pCurSpace->ptBound[2].y + pCurSpace->ptBound[3].y) / 2.0f;
			}
			else
			{
				ptSpace[0].x = pCurSpace->ptBound[0].x;
				ptSpace[0].y = pCurSpace->ptBound[0].y;
				ptSpace[3].x = pCurSpace->ptBound[3].x;
				ptSpace[3].y = pCurSpace->ptBound[3].y;
				ptSpace[1].x = (ptSpace[0].x + pCurSpace->ptBound[1].x) / 2;
				ptSpace[1].y = (ptSpace[0].y + pCurSpace->ptBound[1].y) / 2;
				ptSpace[2].x = (ptSpace[3].x + pCurSpace->ptBound[2].x) / 2;
				ptSpace[2].y = (ptSpace[3].y + pCurSpace->ptBound[2].y) / 2;
			}

			if (b2DTest)	//XY双向坐标均测试
			{
				//判断点是否在四边形内部
				if (BaseGraphIsPoint2fIn2DPolygon(ptSpace, 4, ptTest))
				{
					nHitI = i;
					break;
				}
			}
			else			//单向坐标测试
			{
				float fMinX = ptSpace[0].x;
				float fMaxX = fMinX;
				for (int nX = 1; nX < 4; ++nX)
				{
					if (ptSpace[nX].x < fMinX)
						fMinX = ptSpace[nX].x;
					if (ptSpace[nX].x > fMaxX)
						fMaxX = ptSpace[nX].x;
				}
				if (ptTest.x >= fMinX && ptTest.x <= fMaxX)
				{
					nHitI = i;
					break;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i <= m_nWordInfoCount; ++i)
		{
			pCurSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
			if (i > 0)
			{
				pPreSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i-1);
				ptSpace[0].x = (pPreSpace->ptBound[0].x + pPreSpace->ptBound[3].x) / 2.0f;
				ptSpace[0].y = (pPreSpace->ptBound[0].y + pPreSpace->ptBound[3].y) / 2.0f;
				ptSpace[1].x = (pPreSpace->ptBound[1].x + pPreSpace->ptBound[2].x) / 2.0f;
				ptSpace[1].y = (pPreSpace->ptBound[1].y + pPreSpace->ptBound[2].y) / 2.0f;
				ptSpace[3].x = (pCurSpace->ptBound[0].x + pCurSpace->ptBound[3].x) / 2.0f;
				ptSpace[3].y = (pCurSpace->ptBound[0].y + pCurSpace->ptBound[3].y) / 2.0f;
				ptSpace[2].x = (pCurSpace->ptBound[1].x + pCurSpace->ptBound[2].x) / 2.0f;
				ptSpace[2].y = (pCurSpace->ptBound[1].y + pCurSpace->ptBound[2].y) / 2.0f;
			}
			else
			{
				ptSpace[0].x = pCurSpace->ptBound[0].x;
				ptSpace[0].y = pCurSpace->ptBound[0].y;
				ptSpace[1].x = pCurSpace->ptBound[1].x;
				ptSpace[1].y = pCurSpace->ptBound[1].y;
				ptSpace[3].x = (ptSpace[0].x + pCurSpace->ptBound[3].x) / 2;
				ptSpace[3].y = (ptSpace[0].y + pCurSpace->ptBound[3].y) / 2;
				ptSpace[2].x = (ptSpace[1].x + pCurSpace->ptBound[2].x) / 2;
				ptSpace[2].y = (ptSpace[1].y + pCurSpace->ptBound[2].y) / 2;
			}

			if (b2DTest)
			{
				if (BaseGraphIsPoint2fIn2DPolygon(ptSpace, 4, ptTest))
				{
					nHitI = i;
					break;
				}
			}
			else
			{
				float fMinY = ptSpace[0].y;
				float fMaxY = fMinY;
				for (int nY = 1; nY < 4; ++nY)
				{
					if (ptSpace[nY].y < fMinY)
						fMinY = ptSpace[nY].y;
					if (ptSpace[nY].x > fMaxY)
						fMaxY = ptSpace[nY].y;
				}
				if (ptTest.y >= fMinY && ptTest.y <= fMaxY)
				{
					nHitI = i;
					break;
				}
			}
		}
	}

	//反馈调用者，是否击中的是行最后位置。
	bIsLineEndPos = (nHitI >= m_nWordInfoCount);
	return nHitI;
}

BOOL CBaseTextLine::GetCaretInfoByGlyphIndex(LPCommonGlyphSpace pCaretInfo, 
											int nIndex,
											BOOL bIsLineEndPos,
											BOOL bLinearTypeset,
											float fCaretHalfWidth)
{
	int nInnerIdx = nIndex - m_nBeginIndex;
	if (!m_pTextParagraph->IsLeftInput())
	{
		// 实际从左到右的位置
		nInnerIdx = m_nWordInfoCount - 1 - nInnerIdx;
	}
	if (nInnerIdx < 0)
	{
		return FALSE;
	}
	if (nInnerIdx < m_nWordInfoCount
		|| (nInnerIdx == m_nWordInfoCount && (bIsLineEndPos || m_nWordInfoCount == 0))
		)
	{
		LPCommonText pTextObj = GetTextObj();
		LPCommonGlyphSpace pSpaceInfo;
		pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nInnerIdx);
		if (pSpaceInfo == NULL)
		{
			return FALSE;
		}
		memcpy(pCaretInfo, pSpaceInfo, sizeof(BaseGraphGlyphSpace));

		if (bLinearTypeset)
		{
			// 光标延后些，防止和边线重叠看不出来
			int nOffset = 5;
			if (!pTextObj->bLeftInput)
			{
				nOffset = -5;
			}
			if (pTextObj->bHorizontal)
			{
				pCaretInfo->ptBound[0].x = pSpaceInfo->ptBound[0].x - fCaretHalfWidth + nOffset;
				pCaretInfo->ptBound[3].x = pSpaceInfo->ptBound[3].x - fCaretHalfWidth + nOffset;

				pCaretInfo->ptBound[1].x = pSpaceInfo->ptBound[0].x + fCaretHalfWidth + nOffset;
				pCaretInfo->ptBound[1].y = pSpaceInfo->ptBound[0].y;
				pCaretInfo->ptBound[2].x = pSpaceInfo->ptBound[3].x + fCaretHalfWidth + nOffset;
				pCaretInfo->ptBound[2].y = pSpaceInfo->ptBound[3].y;
			}
			else
			{
				pCaretInfo->ptBound[0].y = pSpaceInfo->ptBound[0].y - fCaretHalfWidth + nOffset;
				pCaretInfo->ptBound[1].y = pSpaceInfo->ptBound[1].y - fCaretHalfWidth + nOffset;

				pCaretInfo->ptBound[2].x = pSpaceInfo->ptBound[1].x;
				pCaretInfo->ptBound[2].y = pSpaceInfo->ptBound[1].y + fCaretHalfWidth + nOffset;
				pCaretInfo->ptBound[3].x = pSpaceInfo->ptBound[0].x;
				pCaretInfo->ptBound[3].y = pSpaceInfo->ptBound[0].y + fCaretHalfWidth + nOffset;
			}
		}
		else
		{
			// 艺术字和曲线文字光标跟随SpaceInfo的形状
			point3f pt0 = pSpaceInfo->ptBound[0];
			point3f pt1 = pSpaceInfo->ptBound[1];
			point3f pt2 = pSpaceInfo->ptBound[2];
			point3f pt3 = pSpaceInfo->ptBound[3];
			if (nInnerIdx == m_nWordInfoCount && nInnerIdx != 0)
			{
				// 最后一个，回车没有大小，按照前一个字特殊处理下
				pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nInnerIdx-1);
				if (pSpaceInfo == NULL)
				{
					return FALSE;
				}
				memcpy(pCaretInfo, pSpaceInfo, sizeof(BaseGraphGlyphSpace));

				pt0 = pSpaceInfo->ptBound[4];
				pt1 = pSpaceInfo->ptBound[1];
				pt2 = pSpaceInfo->ptBound[2];
				pt3 = pSpaceInfo->ptBound[5];
				if (pt0.x == pt1.x && pt0.y == pt1.y)
				{
					// 没有字距
					pt0 = pSpaceInfo->ptBound[1];
					pt1 = pSpaceInfo->ptBound[0];
					pt2 = pSpaceInfo->ptBound[3];
					pt3 = pSpaceInfo->ptBound[2];
				}
			}
			float fLenX01 = pt1.x - pt0.x;
			float fLenY01 = pt1.y - pt0.y;
			float fLenXY01 = sqrt(fLenX01*fLenX01 + fLenY01*fLenY01);
			float fLenX23 =pt2.x - pt3.x;
			float fLenY23 =pt2.y - pt3.y;
			float fLenXY23 = sqrt(fLenX23*fLenX23 + fLenY23*fLenY23);

			pCaretInfo->ptBound[0].x = pt0.x - fCaretHalfWidth*2/fLenXY01*fLenX01;
			pCaretInfo->ptBound[1].x = pt0.x;
			pCaretInfo->ptBound[0].y = pt0.y - fCaretHalfWidth*2/fLenXY01*fLenY01;
			pCaretInfo->ptBound[1].y = pt0.y;

			pCaretInfo->ptBound[3].x = pt3.x - fCaretHalfWidth*2/fLenXY23*fLenX23;
			pCaretInfo->ptBound[2].x = pt3.x;
			pCaretInfo->ptBound[3].y = pt3.y - fCaretHalfWidth*2/fLenXY23*fLenY23;
			pCaretInfo->ptBound[2].y = pt3.y;
		}

		point2f ptfOffset = m_pTextParagraph->GetOffset();
		point3f ptOffset3;
		ptOffset3.x = ptfOffset.x + m_ptLineOffset.x;
		ptOffset3.y = ptfOffset.y + m_ptLineOffset.y;
		ptOffset3.z = 0;
		//偏移4个点就够了
		BaseGraphOffsetPt3fs(pCaretInfo->ptBound, ptOffset3, BaseGraphGLYPHSPACEPTs);
		return TRUE;
	}

	return FALSE;
}

BOOL CBaseTextLine::GetSelSpaceInfoByGlyphIndex(int nBeginIndex, 
											  int nEndIndex, 
											  BaseARray* pArInfo, 
											  BOOL bLinearTypeset)
{
	int nInnerBeginIdx = nBeginIndex - m_nBeginIndex;
	int nInnerEndIdx   = nEndIndex - m_nBeginIndex;

	if ((nInnerBeginIdx >= 0 && nInnerBeginIdx <= m_nWordInfoCount)
		|| (nInnerEndIdx >= 0 && nInnerEndIdx <= m_nWordInfoCount)
		|| (nInnerBeginIdx <= 0 && nInnerEndIdx >= m_nWordInfoCount)
		)
	{
		if (nInnerBeginIdx < 0)
		{
			nInnerBeginIdx = 0;
		}
		if (nInnerEndIdx > m_nWordInfoCount)
		{
			nInnerEndIdx = m_nWordInfoCount;
		}

		if (!m_pTextParagraph->IsLeftInput())
		{
			// 左打选择相反
			nInnerBeginIdx = m_nWordInfoCount - nInnerBeginIdx;
			nInnerEndIdx = m_nWordInfoCount - nInnerEndIdx;
			int temp = nInnerBeginIdx;
			nInnerBeginIdx = nInnerEndIdx;
			nInnerEndIdx = temp;
		}

		if (nInnerEndIdx > nInnerBeginIdx)
		{
			float fLeft, fRight, fTop, fBottom;
			LPCommonGlyphSpace pNewGlyphSpace;
			LPCommonGlyphSpace pBeginSpace;
			LPCommonGlyphSpace pEndSpace;
			LPCommonText pTextObj = GetTextObj();

			//非线性排版时逐个Glyph处理
			if (bLinearTypeset)
			{
				pNewGlyphSpace = NULL;
				pBeginSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nInnerBeginIdx);
				pEndSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nInnerEndIdx);
				if (pTextObj->bHorizontal)
				{
					fLeft   = pBeginSpace->ptBound[0].x;
					fRight  = pEndSpace->ptBound[0].x;
					fTop    = pBeginSpace->ptBound[0].y;
					fBottom = pBeginSpace->ptBound[3].y;
					if (fLeft != fRight && fTop != fBottom)
					{
						pNewGlyphSpace = new BaseGraphGlyphSpace;
					}
				}
				else
				{
					fLeft   = pBeginSpace->ptBound[0].x;
					fRight  = pBeginSpace->ptBound[1].x;
					fTop    = pBeginSpace->ptBound[0].y;
					fBottom = pEndSpace->ptBound[0].y;
					if (fLeft != fRight && fTop != fBottom)
					{
						pNewGlyphSpace = new BaseGraphGlyphSpace;
					}
				}

				//有效的选区
				if (pNewGlyphSpace != NULL)
				{
					memset(pNewGlyphSpace, 0, sizeof(BaseGraphGlyphSpace));

					pNewGlyphSpace->ptBound[0].x = fLeft;
					pNewGlyphSpace->ptBound[3].x = fLeft;
					pNewGlyphSpace->ptBound[1].x = fRight;
					pNewGlyphSpace->ptBound[2].x = fRight;

					pNewGlyphSpace->ptBound[0].y = fTop;
					pNewGlyphSpace->ptBound[3].y = fBottom;
					pNewGlyphSpace->ptBound[1].y = fTop;
					pNewGlyphSpace->ptBound[2].y = fBottom;

					point2f ptfOffset = m_pTextParagraph->GetOffset();
					point3f ptOffset3;
					ptOffset3.x = ptfOffset.x + m_ptLineOffset.x;
					ptOffset3.y = ptfOffset.y + m_ptLineOffset.y;
					ptOffset3.z = 0;
					//偏移4个点就够了
					BaseGraphOffsetPt3fs(pNewGlyphSpace->ptBound, ptOffset3, BaseGraphGLYPHSPACEPTs);
					BaseAR_Add(pArInfo, pNewGlyphSpace);
					return TRUE;
				}
			}
			else
			{
				for (int i=nInnerBeginIdx; i<nInnerEndIdx; ++i)
				{
					pNewGlyphSpace = NULL;
					pBeginSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, i);
					pNewGlyphSpace = new BaseGraphGlyphSpace;
					memcpy(pNewGlyphSpace, pBeginSpace, sizeof(BaseGraphGlyphSpace));

					//有效的选区
					if (pNewGlyphSpace != NULL)
					{
						point2f ptfOffset = m_pTextParagraph->GetOffset();
						point3f ptOffset3;
						ptOffset3.x = ptfOffset.x + m_ptLineOffset.x;
						ptOffset3.y = ptfOffset.y + m_ptLineOffset.y;
						ptOffset3.z = 0;
						//偏移6个点
						BaseGraphOffsetPt3fs(pNewGlyphSpace->ptBound, ptOffset3, BaseGraphGLYPHSPACEPTs_EX);
						BaseAR_Add(pArInfo, pNewGlyphSpace);
					}
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CBaseTextLine::GetGlyphSpaceInfo(int nIndex, LPCommonGlyphSpace pSpaceInfo)
{
	int nInnerIdx = nIndex - m_nBeginIndex;
	if (!m_pTextParagraph->IsLeftInput())
	{
		// 实际从左到右的位置
		nInnerIdx = m_nWordInfoCount -1 - nInnerIdx;
	}
	if (nInnerIdx < 0)
	{
		return FALSE;
	}
	if (nInnerIdx < m_nWordInfoCount || m_nWordInfoCount == 0)
	{
		LPCommonGlyphSpace pSpaceInfoSrc;
		pSpaceInfoSrc = (LPCommonGlyphSpace)BaseAR_GetAt(&m_arGlyphSpace, nInnerIdx);
		memcpy(pSpaceInfo, pSpaceInfoSrc, sizeof(BaseGraphGlyphSpace));

		point2f ptfOffset = m_pTextParagraph->GetOffset();
		point3f ptOffset3;
		ptOffset3.x = ptfOffset.x + m_ptLineOffset.x;
		ptOffset3.y = ptfOffset.y + m_ptLineOffset.y;
		ptOffset3.z = 0;

		pSpaceInfo->ptPos.x += ptOffset3.x;
		pSpaceInfo->ptPos.y += ptOffset3.y;
		BaseGraphOffsetPt3fs(pSpaceInfo->ptBound, ptOffset3, BaseGraphGLYPHSPACEPTs_EX);

		return TRUE;
	}

	return FALSE;
}

BOOL CBaseTextLine::IsGlyphIndexInLine(int nIndex, BOOL bIsLineEndPos)
{
	LPCommonText pTextObj = GetTextObj();
	int nInnerIdx = nIndex - m_nBeginIndex;
	if (nInnerIdx < m_nWordInfoCount 
		|| (nInnerIdx == m_nWordInfoCount && (bIsLineEndPos || m_nWordInfoCount == 0))
		)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBaseTextLine::IsGlyphIndexAtLineEnd(int nIndex)
{
	if (nIndex - m_nBeginIndex == m_nWordInfoCount)
	{
		return TRUE;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// CBaseTextParagraph

CBaseTextParagraph::CBaseTextParagraph(LPCommonText pTextObj, int nParaIndex)
{
	m_pTextObj            = pTextObj;
	m_nPreAllWordInfoCnt  = 0;
	m_nChgLevel           = CBaseTypeset::TTLWordChg;
	m_nParaIndex          = nParaIndex;
	m_fWidth              = 0;
	m_fHeight             = 0;
	m_fRealWidth          = 0;
	m_fRealHeight         = 0;
	m_ptOffset.x          = 0;
	m_ptOffset.y          = 0;
	BaseAR_Init(&m_arWordInfo);
	BaseAR_Init(&m_arLine);
}

CBaseTextParagraph::~CBaseTextParagraph()
{
	ReleaseTempData();

	int nWordInfoCnt = BaseAR_GetCount(&m_arWordInfo);
	for (int i = 0; i < nWordInfoCnt; ++i)
	{
		LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&m_arWordInfo, i);
		BaseGraphWordInfoRelease(pWordInfo);
		delete pWordInfo;
	}
	BaseAR_RemoveAll(&m_arWordInfo);
}

LPCommonText CBaseTextParagraph::GetTextObj()
{
	return m_pTextObj;
}

LPCommonTextParagraph CBaseTextParagraph::GetParagraphObj()
{
	return (LPCommonTextParagraph)BaseAR_GetAt(&GetTextObj()->arParagraph, m_nParaIndex);
}


float CBaseTextParagraph::GetRealHeight()
{
	return m_fRealHeight;
}

float CBaseTextParagraph::GetRealWidth()
{
	return m_fRealWidth;
}

float CBaseTextParagraph::GetHeight()
{
	return m_fHeight;
}

float CBaseTextParagraph::GetWidth()
{
	return m_fWidth;
}

point2f CBaseTextParagraph::GetOffset()
{
	return m_ptOffset;
}

void CBaseTextParagraph::SetOffset(point2f ptOffset)
{
	m_ptOffset = ptOffset;
}

rectf CBaseTextParagraph::GetRect()
{
	rectf rt;
	BaseRectf_SetRect(&rt, m_ptOffset.x, m_ptOffset.y, m_ptOffset.x + m_fRealWidth, m_ptOffset.y + m_fRealHeight);
	return rt;
}

float CBaseTextParagraph::GetIndent()
{
	return 0;
}

float CBaseTextParagraph::GetSymbolWidth()
{
	return 0;
}

float CBaseTextParagraph::GetSymbolHeight()
{
	return 0;
}

BOOL CBaseTextParagraph::IsLineWrap()
{
	return m_pTextObj->bLineWrap;
}

BOOL CBaseTextParagraph::IsFixedFrame()
{
	return m_pTextObj->bFixedFrame;
}

BOOL CBaseTextParagraph::IsHorizontal()
{
	return m_pTextObj->bHorizontal;
}

BOOL CBaseTextParagraph::IsLeftInput()
{
	return m_pTextObj->bLeftInput;
}

int CBaseTextParagraph::GetCharCount()
{
	return BaseGraphTypeset_GetParagraphCharCount(m_pTextObj, m_nParaIndex);
}

int CBaseTextParagraph::GetWordInfoCount()
{
	return BaseAR_GetCount(&m_arWordInfo);
}

LPCommonWordInfo CBaseTextParagraph::GetWordInfo(int nIndex)
{
	return (LPCommonWordInfo)BaseAR_GetAt(&m_arWordInfo, nIndex);
}

int CBaseTextParagraph::GetLineWordInfoCount(int nLineIdx)
{
	//本函数不负责参数合法性检查
	if (nLineIdx >= 0)
	{
		return ((CBaseTextLine*)BaseAR_GetAt(&m_arLine, nLineIdx))->GetWordInfoCount();
	}
	else
	{
		int nLineCnt = BaseAR_GetCount(&m_arLine);
		if (nLineCnt > 0)
		{
			return ((CBaseTextLine*)BaseAR_GetAt(&m_arLine, nLineCnt-1))->GetWordInfoCount();
		}
	}
	return 0;
}

int CBaseTextParagraph::GetTrailLineWordInfoBeginIndex()
{
	int nLineCnt = BaseAR_GetCount(&m_arLine);
	if (nLineCnt > 0)
	{
		// 左打行顺序颠倒
		return ((CBaseTextLine*)BaseAR_GetAt(&m_arLine, nLineCnt-1))->GetWordInfoBeginIndex();
	}
	return 0;
}

BaseGraphUnit* CBaseTextParagraph::GetUnit(int nWordInfoIdx)
{
	return BaseGraphTypeset_GetUnitByWordInfo(m_pTextObj, m_nPreAllWordInfoCnt + nWordInfoIdx);
}

void CBaseTextParagraph::Typeset(BOOL bIsHorizontal)
{
	ReleaseTempData();
	if (bIsHorizontal)
	{
		TypesetHorizontal();
	}
	else
	{
		TypesetVertical();
	}
}

void CBaseTextParagraph::UpdateWordInfoArray(LPCommonText pTextObj)
{
	LPCommonWordInfo pWordInfo = NULL;
	LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&pTextObj->arParagraph, m_nParaIndex);

	int nParaCharCnt = 0;
	if (m_nParaIndex < BaseAR_GetCount(&pTextObj->arParagraph)-1)
		nParaCharCnt = ((LPCommonTextParagraph)BaseAR_GetAt(&pTextObj->arParagraph, m_nParaIndex+1))->nBeginIndex;
	else
		nParaCharCnt = (int)_tcslen(m_pTextObj->szText);
	nParaCharCnt -= pParaObj->nBeginIndex;

	CString strText = pTextObj->szText;
	// 	if (!m_pTextObj->bLeftInput)
	// 	{
	// 		CString strSub = strText.Mid(pParaObj->nBeginIndex, nParaCharCnt);
	// 		strSub = strSub.MakeReverse();
	// 		strText.Delete(pParaObj->nBeginIndex, nParaCharCnt);
	// 		strText.Insert(pParaObj->nBeginIndex, strSub);
	// 	}
	int nGlyphCnt = 0;
	int nRunCnt = (int)m_uspResult.m_aryVis2Log.size();

	for (int i = 0, j = 0; i < nRunCnt; ++i)
	{
		//逐个Run取
		int nRunIdx = m_uspResult.m_aryVis2Log[i];
		BaseGraphSCRIPTRUN* psrRunInfo = m_uspResult.m_aryRunPtr[nRunIdx];

		int nRunGlyphCnt = (int)psrRunInfo->m_aryCharGlyphs.size();
		nGlyphCnt += nRunGlyphCnt;
	}
	int nOldGlyphCnt = BaseAR_GetCount(&m_arWordInfo);
	//最后两位插入回车，数量不变，之后文字内容改变
	if (nOldGlyphCnt == nGlyphCnt)
	{
		// 数量没有变化
		//return;
	}

	nGlyphCnt = 0;
	for (int i = 0, j = 0; i < nRunCnt; ++i)
	{
		//逐个Run取
		int nRunIdx = m_uspResult.m_aryVis2Log[i];
		BaseGraphSCRIPTRUN* psrRunInfo = m_uspResult.m_aryRunPtr[nRunIdx];

		int nRunGlyphCnt = (int)psrRunInfo->m_aryCharGlyphs.size();
		nGlyphCnt += nRunGlyphCnt;

		int nOldGlyphCnt = BaseAR_GetCount(&m_arWordInfo);
		if (nOldGlyphCnt < nGlyphCnt)
		{
			BaseAR_SetSize(&m_arWordInfo, nGlyphCnt, nRunGlyphCnt);
			for (int n = nOldGlyphCnt; n < nGlyphCnt; ++n)
			{
				pWordInfo = new BaseGraphWordInfo;
				BaseGraphWordInfoInit(pWordInfo);
				BaseAR_SetAt(&m_arWordInfo, n, pWordInfo);
			}
		}

		//一个Run
		for (int k=0; k<nRunGlyphCnt; ++k, ++j)
		{
			pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&m_arWordInfo, j);

			//实际使用的字体
			memcpy(pWordInfo->szFontName, psrRunInfo->m_szFontName, sizeof(TCHAR) * LF_FACESIZE);
			pWordInfo->nCharIndex = psrRunInfo->m_aryCharIndex[k];
			pWordInfo->wChar = strText.GetAt(pParaObj->nBeginIndex + pWordInfo->nCharIndex);
			// 回车符不显示空格
			pWordInfo->bVisible = (bool)BaseGraphIsVisibleChar(pWordInfo->wChar);
			pWordInfo->wGlyphIndex = psrRunInfo->m_aryCharGlyphs[k];
			pWordInfo->bGlyphIndex = !psrRunInfo->m_aryNoGlyphIndex[k];
			pWordInfo->fRTL = psrRunInfo->m_RunAnalysis.fRTL;
			pWordInfo->fAdvanceWidth = (float)psrRunInfo->m_aryAdvanceWidth[k];
			pWordInfo->ptGOffset.x = (float)psrRunInfo->m_aryGOffset[k].du;
			pWordInfo->ptGOffset.y = (float)psrRunInfo->m_aryGOffset[k].dv;

			//SCRIPT_VISATTR关键数据副本
			SCRIPT_VISATTR* pVisAttr = &psrRunInfo->m_aryVisAttribute[k];
			pWordInfo->uJustification = pVisAttr->uJustification;
			pWordInfo->fClusterStart = pVisAttr->fClusterStart;
			pWordInfo->fDiacritic = pVisAttr->fDiacritic;
			pWordInfo->fZeroWidth = pVisAttr->fZeroWidth;
		}
	}

	//删除冗余的
	int nDelCnt = nOldGlyphCnt - nGlyphCnt;
	if (nDelCnt > 0)
	{
		for (int n = nGlyphCnt; n < nOldGlyphCnt; ++n)
		{
			pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&m_arWordInfo, n);
			BaseGraphWordInfoRelease(pWordInfo);
			SAFE_DELETE(pWordInfo) ;
		}
		BaseAR_RemoveAt(&m_arWordInfo, nGlyphCnt, nDelCnt);
	}
}

void CBaseTextParagraph::ZoomUnitAndDistance(float fScaleW, float fScaleH)
{
	LPCommonText pTextObj = m_pTextObj;
	LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&pTextObj->arParagraph, m_nParaIndex);

	m_fRealHeight *= fScaleH;
	m_fRealWidth *= fScaleW;

	if (pTextObj->bHorizontal)
	{
		m_fHeight *= fScaleH;

		//重算行偏移
		float fOffsetX = 0;
		float fParaWidth = m_fWidth - GetIndent() - GetSymbolWidth();
		int nLineCount = BaseAR_GetCount(&m_arLine);
		for (int i = 0; i < nLineCount; ++i)
		{
			CBaseTextLine* pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			pTextLine->ZoomUnitAndDistance(fScaleW, fScaleH);

			point2f ptLinePos = pTextLine->GetLineOffset();
			ptLinePos.x *= fScaleW;
			//ptLinePos.x = fOffsetX + GetIndent() + GetSymbolWidth();

			pTextLine->SetLineOffset(ptLinePos);
		}
	}
	else
	{
		m_fWidth *= fScaleW;

		//重算行偏移
		float fOffsetY = 0;
		float fParaHeight = m_fHeight - GetIndent() - GetSymbolHeight();
		int nLineCount = BaseAR_GetCount(&m_arLine);
		for (int i = 0; i < nLineCount; ++i)
		{
			CBaseTextLine* pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			pTextLine->ZoomUnitAndDistance(fScaleW, fScaleH);

			point2f ptLinePos = pTextLine->GetLineOffset();
			ptLinePos.y *= fScaleH;
			//ptLinePos.y = fOffsetY + GetIndent() + GetSymbolWidth();

			pTextLine->SetLineOffset(ptLinePos);
		}
	}
}

void CBaseTextParagraph::OffsetCal(vector3f* pOffset)
{
	m_ptOffset.x += pOffset->x;
	m_ptOffset.y += pOffset->y;
	BaseRectf_OffsetRect1(&m_rtBorder, pOffset->x, pOffset->y);
}

void CBaseTextParagraph::ConvertCharPosToLineIdx(int& nLineIdx, int nCharIndex, BOOL bIsLineEndPos)
{
	LPCommonTextParagraph pParaObj = GetParagraphObj();
	int nParaCharIdx = nCharIndex - pParaObj->nBeginIndex;
	int nWordInfoPos = CBaseUsp::ScriptCPToGX(m_uspResult, nParaCharIdx, -1);

	int nLineCount = BaseAR_GetCount(&m_arLine);
	for (int i = 0; i < nLineCount; ++i)
	{
		CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
		if (pLine->IsGlyphIndexInLine(nWordInfoPos, bIsLineEndPos))
		{
			nLineIdx = i;
			return;
		}
	}

	nLineIdx = -1;
}

BOOL CBaseTextParagraph::IsCharPosAtLineEnd(BOOL nCharIndex)
{
	LPCommonTextParagraph pParaObj = GetParagraphObj();
	int nParaCharIdx = nCharIndex - pParaObj->nBeginIndex;
	int nWordInfoPos = CBaseUsp::ScriptCPToGX(m_uspResult, nParaCharIdx, -1);

	int nLineCount = BaseAR_GetCount(&m_arLine);
	for (int i = 0; i < nLineCount; ++i)
	{
		CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
		if (pLine->IsGlyphIndexAtLineEnd(nWordInfoPos))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBaseTextParagraph::IsHitTextByPoint(point2f& pt, 
										BOOL bCaptureMouse, 
										BOOL bLinearTypeset)
{
	point2f ptParaOffset = m_ptOffset;
	float fHeight = m_fHeight;
	float fWidth;
	if (bCaptureMouse)
	{
		fWidth = max(m_fWidth, m_fRealWidth);
		fHeight = max(m_fHeight, m_fRealHeight);
	}
	else
	{
		fWidth = m_fWidth;
		fHeight = m_fHeight;
	}

	point2f ptPara = pt;
	ptPara.x -= ptParaOffset.x;
	ptPara.y -= ptParaOffset.y;

	LPCommonTextParagraph pParaObj;
	CBaseTextLine* pLine;
	int nLineCnt;

	if (bLinearTypeset)
	{
		//线性排版
		rectf rtPara;
		BaseRectf_SetRect(&rtPara, 0, 0, fWidth, fHeight);
		if (BaseRectf_PtInRect(&rtPara, ptPara))
		{
			pParaObj = GetParagraphObj();
			nLineCnt = BaseAR_GetCount(&m_arLine);
			for (int i = nLineCnt-1; i >= 0; --i)
			{
				pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
				if (pLine->IsHitTextByPoint(ptPara, bCaptureMouse, TRUE))
				{
					return TRUE;
				}
			}
		}
	}
	else
	{
		//非线性排版
		pParaObj = GetParagraphObj();
		nLineCnt = BaseAR_GetCount(&m_arLine);
		WORD wTestLogic = bCaptureMouse ? 0x0007 : 0x0006;
		for (int i = nLineCnt-1; i >= 0; --i)
		{
			pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			BOOL bIsLineEndPos;
			if (pLine->GetWordInfoPosFromPointEx(bIsLineEndPos, ptPara, wTestLogic) >= 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CBaseTextParagraph::GetCharPosFromPoint(BOOL& bIsLineEndPos, 
										  point2f& pt, 
										  BOOL bCaptureMouse, 
										  BOOL bLinearTypeset)
{
	point2f ptParaOffset = m_ptOffset;
	float fHeight = m_fHeight;
	float fWidth;
	if (bCaptureMouse)
	{
		fWidth = max(m_fWidth, m_fRealWidth);
		fHeight = max(m_fHeight, m_fRealHeight);
	}
	else
	{
		fWidth = m_fWidth;
		fHeight = m_fHeight;
	}

	point2f ptPara = pt;
	ptPara.x -= ptParaOffset.x;
	ptPara.y -= ptParaOffset.y;

	LPCommonTextParagraph pParaObj;
	CBaseTextLine* pLine;
	int nLineCnt;
	WORD wTestLogic = bCaptureMouse ? 0x0007 : 0x0006;

	if (bLinearTypeset)
	{
		//线性排版
		rectf rtPara;
		BaseRectf_SetRect(&rtPara, 0, 0, fWidth, fHeight);
		if (BaseRectf_PtInRect(&rtPara, ptPara))
		{
			pParaObj = GetParagraphObj();
			nLineCnt = BaseAR_GetCount(&m_arLine);

			for (int i = 0; i < nLineCnt; ++i)
			{
				pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
				//S1、先获得在行内的字形位置。
				int nWordInfoPos = pLine->GetWordInfoPosFromPoint(bIsLineEndPos, ptPara, wTestLogic);
				if (nWordInfoPos >= 0)
				{
					if (!IsLeftInput())
					{
						nWordInfoPos = pLine->GetWordInfoCount() - nWordInfoPos;
					}
					//S2、然后转换成段内字形位置。
					nWordInfoPos += pLine->GetWordInfoBeginIndex();
					//S3、再转换成字符索引。
					int nCharPos = CBaseUsp::ScriptGXToCP(m_uspResult, nWordInfoPos, -1);
					return nCharPos + pParaObj->nBeginIndex;
				}
			}
		}
	}
	else
	{
		//非线性排版
		pParaObj = GetParagraphObj();
		nLineCnt = BaseAR_GetCount(&m_arLine);
		for (int i = nLineCnt-1; i >= 0; --i)
		{
			CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			int nWordInfoPos = pLine->GetWordInfoPosFromPointEx(bIsLineEndPos, ptPara, wTestLogic);
			if (nWordInfoPos >= 0)
			{
				nWordInfoPos += pLine->GetWordInfoBeginIndex();
				int nCharPos = CBaseUsp::ScriptGXToCP(m_uspResult, nWordInfoPos, -1);
				return nCharPos + pParaObj->nBeginIndex;
			}
		}
	}

	bIsLineEndPos = FALSE;
	return -1;
}

int CBaseTextParagraph::GetCharPosFromPoint(BOOL& bIsLineEndPos, 
										  point2f& pt, 
										  int nLineIdx, 
										  BOOL bCaptureMouse, 
										  BOOL bLinearTypeset)
{
	int nLineCount = BaseAR_GetCount(&m_arLine);
	if (nLineIdx >= 0 && nLineIdx < nLineCount)
	{
		point2f ptParaOffset = m_ptOffset;
		point2f ptPara = pt;
		ptPara.x -= ptParaOffset.x;
		ptPara.y -= ptParaOffset.y;

		LPCommonTextParagraph pParaObj = GetParagraphObj();
		CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, nLineIdx);
		//单向坐标测试
		WORD wTestLogic = bCaptureMouse ? 0x0001 : 0x0000;
		int nWordInfoPos;
		if (bLinearTypeset)
			nWordInfoPos = pLine->GetWordInfoPosFromPoint(bIsLineEndPos, ptPara, wTestLogic);
		else
			nWordInfoPos = pLine->GetWordInfoPosFromPointEx(bIsLineEndPos, ptPara, wTestLogic);

		if (nWordInfoPos >= 0)
		{
			nWordInfoPos += pLine->GetWordInfoBeginIndex();
			int nCharPos = CBaseUsp::ScriptGXToCP(m_uspResult, nWordInfoPos, -1);
			return nCharPos + pParaObj->nBeginIndex;
		}
	}

	bIsLineEndPos = FALSE;
	return -1;
}

BOOL CBaseTextParagraph::GetCaretInfoByCharPos(LPCommonGlyphSpace pCaretInfo, 
											 int nCharIndex, 
											 BOOL bIsLineEndPos, 
											 BOOL bFullHeight,
											 BOOL bLinearTypeset, 
											 float fCaretHalfWidth)
{
	//S1、先获得在行内的字符位置索引。
	LPCommonTextParagraph pParaObj = GetParagraphObj();
	int nParaCharIdx = nCharIndex - pParaObj->nBeginIndex;
	//S2、然后转换成段内字形位置。
	int nWordInfoPos = CBaseUsp::ScriptCPToGX(m_uspResult, nParaCharIdx, -1);

	//S3、再查找字形所在行和行内索引。
	return GetCaretInfoByGlyphIndex(pCaretInfo, nWordInfoPos, bIsLineEndPos, bFullHeight, bLinearTypeset, fCaretHalfWidth);
}

BOOL CBaseTextParagraph::GetCaretInfoByGlyphIndex(LPCommonGlyphSpace pCaretInfo, 
												int nGlyphIndex, 
												BOOL bIsLineEndPos, 
												BOOL bFullHeight, 
												BOOL bLinearTypeset, 
												float fCaretHalfWidth)
{
	int nLineCount = BaseAR_GetCount(&m_arLine);
	for (int i = 0; i < nLineCount; ++i)
	{
		CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
		if (pLine->GetCaretInfoByGlyphIndex(pCaretInfo, nGlyphIndex, bIsLineEndPos, bLinearTypeset, fCaretHalfWidth))
		{
			//取全高（包含行距等附加高度）。
			if (bFullHeight)
			{
				LPCommonText pTextObj = m_pTextObj;

				float fLineDistance = pLine->GetLineDistance();
				if (pTextObj->bHorizontal)
				{
					pCaretInfo->ptBound[2].y += fLineDistance;
					pCaretInfo->ptBound[3].y += fLineDistance;
				}
				else
				{
					if (pTextObj->bLeftInput)
					{
						pCaretInfo->ptBound[1].x += fLineDistance;
						pCaretInfo->ptBound[2].x += fLineDistance;
					}
					else
					{
						pCaretInfo->ptBound[3].x -= fLineDistance;
						pCaretInfo->ptBound[0].x -= fLineDistance;
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBaseTextParagraph::GetSelSpaceInfoByCharPos(int nBeginIndex, 
												int nEndIndex, 
												BaseARray* pArInfo,
												BOOL bLinearTypeset)
{
	BOOL bRtn = FALSE;
	int nTemp = min(nBeginIndex, nEndIndex);
	nEndIndex = max(nTemp, nEndIndex);
	nBeginIndex = nTemp;

	//S1、先转换成段落字符索引
	LPCommonTextParagraph pParaObj = GetParagraphObj();
	int nInnerBeginIdx = nBeginIndex - pParaObj->nBeginIndex;
	int nInnerEndIdx   = nEndIndex - pParaObj->nBeginIndex;
	if (nInnerEndIdx < 0 || nInnerBeginIdx >= nInnerEndIdx)
	{
		return FALSE;
	}

	//S2、逐个Run处理
	int nLineCnt = BaseAR_GetCount(&m_arLine);
	int nRunCnt = (int)m_uspResult.m_aryRunPtr.size();
	for (int i = 0; i < nRunCnt; ++i)	
	{
		BaseGraphSCRIPTRUN* psrRunInfo = m_uspResult.m_aryRunPtr[i];
		if (psrRunInfo->m_nRunCharPos >= nInnerEndIdx)
			break;
		if (psrRunInfo->m_nRunCharPos + psrRunInfo->m_nRunCharLen <= nInnerBeginIdx)
			continue;

		int nMinChar = max(psrRunInfo->m_nRunCharPos, nInnerBeginIdx);
		int nRunLast = psrRunInfo->m_nRunCharPos + psrRunInfo->m_nRunCharLen;
		int nMaxChar = min(nRunLast, nInnerEndIdx);

		int nStartGlyph = CBaseUsp::ScriptCPToGX(m_uspResult, nMinChar, i);
		int nEndGlyph   = CBaseUsp::ScriptCPToGX(m_uspResult, nMaxChar, i);

		if (nStartGlyph > nEndGlyph)
		{
			int nTempGlyph = nStartGlyph;
			nStartGlyph = nEndGlyph;
			nEndGlyph = nTempGlyph;
		}

		//S3、一个Run可能跨越多行
		for (int j = 0; j < nLineCnt; ++j)
		{
			CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, j);
			pLine->GetSelSpaceInfoByGlyphIndex(nStartGlyph, nEndGlyph, pArInfo, bLinearTypeset);
		}
	}

	return TRUE;
}

BOOL CBaseTextParagraph::GetCharPosSpaceInfo(int nCharIndex, LPCommonGlyphSpace pSpaceInfo)
{
	//暂时仅作简单处理

	//先转换成Glyph索引
	LPCommonTextParagraph pParaObj = GetParagraphObj();
	int nParaCharIdx = nCharIndex - pParaObj->nBeginIndex;
	int nWordInfoPos = CBaseUsp::ScriptCPToGX(m_uspResult, nParaCharIdx, -1);

	int nLineCount = BaseAR_GetCount(&m_arLine);
	for (int i = 0; i < nLineCount; ++i)
	{
		CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
		if (pLine->GetGlyphSpaceInfo(nWordInfoPos, pSpaceInfo))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CBaseTextParagraph::ReleaseTempData()
{
	//行
	int nCnt;
	nCnt = BaseAR_GetCount(&m_arLine);
	for (int i = 0; i < nCnt; ++i)
	{
		CBaseTextLine* pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
		delete pTextLine;
	}
	BaseAR_RemoveAll(&m_arLine);
}

void CBaseTextParagraph::TypesetHorizontal()
{
	//将横竖两种排版坐标系统一：
	//段落偏移参考点在左上角，偏移量相对于排版矩形中心。
	//行参考点在左上角，偏移量相对于所在段落参考点。

	LPCommonText pTextObj = m_pTextObj;
	int nParaCnt = BaseAR_GetCount(&pTextObj->arParagraph);
	LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&pTextObj->arParagraph, m_nParaIndex);
	int nWordInfoCnt = BaseAR_GetCount(&m_arWordInfo);

	point2f* pvGraphScale = &pTextObj->vGraphScale;
	rectf rtTypesetBorder;
	BaseGraphTypeset_GetTypesetRect(m_pTextObj, rtTypesetBorder, FALSE);

	m_fWidth = rtTypesetBorder.right - rtTypesetBorder.left;
	//行宽 = 段宽 - 缩进 - 项目符号宽度
	float fParaWidth = m_fWidth - GetIndent() - GetSymbolWidth();
	CBaseTextLine* pTextLine = new CBaseTextLine(this, 0, fParaWidth);
	pTextLine->SetLineDistance(pParaObj->fLineDistance * pvGraphScale->y);
	BaseAR_Add(&m_arLine, pTextLine);

	//空段
	if (nWordInfoCnt <= 0)
	{
		AddBlankLine(pTextLine);
		pTextLine->ScaleNullLineGraph(pvGraphScale->x, pvGraphScale->y);
	}

	BOOL bAddWordWrap = pTextObj->bLineWrap && !pTextObj->bFixedFrame;
	LPCommonWordInfo pLastWordInfo = NULL;
	if (bAddWordWrap)
	{
		//自动换行
		for (int i = 0; i < nWordInfoCnt; ++i)
		{
			LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&m_arWordInfo, i);
			if (pTextLine->AddWordHorizontal(pWordInfo, pLastWordInfo, pvGraphScale, bAddWordWrap) == NULL)
			{
				//行已满，不能再添加字符，则添加一个新行
				pTextLine = new CBaseTextLine(this, i, fParaWidth);
				pTextLine->SetLineDistance(pParaObj->fLineDistance * pvGraphScale->y);
				BaseAR_Add(&m_arLine, pTextLine);
				pTextLine->AddWordHorizontal(pWordInfo, NULL, pvGraphScale, bAddWordWrap);
			}
			pLastWordInfo = pWordInfo;
		}

		int nLineCount = BaseAR_GetCount(&m_arLine);
		for (int i = 0; i < nLineCount; i++)
		{
			pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			if (!IsLeftInput())
			{
				pTextLine->ResetHorizontal();
			}
			pTextLine->CalWordsAndLinePos();
			//两端对齐排版
			if (pParaObj->eAlignHorizontal == TEXT_ALIGN_H_ENDS)
				pTextLine->EndAlign(pvGraphScale);
		}
	}
	else
	{
		//非自动折行
		for (int i = 0; i < nWordInfoCnt; ++i)
		{
			LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&m_arWordInfo, i);
			pTextLine->AddWordHorizontal(pWordInfo, pLastWordInfo, pvGraphScale, bAddWordWrap);
			pLastWordInfo = pWordInfo;
		}
		if (!IsLeftInput())
		{
			pTextLine->ResetHorizontal();
		}
		pTextLine->CalWordsAndLinePos();
		//两端对齐排版
		if (pParaObj->eAlignHorizontal == TEXT_ALIGN_H_ENDS)
			pTextLine->EndAlign(pvGraphScale);
	}

	//有段必有行，所以行数必然大于0！
	int nLineCount = BaseAR_GetCount(&m_arLine);

	//段的实际宽度必然是其第一行的实际宽度！
	pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, 0);
	BaseGraphSizef* pSzLineReal = pTextLine->GetRealSizePtr();
	m_fRealWidth = max(0, pSzLineReal->cx);

	float fOffsetY = 0;
	float fLastOffsetY = 0;
	m_rtBorder.top = 0;
	m_rtBorder.bottom = 0;

	for (int i = 0; i < nLineCount; i++)
	{
		pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
		float fOffsetX = 0;
		pSzLineReal = pTextLine->GetRealSizePtr();
		switch (pParaObj->eAlignHorizontal)
		{
		case TEXT_ALIGN_H_LEFT:
		case TEXT_ALIGN_H_ENDS:
			fOffsetX = 0;
			break;
		case TEXT_ALIGN_H_CENTER:
			fOffsetX = (m_fRealWidth - pSzLineReal->cx) / 2;
			break;
		case TEXT_ALIGN_H_RIGHT:
			fOffsetX = m_fRealWidth - pSzLineReal->cx;
			break;
		}
		point2f ptLinePos;
		ptLinePos.x = fOffsetX + GetIndent() + GetSymbolWidth();
		ptLinePos.y = fOffsetY;
		pTextLine->SetLineOffset(ptLinePos);

		fOffsetY += pSzLineReal->cy;
		if (fOffsetY < m_rtBorder.top)
			m_rtBorder.top = fOffsetY;
		if (fOffsetY > m_rtBorder.bottom)
			m_rtBorder.bottom = fOffsetY;

		if (m_nParaIndex != nParaCnt - 1 || i != nLineCount - 1)
		{
			fOffsetY += pTextLine->GetLineDistance();
			if (fOffsetY < m_rtBorder.top)
				m_rtBorder.top = fOffsetY;
			if (fOffsetY > m_rtBorder.bottom)
				m_rtBorder.bottom = fOffsetY;
		}

		if (fOffsetY < fLastOffsetY)
			fOffsetY = fLastOffsetY;
		fLastOffsetY = fOffsetY;
	}

	m_fHeight = fOffsetY;
	m_fRealHeight = m_fHeight;
}

void CBaseTextParagraph::TypesetVertical()
{
	LPCommonText pTextObj = m_pTextObj;
	int nParaCnt = BaseAR_GetCount(&pTextObj->arParagraph);
	LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&pTextObj->arParagraph, m_nParaIndex);
	int nWordInfoCnt = BaseAR_GetCount(&m_arWordInfo);

	point2f* pvGraphScale = &pTextObj->vGraphScale;
	rectf rtTypesetBorder;
	BaseGraphTypeset_GetTypesetRect(m_pTextObj, rtTypesetBorder, FALSE);

	m_fHeight = rtTypesetBorder.bottom - rtTypesetBorder.top;
	//行高 = 段高 - 缩进 - 项目符号高度
	float fParaHeight = m_fHeight - GetIndent() - GetSymbolHeight();
	CBaseTextLine* pTextLine = new CBaseTextLine(this, 0, fParaHeight);
	pTextLine->SetLineDistance(pParaObj->fLineDistance * pvGraphScale->x);
	BaseAR_Add(&m_arLine, pTextLine);

	//空段
	if (nWordInfoCnt <= 0)
	{
		AddBlankLine(pTextLine);
		pTextLine->ScaleNullLineGraph(pvGraphScale->y, pvGraphScale->x);
	}

	BOOL bAddWordWrap = pTextObj->bLineWrap && !pTextObj->bFixedFrame;
	LPCommonWordInfo pLastWordInfo = NULL;
	if (bAddWordWrap)
	{
		//自动换行
		for (int i = 0; i < nWordInfoCnt; ++i)
		{
			LPCommonWordInfo pWordInfo = GetWordInfo(i);
			if (pTextLine->AddWordVertical(pWordInfo, pLastWordInfo, pvGraphScale, bAddWordWrap) == NULL)
			{
				//行已满，不能再添加字符，则添加一个新行
				pTextLine = new CBaseTextLine(this, i, fParaHeight);
				pTextLine->SetLineDistance(pParaObj->fLineDistance * pvGraphScale->x);
				BaseAR_Add(&m_arLine, pTextLine);
				pTextLine->AddWordVertical(pWordInfo, NULL, pvGraphScale, bAddWordWrap);
			}
			pLastWordInfo = pWordInfo;
		}

		int nLineCount = BaseAR_GetCount(&m_arLine);
		for (int i = 0; i < nLineCount; i++)
		{
			pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			if (!IsLeftInput())
			{
				//列显竖排行内顺序不变，行顺序改变
				//pTextLine->ResetVertical();
			}
			pTextLine->CalWordsAndLinePos();
			//两端对齐排版
			if (pParaObj->eAlignHorizontal == TEXT_ALIGN_H_ENDS)
				pTextLine->EndAlign(pvGraphScale);
		}
	}
	else
	{
		for (int i = 0; i < nWordInfoCnt; ++i)
		{
			LPCommonWordInfo pWordInfo = GetWordInfo(i);
			pTextLine->AddWordVertical(pWordInfo, pLastWordInfo, pvGraphScale, bAddWordWrap);
			pLastWordInfo = pWordInfo;
		}

		if (!IsLeftInput())
		{
			//pTextLine->ResetVertical();
		}
		pTextLine->CalWordsAndLinePos();
		//两端对齐排版
		if (pParaObj->eAlignHorizontal == TEXT_ALIGN_H_ENDS)
			pTextLine->EndAlign(pvGraphScale);
	}

	int nLineCount = BaseAR_GetCount(&m_arLine);
	pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, 0);
	BaseGraphSizef* pSzLineReal = pTextLine->GetRealSizePtr();
	m_fRealHeight = max(0, pSzLineReal->cy);

	//非末段
	BOOL bNotLastPara = m_nParaIndex != nParaCnt - 1;

	float fOffsetX = 0;
	float fLastOffsetX = 0;
	if (pTextObj->bLeftInput)
	{
		for (int i = 0; i < nLineCount; i++)
		{
			pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			float fOffsetY = 0;
			pSzLineReal = pTextLine->GetRealSizePtr();
			switch (pParaObj->eAlignHorizontal)
			{
			case TEXT_ALIGN_H_LEFT:
			case TEXT_ALIGN_H_ENDS:
				fOffsetY = 0;
				break;
			case TEXT_ALIGN_H_CENTER:
				fOffsetY =(m_fRealHeight - pSzLineReal->cy) / 2;
				break;
			case TEXT_ALIGN_H_RIGHT:
				fOffsetY = m_fRealHeight - pSzLineReal->cy;
				break;
			}
			point2f ptLinePos;
			ptLinePos.x = fOffsetX;
			ptLinePos.y = fOffsetY + GetIndent() + GetSymbolWidth();
			pTextLine->SetLineOffset(ptLinePos);
			fOffsetX += (pSzLineReal->cx + pTextLine->GetLineDistance());

			if (fOffsetX < fLastOffsetX)
				fOffsetX = fLastOffsetX;
			fLastOffsetX = fOffsetX;
		}

		if (!bNotLastPara)
			fOffsetX -= pTextLine->GetLineDistance();
	}
	else
	{
		for (int i = nLineCount-1; i >= 0; i--)
		{
			pTextLine = (CBaseTextLine*)BaseAR_GetAt(&m_arLine, i);
			float fOffsetY = 0;
			pSzLineReal = pTextLine->GetRealSizePtr();
			switch (pParaObj->eAlignHorizontal)
			{
			case TEXT_ALIGN_H_LEFT:
			case TEXT_ALIGN_H_ENDS:
				fOffsetY = 0;
				break;
			case TEXT_ALIGN_H_CENTER:
				fOffsetY =(m_fRealHeight - pSzLineReal->cy) / 2;
				break;
			case TEXT_ALIGN_H_RIGHT:
				fOffsetY = m_fRealHeight - pSzLineReal->cy;
				break;
			}

			//扣除末段末行的行间距
			if (bNotLastPara || i != nLineCount-1)
				fOffsetX += pTextLine->GetLineDistance();

			if (fOffsetX < fLastOffsetX)
				fOffsetX = fLastOffsetX;
			fLastOffsetX = fOffsetX;

			point2f ptLinePos;
			ptLinePos.x = fOffsetX;
			ptLinePos.y = fOffsetY + GetIndent() + GetSymbolWidth();
			pTextLine->SetLineOffset(ptLinePos);
			fOffsetX += pSzLineReal->cx;
		}
	}

	m_fWidth = fOffsetX;
	m_fRealWidth = m_fWidth;
}

void CBaseTextParagraph::AddBlankLine(CBaseTextLine* pLine)
{
	//添加一个空行，pLine由主调者申请。

	LPCommonText pTextObj = m_pTextObj;
	int nLastParaIdx = m_nParaIndex - 1;
	if (nLastParaIdx >= 0)
	{
		//取上一段最后一个字的属性
		BaseARray* pParaArray = BaseGraphTypeset_GetTextParagraphArray(m_pTextObj);
		CBaseTextParagraph* pLastPara = (CBaseTextParagraph*)BaseAR_GetAt(pParaArray, nLastParaIdx);
		int nWordInfoCnt = pLastPara->GetWordInfoCount();
		LPCommonWordInfo pWordInfo = pLastPara->GetWordInfo(nWordInfoCnt - 1);
		if(pWordInfo)
			pLine->AddNullWord(pWordInfo);
	}
	else
	{
		//生成一个“\0”字符
		LPCommonWordInfo pWordInfo = new BaseGraphWordInfo;
		BaseGraphTypeset_GenerateWordInfo(m_pTextObj, pWordInfo, ' ', 0);
		pWordInfo->wChar = '\0';
		pLine->AddNullWord(pWordInfo);
		BaseGraphWordInfoRelease(pWordInfo);
		SAFE_DELETE(pWordInfo);
	}
}
