#include <vector>
#include "stdafx.h"
#include "TextTypeset.h"
#include "TextTypesetDataAPI.h"
#include "TextParagraph.h"
#include "DataRender\DataRenderUtityFunc.h"
#include "DataRender\ShapeForm.h"
#include <assert.h>
//#include "BeCurve.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
BaseGraphTypesetInterface g_TypesetInterface;
#define BaseGraphTYPESET_HAS_ENV	(m_pTypesetPrivate->lpParam != NULL)
#define BaseGraphTYPESET_ISSTRETCH(pStretch)	((pStretch) != NULL && (pStretch)->fThick > ABS_FLOAT_0)

//过滤换行符，所有的\r\n和\r均转换为\n
int BaseGraphText_AdjustParaMark(LPTSTR szDes, LPCTSTR szSrc)
{
	assert(szSrc);
	int nLength = (int)_tcslen(szSrc);

	int nReadIndex = 0;
	int nWriteIndex = 0;
	while (nReadIndex < nLength)
	{
		if (szSrc[nReadIndex] == _T('\r'))
		{
			if (szDes)
				szDes[nWriteIndex] = _T('\n');
			nReadIndex += 1;
			nWriteIndex += 1;
			if (szSrc[nReadIndex] == _T('\n'))
			{
				nReadIndex += 1;
			}
		}
		else
		{
			if (szDes)
				szDes[nWriteIndex] = szSrc[nReadIndex];
			nWriteIndex += 1;
			nReadIndex += 1;
		}
	}

	return nWriteIndex;
}


//////////////////////////////////////////////////////////////////////////
// CBaseTypeset
CBaseTypeset::CBaseTypeset()
{
	m_pTextObj = NULL;
	m_pTypesetPrivate = NULL;
	m_bInsertReturn = FALSE;

	BaseGraphTextProp_Init(&m_TextProp);
	BaseGraphFont_Init(&m_font);
	BaseGraphStaticEffect_Init(&m_fillColor);
	m_fillColor.face.disable = 0;
	m_fillColor.face.color.color = 0xff0000ff;
	BaseGraphMaterial_Init(&m_3DColor);
	BaseGraphUnderLine_Init(&m_underLine);
#ifdef _DEBUG
	m_nLockCount = 0;
#endif
	InitializeCriticalSection(&m_TypesetLock);

	m_pVecPageInfo = NULL;
	m_bShowAllPage = TRUE;
}

CBaseTypeset::~CBaseTypeset()
{
	BaseGraphTextProp_Release(&m_TextProp);
	BaseGraphFont_Release(&m_font);
	BaseGraphStaticEffect_Release(&m_fillColor);
	BaseGraphMaterial_Release(&m_3DColor);
	BaseGraphUnderLine_Release(&m_underLine);

	DeleteCriticalSection(&m_TypesetLock);
}

void CBaseTypeset::Lock()
{
	EnterCriticalSection(&m_TypesetLock);
#if _DEBUG
	m_nLockCount++;
	assert(m_nLockCount <= 60);
#endif
}

void CBaseTypeset::Unlock()
{
#if _DEBUG
	m_nLockCount--;
	assert(m_nLockCount >= 0);
#endif
	LeaveCriticalSection(&m_TypesetLock);
}

void CBaseTypeset::Attach(LPCommonText pTextObj)
{
	if (pTextObj != NULL)
	{
		LPCommonTypesetPrivate pTypesetPrivate = (LPCommonTypesetPrivate)pTextObj->pTypesetPrivate;
		if (m_pTextObj == pTextObj && m_pTypesetPrivate == pTypesetPrivate && pTypesetPrivate != NULL)
			return;

		if (pTypesetPrivate == NULL)
		{
			pTypesetPrivate = new BaseGraphTypesetPrivate;
			BaseGraphTypesetPrivateData_Init(pTypesetPrivate);
			pTextObj->pTypesetPrivate = pTypesetPrivate;
		}

		m_pTextObj = pTextObj;
		m_pTypesetPrivate = pTypesetPrivate;

		//更新段中的排版器接口
		BaseARray* pParaAry = &m_pTypesetPrivate->m_arParagraph;
		int nParaCount = BaseAR_GetCount(pParaAry);
		for (int i = 0; i < nParaCount; i++)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(pParaAry, i);
			pParagraph->SetParagraphInfo(pTextObj, i);
		}
	}
}

void CBaseTypeset::Attach(LPCommonText pTextObj, LPVOID lpParam)
{
	if (pTextObj != NULL)
	{
		LPCommonTypesetPrivate pTypesetPrivate = (LPCommonTypesetPrivate)pTextObj->pTypesetPrivate;
		if (m_pTextObj == pTextObj && m_pTypesetPrivate == pTypesetPrivate && pTypesetPrivate != NULL)
			return;

		if (pTypesetPrivate == NULL)
		{
			pTypesetPrivate = new BaseGraphTypesetPrivate;
			BaseGraphTypesetPrivateData_Init(pTypesetPrivate);
			pTextObj->pTypesetPrivate = pTypesetPrivate;
		}

		m_pTextObj = pTextObj;
		pTextObj->lpParam = lpParam;
		m_pTypesetPrivate = pTypesetPrivate;
		m_pTypesetPrivate->lpParam = lpParam;

		//更新段中的排版器接口
		BaseARray* pParaAry = &m_pTypesetPrivate->m_arParagraph;
		int nParaCount = BaseAR_GetCount(pParaAry);
		for (int i = 0; i < nParaCount; i++)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(pParaAry, i);
			pParagraph->SetParagraphInfo(pTextObj, i);
		}
	}
}

void CBaseTypeset::ReleaseAllTempData()
{
	ReleaseParagraph();	
}

BOOL CBaseTypeset::ValidateTypesetPrivateData()
{
	if (m_pTypesetPrivate != NULL)
	{
		if (BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph) != BaseAR_GetCount(&m_pTextObj->arParagraph))
		{
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

LPCTSTR CBaseTypeset::GetText()
{
	if (m_pTextObj != NULL)
	{
		return m_pTextObj->szText;
	}
	return NULL;
}

int CBaseTypeset::GetTextLength()
{
	if (m_pTextObj != NULL && m_pTextObj->szText != NULL)
	{
		return (int)_tcslen(m_pTextObj->szText);
	}
	return 0;
}

LPCTSTR CBaseTypeset::GetString(int nStart, int nEnd)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nStart < 0)
		nStart = 0;
	if (nStart > nStrLen)
		nStart = nStrLen;
	if (nEnd < 0 || nEnd > nStrLen)
		nEnd = nStrLen;

	int nNewCnt = nEnd - nStart;
	if (nNewCnt < 0)
		return NULL;

	LPTSTR szRet = new TCHAR[nNewCnt + 1];
	memset(szRet, 0, sizeof(TCHAR) * (nNewCnt + 1));
	_tcsncpy_s(szRet, nNewCnt + 1, m_pTextObj->szText + nStart, nNewCnt);

	return szRet;
}

void CBaseTypeset::GetProperty(int nIndex,
						LPCommonFont& pFont, 
						LPCommonStaticEffect& p2DColor, 
						BaseGraphMaterial*& p3DColor, 
						LPCommonUnderLine& pUnderLine)
{
	pFont = (LPCommonFont)GetFont(nIndex);
	p2DColor = (LPCommonStaticEffect)GetTextStaticEffect(nIndex);
	p3DColor = (BaseGraphMaterial*)GetText3DColor(nIndex);
	pUnderLine = (LPCommonUnderLine)GetUnderLine(nIndex);
}

LPCommonTextProp CBaseTypeset::GetTextProp()
{
	LPCommonTextProp pTextProp;
	pTextProp = &m_pTextObj->stuTextProp;
	if (pTextProp != NULL)
	{
		BaseGraphTextProp_Copy(&m_TextProp, pTextProp);
	}
	return &m_TextProp;
}

LPCBaseFont CBaseTypeset::GetFont(int nIndex)
{
	LPCommonFont pFont;
	pFont = BaseGraphTextProp_GetFont(&m_pTextObj->stuTextProp, nIndex);
	if (pFont != NULL)
	{
		BaseGraphFont_Copy(&m_font, pFont);
	}
	return &m_font;
}

void CBaseTypeset::SetFont(LPCBaseFont pFont)
{
	if (pFont != NULL)
	{
		BaseGraphFont_Copy(&m_font, pFont);
	}
}

void CBaseTypeset::SetFont(int nIndex, int nCount, LPCBaseFont lpFont, DWORD dwMask)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
	{
		if (nStrLen > 0)
		{
			return;
		}
		else
		{
			nIndex = 0;
			nCount = 1;
		}
	}

	BaseGraphTextProp_SetFont(&m_pTextObj->stuTextProp, nIndex, nCount, lpFont, dwMask);
}

void CBaseTypeset::SetFontProperty(int nIndex, int nCount, const LPVOID lpPropData, const LPVOID lp2ndPropData, DWORD dwMask)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
	{
		if (nStrLen > 0)
		{
			return;
		}
		else
		{
			nIndex = 0;
			nCount = 1;
		}
	}
	BaseGraphTextProp_FontProperty(&m_pTextObj->stuTextProp, nIndex, nCount, lpPropData, lp2ndPropData, dwMask);
}

void CBaseTypeset::UpdateFontProperty(float fWRate, float fHRate)
{
	BaseARray* paryFont = &m_pTextObj->stuTextProp.arFont;
	int nPropCnt = BaseAR_GetCount(paryFont);
	if (nPropCnt <= 0)
		return;

	LPCommonFont pCurFont;
	for (int nC=0;nC<nPropCnt;nC++)
	{
		//查找当前字符的字体属性索引
		pCurFont = (LPCommonFont)BaseAR_GetAt(paryFont,nC);
		int nTemp=0;
		if (pCurFont != NULL)
		{
			nTemp=(int)pCurFont->lfWidth*fWRate;
			pCurFont->lfWidth=(float)nTemp;
			nTemp=(int)pCurFont->lfHeight*fHRate;
			pCurFont->lfHeight=(float)nTemp;
			nTemp=(int)pCurFont->lfDistance*fWRate;
			pCurFont->lfDistance=(float)nTemp;
		}
	}
}
LPCBaseUnderLine CBaseTypeset::GetUnderLine(int nIndex)
{
	LPCommonUnderLine pUnderLine;
	pUnderLine = BaseGraphTextProp_GetUnderLine(&m_pTextObj->stuTextProp, nIndex);
	if (pUnderLine != NULL)
	{
		BaseGraphUnderLine_Copy(&m_underLine, pUnderLine);
	}
	return &m_underLine;
}
void CBaseTypeset::SetUnderLine(int nIndex, 
						int nCount,
						LPCBaseUnderLine lpUnderLine,
						DWORD dwMask)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
	{
		if (nStrLen > 0)
		{
			return;
		}
		else
		{
			nIndex = 0;
			nCount = 1;
		}
	}

	BaseGraphTextProp_SetUnderLine(&m_pTextObj->stuTextProp, nIndex, nCount, lpUnderLine, dwMask);
}

void CBaseTypeset::SetUnderLineProperty(int nIndex, 
								int nCount,
								const LPVOID lpPropData,
								DWORD dwMask)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
	{
		if (nStrLen > 0)
		{
			return;
		}
		else
		{
			nIndex = 0;
			nCount = 1;
		}
	}
	BaseGraphTextProp_SetUnderLineProperty(&m_pTextObj->stuTextProp, nIndex, nCount, lpPropData, dwMask);
}

LPCBaseStaticEffect CBaseTypeset::GetTextStaticEffect(int nIndex)
{
	LPCommonStaticEffect p2DColor;
	p2DColor = BaseGraphTextProp_GetStaticEffect(&m_pTextObj->stuTextProp, nIndex);
	if (p2DColor != NULL)
	{
		BaseGraphStaticEffect_Copy(&m_fillColor, p2DColor);
	}
	return &m_fillColor;
}

void CBaseTypeset::SetTextStaticEffect(int nIndex, int nCount, LPCBaseStaticEffect lpFillColor, DWORD dwMask)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
	{
		if (nStrLen > 0)
		{
			return;
		}
		else
		{
			nIndex = 0;
			nCount = 1;
		}
	}
	BaseGraphTextProp_SetStaticEffect(&m_pTextObj->stuTextProp, nIndex, nCount, lpFillColor, dwMask);
}

BaseGraphMaterial* CBaseTypeset::GetText3DColor(int nIndex)
{
	BaseGraphMaterial* p3DColor;
	p3DColor = BaseGraphTextProp_Get3DColor(&m_pTextObj->stuTextProp, nIndex);
	if (p3DColor != NULL)
	{
		BaseGraphMaterial_Copy(&m_3DColor, p3DColor);
	}
	return &m_3DColor;
}

void CBaseTypeset::SetText3DColor(int nIndex, int nCount, BaseGraphMaterial* p3DColor)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
		return;
	BaseGraphTextProp_Set3DColor(&m_pTextObj->stuTextProp, nIndex, nCount, p3DColor);
}
void CBaseTypeset::SetTextSubFace3DColor(int nIndex, int nCount, int nSubFace, BaseGraphMaterial* p3DColor)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
		return;

	BaseGraphTextProp_SetSubFace3DColor(&m_pTextObj->stuTextProp, nIndex, nCount, nSubFace, p3DColor);
}

void CBaseTypeset::UpLoadColorToUnit(int nIndex, int nCount, DWORD dwUpdate)
{
	//更新Unit数据
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	int nEndIdx = nIndex + nCount;
	if (nEndIdx > nStrLen || nEndIdx <= 0)
	{
		return;
	}
	//上载什么类型Unit
	BOOL bUpload2D = FALSE;
	BOOL bUpload3D = FALSE;
	if (dwUpdate == BaseGraphTEXT_UNIT_AUTO)
	{
		if (BaseGraphTYPESET_ISSTRETCH(m_pTextObj->pStretch))
			bUpload3D = TRUE;
		else
			bUpload2D = TRUE;
	}
	else
	{
		if ((dwUpdate & BaseGraphTEXT_UNIT_2D) != 0)
			bUpload2D = TRUE;
		if ((dwUpdate & BaseGraphTEXT_UNIT_3D) != 0)
			bUpload3D = TRUE;
	}

	BOOL bHasEnv = BaseGraphTYPESET_HAS_ENV;
	LPCommonTextProp pTextProp = &m_pTextObj->stuTextProp;
	BaseARray* p2DUnitAry = &m_pTextObj->ar2DUnit;
	if (m_pTextObj->bPageMode)
	{
		p2DUnitAry = &m_pTextObj->ar2DUnitTemp;
	}
	BaseARray* p3DUnitAry = p2DUnitAry;
	int n2DUnitCnt = BaseAR_GetCount(p2DUnitAry);
	int n3DUnitCnt = BaseAR_GetCount(p3DUnitAry);
	int nAllWordInfoCnt = 0;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);
	for (int nP=0; nP<nDynParaCnt; ++nP)
	{
		LPCommonTextParagraph pParagraph = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nP);
		CBaseTextParagraph* pDynPara = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nP);

		BaseARray* paryWordInfo = &pDynPara->m_arWordInfo;
		int nWordInfoCnt = BaseAR_GetCount(paryWordInfo);
		for (int i = 0; i < nWordInfoCnt; ++i)
		{
			LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, i);
			int nCharIdx = pParagraph->nBeginIndex + pWordInfo->nCharIndex;
			if (nCharIdx >= nIndex && nCharIdx < nEndIdx)
			{
				int nUnitIdx = i + nAllWordInfoCnt;
				//2D Unit
				if (bUpload2D)
				{
					BaseGraphUnit* p2DUnit = (BaseGraphUnit*)BaseAR_GetAt(p2DUnitAry, nUnitIdx);
					if (p2DUnit != NULL)
					{
						LPCommonStaticEffect p2DColor = BaseGraphTextProp_GetStaticEffect(pTextProp, nCharIdx);
						BaseGraphStaticEffect_Copy(&p2DUnit->u.u2d.staticeffect, p2DColor);
					}
				}
				//3D Unit
				if (bUpload3D)
				{
					BaseGraphUnit* p3DUnit = (BaseGraphUnit*)BaseAR_GetAt(p3DUnitAry, nUnitIdx);
					if (p3DUnit != NULL)
					{
						LPVOID* par3DColor = BaseGraphTextProp_Get3DColorArray(pTextProp, nCharIdx);
						if (par3DColor != NULL)
						{
							BaseGraphModel* pMod = p3DUnit->u.u3d.pmod;
							int nSubView = min(pMod->nviewsub, BaseGraphTEXT_SUBFACE_CNT);
							for (int iM = 0; iM < nSubView; ++iM)
							{
								BaseGraphMaterial* pUnitMat = (BaseGraphMaterial*)(pMod->viewsub[iM].pMaterial);
								if (pUnitMat == NULL)
								{
									pUnitMat = new BaseGraphMaterial;
									BaseGraphMaterial_Init(pUnitMat);
									pMod->viewsub[i].pMaterial = pUnitMat;
								}
								BaseGraphMaterial_Copy(pUnitMat, (BaseGraphMaterial*)(par3DColor[iM]));
							}
						}
					}
				}
				//更新Env
				if (bHasEnv)
					OnRefreshUnit(nUnitIdx, 1);
			}
		}

		nAllWordInfoCnt += nWordInfoCnt;
	}
}
void CBaseTypeset::GetBorderRect(rectf& rtBorder)
{
	rtBorder = m_pTextObj->rtBorder;
}
void CBaseTypeset::SetBorderRect(rectf rtBorder)
{
	if (rtBorder.left == m_pTextObj->rtBorder.left 
		&& rtBorder.right == m_pTextObj->rtBorder.right
		&& rtBorder.top == m_pTextObj->rtBorder.top
		&& rtBorder.bottom == m_pTextObj->rtBorder.bottom)
	{
		return;
	}
	else
	{
		m_pTextObj->rtBorder = rtBorder;
		//如果矩形大小发生变化，要重新对段排版
		if ((rtBorder.right - rtBorder.left != m_pTextObj->rtBorder.right - m_pTextObj->rtBorder.left && m_pTextObj->bHorizontal)
			|| (rtBorder.bottom - rtBorder.top != m_pTextObj->rtBorder.bottom - m_pTextObj->rtBorder.top && !m_pTextObj->bHorizontal))
		{
			ReleaseParagraph();
		}
		Typeset(CBaseTypeset::TTLArrange);
	}
}

void CBaseTypeset::GetTypesetRect(rectf& rtTypeset, BOOL bReCal)
{
	if (bReCal)
	{
		rtTypeset = m_pTextObj->rtBorder;
		if (BaseRectf_IsRectEmpty(&rtTypeset))
		{
			memset(&rtTypeset, 0, sizeof(rectf));
		}
	}
	else
	{
		rtTypeset = m_pTypesetPrivate->rtTypesetBorder;
	}
}

void CBaseTypeset::SetTypesetRect(rectf& rtTypeset)
{
	m_pTypesetPrivate->rtTypesetBorder = rtTypeset;
}

void CBaseTypeset::GetModelRect(rectf& rtRect, BOOL bIgnoreSpecial)
{
	BaseARray* paryUnit = &m_pTextObj->ar2DUnit;
	int nUnitCnt = BaseAR_GetCount(paryUnit);
	//忽略特殊效果Unit
	if (bIgnoreSpecial)
		nUnitCnt -= m_pTextObj->nSpecialUnitCnt;
	if (nUnitCnt > 0)
	{
		float fLeft, fTop, fRight, fBottom;
		rectf* pRect;
		BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnit, 0);
		pRect = &(pUnit->u.u2d.graph.bound);
		fLeft = pRect->left + pUnit->pos.x;
		fTop = pRect->top - pUnit->pos.y;
		fRight = pRect->right + pUnit->pos.x;
		fBottom = pRect->bottom - pUnit->pos.y;

		for (int i = 1; i < nUnitCnt; ++i)
		{
			pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnit, i);
			pRect = &(pUnit->u.u2d.graph.bound);
			fLeft = min(fLeft, pRect->left + pUnit->pos.x);
			fTop = min(fTop, pRect->top - pUnit->pos.y);
			fRight = max(fRight, pRect->right + pUnit->pos.x);
			fBottom = max(fBottom, pRect->bottom - pUnit->pos.y);
		}
		rtRect.left = fLeft;
		rtRect.top = fTop;
		rtRect.right = fRight;
		rtRect.bottom = fBottom;
	}
	else
	{
		memset(&rtRect, 0, sizeof(rectf));
	}
}
BOOL CBaseTypeset::IsHorizontal()
{
	return m_pTextObj->bHorizontal;
}
void CBaseTypeset::SetHorizontal(BOOL bHorizontal)
{
	if (m_pTextObj->bHorizontal != bHorizontal)
	{
		m_pTextObj->bHorizontal = bHorizontal;
		//竖排和横排切换需要重新取字模
		Typeset(CBaseTypeset::TTLWordChg);
	}
}
BOOL CBaseTypeset::IsLeftInput()
{
	return m_pTextObj->bLeftInput;
}

void CBaseTypeset::SetLeftInput(BOOL bLeftInput)
{
	if (m_pTextObj->bLeftInput != bLeftInput)
	{
		m_pTextObj->bLeftInput = bLeftInput;

		if (m_pTextObj->bHorizontal
			&& CBaseUsp::ScriptIsComplex(m_pTextObj->szText, 0, (int)_tcslen(m_pTextObj->szText)))
		{
			//横排版且是复杂文字将影响字模，需要重取字模。
			Typeset(CBaseTypeset::TTLWordChg);
		}
		else
		{
			//竖排版不影响字模，无需重取字模。
			Typeset(CBaseTypeset::TTLArrange);
		}
	}
}

BOOL CBaseTypeset::IsLineWrap()
{
	return m_pTextObj->bLineWrap;
}

void CBaseTypeset::SetLineWrap(BOOL bLineWrap)
{
	if (m_pTextObj->bLineWrap != bLineWrap)
	{
		m_pTextObj->bLineWrap = bLineWrap;
		Typeset(CBaseTypeset::TTLArrange);
	}
}

BOOL CBaseTypeset::IsRLC(int nIndex)
{
	int nLen = (int)_tcslen(m_pTextObj->szText);
	return (BaseGraphISCRLF(m_pTextObj->szText, nIndex, nLen));
}

BOOL CBaseTypeset::IsFixedFrame()
{
	return m_pTextObj->bFixedFrame;
}

BOOL CBaseTypeset::IsArtWord()
{
	return m_pTextObj->pArtWord != NULL;
}

BOOL CBaseTypeset::IsPathTypeset()
{
	return m_pTextObj->pTypesetPath != NULL;
}

BOOL CBaseTypeset::IsLinearTypeset()
{
	//线性排版
	return (m_pTextObj->pArtWord == NULL && m_pTextObj->pTypesetPath == NULL);
}

E_TextAlignVeritical CBaseTypeset::GetVerticalAlign()
{
	return m_pTextObj->eAlignVertical;
}

void CBaseTypeset::SetVerticalAlign(E_TextAlignVeritical eAlignVertical, int nLevel)
{
	if (m_pTextObj->eAlignVertical != eAlignVertical)
	{
		m_pTextObj->eAlignVertical = eAlignVertical;

		if (nLevel > CBaseTypeset::TTLNone)
			Typeset(nLevel);
	}
}

BaseARray* CBaseTypeset::GetDynParagraphArray()
{
	if (m_pTypesetPrivate != NULL)
	{
		return &m_pTypesetPrivate->m_arParagraph;
	}
	return NULL;
}

LPVOID CBaseTypeset::GetDynParagraph(int nParaIdx)
{
	if (m_pTypesetPrivate != NULL)
	{
		return (LPVOID)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
	}
	return NULL;
}

int CBaseTypeset::GetParagraphWordInfoCount(int nParaIdx)
{
	CBaseTextParagraph* pDynPara = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
	return BaseAR_GetCount(&pDynPara->m_arWordInfo);
}

BaseARray* CBaseTypeset::GetParagraphWordInfoArray(int nParaIdx)
{
	CBaseTextParagraph* pDynPara = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
	return &pDynPara->m_arWordInfo;
}

int CBaseTypeset::GetParagraphLineWordInfoCount(int nParaIdx, int nLineIdx)
{
	CBaseTextParagraph* pDynPara = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
	return pDynPara->GetLineWordInfoCount(nLineIdx);
}

int CBaseTypeset::GetParagraphTrailLineWordInfoBeginIndex(int nParaIdx)
{
	CBaseTextParagraph* pDynPara = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
	return pDynPara->GetTrailLineWordInfoBeginIndex();
}

int CBaseTypeset::GetParagraphIndexByCharPos(int nIndex)
{
	int nParaCount = BaseAR_GetCount(&m_pTextObj->arParagraph);
	for (int i = nParaCount - 1; i >= 0; --i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i);
		if (nIndex >= pParaObj->nBeginIndex)
			return i;
	}
	return -1;
}

BaseARray* CBaseTypeset::GetTextParagraphArray()
{
	return &m_pTypesetPrivate->m_arParagraph;
}

LPCommonTextParagraph CBaseTypeset::GetParagraph(int nIndex)
{
	int nParaCount = BaseAR_GetCount(&m_pTextObj->arParagraph);
	if (nIndex < 0)
	{
		nIndex = nParaCount - 1;
	}
	if (nIndex >= 0 && nIndex < nParaCount)
	{
		return (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nIndex);
	}
	return NULL;
}

int CBaseTypeset::GetParagraphCharCount(int nIndex)
{
	int nParaLen;

	BaseARray* paryPara = &m_pTextObj->arParagraph;
	int nParaCnt = BaseAR_GetCount(paryPara);
	LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nIndex);
	if (nIndex < nParaCnt-1)
		nParaLen = ((LPCommonTextParagraph)BaseAR_GetAt(paryPara, nIndex+1))->nBeginIndex;
	else
		nParaLen = (int)_tcslen(m_pTextObj->szText);
	nParaLen -= pParagObj->nBeginIndex;

	return nParaLen;
}

E_TextAlignHorizontal CBaseTypeset::GetParagraphHorAlign(int nParagIdx)
{
	LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nParagIdx);
	if (pParaObj != NULL)
	{
		return pParaObj->eAlignHorizontal;
	}
	return TEXT_ALIGN_H_LEFT;
}

void CBaseTypeset::SetParagraphHorAlign(E_TextAlignHorizontal eAlign, 
									  int nParagIdx,
									  int nCount,
									  int nLevel)
{
	BaseARray* parParag = &m_pTextObj->arParagraph;
	int nParagCnt = BaseAR_GetCount(parParag);
	if (nParagIdx < 0)
	{
		nParagIdx = 0;
		nCount = nParagCnt;
	}
	if (nCount <= 0)
		return;

	for (int i=nParagIdx; i<nParagIdx + nCount; ++i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(parParag, i);
		if (pParaObj != NULL)
			pParaObj->eAlignHorizontal = eAlign;
	}

	//立即排版
	if (nLevel > CBaseTypeset::TTLNone)
		Typeset(nLevel);
}

float CBaseTypeset::GetParagraphDistance(int nParagIdx)
{
	LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nParagIdx);
	if (pParaObj != NULL)
	{
		return pParaObj->fLineDistance;
	}
	return 0;
}

void CBaseTypeset::SetParagraphDistance(float fDistance, 
									  int nParagIdx,
									  int nCount,
									  int nLevel)
{
	BaseARray* parParag = &m_pTextObj->arParagraph;
	int nParagCnt = BaseAR_GetCount(parParag);
	if (nParagIdx < 0)
	{
		nParagIdx = 0;
		nCount = nParagCnt;
	}
	if (nCount <= 0)
		return;

	for (int i=nParagIdx; i<nParagIdx + nCount; ++i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(parParag, i);
		if (pParaObj != NULL)
			pParaObj->fLineDistance = fDistance;
	}

	//立即排版
	if (nLevel > CBaseTypeset::TTLNone)
		Typeset(nLevel);
}

void CBaseTypeset::Update2DUnitArray(int nLevel, BOOL bAllUnit/* = TRUE*/)
{
	float fGraphScaleX = m_pTextObj->vGraphScale.x;
	float fGraphScaleY = m_pTextObj->vGraphScale.y;
	BOOL bGraphScale = BaseGraphTYPESET_NEEDGRAPHSCALE(fGraphScaleX, fGraphScaleY);

	BaseGraphUnit* pUnit = NULL;
	BaseARray* paryUnit = &m_pTextObj->ar2DUnit;
	BaseARray* paryUnitTemp = &m_pTextObj->ar2DUnitTemp;
	int nUnitCount = BaseAR_GetCount(paryUnit);
	LPCommonTextParagraph pParagObj = NULL;
	CBaseTextParagraph* pDynParag = NULL;
	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);
	int chgIndex = 0;
	int chgCnt = m_chgCnt;
	int perCnt = 0;
	int chgBegin = 0;
	BOOL bFind = FALSE;
	BOOL bDelted = FALSE;
	int i2DUnitIndex = 0;

	m_pTextObj->nTextUnitCnt = 0;
	for (int nP=0; nP<nDynParaCnt; ++nP)
	{
		pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nP);
		pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nP);
		BaseARray* paryWordInfo = &pDynParag->m_arWordInfo;
		int nWordInfoCnt = BaseAR_GetCount(paryWordInfo);
		//文字Unit个数
		m_pTextObj->nTextUnitCnt += nWordInfoCnt;

		if (pDynParag->m_nChgLevel == CBaseTypeset::TTLWordDel && nLevel == CBaseTypeset::TTLWordDel)
		{
			//删除多余的
			if (!bDelted)
			{
				bDelted = TRUE;
				if (m_pTextObj->bPageMode)
				{
					// 公告板增删文字
					for (int nL = 0; nL<m_chgCnt; ++nL)
					{
						pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnitTemp, m_chgIndex+nL);
						common_release_unit(pUnit);
						SAFE_DELETE(pUnit);
					}
					BaseAR_RemoveAt(paryUnitTemp, m_chgIndex, m_chgCnt);
					BaseAR_RemoveAt(paryUnit, m_chgIndex, m_chgCnt);
				}
				else
				{
					for (int nL = 0; nL<m_chgCnt; ++nL)
					{
						pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnit, m_chgIndex+nL);
						common_release_unit(pUnit);
						SAFE_DELETE(pUnit);
					}
					BaseAR_RemoveAt(paryUnit, m_chgIndex, m_chgCnt);
				}
			}
			if (!IsLeftInput())
			{
				bFind = TRUE;
			}
		}

		// 更新部分和更新结束到段末位需要更新，否则显示容易重叠
		if (bFind)
		{
			chgBegin = 0;
			perCnt = nWordInfoCnt;
		}
		else
		{
			if (m_chgIndex >= chgIndex && m_chgIndex < chgIndex + nWordInfoCnt)
			{
				bFind = TRUE;
				chgBegin = m_chgIndex-chgIndex;
				perCnt = nWordInfoCnt - chgBegin;
			}
		}

		if (bFind)
		{
			// 完全排版重新取字模
			if (pDynParag->m_nChgLevel == CBaseTypeset::TTLWordAdd
				|| nLevel == CBaseTypeset::TTLReTypeset)
			{
				perCnt = min(perCnt, chgCnt);
				for (int i=chgBegin; i<chgBegin+perCnt; i++)
				{
					LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, i);
					int nCharIndex = pParagObj->nBeginIndex + pWordInfo->nCharIndex;
					if (m_pTextObj->bPageMode && !bAllUnit)
					{
						//只取得当前页Unit
						int pageStart = 0, pageEnd = 0;
						int pageSize = m_pVecPageInfo->size();
						if (pageSize == 0)
						{
							pageStart = 0;
							pageEnd = _tcslen(m_pTextObj->szText);
						}
						else
						{
							PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
							pageStart = pageInfo.nStartIndex;
							pageEnd = pageStart + pageInfo.nCharCnt - 1;
						}
						if (nCharIndex < pageStart || nCharIndex > pageEnd)
						{
							continue;
						}
						// 翻页，直接从缓存中取得
						BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnitTemp, chgIndex+i);
						BaseAR_InsertAt(paryUnit, chgIndex+i, pUnit, 1);
					}
					else if (m_pTextObj->bPageMode && m_pTextObj->bStartEdit)
					{
						// 双击进入编辑，直接从缓存中取得
						BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnitTemp, chgIndex+i);
						BaseAR_InsertAt(paryUnit, chgIndex+i, pUnit, 1);
					}
					else
					{
						LPCBaseStaticEffect pFillColor = GetTextStaticEffect(pParagObj->nBeginIndex + pWordInfo->nCharIndex);
						pUnit = new BaseGraphUnit;
						common_init_unit(pUnit, FALSE);
						CreateUnit(pUnit, pWordInfo, pFillColor);
						if (bGraphScale)
							vg_zoom(&(pUnit->u.u2d.graph), fGraphScaleX, fGraphScaleY, 1, 0, 0, 0);

						BaseAR_InsertAt(paryUnit, chgIndex+i, pUnit, 1);
						if (m_pTextObj->bPageMode)
						{
							BaseAR_InsertAt(paryUnitTemp, chgIndex+i, pUnit, 1);
						}
					}
				}

				if (chgCnt - perCnt == 0)
				{
					bFind = FALSE;
					chgBegin += perCnt;
					perCnt = nWordInfoCnt - chgBegin;
				}
			}

			if ((m_pTextObj->bPageMode && !m_bShowAllPage) || IsFixedFrame())
			{
				chgBegin = 0;
				perCnt = 0;
			}
			// 更改结束到段尾进行更新
			for (int i=chgBegin; i<chgBegin+perCnt; i++)
			{
				LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, i);
				LPCBaseStaticEffect pFillColor = GetTextStaticEffect(pParagObj->nBeginIndex + pWordInfo->nCharIndex);
				pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnit, chgIndex+i);
				//Copy内部会ReleaseGraph
				CopyVectorGraph(&pUnit->u.u2d.graph, &pWordInfo->vgGraph);
				BaseGraphStaticEffect_Copy(&pUnit->u.u2d.staticeffect, pFillColor);

				pUnit->dwUserID = BaseGraphUNIT_USERID_UNDEF;
				if (bGraphScale)
					vg_zoom(&(pUnit->u.u2d.graph), fGraphScaleX, fGraphScaleY, 1, 0, 0, 0);
			}	

			chgCnt -= perCnt;
			if (chgCnt <= 0)
			{
				bFind = FALSE;
			}
		}
		chgIndex += nWordInfoCnt;
	}

	// 删除文字unit以后的unit（下划线等，之后重新生成）
	int nWordCnt = m_pTextObj->nTextUnitCnt;
	int nTotalCnt = BaseAR_GetCount(&m_pTextObj->ar2DUnit);
	DeleteUnit(nWordCnt, nTotalCnt-nWordCnt);
}

void CBaseTypeset::Update3DUnitArray(int nLevel, BOOL bAllUnit/* = TRUE*/)
{
	if (BaseGraphTYPESET_ISSTRETCH(m_pTextObj->pStretch))
	{
		BaseGraphUnit* p2DUnit = NULL;
		BaseGraphUnit* p3DUnit = NULL;
		int nAll3DUnitCnt = 0;

		LPCommonTextProp pProp = &m_pTextObj->stuTextProp;
		BaseARray* pary2DUnit = &m_pTextObj->ar2DUnit;
		BaseARray* pary3DUnit = &m_pTextObj->ar3DUnit;
		int n2DUnitCnt = BaseAR_GetCount(pary2DUnit);
		int n3DUnitCnt = BaseAR_GetCount(pary3DUnit);

		LPCommonTextParagraph pParagObj;
		CBaseTextParagraph* pDynParag;
		BaseGraphMaterial** p3DColor;
		BaseARray* paryPara = &m_pTextObj->arParagraph;
		BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
		int nDynParaCnt = BaseAR_GetCount(paryDynPara);
		int chgIndex = 0;
		int chgCnt = m_chgCnt;
		int perCnt = 0;
		int chgBegin = 0;
		BOOL bFind = FALSE;
		BOOL bDelted = FALSE;
		int i2DUnitIndex = 0;

		////文字的Unit
		for (int nP=0; nP<nDynParaCnt; ++nP)
		{
			pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nP);
			pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nP);

			BaseARray* paryWordInfo = &pDynParag->m_arWordInfo;
			int nWordInfoCnt = BaseAR_GetCount(paryWordInfo);

			if (pDynParag->m_nChgLevel == CBaseTypeset::TTLWordDel)
			{
				//删除多余的
				if (!bDelted)
				{
					bDelted = TRUE;
					for (int nL = 0; nL<m_chgCnt; ++nL)
					{
						p3DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary3DUnit, m_chgIndex+nL);
						common_release_unit(p3DUnit);
						SAFE_DELETE(p3DUnit);
					}
					BaseAR_RemoveAt(pary3DUnit, m_chgIndex, m_chgCnt);
				}
			}

			// 更新部分和更新结束到段末位需要更新，否则显示容易重叠
			if (bFind)
			{
				chgBegin = 0;
				perCnt = nWordInfoCnt;
			}

			if (m_chgIndex >= chgIndex && m_chgIndex < chgIndex + nWordInfoCnt)
			{
				bFind = TRUE;
				chgBegin = m_chgIndex-chgIndex;
				perCnt = nWordInfoCnt - chgBegin;
			}

			if (bFind)
			{
				// 完全排版重新取字模
				if (pDynParag->m_nChgLevel == CBaseTypeset::TTLWordAdd
					|| nLevel == CBaseTypeset::TTLReTypeset)
				{
					perCnt = min(perCnt, chgCnt);
					for (int i=chgBegin; i<chgBegin+perCnt; i++)
					{
						LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, i);
						int nCharIndex = pParagObj->nBeginIndex + pWordInfo->nCharIndex;
						if (m_pTextObj->bPageMode && !bAllUnit)
						{
							//只取得当前页Unit
							int pageStart = 0, pageEnd = 0;
							int pageSize = m_pVecPageInfo->size();
							if (pageSize == 0)
							{
								pageStart = 0;
								pageEnd = _tcslen(m_pTextObj->szText);
							}
							else
							{
								PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
								pageStart = pageInfo.nStartIndex;
								pageEnd = pageStart + pageInfo.nCharCnt - 1;
							}
							if (nCharIndex < pageStart || nCharIndex > pageEnd)
							{
								continue;
							}
						}
						//p2DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, i2DUnitIndex++);
						p2DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, chgIndex+i);
						p3DColor = (BaseGraphMaterial**)BaseGraphTextProp_Get3DColorArray(pProp, pParagObj->nBeginIndex + pWordInfo->nCharIndex);
						p3DUnit = Create3DUnitUse2DUnit(p2DUnit, p3DColor);
						BaseAR_InsertAt(pary3DUnit, chgIndex+i, p3DUnit, 1);
					}

					if (chgCnt - perCnt == 0)
					{
						bFind = FALSE;
						chgBegin += perCnt;
						perCnt = nWordInfoCnt - chgBegin;
					}
				}

				if (m_pTextObj->bPageMode || IsFixedFrame())
				{
					chgBegin = 0;
					perCnt = 0;
				}
				// 更改结束到段尾进行更新
				for (int i=chgBegin; i<chgBegin+perCnt; i++)
				{
					LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, i);
					LPCBaseStaticEffect pFillColor = GetTextStaticEffect(pParagObj->nBeginIndex + pWordInfo->nCharIndex);

					p2DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, chgIndex+i);
					p3DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary3DUnit, chgIndex+i);
					p3DColor = (BaseGraphMaterial**)BaseGraphTextProp_Get3DColorArray(pProp, pParagObj->nBeginIndex + pWordInfo->nCharIndex);
					//2012.12.12. 字拉伸后有3个子面
					Modify2DUnitTo3DUnit(p2DUnit, p3DUnit, p3DColor);
				}

				p3DUnit->pos= p2DUnit->pos;
				chgCnt -= perCnt;
				if (chgCnt <= 0)
				{
					bFind = FALSE;
				}
			}
			chgIndex += nWordInfoCnt;
			nAll3DUnitCnt += nWordInfoCnt;
		}		

		////下滑线的Unit
		if (m_pTextObj->nUnderlineUnitCnt > 0)
		{
			for (int i = nAll3DUnitCnt; i < n2DUnitCnt; ++i)
			{
				p2DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, i);
				if (i < n3DUnitCnt)
				{
					p3DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary3DUnit, i);
					common_release_unit_material(p3DUnit);
					Modify2DUnitTo3DUnit(p2DUnit, p3DUnit, NULL);
				}
				else
				{
					p3DUnit = Create3DUnitUse2DUnit(p2DUnit, (BaseGraphMaterial**)NULL);
					BaseAR_Add(pary3DUnit, p3DUnit);
				}
				p3DUnit->pos= p2DUnit->pos;
			}
			nAll3DUnitCnt += m_pTextObj->nUnderlineUnitCnt;
		}

		////特殊效果的Unit
		if (m_pTextObj->nSpecialUnitCnt > 0)
		{
			for (int i = nAll3DUnitCnt; i < n2DUnitCnt; ++i)
			{
				p2DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, i);
				if (i < n3DUnitCnt)
				{
					p3DUnit = (BaseGraphUnit*)BaseAR_GetAt(pary3DUnit, i);
					common_release_unit_material(p3DUnit);
					Modify2DUnitTo3DUnit(p2DUnit, p3DUnit, NULL);
				}
				else
				{
					p3DUnit = Create3DUnitUse2DUnit(p2DUnit, (BaseGraphMaterial**)NULL);
					BaseAR_Add(pary3DUnit, p3DUnit);
				}
				p3DUnit->pos= p2DUnit->pos;
				common_set_unit_material_diffuse(p3DUnit, 0);
			}
			nAll3DUnitCnt += m_pTextObj->nSpecialUnitCnt;
		}
	}
}

int CBaseTypeset::GetUnitCount()
{
	return BaseAR_GetCount(&m_pTextObj->ar2DUnit);
}

int CBaseTypeset::Get3DUnitCount()
{
	return BaseAR_GetCount(&m_pTextObj->ar3DUnit);
}

BaseGraphUnit* CBaseTypeset::GetUnit(int nIndex)
{
	if (nIndex >= 0 && nIndex < BaseAR_GetCount(&m_pTextObj->ar2DUnit))
	{
		return (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar2DUnit, nIndex);
	}
	return NULL;
}

BaseGraphUnit* CBaseTypeset::Get3DUnit(int nIndex)
{
	if (nIndex >= 0 && nIndex < BaseAR_GetCount(&m_pTextObj->ar3DUnit))
	{
		return (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar3DUnit, nIndex);
	}
	return NULL;
}

void CBaseTypeset::CreateUnit(BaseGraphUnit* pUnit, LPCommonWordInfo pWordInfo, const static_effect* pFillColor)
{
	CopyVectorGraph(&pUnit->u.u2d.graph, &pWordInfo->vgGraph);
	BaseGraphStaticEffect_Copy(&pUnit->u.u2d.staticeffect, pFillColor);
}

BaseGraphUnit* CBaseTypeset::Create3DUnitUse2DUnit(BaseGraphUnit* p2DUnit, BaseGraphUnit* pTempl3DUnit)
{
	BaseGraphUnit* pNew3DUnit = common_create_3dunit_from_2dunit(p2DUnit, NULL);
	if (pTempl3DUnit == NULL)
		g_TypesetInterface.Modify2DUnitTo3DUnit(pNew3DUnit, p2DUnit, NULL, BaseGraphTEXT_SUBFACE_CNT, m_pTextObj->pStretch);
	else
		g_TypesetInterface.Modify2DUnitTo3DUnit(pNew3DUnit, p2DUnit, NULL/*???*/, BaseGraphTEXT_SUBFACE_CNT, m_pTextObj->pStretch);

	return pNew3DUnit;
}

BaseGraphUnit* CBaseTypeset::Create3DUnitUse2DUnit(BaseGraphUnit* p2DUnit, BaseGraphMaterial** p3DColor)
{
	BaseGraphUnit* pNew3DUnit = common_create_3dunit_from_2dunit(p2DUnit, NULL);
	g_TypesetInterface.Modify2DUnitTo3DUnit(pNew3DUnit, p2DUnit, p3DColor, BaseGraphTEXT_SUBFACE_CNT, m_pTextObj->pStretch);

	return pNew3DUnit;
}

void CBaseTypeset::Modify2DUnitTo3DUnit(BaseGraphUnit* p2DUnit, BaseGraphUnit* p3DUnit, BaseGraphMaterial** p3DColor)
{
	g_TypesetInterface.Modify2DUnitTo3DUnit(p3DUnit, p2DUnit, p3DColor, BaseGraphTEXT_SUBFACE_CNT, m_pTextObj->pStretch);
}

void CBaseTypeset::AddUnit(BaseGraphUnit* pUnit)
{
	BaseAR_Add(&m_pTextObj->ar2DUnit, pUnit);

	if (BaseGraphTYPESET_ISSTRETCH(m_pTextObj->pStretch))
	{
		BaseGraphUnit* pNew3DUnit = Create3DUnitUse2DUnit(pUnit, (BaseGraphUnit*)NULL);
		BaseAR_Add(&m_pTextObj->ar3DUnit, pNew3DUnit);
	}
}

void CBaseTypeset::InsertUnit(BaseGraphUnit* pUnit, int nIndex, BaseGraphUnit* pTempl3DUnit)
{
	if (nIndex >= 0 && nIndex <= BaseAR_GetCount(&m_pTextObj->ar2DUnit))
	{
		BaseAR_InsertAt(&m_pTextObj->ar2DUnit, nIndex, pUnit, 1);
	}

	if (BaseGraphTYPESET_ISSTRETCH(m_pTextObj->pStretch))
	{
		if (nIndex >= 0 && nIndex <= BaseAR_GetCount(&m_pTextObj->ar3DUnit))
		{
			BaseGraphUnit* pNew3DUnit = Create3DUnitUse2DUnit(pUnit, pTempl3DUnit);
			BaseAR_InsertAt(&m_pTextObj->ar3DUnit, nIndex, pNew3DUnit, 1);
		}
	}
}

void CBaseTypeset::DeleteUnit(int nIndex, int nCount)
{
	if (nIndex >= 0 && nIndex < BaseAR_GetCount(&m_pTextObj->ar2DUnit))
	{
		for (int i = 0; i < nCount; i++)
		{
			BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar2DUnit, nIndex + i);
			common_release_unit(pUnit);
			delete pUnit;
		}
		BaseAR_RemoveAt(&m_pTextObj->ar2DUnit, nIndex, nCount);
	}

	int n3DUnitCnt = BaseAR_GetCount(&m_pTextObj->ar3DUnit);
	if (nIndex >= 0 && nIndex < n3DUnitCnt)
	{
		if (n3DUnitCnt < nCount)
			nCount = n3DUnitCnt;
		for (int i = 0; i < nCount; i++)
		{
			BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar3DUnit, nIndex + i);
			common_release_unit(pUnit);
			delete pUnit;
		}
		BaseAR_RemoveAt(&m_pTextObj->ar3DUnit, nIndex, nCount);
	}
}

void CBaseTypeset::DeleteAllUnit(WORD wdUnitType)
{
	int nUnitCount;
	if ((wdUnitType & BaseGraphTEXT_UNIT_2D) != 0)
	{
		if (m_pTextObj->bPageMode)
		{
			nUnitCount = BaseAR_GetCount(&m_pTextObj->ar2DUnitTemp);
			for (int i = 0; i < nUnitCount; i++)
			{
				BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar2DUnitTemp, i);
				common_release_unit(pUnit);
				SAFE_DELETE(pUnit);
			}
		}
		else
		{
			nUnitCount = BaseAR_GetCount(&m_pTextObj->ar2DUnit);
			for (int i = 0; i < nUnitCount; i++)
			{
				BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar2DUnit, i);
				common_release_unit(pUnit);
				SAFE_DELETE(pUnit);
			}
		}
		BaseAR_RemoveAll(&m_pTextObj->ar2DUnit);
		BaseAR_RemoveAll(&m_pTextObj->ar2DUnitTemp);
		
	}
	if ((wdUnitType & BaseGraphTEXT_UNIT_3D) != 0)
	{
		nUnitCount = BaseAR_GetCount(&m_pTextObj->ar3DUnit);
		for (int i = 0; i < nUnitCount; i++)
		{
			BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar3DUnit, i);
			common_release_unit(pUnit);
			delete pUnit;
		}
		BaseAR_RemoveAll(&m_pTextObj->ar3DUnit);
	}
}

void CBaseTypeset::ReleaseUnit(int nIndex, int nCount)
{
	if (nIndex < 0)
	{
		ReleaseAllUnit();
		return;
	}

	for (int i = 0; i < nCount; i++)
	{
		BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar2DUnit, i);
		common_release_unit(pUnit);
	}

	int n3DUnitCnt = BaseAR_GetCount(&m_pTextObj->ar3DUnit);
	if (n3DUnitCnt < nCount)
		nCount = n3DUnitCnt;
	for (int i = 0; i < nCount; i++)
	{
		BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar3DUnit, i);
		common_release_unit(pUnit);
	}
}

void CBaseTypeset::ReleaseAllUnit()
{
	int nUnitCount = BaseAR_GetCount(&m_pTextObj->ar2DUnit);
	for (int i = 0; i < nUnitCount; i++)
	{
		BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar2DUnit, i);
		common_release_unit(pUnit);
	}

	nUnitCount = BaseAR_GetCount(&m_pTextObj->ar3DUnit);
	for (int i = 0; i < nUnitCount; i++)
	{
		BaseGraphUnit* pUnit = (BaseGraphUnit*)BaseAR_GetAt(&m_pTextObj->ar3DUnit, i);
		common_release_unit(pUnit);
	}
}

void CBaseTypeset::MP_OnDeleteString(int nIndex, int nCount, BOOL bWholeRender)
{
	BaseGraphTextProp_DeleteString(&m_pTextObj->stuTextProp, nIndex, nCount, GetTextLength(), bWholeRender);
}

void CBaseTypeset::MP_OnInsertString(int nIndex, int nCount)
{
	BaseGraphTextProp_InsertString(&m_pTextObj->stuTextProp, nIndex, nCount, GetTextLength());
}

BOOL CBaseTypeset::InsertTabChar(int nBeginIndexm, int nEndIndex, BOOL bPressShift, BOOL bPressCtrl)
{
	//todo Tab 用于分级缩进
	return TRUE;
}

BOOL CBaseTypeset::IsPageMode()
{
	return m_pTextObj->bPageMode;
}

void CBaseTypeset::SetPageMode(BOOL bPageMode)
{
	m_pTextObj->bPageMode = bPageMode;
}

BOOL CBaseTypeset::IsShowAllPage()
{
	return m_bShowAllPage;
}

void CBaseTypeset::SetShowPage(BOOL bShowAllPage, BOOL bReTypeset/* = TRUE*/)
{
	m_bShowAllPage = bShowAllPage;
	if (bReTypeset)
	{
		TypesetPartly(CBaseTypeset::TTLPageChange);
	}
}

UINT CBaseTypeset::GetCurPageIndex()
{
	return m_pTextObj->nCurPageIndex;
}

void CBaseTypeset::SetCurPageIndex(int iCurPageIndex)
{
	iCurPageIndex = max(0, iCurPageIndex);
	iCurPageIndex = min(m_pVecPageInfo->size()-1, iCurPageIndex);
	m_pTextObj->nCurPageIndex = iCurPageIndex;
	TypesetPartly(CBaseTypeset::TTLPageChange);
}

VectPage* CBaseTypeset::GetPageInfo()
{
	return m_pVecPageInfo;
}

void CBaseTypeset::SetPageInfo(VectPage* pPageInfo)
{
	//  直接操作公告板中的分页信息
	m_pVecPageInfo = pPageInfo;
}

void CBaseTypeset::Typeset(int nLevel, int nIndex/*=-1*/, int nCount/*=-1*/)
{
	// 默认全部修改
	if (nIndex == -1 && nCount == -1)
	{
		m_chgIndex = 0;
		m_chgCnt = _tcslen(m_pTextObj->szText);
	}
	else
	{
		m_chgIndex = nIndex;
		m_chgCnt = nCount;
	}
	if (m_chgIndex == 0 && m_chgCnt == _tcslen(m_pTextObj->szText) && nLevel >= TTLWordDel)
	{
		//全部更新
		nLevel = TTLReTypeset;
	}

	//加锁
	OnLock();

	SetParagraphChgLevel(nLevel, -1, FALSE);
	TypesetPartly(nLevel);

	//解锁
	OnUnLock();
}

void CBaseTypeset::TypesetPartly(int nLevel)
{
	int nCharCntOld = BaseAR_GetCount(&m_pTextObj->ar2DUnit);

	if (m_pTextObj->stuTextProp.arUnderLine.m_nSize != 0)
	{
		int nCnt = BaseAR_GetCount(&m_pTextObj->stuTextProp.arUnderLine);
		for (int i=0; i<nCnt; ++i)
		{
			LPCommonUnderLine pUnderLine = (LPCommonUnderLine)BaseAR_GetAt(&m_pTextObj->stuTextProp.arUnderLine, i);
			if (pUnderLine->fHeight != 0)
			{
				// 有下划线，全部更新
				OnRefreshAllUnit(nLevel);
				break;
			}
		}
	}

	//加锁
	OnLock();

	if (IsFixedFrame())
	{
		// 固定字框需要全部更新
		nLevel = TTLReTypeset;
		m_chgIndex = 0;
		m_chgCnt =  GetTextLength();
	}
	
	if (nLevel >= CBaseTypeset::TTLReTypeset)
	{
		DeleteAllUnit(BaseGraphTEXT_UNIT_2D3D);
		ReleaseAllTempData();
	}


	//准备排版使用的矩形(自动折行和固定字框有宽高范围限制)
	CalTypesetBorder();

	int nParaCount = BaseAR_GetCount(&m_pTextObj->arParagraph);
	int nDynParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	if (nParaCount != nDynParaCnt && nLevel < CBaseTypeset::TTLWordChg)
		nLevel = CBaseTypeset::TTLWordChg;
	if (m_pTextObj->bFixedFrame)
	{
		if (nLevel < CBaseTypeset::TTLScaleGraph)
			nLevel = CBaseTypeset::TTLScaleGraph;
		SetParagraphChgLevel(nLevel, -1, FALSE);
	}
	//time_t nTm = clock();
	//分段USP处理
	if (nLevel > TTLScaleFont)
	{
		//增删改文字需要更新(重新计算段落)
		SplitParagraph();
	}

	int nUnderLineCnt = 0;
	//计算出全部页的unit，如果只是页切换，不再重新计算unit，提高效率
	if (nLevel != CBaseTypeset::TTLPageChange)
	{
		//生成字模信息
		GenerateWordInfo();
		//TRACE(_T("TypesetPartly::GenerateWordInfo = %d\n"), clock() - nTm);nTm = clock();

		//更新段排版信息（段，行和内部字符的位置信息）
		GenerateParagraphInfo();
		//TRACE(_T("TypesetPartly::GenerateParagraphInfo = %d\n"), clock() - nTm);nTm = clock();
		//确定段的位置。如果固定字框，则本次为试算。
		SetParagraphPos();
		//TRACE(_T("TypesetPartly::SetParagraphPos = %d\n"), clock() - nTm);nTm = clock();
		if ((nLevel == CBaseTypeset::TTLDistance || nLevel == CBaseTypeset::TTLArrange)
			&& m_pTextObj->bPageMode && !IsShowAllPage())
		{
			// 字距改变和拖拽大小，不重新生成字模
			DeleteAllUnit(BaseGraphTEXT_UNIT_2D3D);
			nLevel = CBaseTypeset::TTLReTypeset;
			m_bShowAllPage = FALSE;
		}
		if (IsArtWord())
		{
			//艺术字unit需要重新生成
			DeleteAllUnit(BaseGraphTEXT_UNIT_2D3D);
			nLevel = CBaseTypeset::TTLReTypeset;
			m_chgIndex = 0;
			m_chgCnt =  GetTextLength();
		}
		//更新本体2DUnit数组
		Update2DUnitArray(nLevel);
		//TRACE(_T("TypesetPartly::Update2DUnitArray = %d\n"), clock() - nTm);nTm = clock();
		//固定字框
		if (m_pTextObj->bFixedFrame)
		{
			//固定字框计算
			FixedFrameCal();
			//二次确定段位置
			SetParagraphPos();
		}

		//下划线：下划线在字后面；允许3D拉伸。
		//未完成：下划线的艺术变形与沿路径排列。
		nUnderLineCnt = GenerateUnderLineInfo();

		//艺术字排版
		if (m_pTextObj->pArtWord != NULL)
		{
			ArtWordForm();
		}

		//沿路径排版
		if (m_pTextObj->pTypesetPath != NULL)
		{
			PathTypesetForm();
		}

		//更新下划线Unit
		UpdateUnderLineUnit(nUnderLineCnt);

		//确定每一个unit的位置
		UpdateUnitPos(nUnderLineCnt);
		//TRACE(_T("TypesetPartly::UpdateUnitPos = %d\n"), clock() - nTm);nTm = clock();
		//更新分页信息
		if (m_pTextObj->bPageMode)
		{
			UpdatePageInfo();
		}	
		
		MakeSpecialEffect();

		//更新本体3DUnit数组
		Update3DUnitArray(nLevel);
	}

	BOOL bPageChanged = FALSE;
	if (m_pTextObj->bPageMode)
	{
		if (IsShowAllPage() && nLevel != CBaseTypeset::TTLPageChange && nLevel >= CBaseTypeset::TTLArrange)
		{
			// 编辑状态，使用之前的unit即可，提高效率
		}
		else
		{
			if (m_pVecPageInfo != NULL && m_pVecPageInfo->size() >1)
			{
				// 只有一页，不重新计算
				//得到当前页的unit
				//DeleteAllUnit(BaseGraphTEXT_UNIT_2D3D);
				BaseAR_SetSize(&m_pTextObj->ar2DUnit, 0, 0);

				bPageChanged = TRUE;
				nLevel = TTLReTypeset;
				m_chgIndex = 0;
				m_chgCnt =  GetTextLength();

				//更新本体2DUnit数组
				Update2DUnitArray(nLevel, m_bShowAllPage);
				//TRACE(_T("TypesetPartly::Update2DUnitArray = %d\n"), clock() - nTm);nTm = clock();

				//下划线：下划线在字后面；允许3D拉伸。
				//未完成：下划线的艺术变形与沿路径排列。
				nUnderLineCnt = GenerateUnderLineInfo(m_bShowAllPage);

				//艺术字排版
				if (m_pTextObj->pArtWord != NULL)
				{
					ArtWordForm();
				}

				//沿路径排版
				if (m_pTextObj->pTypesetPath != NULL)
				{
					PathTypesetForm();
				}

				//更新下划线Unit
				UpdateUnderLineUnit(nUnderLineCnt, m_bShowAllPage);
				UpdateUnitPos(nUnderLineCnt, m_bShowAllPage);
				//TRACE(_T("TypesetPartly::UpdateUnitPos = %d\n"), clock() - nTm);nTm = clock();

				MakeSpecialEffect();

				//更新本体3DUnit数组
				Update3DUnitArray(nLevel, m_bShowAllPage);
			}
		}
	}
	m_pTextObj->bStartEdit = FALSE;

	int nParaIndex = GetParagraphIndexByCharPos(m_chgIndex);
	LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nParaIndex);
	CBaseTextParagraph* pParag = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIndex);

	// 当前页文字数量
	if (m_pTextObj->bPageMode && !m_bShowAllPage && GetTextLength() != 0 && m_pVecPageInfo != NULL)
	{
		PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
		m_chgIndex = 0;
		m_chgCnt = pageInfo.nCharCnt;
	}
	//更新RenderUnit数组
	if (nLevel == CBaseTypeset::TTLWordDel)
	{
		OnDeleteUnit(m_chgIndex, m_chgCnt, TRUE);
		if (IsFixedFrame())
		{
			// 固定字框，本段更新
			OnRefreshUnit(0, GetTextLength());
		}
	}
	else if (nLevel == CBaseTypeset::TTLWordAdd)
	{
		OnInsertUnit(m_chgIndex, m_chgCnt, TRUE);
		if (IsFixedFrame())
		{
			// 固定字框，本段更新
			OnRefreshUnit(0, GetTextLength());
		}
	}
	else if (nLevel == CBaseTypeset::TTLReTypeset)
	{
		if (IsPageMode() && bPageChanged)
		{
			// 翻页时效率优化
			int nDiff = m_chgCnt - nCharCntOld;
			if (nDiff > 0)
			{
				OnInsertUnit(0, nDiff, TRUE);
			}
			else if (nDiff < 0)
			{
				OnDeleteUnit(0, -nDiff, TRUE);
			}
			OnRefreshAllUnit(nLevel);

			//TRACE(_T("CurPageCharCnt=%d, Diff=%d, TypesetPartly::OnRefreshAllUnit = %d\n"), m_chgCnt, nDiff, clock() - nTm);nTm = clock();
		}
		else
		{
			OnRefreshUnit(0, GetTextLength());

// 			OnDeleteAllUnit();
// 			//TRACE(_T("TypesetPartly::OnDeleteAllUnit = %d\n"), clock() - nTm);nTm = clock();
// 			OnInsertUnit(m_chgIndex, m_chgCnt, TRUE);
// 			//TRACE(_T("TypesetPartly::OnInsertUnit = %d\n"), clock() - nTm);nTm = clock();
		}
	}
	else
	{
		OnRefreshUnit(m_chgIndex, m_chgCnt);
		if (IsFixedFrame())
		{
			// 固定字框，本段更新
			OnRefreshUnit(0, GetTextLength());
		}
	}

	SetParagraphChgLevel(CBaseTypeset::TTLNone, -1, TRUE);
	//TRACE(_T("TypesetPartly::End = %d\n"), clock() - nTm);nTm = clock();

	//解锁
	OnUnLock();
}

int CBaseTypeset::DeleteString(int nIndex, int nCount)
{
	OnLock();

	//返回实际删除的字符数
	int nDelCount = nCount;
	int nOldLength = (int)_tcslen(m_pTextObj->szText);

	if (nIndex < 0 || nDelCount < 0)
	{
		OnUnLock();
		return 0;
	}

	m_chgIndex = nIndex;
	m_chgUnitIndex = nIndex;

	if (m_chgIndex < 0 || m_chgIndex >= nOldLength)
	{
		OnUnLock();
		return 0;
	}
	if (m_chgIndex + nDelCount > nOldLength)
	{
		nDelCount = nOldLength - m_chgIndex;
	}

	if (m_chgIndex == nOldLength)
	{
		// 最后无需删除
		OnUnLock();
		return 0;
	}

	//回车与换行必须同时删除
	BOOL bDelRet = FALSE;
	//头
	if (BaseGraphISCRLF(m_pTextObj->szText, m_chgIndex, nOldLength))
	{
		if (nDelCount < 2)
		{
			++nDelCount;
		}
		bDelRet = TRUE;
	}
	else if (m_chgIndex >= 1 && BaseGraphISCRLF(m_pTextObj->szText, m_chgIndex-1, nOldLength))
	{
		--m_chgIndex;
		++nDelCount;
		bDelRet = TRUE;
	}

	//尾
	int nRearIdx = m_chgIndex + nDelCount - 1;
	if (nRearIdx >= 0 && BaseGraphISCRLF(m_pTextObj->szText, nRearIdx, nOldLength))
	{
		++nDelCount;
		bDelRet = TRUE;
	}
	m_chgCnt = nDelCount;

	//暂存删除的字符串
	LPTSTR szDelete = new TCHAR[nDelCount + 1];
	memset(szDelete, 0, sizeof(TCHAR) * (nDelCount + 1));
	_tcsncpy_s(szDelete, nDelCount + 1, m_pTextObj->szText + m_chgIndex, nDelCount);

	//生成新字符串
	LPTSTR	szNewText = new TCHAR[nOldLength - nDelCount + 1];
	memset(szNewText, 0, sizeof(TCHAR) * (nOldLength - nDelCount + 1));
	if (m_chgIndex > 0)
	{
		_tcsncpy_s(szNewText, nOldLength - nDelCount + 1, m_pTextObj->szText, m_chgIndex);
	}
	int nCharLeft = nOldLength - m_chgIndex - nDelCount;
	if (nCharLeft > 0)
	{
		_tcsncpy_s(szNewText+m_chgIndex, nCharLeft+1, m_pTextObj->szText+m_chgIndex+nDelCount, nCharLeft);
	}
	SAFEDELETE_ARRAY(m_pTextObj->szText);
	m_pTextObj->szText = szNewText;

	if (!IsLeftInput() && bDelRet)
	{
		OnUnLock();

		//删除回车,整体替换
		CString strNew = szNewText;
		ReplaceString(strNew,TRUE);
		return nDelCount;
	}

	//修正property数组的索引值
	MP_OnDeleteString(m_chgIndex, nDelCount, BaseGraphText_IsWholeRender(m_pTextObj));


	//更新段信息
	int nParaCount = BaseAR_GetCount(&m_pTextObj->arParagraph);
	int nParaIndex = GetParagraphIndexByCharPos(m_chgIndex);
	int nDeleParaCount = 0;
	for (int i = 0; i < nDelCount; i++)
	{
		if (BaseGraphISCRLF(szDelete, i, nDelCount))
		{
			nDeleParaCount += 1;
			++i;
		}
	}

	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	CBaseTextParagraph* pDynParag= (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nParaIndex);
	if (pDynParag != NULL)
		pDynParag->m_nChgLevel = CBaseTypeset::TTLWordDel;

	for (int i = nParaIndex + nDeleParaCount + 1; i < nParaCount; i++)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i);
		pParaObj->nBeginIndex -= nDelCount;
		pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
		if (pDynParag != NULL)
		{
			pDynParag->SetParagraphIndex(pDynParag->GetParagraphIndex() - nDeleParaCount);
			pDynParag->m_nChgLevel = CBaseTypeset::TTLOffset;
		}
	}
	for (int i = 0; i < nDeleParaCount; i++)
	{
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i + nParaIndex + 1);
		if (pParagraph != NULL)
			delete pParagraph;

		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i + nParaIndex + 1);
		BaseGraphText_ReleaseParagraph(pParaObj);
		SAFE_DELETE(pParaObj);
	}

	if (nDeleParaCount > 0)
	{
		BaseAR_RemoveAt(paryDynPara, nParaIndex + 1, nDeleParaCount);
		BaseAR_RemoveAt(&m_pTextObj->arParagraph, nParaIndex + 1, nDeleParaCount);
	}

	SAFE_DELETE(szDelete);

	nParaCount = BaseAR_GetCount(&m_pTextObj->arParagraph);
	for (int i = 0; i < nParaCount; ++i)
	{
		pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
		pDynParag->m_nChgLevel = CBaseTypeset::TTLWordDel;
	}

	TypesetPartly(CBaseTypeset::TTLWordDel);

	OnUnLock();
	return nDelCount;
}

int CBaseTypeset::InsertChar(int nIndex, TCHAR tChar)
{
	OnLock();

	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	if (nIndex > nTextLength)
		nIndex = nTextLength;
	if (nIndex < 0)
		nIndex = 0;

	m_bInsertReturn = FALSE;
	m_chgIndex = nIndex;
	m_chgCnt = 1;

	LPTSTR szNewText = new TCHAR[nTextLength + 2];
	memset(szNewText, 0, sizeof(TCHAR) * (nTextLength + 2));
	if (m_chgIndex > 0)
		_tcsncpy_s(szNewText, nTextLength + 2, m_pTextObj->szText, m_chgIndex);

	szNewText[m_chgIndex] = tChar;

	int nCharLeft = nTextLength - m_chgIndex;
	//字符串结尾符也要拷贝
	_tcsncpy_s(szNewText + m_chgIndex + 1, nCharLeft + 1, m_pTextObj->szText + m_chgIndex, nCharLeft + 1);

	SAFEDELETE_ARRAY(m_pTextObj->szText);
	m_pTextObj->szText = szNewText;

	MP_OnInsertString(m_chgIndex, 1);

	//m_chgIndex = nIndex;

	int nParaIndex = GetParagraphIndexByCharPos(m_chgIndex);
	int nParaCount = BaseAR_GetCount(&m_pTextObj->arParagraph);
	assert(nParaIndex >= 0 && nParaIndex < nParaCount);

	//当前段字符修改等级
	CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIndex);
	if (pDynParag != NULL)
		pDynParag->m_nChgLevel = CBaseTypeset::TTLWordAdd;

	for (int i = 0; i < nParaCount; ++i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i);
		if (i > nParaIndex)
		{
			pParaObj->nBeginIndex += 1;
		}

		pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
		pDynParag->m_nChgLevel = CBaseTypeset::TTLWordAdd;
	}

	TypesetPartly(CBaseTypeset::TTLWordAdd);

	OnUnLock();
	return 1;
}

int CBaseTypeset::InsertString(int nIndex, LPCTSTR szText)
{
	int nInsertLength = (int)_tcslen(szText);
	if (nInsertLength <= 0)
		return 0;

	CString strNewInsert = szText;
	if (IsArtWord() || IsPathTypeset())
	{
		// 艺术字和曲线文字目前只处理单行,插入的回车删除掉
		strNewInsert.Replace(_T("\r\n"), _T(""));
		strNewInsert.Replace(_T("\n"), _T(""));
	}
	if (strNewInsert.IsEmpty())
	{
		return 0;
	}
	LPCTSTR szOldInsertText = NULL;
	LPCTSTR szNewInsertText = szText;
	if (strNewInsert.Compare(szText) != 0)
	{
		szOldInsertText = szText;
		LPTSTR szNewInsert = new TCHAR[strNewInsert.GetLength() + 1];
		memcpy(szNewInsert, strNewInsert.GetBuffer(strNewInsert.GetLength()), sizeof(TCHAR)*(strNewInsert.GetLength() + 1));
		szNewInsertText = szNewInsert;
	}
	nInsertLength = (int)_tcslen(szNewInsertText);
	if (nInsertLength <= 0)
		return 0;

	OnLock();

	int nOldLength = (int)_tcslen(m_pTextObj->szText);
	if (nIndex > nOldLength)
		nIndex = nOldLength;
	if (nIndex < 0)
		nIndex = 0;

	m_bInsertReturn = FALSE;
	m_chgIndex = nIndex;
	m_chgCnt = nInsertLength;

	LPTSTR szInsert = new TCHAR[nInsertLength + 1];
	memcpy(szInsert, szNewInsertText, sizeof(TCHAR)*(nInsertLength + 1));

	//生成新字符串
	LPTSTR szNewText = new TCHAR[nOldLength + nInsertLength + 1];
	memset(szNewText, 0, sizeof(TCHAR) * (nOldLength + nInsertLength + 1));
	if (m_chgIndex > 0)
		_tcsncpy_s(szNewText, nInsertLength + nOldLength + 1, m_pTextObj->szText, m_chgIndex);

	_tcsncpy_s(szNewText + m_chgIndex, nInsertLength + nOldLength - m_chgIndex + 1, szInsert, nInsertLength);
	SAFEDELETE_ARRAY(szInsert);

	int nCharLeft = nOldLength - m_chgIndex;
	if (nCharLeft > 0)
		_tcsncpy_s(szNewText + m_chgIndex + nInsertLength, nCharLeft + 1, m_pTextObj->szText + m_chgIndex, nCharLeft);

	CString strOld = m_pTextObj->szText;
	CString strNew = szNewText;
	SAFEDELETE_ARRAY(m_pTextObj->szText);
	m_pTextObj->szText = szNewText;

	int nNewLength = (int)_tcslen(m_pTextObj->szText);
	for (int i = 0; i < nInsertLength; i++)
	{
		if (BaseGraphISCRLF(m_pTextObj->szText, m_chgIndex + i, nNewLength))
		{
			m_bInsertReturn = TRUE;
		}
	}

	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;

	MP_OnInsertString(nIndex, nInsertLength);

	//更新段信息
	int nParaCount = BaseAR_GetCount(paryPara);
	LPCommonTextParagraph pOldParaObj = NULL;
	for (int i = nParaCount - 1; i >= 0; i--)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, i);
		if (pParaObj->nBeginIndex <= nIndex)
		{
			pOldParaObj = pParaObj;
			CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
			if (pDynParag != NULL)
				pDynParag->m_nChgLevel = CBaseTypeset::TTLWordAdd;
			break;
		}
	}

	BaseARray arNewPara;
	BaseAR_Init(&arNewPara);
	for (int i = 0; i < nInsertLength; i++)
	{
		if (BaseGraphISCRLF(m_pTextObj->szText, nIndex + i, nNewLength))
		{
			m_bInsertReturn = TRUE;
			LPCommonTextParagraph pNewParaObj = new BaseGraphTextParagraph;
			BaseGraphText_InitParagraph(pNewParaObj);
			BaseGraphText_CopyParagraph(pNewParaObj, pOldParaObj);
			pNewParaObj->nBeginIndex = nIndex + i + 2;
			BaseAR_Add(&arNewPara, pNewParaObj);
			++i;
		}
	}

	int nNewParaCount = BaseAR_GetCount(&arNewPara);
	for (int i = nParaCount - 1; i >= 0; i--)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, i);
		if (pParaObj->nBeginIndex > nIndex)
		{
			pParaObj->nBeginIndex += nInsertLength;
			CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
			pDynParag->SetParagraphIndex(pDynParag->GetParagraphIndex() + nNewParaCount);
			pDynParag->m_nChgLevel = CBaseTypeset::TTLOffset;
		}
		else
		{
			if (nNewParaCount > 0)
			{
				BaseAR_InsertArrayAt(paryPara, i + 1, &arNewPara);
				nParaCount = BaseAR_GetCount(paryPara);
				for (int j = 0; j < nNewParaCount; j++)
				{
					CBaseTextParagraph* pNewParagraph = new CBaseTextParagraph(m_pTextObj, i + 1 + j);
					pNewParagraph->m_nChgLevel = CBaseTypeset::TTLWordAdd;
					BaseAR_InsertAt(paryDynPara, i + 1 + j, pNewParagraph, 1);
				}
				BaseAR_RemoveAll(&arNewPara);
			}

			break;
		}
	}
	int nCharCnt = 0;
	for (int i = 0; i < nParaCount; ++i)
	{
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
		pDynParag->m_nChgLevel = CBaseTypeset::TTLWordAdd;
	}

	TypesetPartly(CBaseTypeset::TTLWordAdd);

	OnUnLock();
	if (szOldInsertText != NULL)
	{
		szText = szOldInsertText;
	}
	return nInsertLength;
}

void CBaseTypeset::ReplaceString(LPCTSTR szText, BOOL bKeepProperty)
{
	//替换字符串。
	//1、可以指定是否保留各个字的属性。
	//2、假定该文字已经被排版过！

	if (m_pTextObj != NULL && m_pTypesetPrivate != NULL)
	{
		OnLock();

		if (m_pTextObj->szText)
		{
			//if (_tcscmp(m_pTextObj->szText, szText) == 0 && bKeepProperty)
			//{
			//	OnUnLock();
			//	return;
			//}

			delete m_pTextObj->szText;
			m_pTextObj->szText = NULL;
		}

		int nTextLength;
		if (szText != NULL)
		{
			nTextLength = (int)_tcslen(szText);
			m_pTextObj->szText = new TCHAR[nTextLength + 1];
			_tcscpy_s(m_pTextObj->szText, nTextLength + 1, szText);
		}
		else
		{
			nTextLength = 0;
			m_pTextObj->szText = new TCHAR[1];
			memset(m_pTextObj->szText, 0, sizeof(TCHAR));
		}

		//属性表调整
		int nNewPropCnt = nTextLength > 0 ? nTextLength : 1;
		BaseGraphTextProp_ResizePropIndexArray(&m_pTextObj->stuTextProp, nNewPropCnt, bKeepProperty);

		//多段文字的段落调整
		LPCommonTextParagraph pParaObj;
		std::vector<int> vParaIndex;
		BaseGraphText_SplitStringToParagraphArray(vParaIndex, m_pTextObj->szText);
		int nNewParaCnt = (int)vParaIndex.size();
		int nOldParaCnt = (int)BaseAR_GetSize(&m_pTextObj->arParagraph);
		if (nNewParaCnt < nOldParaCnt)
		{
			//删除多余段
			for (int i=nOldParaCnt-1; i>=nNewParaCnt; --i)
			{
				pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i);
				BaseGraphText_ReleaseParagraph(pParaObj);
				SAFE_DELETE(pParaObj);
				BaseAR_RemoveAt(&m_pTextObj->arParagraph, i, 1);
			}
		}
		else if (nNewParaCnt > nOldParaCnt)
		{
			//追加段
			pParaObj = NULL;
			if (nOldParaCnt > 0)
			{
				pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nOldParaCnt-1);
			}
			else
			{
				pParaObj = new BaseGraphTextParagraph;
				BaseGraphText_InitParagraph(pParaObj);
			}

			int nAddCnt = nNewParaCnt - nOldParaCnt;
			while(nAddCnt > 0)
			{
				LPCommonTextParagraph pParaNew = new BaseGraphTextParagraph;
				BaseGraphText_InitParagraph(pParaNew);
				BaseGraphText_CopyParagraph(pParaNew, pParaObj);
				BaseAR_Add(&m_pTextObj->arParagraph, pParaNew);
				--nAddCnt;
			}

			if (nOldParaCnt <= 0)
			{
				BaseGraphText_ReleaseParagraph(pParaObj);
				SAFE_DELETE(pParaObj);
			}
		}

		//刷新段中的字索引
		int nParaIndx = 0;
		for (int i = 0; i < nNewParaCnt; ++i)
		{
			pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i);
			pParaObj->nBeginIndex = vParaIndex[i];
		}

		Typeset(CBaseTypeset::TTLReTypeset);

		OnUnLock();
	}
}

BOOL CBaseTypeset::CopyStringToClipboard(int nStart, int nEnd)
{
	int nCopyStrCnt = nEnd - nStart + 1;
	if (nCopyStrCnt <= 0)
		return FALSE;
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nStart < 0 || nStart >= nStrLen || nEnd < 0 || nEnd >= nStrLen)
		return FALSE;

	::EmptyClipboard();
	if (::OpenClipboard(NULL))
	{
		int nStrSize = nCopyStrCnt + 1;
		LPTSTR szCopy = new TCHAR[nStrSize];
		_tcsncpy_s(szCopy, nStrSize, m_pTextObj->szText + nStart, nCopyStrCnt);
		szCopy[nCopyStrCnt] = _T('\0');

		HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, nStrSize*sizeof(TCHAR));
		void* pData = (void*)::GlobalLock(hData);
		memcpy(pData, szCopy, nStrSize*sizeof(TCHAR));
		::GlobalUnlock(hData);
		delete []szCopy;

		::SetClipboardData(CF_UNICODETEXT, hData);
		::CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

LPCTSTR CBaseTypeset::GetStringFromClipboard()
{
	LPTSTR szRet = NULL;
	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		if (::OpenClipboard(NULL))
		{
			HANDLE hData = ::GetClipboardData(CF_UNICODETEXT);
			if (hData != NULL)
			{
				LPCTSTR pData = (LPCTSTR)::GlobalLock(hData);
				int nStrLen = (int)_tcslen(pData);
				szRet = new TCHAR[nStrLen + 1];
				memcpy(szRet, pData, (nStrLen + 1)*sizeof(TCHAR));
				::GlobalUnlock(hData);
			}
			::CloseClipboard();
		}
	}
	return szRet;
}

void CBaseTypeset::FontZoom(int nIndex, int nCount, float fZoomRate)
{
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	if (nIndex < 0)
	{
		nIndex = 0;
		nCount = nStrLen;
	}
	if (nCount <= 0)
	{
		return;
	}
	int nEndPos = nIndex + nCount;
	int nEndIdx = nIndex + nCount - 1;

	LPCommonTextProp pProp = &m_pTextObj->stuTextProp;
	BaseARray* paryIndex = &pProp->arIndex;
	for (int nC = nIndex; nC <= nEndIdx; ++nC)
	{
		LPCommonTextPropIndex pCurIndex = BaseGraphTextProp_GetPropIndex(paryIndex, nC);
		if (pCurIndex != NULL)
		{
			LPCommonFont pFont = (LPCommonFont)GetFont(nC);
			pFont->lfWidth = pFont->lfWidth * fZoomRate;
			pFont->lfHeight = pFont->lfHeight * fZoomRate;
			if (pFont->lfWidth < 5)
				pFont->lfWidth = 5;
			if (pFont->lfHeight < 5)
				pFont->lfHeight = 5;
			SetFont(nC, 1, pFont, BaseGraphTEXTFONTMASK_WIDTH | BaseGraphTEXTFONTMASK_HEIGHT);
		}
	}
}

BOOL CBaseTypeset::AdjustTypesetRealRect(rectf& rtReal)
{
	//不合理的（难以操作的）矩形框
	BOOL bAdjustW = rtReal.left >= rtReal.right;
	BOOL bAdjustH = rtReal.top >= rtReal.bottom;

	if (bAdjustW || bAdjustH)
	{
		BOOL bReleaseFont = FALSE;
		BOOL bReleasePara = FALSE;
		LPCommonFont pFont = GetFont(0);
		LPCommonTextParagraph pParagraph = GetParagraph(-1);
		if (pFont == NULL || pParagraph == NULL)
		{
			//从属性页取字属性
			if (g_TypesetInterface.GetPropFromPropPage != NULL)
			{
				if (pFont == NULL)
				{
					BaseGraphFont objFont;
					BaseGraphFont_Init(&objFont);
					if (g_TypesetInterface.GetPropFromPropPage(&objFont, NULL, NULL))
					{
						bReleaseFont = TRUE;
						pFont = &objFont;
					}
					else
					{
						BaseGraphFont_Release(&objFont);
					}
				}

				if (pParagraph == NULL)
				{
					BaseGraphTextParagraph objParagraph;
					BaseGraphText_InitParagraph(&objParagraph);
					if (g_TypesetInterface.GetPropFromPropPage(NULL, &objParagraph, NULL))
					{
						bReleasePara = TRUE;
						pParagraph = &objParagraph;
					}
					else
					{
						BaseGraphText_ReleaseParagraph(&objParagraph);
					}
				}
			}
		}

		if (pFont != NULL && pParagraph != NULL)
		{
			BaseGraphWordInfo* pDefWordInf = new BaseGraphWordInfo;
			BaseGraphWordInfoInit(pDefWordInf);
			pDefWordInf->wChar = _T('\40');
			pDefWordInf->bGlyphIndex = false;
			pDefWordInf->fDistance = pFont->lfDistance;
			BaseGraphWordInfoFill(NULL, pDefWordInf, NULL, pFont, m_pTextObj->bHorizontal);

			if (m_pTextObj->bHorizontal)
			{
				if (bAdjustW)
				{
					switch (pParagraph->eAlignHorizontal)
					{
					case TEXT_ALIGN_H_LEFT:
					case TEXT_ALIGN_H_ENDS:
						{
							rtReal.left = m_pTextObj->rtBorder.left;
							rtReal.right = rtReal.left + (pDefWordInf->rtCell.right - pDefWordInf->rtCell.left);
							break;
						}
					case TEXT_ALIGN_H_RIGHT:
						{
							rtReal.right = m_pTextObj->rtBorder.right;
							rtReal.left = rtReal.right - (pDefWordInf->rtCell.right - pDefWordInf->rtCell.left);
							break;
						}
					case TEXT_ALIGN_H_CENTER:
						{
							float fHalf = (pDefWordInf->rtCell.right - pDefWordInf->rtCell.left) / 2;
							rtReal.left = -fHalf;
							rtReal.right = fHalf;
							break;
						}
					}
				}

				if (bAdjustH)
				{
					switch (m_pTextObj->eAlignVertical)
					{
					case TEXT_ALIGN_V_TOP:
						{
							rtReal.top = m_pTextObj->rtBorder.top;
							rtReal.bottom = rtReal.top + (pDefWordInf->rtCell.bottom - pDefWordInf->rtCell.top);
							break;
						}
					case TEXT_ALIGN_V_CENTER:
						{
							float fHalf = (pDefWordInf->rtCell.bottom - pDefWordInf->rtCell.top) / 2;
							rtReal.top = -fHalf;
							rtReal.bottom = fHalf;
							break;
						}
					case TEXT_ALIGN_V_BOTTOM:
						{
							rtReal.bottom = m_pTextObj->rtBorder.bottom;
							rtReal.top = rtReal.bottom - (pDefWordInf->rtCell.bottom - pDefWordInf->rtCell.top);
							break;
						}
					}
				}
			}
			else
			{
				if (bAdjustW)
				{
					switch (m_pTextObj->eAlignVertical)
					{
					case TEXT_ALIGN_V_TOP:
						{
							rtReal.left = m_pTextObj->rtBorder.left;
							rtReal.right = rtReal.left + (pDefWordInf->rtCell.right - pDefWordInf->rtCell.left);
							break;
						}
					case TEXT_ALIGN_V_CENTER:
						{
							float fHalf = (pDefWordInf->rtCell.right - pDefWordInf->rtCell.left) / 2;
							rtReal.left = -fHalf;
							rtReal.right = fHalf;
							break;
						}
					case TEXT_ALIGN_V_BOTTOM:
						{
							rtReal.right = m_pTextObj->rtBorder.left;
							rtReal.left = rtReal.right - (pDefWordInf->rtCell.right - pDefWordInf->rtCell.left);
							break;
						}
					}
				}

				if (bAdjustH)
				{
					switch (pParagraph->eAlignHorizontal)
					{
					case TEXT_ALIGN_H_LEFT:
					case TEXT_ALIGN_H_ENDS:
						{
							rtReal.top = m_pTextObj->rtBorder.top;
							rtReal.bottom = rtReal.top + (pDefWordInf->rtCell.bottom - pDefWordInf->rtCell.top);
							break;
						}
					case TEXT_ALIGN_H_RIGHT:
						{
							rtReal.bottom = m_pTextObj->rtBorder.bottom;
							rtReal.top = rtReal.bottom - (pDefWordInf->rtCell.bottom - pDefWordInf->rtCell.top);
							break;
						}
					case TEXT_ALIGN_H_CENTER:
						{
							float fHalf = (pDefWordInf->rtCell.bottom - pDefWordInf->rtCell.top) / 2;
							rtReal.top = -fHalf;
							rtReal.bottom = fHalf;
							break;
						}
					}
				}
			}

			BaseGraphWordInfoRelease(pDefWordInf);
			delete pDefWordInf;
		}
		else
		{
			rtReal.left = -BaseGraphTYPESET_DIMENSION_MIN/2;
			rtReal.right = BaseGraphTYPESET_DIMENSION_MIN/2;
			rtReal.top = -BaseGraphTYPESET_DIMENSION_MIN/2;
			rtReal.bottom = BaseGraphTYPESET_DIMENSION_MIN/2;
		}

		if (bReleaseFont)
			BaseGraphFont_Release(pFont);
		if (bReleasePara)
			BaseGraphText_ReleaseParagraph(pParagraph);

		return TRUE;
	}

	return FALSE;
}

void CBaseTypeset::FixedFrameCal()
{
	if (m_pTextObj->bFixedFrame)
	{
		//缩放系数
		float fScaleWidth = 1.0;
		float fScaleHeight = 1.0;

		//固定字框计算
		float fMaxWidth = 0;
		float fMaxHeight = 0;

		BaseARray* paryParagraph = &m_pTypesetPrivate->m_arParagraph;
		BaseARray* paryParaObj = &m_pTextObj->arParagraph;
		CBaseTextParagraph* pParagraph;
		int nParaCnt = BaseAR_GetCount(paryParagraph);

		if (m_pTextObj->bHorizontal)
		{
			for (int i = 0; i < nParaCnt; ++i)
			{
				pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
				float fRealWidth = pParagraph->GetRealWidth();
				float fRealHeight = pParagraph->GetRealHeight();

				fMaxHeight += fRealHeight;
				if (fRealWidth > fMaxWidth)
					fMaxWidth = fRealWidth;
			}
		}
		else
		{
			for (int i = 0; i < nParaCnt; ++i)
			{
				pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
				float fRealWidth = pParagraph->GetRealWidth();
				float fRealHeight = pParagraph->GetRealHeight();

				fMaxWidth += fRealWidth;
				if (fRealHeight > fMaxHeight)
					fMaxHeight = fRealHeight;
			}
		}

		float fBorderW = BaseRectf_Width(&m_pTypesetPrivate->rtTypesetBorder);
		float fBorderH = BaseRectf_Height(&m_pTypesetPrivate->rtTypesetBorder);

		//宽度超出了
		if (fMaxWidth > fBorderW && fMaxWidth > ABS_FLOAT_0)
		{
			fScaleWidth = fBorderW / fMaxWidth;
		}
		else
		{
			fScaleWidth = 1.0;
		}

		if (fMaxHeight > fBorderH && fMaxHeight > ABS_FLOAT_0)
		{
			fScaleHeight = fBorderH / fMaxHeight;
		}
		else
		{
			fScaleHeight = 1.0;
		}

		if (fScaleWidth < 1.0 || fScaleHeight < 1.0)
		{
			for (int i = 0; i < nParaCnt; ++i)
			{
				pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
				pParagraph->ZoomUnitAndDistance(fScaleWidth, fScaleHeight);
			}
		}
	}
}

void CBaseTypeset::OffsetCal(vector3f* pOffset)
{
	if (m_pTypesetPrivate == NULL)
		return;

	int nParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	for (int i = 0; i < nParaCnt; ++i)
	{
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
		pParagraph->OffsetCal(pOffset);
	}
}

void CBaseTypeset::GenerateWordInfo(LPCommonWordInfo pWordInfo, TCHAR ch, int nFontIndex)
{
	//用指定索引位置的字取ch的字模信息。
	//pWordInfo由主调者申请维护
	BaseGraphWordInfoInit(pWordInfo);
	pWordInfo->wChar = ch;
	LPCommonFont pFont = GetFont(nFontIndex);
	if (pFont != NULL)
	{
		pWordInfo->bVisible = (bool)BaseGraphIsVisibleChar(pWordInfo->wChar);
		pWordInfo->bGlyphIndex = false;
		pWordInfo->fDistance = pFont->lfDistance;

		BaseGraphWordInfoFill(NULL, pWordInfo, NULL, pFont, m_pTextObj->bHorizontal);
		if (pWordInfo->wChar == VK_TAB)
		{
			// Tab只占空位置，没有字形
			common_release_vectorgraph(&pWordInfo->vgGraph);
		}
	}
}

LPCommonWordInfo CBaseTypeset::CreateDefaultWordInfo(TCHAR ch)
{
	BaseGraphWordInfo* pDefWordInf = NULL;

	BOOL bReleaseFont = FALSE;
	LPCommonFont pFont = GetFont(0);
	if (pFont == NULL)
	{
		//从属性页取字属性
		if (g_TypesetInterface.GetPropFromPropPage != NULL)
		{
			BaseGraphFont objFont;
			BaseGraphFont_Init(&objFont);
			if (g_TypesetInterface.GetPropFromPropPage(&objFont, NULL, NULL))
			{
				pFont = &objFont;
				bReleaseFont = TRUE;
			}
			else
			{
				BaseGraphFont_Release(&objFont);
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}

	pDefWordInf = new BaseGraphWordInfo;
	BaseGraphWordInfoInit(pDefWordInf);
	pDefWordInf->wChar = ch;
	pDefWordInf->bGlyphIndex = false;
	pDefWordInf->fDistance = pFont->lfDistance;
	BaseGraphWordInfoFill(NULL, pDefWordInf, NULL, pFont, m_pTextObj->bHorizontal);
	if (pDefWordInf->wChar == VK_TAB)
	{
		// Tab只占空位置，没有字形
		common_release_vectorgraph(&pDefWordInf->vgGraph);
	}

	if (bReleaseFont)
		BaseGraphFont_Release(pFont);

	return pDefWordInf;
}

void CBaseTypeset::SetParagraphChgLevel(int nLevel, int nParaIdx, BOOL bReset)
{
	CBaseTextParagraph* pDynParag;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);
	if (nParaIdx >= 0)
	{
		if (nParaIdx < nDynParaCnt)
		{
			pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nParaIdx);
			if (bReset || nLevel > pDynParag->m_nChgLevel)
				pDynParag->m_nChgLevel = nLevel;
		}
	}
	else
	{
		for (int i = 0; i < nDynParaCnt; ++i)
		{
			pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
			if (bReset || nLevel > pDynParag->m_nChgLevel)
				pDynParag->m_nChgLevel = nLevel;
		}
	}
}

void CBaseTypeset::ArtWordForm()
{
	LPBaseARtTypeset pArt = m_pTextObj->pArtWord;
	BaseARray* parHandlePt[ARTPATHTYPECNT] = {&pArt->arHandlePointUp,
										&pArt->arHandlePointDown};

	float fOldLineW, fLineW;
	float fOldLineH, fLineH;
	point2f	ptOldLineCent, ptLineCent;
	fOldLineW = fOldLineH = -1;
	ptOldLineCent.x = ptOldLineCent.y = 0;

	//准备控制点数据
	LPCommonMaskHandle pNewHandle, pDesHandle, pSrcHandle;
	BaseARray arDesHandlePt[ARTPATHTYPECNT];
	for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
	{
		int nCtrlCnt = BaseAR_GetCount(parHandlePt[ptIndex]);
		if (nCtrlCnt < 1)
			continue;
		BaseAR_Init(&arDesHandlePt[ptIndex]);
		for (int nCtrl=0; nCtrl<nCtrlCnt; ++nCtrl)
		{
			LPCommonMaskHandle pSrcHandle = (LPCommonMaskHandle)BaseAR_GetAt(parHandlePt[ptIndex], nCtrl);
			pNewHandle = new CBaseMaskHandle;
			pNewHandle->m_bContinue = pSrcHandle->m_bContinue;
			BaseAR_Add(&arDesHandlePt[ptIndex], pNewHandle);
		}
	}

	//Pos Rip
	BaseGraphUnit* pUnit;
	int nAllWordInfoCnt = 0;
	BaseARray* paryUnit = &m_pTextObj->ar2DUnit;
	int nUnitCount = BaseAR_GetCount(paryUnit);

	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);

	eBaseARtTypesetPath nType = pArt->ePath;
	rectf rtBorder;
	GetTypesetRect(rtBorder, FALSE);

	point3f pt3fTemp;
	LPCommonGlyphSpace pGlyphSpace;
	CBasePolgonRip ripTool;

	for (int nP=0; nP<nDynParaCnt; ++nP)
	{
		LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nP);
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nP);

		BaseARray* paryWordInfo = &pDynParag->m_arWordInfo;
		int nParagWordInfoCnt = BaseAR_GetCount(paryWordInfo);

		//每行作为一个控制单元
		BaseARray* paryLine = pDynParag->GetLineArray();
		int nLineCnt = BaseAR_GetCount(paryLine);
		for (int nL=0; nL<nLineCnt; ++nL)
		{
			CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
			int nWordInfoCnt = pLine->GetWordInfoCount();
			int nWordInfoBegin = pLine->GetWordInfoBeginIndex();

			//用行参数更新控制点
			BaseGraphSizef* pSzLineReal = pLine->GetRealSizePtr();
			// 艺术字单行，否则宽度为上一次宽度，需要再计算一次
			fLineW = pSzLineReal->cx;
			fLineH = pSzLineReal->cy;
			ptLineCent.x = fLineW/2;
			ptLineCent.y = 0;
			float* ptLenRealProp[ARTPATHTYPECNT] = {0};
			if (fOldLineW != fLineW || fOldLineH != fLineH 
				|| ptOldLineCent.x != ptLineCent.x || ptOldLineCent.y != ptLineCent.y)
			{
				for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
				{
					int nCtrlCnt = BaseAR_GetCount(parHandlePt[ptIndex]);
					for (int nCtrl=0; nCtrl<nCtrlCnt; ++nCtrl)
					{
						pDesHandle = (LPCommonMaskHandle)BaseAR_GetAt(&arDesHandlePt[ptIndex], nCtrl);
						pSrcHandle = (LPCommonMaskHandle)BaseAR_GetAt(parHandlePt[ptIndex], nCtrl);

						pDesHandle->m_Pt.x = (ptLineCent.x + pSrcHandle->m_Pt.x * fLineW);
						pDesHandle->m_Pt.y = (ptLineCent.y + pSrcHandle->m_Pt.y * fLineH);
						pDesHandle->m_HelpPt[0].x = (ptLineCent.x + pSrcHandle->m_HelpPt[0].x * fLineW);
						pDesHandle->m_HelpPt[0].y = (ptLineCent.y + pSrcHandle->m_HelpPt[0].y * fLineH);
						pDesHandle->m_HelpPt[1].x = (ptLineCent.x + pSrcHandle->m_HelpPt[1].x * fLineW);
						pDesHandle->m_HelpPt[1].y = (ptLineCent.y + pSrcHandle->m_HelpPt[1].y * fLineH);
					}
					pSrcHandle = (LPCommonMaskHandle)BaseAR_GetAt(parHandlePt[ptIndex], 0);
					float posXStart = pSrcHandle->m_Pt.x;
					pSrcHandle = (LPCommonMaskHandle)BaseAR_GetAt(parHandlePt[ptIndex], nCtrlCnt-1);
					float posXEnd = pSrcHandle->m_Pt.x;
					float ptLenReal = posXEnd - posXStart;
					ptLenRealProp[ptIndex] = new float[nCtrlCnt];
					for (int nCtrl=0; nCtrl<nCtrlCnt; ++nCtrl)
					{
						LPCommonMaskHandle pSrcHandle = (LPCommonMaskHandle)BaseAR_GetAt(parHandlePt[ptIndex], nCtrl);
						ptLenRealProp[ptIndex][nCtrl] = (pSrcHandle->m_Pt.x - posXStart) / ptLenReal;
					}

					fOldLineW = fLineW;
					fOldLineH = fLineH;
					ptOldLineCent = ptLineCent;
				}
			}

			//图形变形
			float fLastPos = 0;
			for (int nW=0; nW<nWordInfoCnt; ++nW)
			{
				int nWordInfoIdx = nWordInfoBegin + nW;
				LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, nWordInfoIdx);
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&pLine->m_arGlyphSpace, nW);

				int nUnitIdx = nWordInfoIdx + nAllWordInfoCnt;
				pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnit, nUnitIdx);
				float fUnitWidth = pUnit->u.u2d.graph.bound.right - pUnit->u.u2d.graph.bound.left;
				float fWordDistance = pWordInfo->fDistance;
				float fWordProp = fUnitWidth / (fUnitWidth + fWordDistance);	// 考虑字间距

				//////////////////////////////////////////////////////////////////////////
				// Begin of a Glyph
				int nIndex = nW;
				int m_nRipCnt = nWordInfoCnt;
				vectorgraph *pRipObjct = &pUnit->u.u2d.graph;

				// 按照每个实际字宽计算划分比例，防止小字变宽
				float ft0 = fLastPos / fLineW;
				float fCurPos = pGlyphSpace->ptBound[1].x - pGlyphSpace->ptBound[0].x + fLastPos;
				float ft1 = fCurPos / fLineW;;
				int ptPerIndex[ARTPATHTYPECNT] = {0};
				for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
				{
					int nCtrlCnt = BaseAR_GetCount(parHandlePt[ptIndex]);
					for (int nCtrl=nCtrlCnt; nCtrl>0; --nCtrl)
					{
						if (ft0 >= ptLenRealProp[ptIndex][nCtrl-1]/* && ft1 <= ptLenRealProp[ptIndex][nCtrl]*/)
						{
							//如果一条曲线有三个以上点，按X轴距离等分,文字跨越点时候可能显示效果不好
							ptPerIndex[ptIndex] = nCtrl - 1;
							break;
						}
					}
				}

				// 暂时不支持一个文字从两条曲线上取点计算，文字少时候可能显示不理想
				point2f fPos0[ARTPATHTYPECNT] = {0}, fPos1[ARTPATHTYPECNT] = {0};
				for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
				{
					float fIndexStart = ptLenRealProp[ptIndex][ptPerIndex[ptIndex]] * fLineW;
					float fIndexEnd = ptLenRealProp[ptIndex][ptPerIndex[ptIndex]+1] * fLineW;
					float fIndexLen = fIndexEnd - fIndexStart;
					ft0 = (fLastPos - fIndexStart)/(float)(fIndexLen);
					ft1 = (fCurPos - fIndexStart)/(float)(fIndexLen);
					ft1 = ft0 + (ft1 - ft0) * fWordProp;
					// 防止点距离小于文字计算出错
					ft0 = min(ft0, 1.0);
					ft1 = min(ft1, 1.0);
					point2f CtrlPt[4];
					CommonBezierGetPtArray(arDesHandlePt[ptIndex], ptPerIndex[ptIndex], CtrlPt);
					fPos0[ptIndex] = CommonBezierGetValue(CtrlPt, ft0);
					fPos1[ptIndex] = CommonBezierGetValue(CtrlPt, ft1);
				}
				fLastPos = fCurPos;

				point2f pt[4];
				pt[0].x = fPos0[0].x;
				pt[0].y = fPos0[0].y;
				pt[1].x = fPos0[1].x;
				pt[1].y = fPos0[1].y;
				pt[2].x = fPos1[1].x;
				pt[2].y = fPos1[1].y;
				pt[3].x = fPos1[0].x;
				pt[3].y = fPos1[0].y;

				//图形变形
				CFourPtForm objForm;
				// 防止文字实际高度宽度变化过大，按照字模大小变形
				pRipObjct->bound = pWordInfo->rtCell;
				objForm.Create(pRipObjct->bound, pt);
				ripTool.Form(objForm, &pUnit->u.u2d.graph);

				//图形位置变形
				pt3fTemp = pGlyphSpace->ptBound[3];
				pt3fTemp.x = -pt3fTemp.x;
				pt3fTemp.y = -pt3fTemp.y;
				BaseGraphOffsetPt3fs(pGlyphSpace->ptBound, pt3fTemp, BaseGraphGLYPHSPACEPTs_EX);
				ripTool.Form(objForm, pGlyphSpace->ptBound, BaseGraphGLYPHSPACEPTs_EX);
				//pt3fTemp.x = -pt3fTemp.x;
				pt3fTemp.x = 0;
				pt3fTemp.y = -pt3fTemp.y;
				BaseGraphOffsetPt3fs(pGlyphSpace->ptBound, pt3fTemp, BaseGraphGLYPHSPACEPTs_EX);
			}
			for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
			{
				SAFEDELETE_ARRAY(ptLenRealProp[ptIndex]);
			}

			//末尾
			int nGlyphSpaceCnt = BaseAR_GetCount(&pLine->m_arGlyphSpace);
			if (nGlyphSpaceCnt > 1)
			{
				LPCommonGlyphSpace pLastSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&pLine->m_arGlyphSpace, nGlyphSpaceCnt - 2);
				LPCommonGlyphSpace pEndSpace  = (LPCommonGlyphSpace)BaseAR_GetAt(&pLine->m_arGlyphSpace, nGlyphSpaceCnt - 1);
				memcpy(pEndSpace, pLastSpace, sizeof(BaseGraphGlyphSpace));
				if (m_pTextObj->bHorizontal)
				{
					pEndSpace->ptBound[0] = pEndSpace->ptBound[4];
					pEndSpace->ptBound[1] = pEndSpace->ptBound[4];
					pEndSpace->ptBound[2] = pEndSpace->ptBound[5];
					pEndSpace->ptBound[3] = pEndSpace->ptBound[5];
					pEndSpace->ptPos.x = pEndSpace->ptBound[5].x;
				}
			}
		}

		nAllWordInfoCnt += nParagWordInfoCnt;
	}

	for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
	{
		int nPointCnt = BaseAR_GetCount(&arDesHandlePt[ptIndex]);
		for (int i = 0; i < nPointCnt; ++i)
		{
			LPCommonMaskHandle pHP = (LPCommonMaskHandle)BaseAR_GetAt(&arDesHandlePt[ptIndex], i);
			SAFE_DELETE(pHP);
		}
		BaseAR_RemoveAll(&arDesHandlePt[ptIndex]);
	}
}

void CBaseTypeset::PathTypesetForm()
{
	LPBaseARtTypeset pPath = m_pTextObj->pTypesetPath;
	BaseARray* parHandlePt = &pPath->arHandlePointDown;
	int nCtrlCnt = BaseAR_GetCount(parHandlePt);
	if (nCtrlCnt < 1)
		return;

	float fOldLineW, fLineW;
	float fOldLineH, fLineH;

	assert(false);
#if 0
	CBaseBeCurve* pTitleCurve = (CBaseBeCurve*)pPath->pTitleCurve;
	if (pTitleCurve == NULL)
	{
		return;
	}
	int nCurveLen = pTitleCurve->GetCurveLength();

	//Pos Rip
	BaseGraphUnit* pUnit;
	int nAllWordInfoCnt = 0;
	BaseARray* paryUnit = &m_pTextObj->ar2DUnit;
	int nUnitCount = BaseAR_GetCount(paryUnit);

	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);

	point3f pt3fTemp;
	pt3fTemp.z = 0;
	LPCommonGlyphSpace pGlyphSpace;
	CBasePolgonRip ripTool;
	point2f ptOffset = {0};

	for (int nP=0; nP<nDynParaCnt; ++nP)
	{
		LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nP);
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nP);

		pDynParag->SetOffset(ptOffset);

		BaseARray* paryWordInfo = &pDynParag->m_arWordInfo;
		int nParagWordInfoCnt = BaseAR_GetCount(paryWordInfo);

		//每行作为一个控制单元
		BaseARray* paryLine = pDynParag->GetLineArray();
		int nLineCnt = BaseAR_GetCount(paryLine);
		for (int nL=0; nL<nLineCnt; ++nL)
		{
			CBaseTextLine* pLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
			pLine->SetLineOffset(ptOffset);

			int nWordInfoCnt = pLine->GetWordInfoCount();
			int nWordInfoBegin = pLine->GetWordInfoBeginIndex();

			//用行参数更新控制点
			BaseGraphSizef* pSzLineReal = pLine->GetRealSizePtr();
			// 曲线文字单行，否则宽度为上一次宽度，需要再计算一次
			fLineW = pSzLineReal->cx;
			fLineH = pSzLineReal->cy;

			// 计算出首个文字位置
			float fLastPos = 0;
			if (m_pTextObj->bCurveFlow)
			{
				// 流动
				fLastPos = nCurveLen * m_pTextObj->dCurTime;
			}
			//图形变形
			if (nWordInfoCnt == 0)
			{
				// 特殊处理下Space形状
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&pLine->m_arGlyphSpace, 0);
				pGlyphSpace->ptBound[1].x = pGlyphSpace->ptBound[0].x + 5;
				pGlyphSpace->ptBound[2].x = pGlyphSpace->ptBound[3].x + 5;

				if (fLastPos < 0 || fLastPos >= nCurveLen)
				{
					// 超过曲线部分隐藏不处理
					//BaseAR_RemoveAt(paryUnit, nIndex, m_nRipCnt-nIndex);
					pGlyphSpace->ptPos.x = 0;
					pGlyphSpace->ptPos.y = 0;
					for (int i=0; i<BaseGraphGLYPHSPACEPTs_EX; i++)
					{
						pGlyphSpace->ptBound[i].x = 0;
						pGlyphSpace->ptBound[i].y = 0;
					}
				}

				double fAngle = 0;
				point2f ptPos = {0};
				pTitleCurve->GetBaseCurve()->GetPosAndAngleEx(fLastPos, ptPos, fAngle);
				fAngle += PAI/2.0;

				CRotateZForm objForm;
				objForm.CreateUseRadian(fAngle);

				pGlyphSpace->ptPos.x = ptPos.x;
				pGlyphSpace->ptPos.y = -ptPos.y;
				pt3fTemp.x = ptPos.x;
				pt3fTemp.y = ptPos.y;

				// 按曲线法线角度旋转后放入曲线位置
				if (pPath->bRotate)
				{
					ripTool.Form(objForm, pGlyphSpace->ptBound, BaseGraphGLYPHSPACEPTs_EX);
				}
				pt3fTemp = pGlyphSpace->ptBound[0];
				pt3fTemp.x = -pt3fTemp.x;
				pt3fTemp.y = -pt3fTemp.y;
				BaseGraphOffsetPt3fs(pGlyphSpace->ptBound, pt3fTemp, BaseGraphGLYPHSPACEPTs_EX);
				float fOffsetX = fLineH*sin(fAngle);
				float fOffsetY = fLineH*cos(fAngle);
				pt3fTemp.x = ptPos.x + fOffsetX;
				pt3fTemp.y = ptPos.y - fOffsetY;
				BaseGraphOffsetPt3fs(pGlyphSpace->ptBound, pt3fTemp, BaseGraphGLYPHSPACEPTs_EX);
			}

			for (int nW=0; nW<nWordInfoCnt; ++nW)
			{
				int nWordInfoIdx = nWordInfoBegin + nW;
				LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, nWordInfoIdx);
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&pLine->m_arGlyphSpace, nW);

				int nUnitIdx = nWordInfoIdx + nAllWordInfoCnt;
				pUnit = (BaseGraphUnit*)BaseAR_GetAt(paryUnit, nUnitIdx);

				//////////////////////////////////////////////////////////////////////////
				// Begin of a Glyph
				int nIndex = nW;
				int m_nRipCnt = nWordInfoCnt;
				vectorgraph *pRipObjct = &pUnit->u.u2d.graph;

				float fCurPos = fLastPos;
				if (fCurPos < 0 || fCurPos >= nCurveLen)
				{
					// 超过曲线部分隐藏不处理
					graph_release(&(pUnit->u.u2d.graph));
					//BaseAR_RemoveAt(paryUnit, nIndex, m_nRipCnt-nIndex);
					pGlyphSpace->ptPos.x = 0;
					pGlyphSpace->ptPos.y = 0;
					for (int i=0; i<BaseGraphGLYPHSPACEPTs_EX; i++)
					{
						pGlyphSpace->ptBound[i].x = 0;
						pGlyphSpace->ptBound[i].y = 0;
					}
				}
				fLastPos = fCurPos + (pGlyphSpace->ptBound[1].x - pGlyphSpace->ptBound[0].x) + pWordInfo->fDistance;

				double fAngle = 0;
				point2f ptPos = {0};
				pTitleCurve->GetBaseCurve()->GetPosAndAngleEx(fCurPos, ptPos, fAngle);
				fAngle += PAI/2.0;
				
				CRotateZForm objForm;
				objForm.CreateUseRadian(fAngle);

				pGlyphSpace->ptPos.x = ptPos.x;
				pGlyphSpace->ptPos.y = -ptPos.y;
				pt3fTemp.x = ptPos.x;
				pt3fTemp.y = ptPos.y;

				// 按曲线法线角度旋转后放入曲线位置
				if (pPath->bRotate)
				{
					ripTool.Form(objForm, &pUnit->u.u2d.graph);
					ripTool.Form(objForm, pGlyphSpace->ptBound, BaseGraphGLYPHSPACEPTs_EX);
				}
				pt3fTemp = pGlyphSpace->ptBound[0];
				pt3fTemp.x = -pt3fTemp.x;
				pt3fTemp.y = -pt3fTemp.y;
				BaseGraphOffsetPt3fs(pGlyphSpace->ptBound, pt3fTemp, BaseGraphGLYPHSPACEPTs_EX);
				float fOffsetX = fLineH*sin(fAngle);
				float fOffsetY = fLineH*cos(fAngle);
				pt3fTemp.x = ptPos.x + fOffsetX;
				pt3fTemp.y = ptPos.y - fOffsetY;
				BaseGraphOffsetPt3fs(pGlyphSpace->ptBound, pt3fTemp, BaseGraphGLYPHSPACEPTs_EX);
				// End of a Glyph
				//////////////////////////////////////////////////////////////////////////
			}
		}

		nAllWordInfoCnt += nParagWordInfoCnt;
	}
#endif
}

void CBaseTypeset::CalTypesetBorder()
{
	GetTypesetRect(m_pTypesetPrivate->rtTypesetBorder, TRUE);
}

void CBaseTypeset::SplitParagraph()
{
	//增量式调整数组大小
	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);
	int nParaCnt = BaseAR_GetCount(paryPara);
	//BOOL bLToR = (m_pTextObj->bHorizontal && !m_pTextObj->bLeftInput) ? FALSE : TRUE;
	BOOL bLToR = TRUE;

	CBaseUsp uspProcessor(NULL);
	CBaseTextParagraph* pDynParag;
	for (int i = 0; i < nParaCnt; ++i)
	{
		if (i < nDynParaCnt)
		{
			pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
			pDynParag->SetParagraphInfo(m_pTextObj, i);
		}
		else
		{
			pDynParag = new CBaseTextParagraph(m_pTextObj, i);
			BaseAR_Add(paryDynPara, pDynParag);
		}

		//分段多语言分析
		if (pDynParag->m_nChgLevel >= CBaseTypeset::TTLWordDel)
		{
			LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, i);
			int nParaCharCnt = GetParagraphCharCount(i);
			BaseGraphSCRIPTRESULT* pUspResult = &pDynParag->m_uspResult;
			uspProcessor.Attach(&pDynParag->m_uspResult);
			uspProcessor.Analyse(
				m_pTextObj->szText, 
				pParagObj->nBeginIndex, 
				nParaCharCnt, 
				&m_pTextObj->stuTextProp, 
				bLToR);

			//更新该段USP字模数组
			pDynParag->UpdateWordInfoArray(m_pTextObj);
		}
	}

	//删除冗余段
	int nDelCnt = nDynParaCnt - nParaCnt;
	if (nDelCnt > 0)
	{
		for (int i=nDynParaCnt-1; i>=nParaCnt; --i)
		{
			pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
			SAFE_DELETE(pDynParag) ;
		}
		BaseAR_RemoveAt(paryDynPara, nParaCnt, nDelCnt);
	}
}

void CBaseTypeset::GenerateWordInfo()
{
	LPCommonWordInfo pWordInfo = NULL;
	LPCommonFont pFont = NULL;

	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);
	int nAllWordInfoCnt = 0;

	HDC hWndDC = NULL;
	HWND hWnd = NULL;
	int chgIndex = 0;
	int chgCnt = m_chgCnt;
	int chgBegin = 0;
	BOOL bFind = FALSE;
	BOOL bInsertOver = FALSE;
	for (int i = 0; i < nDynParaCnt; ++i)
	{
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
		//该段之前所有WordInfo元素个数
		pDynParag->m_nPreAllWordInfoCnt = nAllWordInfoCnt;
		int nWordInfoCnt = BaseAR_GetCount(&pDynParag->m_arWordInfo);

		if (pDynParag->m_nChgLevel >= CBaseTypeset::TTLScaleFont)
		{
			//DC
			if (hWndDC == NULL)
			{
				hWnd = ::GetDesktopWindow();
				hWndDC = ::GetDC(hWnd);
				::SetGraphicsMode(hWndDC, GM_ADVANCED);
				::SetMapMode(hWndDC, MM_TEXT);
			}
		
			int perCnt = 0;
			if (!IsLeftInput())
			{
				bFind = TRUE;
			}
			if (bFind)
			{
				chgBegin = 0;
				perCnt = nWordInfoCnt;
			}
			// 更改位置到段末位需要更新，否则显示容易重叠
			if (m_chgIndex >= chgIndex && m_chgIndex < chgIndex + nWordInfoCnt)
			{
				bFind = TRUE;
				chgBegin = m_chgIndex-chgIndex;
				perCnt = nWordInfoCnt - chgBegin;
			}

			if (bFind)
			{
				if (bInsertOver)
				{
					bFind = FALSE;
				}
				LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, i);
				for (int nL=chgBegin; nL<chgBegin+perCnt; nL++)
				{
					pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&pDynParag->m_arWordInfo, nL);
					pFont = GetFont(pParaObj->nBeginIndex + pWordInfo->nCharIndex);
					pWordInfo->fDistance = pFont->lfDistance;
					pWordInfo->bGlyphIndex = false;		// 不使用字符索引，否则更新字体可能出现乱码
					BaseGraphWordInfoRelease(pWordInfo);
					BaseGraphWordInfoFill(hWndDC, pWordInfo, NULL, pFont, m_pTextObj->bHorizontal);
					if (pWordInfo->wChar == VK_TAB)
					{
						// Tab只占空位置，没有字形
						common_release_vectorgraph(&pWordInfo->vgGraph);
					}
				}

				chgCnt -= perCnt;
				if (chgCnt <= 0)
				{
					bFind = FALSE;
					if (m_bInsertReturn)
					{
						// 插入回车后，下一段信息需要重建
						bFind = TRUE;
						bInsertOver = TRUE;
					}
				}
			}
			chgIndex += nWordInfoCnt;
			nAllWordInfoCnt += nWordInfoCnt;
			continue;
		}
		else if (pDynParag->m_nChgLevel == CBaseTypeset::TTLDistance)
		{
			LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, i);
			for (int nL = 0; nL<nWordInfoCnt; ++nL)
			{
				pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(&pDynParag->m_arWordInfo, nL);
				pFont = GetFont(pParaObj->nBeginIndex + pWordInfo->nCharIndex);
				pWordInfo->fDistance = pFont->lfDistance;
			}
		}

		nAllWordInfoCnt += nWordInfoCnt;
		m_bInsertReturn = FALSE;
	}

	if (hWndDC != NULL)
	{
		::ReleaseDC(hWnd, hWndDC);
	}
}

void CBaseTypeset::GenerateParagraphInfo()
{
	BOOL bHorizontal = m_pTextObj->bHorizontal;
	BOOL bNonLinear = !IsLinearTypeset();
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);
	for (int i = 0; i < nDynParaCnt; i++)
	{
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
		if (pDynParag->m_nChgLevel >= CBaseTypeset::TTLArrange || bNonLinear)
		{
			pDynParag->Typeset(bHorizontal);
		}
	}
}

void CBaseTypeset::ReleaseParagraph()
{
	int nParaCount = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	for (int i = 0; i < nParaCount; i++)
	{
		CBaseTextParagraph* pTextParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
		SAFE_DELETE(pTextParagraph);
	}
	BaseAR_RemoveAll(&m_pTypesetPrivate->m_arParagraph);
}

void CBaseTypeset::SetParagraphPos()
{
	BaseARray* paryParagraph = &m_pTypesetPrivate->m_arParagraph;
	BaseARray* paryParaObj = &m_pTextObj->arParagraph;
	int nParaCount = BaseAR_GetCount(paryParagraph);

	float fBorderWidth;
	float fBorderHeight;
	point2f ptOffset;
	rectf* prtTypesetBorder = &m_pTypesetPrivate->rtTypesetBorder;

	if (m_pTextObj->bHorizontal)		//横排
	{
		float fTextHeight = 0;
		for (int i = 0; i < nParaCount; i++)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
			fTextHeight += pParagraph->GetHeight();
		}

		fBorderWidth = prtTypesetBorder->right - prtTypesetBorder->left;
		fBorderHeight = prtTypesetBorder->bottom - prtTypesetBorder->top;
		switch (m_pTextObj->eAlignVertical)
		{
		case TEXT_ALIGN_V_TOP:
		case TEXT_ALIGN_V_ENDS:
			ptOffset.y = prtTypesetBorder->top;
			break;
		case TEXT_ALIGN_V_CENTER:
			ptOffset.y = prtTypesetBorder->top + (fBorderHeight - fTextHeight) / 2;
			break;
		case TEXT_ALIGN_V_BOTTOM:
			ptOffset.y = prtTypesetBorder->top + (fBorderHeight - fTextHeight);
			break;
		}

		// 竖直方向两端对齐
		float fHeightFree = fBorderHeight - fTextHeight;
		fHeightFree /= nParaCount - 1;

		for (int i = 0; i < nParaCount; i++)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
			LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryParaObj, i);

			//段水平位置
			switch (pParaObj->eAlignHorizontal)
			{
			case TEXT_ALIGN_H_LEFT:
			case TEXT_ALIGN_H_ENDS:
				ptOffset.x = prtTypesetBorder->left;
				break;
			case TEXT_ALIGN_H_CENTER:
				ptOffset.x = prtTypesetBorder->left + (fBorderWidth - pParagraph->GetRealWidth()) / 2 ;
				break;
			case TEXT_ALIGN_H_RIGHT:
				ptOffset.x = prtTypesetBorder->right - pParagraph->GetRealWidth();
				break;
			}

			pParagraph->SetOffset(ptOffset);
			ptOffset.y += pParagraph->GetHeight();
			if (m_pTextObj->eAlignVertical == TEXT_ALIGN_V_ENDS)
			{
				ptOffset.y += fHeightFree;
			}
		}
	}
	else								//竖排
	{
		float fTextWidth = 0;
		for (int i = 0; i < nParaCount; i++)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
			fTextWidth += pParagraph->GetWidth();
		}

		fBorderWidth = prtTypesetBorder->right - prtTypesetBorder->left;
		fBorderHeight = prtTypesetBorder->bottom - prtTypesetBorder->top;
		if (IsLeftInput())				//左打
		{
			switch (m_pTextObj->eAlignVertical)
			{
			case TEXT_ALIGN_V_TOP:
			case TEXT_ALIGN_V_ENDS:
				ptOffset.x = prtTypesetBorder->left;
				break;
			case TEXT_ALIGN_V_CENTER:
				ptOffset.x = prtTypesetBorder->left + (fBorderWidth - fTextWidth) / 2;
				break;
			case TEXT_ALIGN_V_BOTTOM:
				ptOffset.x = prtTypesetBorder->left + (fBorderWidth - fTextWidth);
				break;
			}

			// 竖直方向两端对齐
			float fWidthFree = fBorderWidth - fTextWidth;
			fWidthFree /= nParaCount - 1;

			for (int i = 0; i < nParaCount; i++)
			{
				CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
				LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryParaObj, i);

				switch (pParaObj->eAlignHorizontal)
				{
				case TEXT_ALIGN_H_LEFT:
				case TEXT_ALIGN_H_ENDS:
					ptOffset.y = prtTypesetBorder->top;
					break;
				case TEXT_ALIGN_H_CENTER:
					ptOffset.y = prtTypesetBorder->top + (fBorderHeight - pParagraph->GetRealHeight()) / 2 ;
					break;
				case TEXT_ALIGN_H_RIGHT:
					ptOffset.y = prtTypesetBorder->bottom - pParagraph->GetRealHeight();
					break;
				}

				pParagraph->SetOffset(ptOffset);
				ptOffset.x += pParagraph->GetWidth();
				if (m_pTextObj->eAlignVertical == TEXT_ALIGN_V_ENDS)
				{
					ptOffset.x += fWidthFree;
				}
			}
		}
		else							//右打
		{
			switch (m_pTextObj->eAlignVertical)
			{
			case TEXT_ALIGN_V_TOP:
			case TEXT_ALIGN_V_ENDS:
				ptOffset.x = prtTypesetBorder->right;
				break;
			case TEXT_ALIGN_V_CENTER:
				ptOffset.x = prtTypesetBorder->right - (fBorderWidth - fTextWidth) / 2;
				break;
			case TEXT_ALIGN_V_BOTTOM:
				ptOffset.x = prtTypesetBorder->right - (fBorderWidth - fTextWidth);
				break;
			}

			// 竖直方向两端对齐
			float fWidthFree = fBorderWidth - fTextWidth;
			fWidthFree /= nParaCount - 1;

			for (int i = 0; i < nParaCount; i++)
			{
				CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(paryParagraph, i);
				LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(paryParaObj, i);

				switch (pParaObj->eAlignHorizontal)
				{
				case TEXT_ALIGN_H_LEFT:
				case TEXT_ALIGN_H_ENDS:
					ptOffset.y = prtTypesetBorder->top;
					break;
				case TEXT_ALIGN_H_CENTER:
					ptOffset.y = prtTypesetBorder->top + (fBorderHeight - pParagraph->GetRealHeight()) / 2 ;
					break;
				case TEXT_ALIGN_H_RIGHT:
					ptOffset.y = prtTypesetBorder->bottom - pParagraph->GetRealHeight();
					break;
				}

				ptOffset.x -= pParagraph->GetWidth();
				pParagraph->SetOffset(ptOffset);
				if (m_pTextObj->eAlignVertical == TEXT_ALIGN_V_ENDS)
				{
					ptOffset.x -= fWidthFree;
				}
			}
		}
	}
}

int CBaseTypeset::GenerateUnderLineInfo(BOOL bAllUnit)
{
	int nUnderLineCnt = 0;

	point3f ptSelfOffset = {0, 0, 0};
	point2f* pvGraphScale = &m_pTextObj->vGraphScale;
	LPCommonTextProp pProp = &m_pTextObj->stuTextProp;
	LPCommonUnderLine pUL;

	BOOL bHorizontal = m_pTextObj->bHorizontal;
	BOOL bNonLinear = !IsLinearTypeset();

	BaseARray* paryPara = &m_pTextObj->arParagraph;
	BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(paryDynPara);

	for (int i = 0; i < nDynParaCnt; i++)
	{
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, i);
		//逐行遍历，以建立下划线信息。
		LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, i);
		BaseARray* paryLine = pDynParag->GetLineArray();
		// 清空下划线信息，否则可能添加两次
		int nLineCnt = BaseAR_GetCount(paryLine);
		for (int nL = 0; nL < nLineCnt; ++nL)
		{
			CBaseTextLine* pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
			int nCnt = BaseAR_GetCount(&pTextLine->m_arULInfo);
			for (int i = 0; i < nCnt; ++i)
			{
				LPCommonUnderLineInfo pULInfo = (LPCommonUnderLineInfo)BaseAR_GetAt(&pTextLine->m_arULInfo, i);
				delete pULInfo;
			}
			BaseAR_RemoveAll(&pTextLine->m_arULInfo);

			int nLineWordInfoCnt = pTextLine->GetWordInfoCount();
			int nLineBeginIdx = pTextLine->GetWordInfoBeginIndex();
			float fBaseLineOffset = pTextLine->GetBaseLineOffset();
			// 如果为公告板，只显示当前页下划线
			if (IsPageMode() && !bAllUnit)
			{
				int pageStart = 0, pageEnd = 0;
				int pageSize = m_pVecPageInfo->size();
				if (pageSize == 0)
				{
					pageStart = 0;
					pageEnd = _tcslen(m_pTextObj->szText);
				}
				else
				{
					PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
					pageStart = pageInfo.nStartIndex;
					pageEnd = pageStart + pageInfo.nCharCnt - 1;
				}
				int nCharIndex = nLineBeginIdx + pDynParag->m_nPreAllWordInfoCnt;
				if (nCharIndex < pageStart || nCharIndex > pageEnd)
				{
					continue;
				}
			}

			float fULTemp1, fULTemp2;
			LPCommonUnderLineInfo pULInfo = NULL;
			LPCommonUnderLine pLastUL = NULL;
			BaseARray* paryWordInfo = &pDynParag->m_arWordInfo;

			if (bHorizontal)
			{
				for (int nW = 0; nW < nLineWordInfoCnt; ++nW)
				{
					LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, nLineBeginIdx + nW);
					pUL = BaseGraphTextProp_GetUnderLine(pProp, pParagObj->nBeginIndex + pWordInfo->nCharIndex);

					if (pUL == NULL || pUL->fHeight <= ABS_FLOAT_0)
						pUL = NULL;
					if (pUL != NULL)
					{
						LPCommonGlyphSpace pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&pTextLine->m_arGlyphSpace, nW);
						if (pUL != pLastUL)
						{
							//创建上一个完整下划线
							if (pULInfo != NULL)
							{
								ptSelfOffset.x = -pULInfo->ptBound[0].x;
								ptSelfOffset.y = 0;
								BaseGraphOffsetPt3fs(pULInfo->ptBound, ptSelfOffset, BaseGraphGLYPHSPACEPTs);
							}

							//新下划线开始
							pULInfo = new BaseGraphUnderLineInfo;
							memset(pULInfo, 0, sizeof(BaseGraphUnderLineInfo));
							BaseAR_Add(&pTextLine->m_arULInfo, pULInfo);
							++nUnderLineCnt;

							pULInfo->nCharIndex = pWordInfo->nCharIndex;
							pULInfo->ptPos.x = pSpaceInfo->ptPos.x;
							pULInfo->ptPos.y = pSpaceInfo->ptPos.y;
							pULInfo->ptBound[0].x = pSpaceInfo->ptBound[0].x;
							pULInfo->ptBound[3].x = pSpaceInfo->ptBound[3].x;
							pULInfo->ptBound[1].x = pSpaceInfo->ptBound[4].x;
							pULInfo->ptBound[2].x = pSpaceInfo->ptBound[5].x;

							fULTemp1 = pUL->fHeight/2 * pvGraphScale->y;
							fULTemp2 = pULInfo->ptPos.y + pUL->fDistance * pvGraphScale->y;
							pULInfo->ptBound[3].y = fULTemp2 + fULTemp1;
							pULInfo->ptBound[2].y = fULTemp2 + fULTemp1;
							pULInfo->ptBound[0].y = fULTemp2 - fULTemp1;
							pULInfo->ptBound[1].y = fULTemp2 - fULTemp1;
							pULInfo->ptPos.y -= fBaseLineOffset;
						}
						else
						{
							//延续下划线
							pULInfo->ptBound[1].x = pSpaceInfo->ptBound[4].x;
							pULInfo->ptBound[2].x = pSpaceInfo->ptBound[5].x;
						}
					}
					pLastUL = pUL;
				}

				//创建上一个完整下划线
				if (pULInfo != NULL)
				{
					ptSelfOffset.x = -pULInfo->ptBound[0].x;
					ptSelfOffset.y = 0;
					BaseGraphOffsetPt3fs(pULInfo->ptBound, ptSelfOffset, BaseGraphGLYPHSPACEPTs);
				}
			}
			else
			{
				for (int nW = 0; nW < nLineWordInfoCnt; ++nW)
				{
					LPCommonWordInfo pWordInfo = (LPCommonWordInfo)BaseAR_GetAt(paryWordInfo, nLineBeginIdx + nW);
					pUL = BaseGraphTextProp_GetUnderLine(pProp, pParagObj->nBeginIndex + pWordInfo->nCharIndex);

					if (pUL == NULL || pUL->fHeight <= ABS_FLOAT_0)
						pUL = NULL;
					if (pUL != NULL)
					{
						LPCommonGlyphSpace pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&pTextLine->m_arGlyphSpace, nW);
						if (pUL != pLastUL)
						{
							//创建上一个完整下划线
							if (pULInfo != NULL)
							{
								ptSelfOffset.x = 0;
								ptSelfOffset.y = -pULInfo->ptBound[0].y;
								BaseGraphOffsetPt3fs(pULInfo->ptBound, ptSelfOffset, BaseGraphGLYPHSPACEPTs);
							}

							//新下划线开始
							pULInfo = new BaseGraphUnderLineInfo;
							memset(pULInfo, 0, sizeof(BaseGraphUnderLineInfo));
							BaseAR_Add(&pTextLine->m_arULInfo, pULInfo);
							++nUnderLineCnt;

							pULInfo->nCharIndex = pWordInfo->nCharIndex;
							pULInfo->ptPos.x = pSpaceInfo->ptPos.x;
							pULInfo->ptPos.y = pSpaceInfo->ptPos.y;
							pULInfo->ptBound[0].y = pSpaceInfo->ptBound[0].y;
							pULInfo->ptBound[1].y = pSpaceInfo->ptBound[1].y;
							pULInfo->ptBound[2].y = pSpaceInfo->ptBound[4].y;
							pULInfo->ptBound[3].y = pSpaceInfo->ptBound[5].y;

							fULTemp1 = pUL->fHeight/2 * pvGraphScale->x;
							fULTemp2 = pULInfo->ptPos.x - pUL->fDistance * pvGraphScale->x;
							pULInfo->ptBound[1].x = fULTemp2 + fULTemp1;
							pULInfo->ptBound[2].x = fULTemp2 + fULTemp1;
							pULInfo->ptBound[0].x = fULTemp2 - fULTemp1;
							pULInfo->ptBound[3].x = fULTemp2 - fULTemp1;
							pULInfo->ptPos.x = 0;
						}
						else
						{
							//延续下划线
							pULInfo->ptBound[2].y = pSpaceInfo->ptBound[4].y;
							pULInfo->ptBound[3].y = pSpaceInfo->ptBound[5].y;
						}
					}
					pLastUL = pUL;
				}

				//创建上一个完整下划线
				if (pULInfo != NULL)
				{
					ptSelfOffset.x = 0;
					ptSelfOffset.y = -pULInfo->ptBound[0].y;
					BaseGraphOffsetPt3fs(pULInfo->ptBound, ptSelfOffset, BaseGraphGLYPHSPACEPTs);
				}
			}
		}
	}

	return nUnderLineCnt;
}

void  CBaseTypeset::UpdateUnderLineUnit(int nUnderLineCnt, BOOL bAllUnit)
{
	if (nUnderLineCnt > 0)
	{
		BaseARray* paryUnit = &m_pTextObj->ar2DUnit;
		LPCommonTextProp pProp = &m_pTextObj->stuTextProp;
		BOOL bHorizontal = m_pTextObj->bHorizontal;
		BOOL bNonLinear = !IsLinearTypeset();

		BaseARray* paryPara = &m_pTextObj->arParagraph;
		BaseARray* paryDynPara = &m_pTypesetPrivate->m_arParagraph;
		int nDynParaCnt = BaseAR_GetCount(paryDynPara);
		int nUnitOldCnt = BaseAR_GetCount(paryUnit);

		for (int nP = 0; nP < nDynParaCnt; ++nP)
		{
			CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(paryDynPara, nP);
			LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nP);
			BaseARray* paryLine = pDynParag->GetLineArray();
			int nLineCnt = BaseAR_GetCount(paryLine);

			for (int nL = 0; nL < nLineCnt; ++nL)
			{
				CBaseTextLine* pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
				int nLineWordInfoCnt = pTextLine->GetWordInfoCount();
				int nLineBeginIdx = pTextLine->GetWordInfoBeginIndex();
				float fBaseLineOffset = pTextLine->GetBaseLineOffset();
				// 如果为公告板，只显示当前页下划线
				if (IsPageMode() && !bAllUnit)
				{
					int pageStart = 0, pageEnd = 0;
					int pageSize = m_pVecPageInfo->size();
					if (pageSize == 0)
					{
						pageStart = 0;
						pageEnd = _tcslen(m_pTextObj->szText);
					}
					else
					{
						PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
						pageStart = pageInfo.nStartIndex;
						pageEnd = pageStart + pageInfo.nCharCnt - 1;
					}
					int nCharIndex = nLineBeginIdx + pDynParag->m_nPreAllWordInfoCnt;
					if (nCharIndex < pageStart || nCharIndex > pageEnd)
					{
						continue;
					}
				}

				BaseARray* paryULInfo = &pTextLine->m_arULInfo;
				int nULInfoCnt = BaseAR_GetCount(paryULInfo);

				for (int nU = 0; nU < nULInfoCnt; ++nU)
				{
					//创建下划线Unit
					LPCommonUnderLineInfo pULInfo = (LPCommonUnderLineInfo)BaseAR_GetAt(paryULInfo, nU);
					LPCommonUnderLine pCreateUL = BaseGraphTextProp_GetUnderLine(pProp, pParagObj->nBeginIndex + pULInfo->nCharIndex);
					if (pCreateUL != NULL)
					{
						BaseGraphUnit* pUnit = new BaseGraphUnit;
						common_init_unit(pUnit, FALSE);
						pUnit->dwUserID = BaseGraphUNIT_USERID_UNDERLINE;

						point2f pt[4];
						for (int i = 0; i < 4; ++i)
						{
							pt[i].x = pULInfo->ptBound[i].x;
							pt[i].y = pULInfo->ptBound[i].y;
						}
						CreateRegionVectorGraphFromPoints(&pUnit->u.u2d.graph, pt, 4, FALSE);

						BaseGraphStaticEffect_Copy(&pUnit->u.u2d.staticeffect, &pCreateUL->se2DColor);
						BaseAR_Add(paryUnit, pUnit);
					}
				}
			}
		}

		//下划线Unit个数
		m_pTextObj->nUnderlineUnitCnt = BaseAR_GetCount(paryUnit) - nUnitOldCnt;
	}
	else
	{
		m_pTextObj->nUnderlineUnitCnt = 0;
	}
}

// 更新页信息
void CBaseTypeset::UpdatePageInfo()
{
	//更新2DUnit位置
	point2f ptUnitOffset, ptParagOffset, ptLineOffset;
	CBaseTextParagraph* pParagraph;
	BaseGraphTextParagraph* pParagObj;
	CBaseTextLine* pTextLine;

	UINT nHeight = m_pTextObj->rtBorder.bottom - m_pTextObj->rtBorder.top;
	int iPageCnt = 0;
	int iLastPagePos = 0;
	if (m_pVecPageInfo == NULL)
	{
		return;
	}

	int oldPageCnt = m_pVecPageInfo->size();
	if (oldPageCnt == 0)
	{
		// 至少有一页
		PageInfo pageInfo;
		m_pVecPageInfo->push_back(pageInfo);
		oldPageCnt = 1;
	}
	int newPageCnt = 0;

	rectf rtTypesetReal;
	rtTypesetReal.top = 65535;
	rtTypesetReal.bottom = -65535;
	// 最后一页实际大小
	float fTopLast = rtTypesetReal.top;
	float fBottomLast = rtTypesetReal.bottom;

	int nParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	for (int nP = 0; nP < nParaCnt; ++nP)
	{
		pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nP);
		pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nP);
		BaseARray* paryLine = pParagraph->GetLineArray();
		int nLineCnt = BaseAR_GetCount(paryLine);
		ptParagOffset = pParagraph->GetOffset();

		for (int nL = 0; nL < nLineCnt; nL++)
		{
			pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
			ptLineOffset = pTextLine->GetLineOffset();
			ptUnitOffset = ptParagOffset + ptLineOffset;

			//排版后实际占据的矩形
			BaseGraphSizef* pLineSize = pTextLine->GetRealSizePtr();
			float fTopOld = rtTypesetReal.top;
			float fBottomOld = rtTypesetReal.bottom;
			rtTypesetReal.top = min(rtTypesetReal.top, ptUnitOffset.y);
			rtTypesetReal.bottom = max(rtTypesetReal.bottom, ptUnitOffset.y + pLineSize->cy);
			fTopLast = rtTypesetReal.top;
			fBottomLast = rtTypesetReal.bottom;

			// 超出这个高度放此行到下一页
			if (rtTypesetReal.bottom - rtTypesetReal.top - iLastPagePos > nHeight)
			{
				point2f ptParagOffsetNew;
				if (nL == 0)
				{
					if (nP == 0)
					{
						// rtBorder高度小于一个字,扩充边界重新计算
						m_pTextObj->rtBorder.bottom = rtTypesetReal.bottom;
						nHeight = m_pTextObj->rtBorder.bottom - m_pTextObj->rtBorder.top;
						nL--;
						continue;
					}
					// 此段落都放入下一页
					pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nP-1);
					pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nP-1);
					BaseARray* paryLine = pParagraph->GetLineArray();
					int nLineCnt = BaseAR_GetCount(paryLine);
					ptParagOffsetNew = pParagraph->GetOffset();

					pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nLineCnt-1);
					ptLineOffset = pTextLine->GetLineOffset();
					ptUnitOffset = ptParagOffsetNew + ptLineOffset;
				}
				else
				{
					pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL-1);
					ptLineOffset = pTextLine->GetLineOffset();
					ptUnitOffset = ptParagOffset + ptLineOffset;
				}

				//排版后实际占据的矩形
				BaseGraphSizef* pLineSize = pTextLine->GetRealSizePtr();
				rtTypesetReal.top = min(fTopOld, ptUnitOffset.y);
				rtTypesetReal.bottom = max(fBottomOld, ptUnitOffset.y + pLineSize->cy);

				int iLineStartPos = pParagObj->nBeginIndex + pTextLine->GetWordInfoBeginIndex();
				int iLineEndPos = iLineStartPos + pTextLine->GetWordInfoCount();

				// 重置当前位置信息
				pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nP);
				pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, nP);
				ptParagOffset = pParagraph->GetOffset();

				// 防止丢掉分页信息中其他元素，如时间等,不能清空整体
				if (newPageCnt >= oldPageCnt)
				{
					// 页数增加
					PageInfo pageInfo;
					//记录每一页的开始字符位置和长度
					PageInfo lastPageInfo = m_pVecPageInfo->at(m_pVecPageInfo->size() - 1);
					pageInfo.nStartIndex = lastPageInfo.nStartIndex + lastPageInfo.nCharCnt;
					pageInfo.nCharCnt = iLineEndPos - pageInfo.nStartIndex;
					pageInfo.nStartHeight = lastPageInfo.nStartHeight + lastPageInfo.nHeight;
					pageInfo.nHeight = rtTypesetReal.bottom - rtTypesetReal.top - iLastPagePos;
					pageInfo.fStartTime = lastPageInfo.fStartTime + lastPageInfo.fTimeLen;
					pageInfo.fTimeLen = lastPageInfo.fTimeLen;
					m_pVecPageInfo->insert(m_pVecPageInfo->begin()+newPageCnt, 1, pageInfo);

					iLastPagePos += pageInfo.nHeight;
				}
				else
				{
					PageInfo& pageInfo = m_pVecPageInfo->at(newPageCnt);
					//记录每一页的开始字符位置和长度
					if (newPageCnt == 0)
					{
						pageInfo.nStartIndex = 0;
						pageInfo.nCharCnt = iLineEndPos;
						pageInfo.nStartHeight = 0;
						pageInfo.nHeight = rtTypesetReal.bottom - rtTypesetReal.top - iLastPagePos;
					}
					else
					{
						PageInfo lastPageInfo = m_pVecPageInfo->at(newPageCnt-1);
						pageInfo.nStartIndex = lastPageInfo.nStartIndex + lastPageInfo.nCharCnt;
						pageInfo.nCharCnt = iLineEndPos - pageInfo.nStartIndex;
						pageInfo.nStartHeight = lastPageInfo.nStartHeight + lastPageInfo.nHeight;
						pageInfo.nHeight = rtTypesetReal.bottom - rtTypesetReal.top - iLastPagePos;
					}
					iLastPagePos += pageInfo.nHeight;
				}
				if (nP == nParaCnt-1 && nL == nLineCnt-1)
				{
					//最后一页
				}
				newPageCnt++;
			}
		}
	}

	// 最后一页
	if (newPageCnt >= oldPageCnt)
	{
		// 页数增加
		PageInfo pageInfo;
		//记录每一页的开始字符位置和长度
		PageInfo lastPageInfo = m_pVecPageInfo->at(newPageCnt-1);
		pageInfo.nStartIndex = lastPageInfo.nStartIndex + lastPageInfo.nCharCnt;
		pageInfo.nCharCnt = GetTextLength() - pageInfo.nStartIndex;
		pageInfo.nStartHeight = lastPageInfo.nStartHeight + lastPageInfo.nHeight;
		pageInfo.nHeight = fBottomLast - fTopLast - iLastPagePos;
		pageInfo.fStartTime = lastPageInfo.fStartTime + lastPageInfo.fTimeLen;
		pageInfo.fTimeLen = lastPageInfo.fTimeLen;
		m_pVecPageInfo->insert(m_pVecPageInfo->begin()+newPageCnt, 1, pageInfo);

		iLastPagePos += pageInfo.nHeight;
	}
	else
	{
		PageInfo& pageInfo = m_pVecPageInfo->at(newPageCnt);
		//记录每一页的开始字符位置和长度
		if (newPageCnt == 0)
		{
			pageInfo.nStartIndex = 0;
			pageInfo.nCharCnt = GetTextLength();
			pageInfo.nStartHeight = 0;
			pageInfo.nHeight = nHeight;
		}
		else
		{
			PageInfo lastPageInfo = m_pVecPageInfo->at(newPageCnt-1);
			pageInfo.nStartIndex = lastPageInfo.nStartIndex + lastPageInfo.nCharCnt;
			pageInfo.nCharCnt = GetTextLength() - pageInfo.nStartIndex;
			pageInfo.nStartHeight = lastPageInfo.nStartHeight + lastPageInfo.nHeight;
			pageInfo.nHeight = fBottomLast - fTopLast - iLastPagePos;
		}
		iLastPagePos += pageInfo.nHeight;
	}
	newPageCnt++;

	int diffCnt = oldPageCnt - newPageCnt;
	for (int i=0; i<diffCnt; i++)
	{
		VectPage::iterator it = m_pVecPageInfo->begin()+newPageCnt;
		 m_pVecPageInfo->erase(it);
	}
	m_pTextObj->nCurPageIndex = min(m_pVecPageInfo->size()-1, m_pTextObj->nCurPageIndex);
}

void CBaseTypeset::UpdateUnitPos(int nUnderLineCnt, BOOL bAllUnit/* = TRUE*/)
{
	//排版后实际占据的矩形
	rectf* prtTypesetReal = &m_pTextObj->rtTypesetReal;
	prtTypesetReal->left = 65535;
	prtTypesetReal->top = 65535;
	prtTypesetReal->right = -65535;
	prtTypesetReal->bottom = -65535;

	rectf rtTypesetReal;
	rtTypesetReal.left = 65535;
	rtTypesetReal.top = 65535;
	rtTypesetReal.right = -65535;
	rtTypesetReal.bottom = -65535;

	//更新2DUnit位置
	point2f ptUnitOffset, ptParagOffset, ptLineOffset;
	CBaseTextParagraph* pParagraph;
	CBaseTextLine* pTextLine;
	BaseARray* pary2DUnit = &m_pTextObj->ar2DUnit;
	BaseGraphUnit* pUnit = NULL;
	LPCommonGlyphSpace pSpaceInfo;
	int nUnitCnt = 0;
	int nUnitCntNew = 0;

	//隐藏越出边框者
	rectf rtHideBorder = m_pTextObj->rtBorder;
	BaseRectf_InflateRect1(&rtHideBorder, 2, 2);
	BOOL bHideBorderCrosserC = m_pTextObj->bHideBorderCrosserC;
	BOOL bHideBorderCrosserL = m_pTextObj->bHideBorderCrosserL;
	if (!m_pTextObj->bUseBorderEditMode || m_pTextObj->bFixedFrame)
	{
		bHideBorderCrosserC = FALSE;
		bHideBorderCrosserL = FALSE;
	}

	//排版总行数极限
	BOOL bHideLine;
	BOOL bHorizontal = m_pTextObj->bHorizontal;
	int nLineCounter = 0;
	int nLinesLimit = 65535;
	if (m_pTextObj->nLinesLimit > -1)
		nLinesLimit = m_pTextObj->nLinesLimit;

	int iPageStartPos = 0;
	int iPageOffset = 0;
	int nParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	for (int nP = 0; nP < nParaCnt; ++nP)
	{
		pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nP);
		BaseARray* paryLine = pParagraph->GetLineArray();
		int nLineCnt = BaseAR_GetCount(paryLine);
		ptParagOffset = pParagraph->GetOffset();

		for (int nL = 0; nL < nLineCnt; nL++)
		{
			pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
			ptLineOffset = pTextLine->GetLineOffset();
			ptUnitOffset = ptParagOffset + ptLineOffset;
			if (nP == 0 && nL == 0)
			{
				// 第一页开始位置，不一定为0
				iPageStartPos = -ptUnitOffset.y;
			}

			//排版后实际占据的矩形
			BaseGraphSizef* pLineSize = pTextLine->GetRealSizePtr();
			prtTypesetReal->top = min(prtTypesetReal->top, ptUnitOffset.y);
			prtTypesetReal->bottom = max(prtTypesetReal->bottom, ptUnitOffset.y + pLineSize->cy);
			prtTypesetReal->left = min(prtTypesetReal->left, ptUnitOffset.x);
			prtTypesetReal->right = max(prtTypesetReal->right, ptUnitOffset.x + pLineSize->cx);

			bHideLine = (++nLineCounter > nLinesLimit);

			int nW;
			int nUnitCntCopy = nUnitCntNew;
			int nWordInfoCnt = pTextLine->GetWordInfoCount();
			int nArtStartPos = 0;
			for (nW = 0; nW < nWordInfoCnt; ++nW)
			{
				if (m_pTextObj->bPageMode && !bAllUnit && m_pVecPageInfo->size() > 0)
				{
					//公告板只显示当前页内容
					PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
					if (nUnitCnt < pageInfo.nStartIndex)
					{
						nUnitCnt++;
						continue;
					}
					else if (nUnitCnt == pageInfo.nStartIndex)
					{
						// 当前页相对于首页开始的偏移
						iPageOffset = -ptUnitOffset.y;
						iPageOffset -= iPageStartPos;
					}
					else if (nUnitCnt >= pageInfo.nStartIndex + pageInfo.nCharCnt)
					{
						nUnitCnt++;
						break;
					}
					nUnitCnt++;
				}

				pUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, nUnitCntNew);
				++nUnitCntNew;
				if (pUnit == NULL)
				{
					continue;
				}

 				vectorgraph* pGraph = &(pUnit->u.u2d.graph);
				// 左打本行内容显示相反,竖排行内顺序一致，行顺序从右往左
				if (IsLeftInput() || !IsHorizontal())
				{
					pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&pTextLine->m_arGlyphSpace, nW);
				}
				else
				{
					pSpaceInfo = (LPCommonGlyphSpace)BaseAR_GetAt(&pTextLine->m_arGlyphSpace, nWordInfoCnt - 1 - nW);
				}

				if (IsArtWord()&& pUnit != NULL)
 				{
					pUnit->pos.y += -(ptUnitOffset.y + pSpaceInfo->ptPos.y + pSpaceInfo->ptPosOffset.y);

					// 艺术字变形时，graph.bound已经按顺序编排好
					if (nW == 0)
					{
						pUnit->pos.x = ptUnitOffset.x + pSpaceInfo->ptPos.x + pSpaceInfo->ptPosOffset.x;
						nArtStartPos = pUnit->pos.x;
					}
					else
					{
						pUnit->pos.x = nArtStartPos;
					}
 					// 艺术字的rect和普通标题字不同，必须包含全部uint
 					rtTypesetReal.top = min(rtTypesetReal.top, pUnit->u.u2d.graph.bound.top - pUnit->pos.y);
 					rtTypesetReal.bottom = max(rtTypesetReal.bottom, pUnit->u.u2d.graph.bound.bottom - pUnit->pos.y);
 					rtTypesetReal.left = min(rtTypesetReal.left, pUnit->pos.x + pUnit->u.u2d.graph.bound.left);
 					rtTypesetReal.right = max(rtTypesetReal.right, pUnit->pos.x + pUnit->u.u2d.graph.bound.right);
 				}
				else if (IsPathTypeset() && pUnit != NULL)
				{
					// 曲线文字的形状都变化完毕，调整位置即可
					pUnit->pos.y = pSpaceInfo->ptPos.y;
					pUnit->pos.x = pSpaceInfo->ptPos.x;
				}
				else
				{
					pUnit->pos.x = ptUnitOffset.x + pSpaceInfo->ptPos.x + pSpaceInfo->ptPosOffset.x;
					pUnit->pos.y = -(ptUnitOffset.y + pSpaceInfo->ptPos.y + pSpaceInfo->ptPosOffset.y);
					pUnit->pos.y -= iPageOffset;
				}
 
 				//隐藏单unit。一行中任意unit隐藏，则整行隐藏。
 				if (bHideBorderCrosserC)
 				{
 					if (bHorizontal)
 					{
 						if (pUnit->pos.x + pGraph->bound.right > rtHideBorder.right
 							|| pUnit->pos.x + pGraph->bound.left < rtHideBorder.left)
 						{
 							graph_release(pGraph);
 						}
 					}
 					else
 					{
 						if (-pUnit->pos.y + pGraph->bound.bottom > rtHideBorder.bottom
 							|| -pUnit->pos.y + pGraph->bound.top < rtHideBorder.top)
 						{
 							graph_release(pGraph);
 						}
 					}
 				}
 				if (bHideBorderCrosserL)
 				{
 					if (bHorizontal)
 					{
 						if (-pUnit->pos.y + pGraph->bound.bottom > rtHideBorder.bottom
 							|| -pUnit->pos.y + pGraph->bound.top < rtHideBorder.top)
 						{
 							bHideLine = TRUE;
 						}
 					}
 					else
 					{
 						if (pUnit->pos.x + pGraph->bound.right > rtHideBorder.right
 							|| pUnit->pos.x + pGraph->bound.left < rtHideBorder.left)
 						{
 							bHideLine = TRUE;
 						}
 					}
 				}
 			}
 
 			//隐藏整行
 			if (bHideLine)
 			{
 				for (nW = 0; nW < nWordInfoCnt; ++nW)
 				{
 					pUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, nUnitCntCopy++);
 					graph_release(&(pUnit->u.u2d.graph));
 				}
 			}
 		}
	}

	nUnitCnt = nUnitCntNew;
	iPageStartPos = 0;
	iPageOffset = 0;
	//下划线Unit定位
	if (nUnderLineCnt > 0)
	{
		nLineCounter = 0;
		for (int nP = 0; nP < nParaCnt; ++nP)
		{
			pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nP);
			BaseARray* paryLine = pParagraph->GetLineArray();
			int nLineCnt = BaseAR_GetCount(paryLine);
			ptParagOffset = pParagraph->GetOffset();

			for (int nL = 0; nL < nLineCnt; ++nL)
			{
				pTextLine = (CBaseTextLine*)BaseAR_GetAt(paryLine, nL);
				ptLineOffset = pTextLine->GetLineOffset();
				ptUnitOffset = ptParagOffset + ptLineOffset;
				if (nP == 0 && nL == 0)
				{
					// 第一页开始位置，不一定为0
					iPageStartPos = -ptUnitOffset.y;
				}
				if (m_pTextObj->bPageMode && !bAllUnit && m_pVecPageInfo->size() > 0)
				{
					// 当前页相对于首页开始的偏移
					PageInfo pageInfo = m_pVecPageInfo->at(m_pTextObj->nCurPageIndex);
					int nLineBeginIdx = pTextLine->GetWordInfoBeginIndex();
					int nCharIndex = nLineBeginIdx + pParagraph->m_nPreAllWordInfoCnt;
					if (nCharIndex == pageInfo.nStartIndex)
					{
						// 当前页相对于首页开始的偏移
						iPageOffset = -ptUnitOffset.y;
						iPageOffset -= iPageStartPos;
					}
				}
				//bHideLine = (++nLineCounter > nLinesLimit);

				BaseARray* paryULInfo = &pTextLine->m_arULInfo;
				int nULCnt = BaseAR_GetCount(paryULInfo);
				for (int nU = 0; nU < nULCnt; ++nU)
				{
					pUnit = (BaseGraphUnit*)BaseAR_GetAt(pary2DUnit, nUnitCnt);
					if (pUnit != NULL)
					{
						LPCommonUnderLineInfo pULInfo = (LPCommonUnderLineInfo)BaseAR_GetAt(paryULInfo, nU);
						pUnit->pos.x = ptUnitOffset.x + pULInfo->ptPos.x;
						pUnit->pos.y = -(ptUnitOffset.y + pULInfo->ptPos.y);
						pUnit->pos.y -= iPageOffset;
// 						if (bHideLine)
// 							graph_release(&(pUnit->u.u2d.graph));
						++nUnitCnt;
					}
				}
			}
		}
	}

	//调整不合理的矩形框
	AdjustTypesetRealRect(*prtTypesetReal);
	if (!m_pTextObj->bUseBorderEditMode && !m_pTextObj->bFixedFrame)
	{
		m_pTextObj->rtBorder = *prtTypesetReal;
	}
}

int CBaseTypeset::MakeSpecialEffect()
{
	//特殊效果
	int nSpecialUnitCnt = 0;
	common_special_effect* pSpecial = m_pTextObj->pSpecialEffect;
	if (pSpecial != NULL)
	{
		if (pSpecial->m_GraphType == SELF_BACK)
		{
			//自绘背景
			BaseARray* paryUnit = &m_pTextObj->ar2DUnit;
			BaseGraphUnit* pUnit = new BaseGraphUnit;
			common_init_unit(pUnit, FALSE);
			pUnit->dwUserID = BaseGraphUNIT_USERID_SPECIAL;

			rectf rtRect, rtUnit;
			GetModelRect(rtRect, FALSE);

			BOOL bHorizontal = m_pTextObj->bHorizontal;
			rectf rtBKRect;
			if (bHorizontal)
			{
				rtBKRect = pSpecial->m_BKRect;
			}
			else
			{
				rtBKRect.left = pSpecial->m_BKRect.bottom;
				rtBKRect.top = pSpecial->m_BKRect.left;
				rtBKRect.right = pSpecial->m_BKRect.top;
				rtBKRect.bottom = pSpecial->m_BKRect.right;
			}

			if (pSpecial->m_bUnitary)
			{
				//归一化
				rtUnit.left = rtRect.left * rtBKRect.left;
				rtUnit.top = rtRect.top * rtBKRect.top;
				rtUnit.right = rtRect.right * rtBKRect.right;
				rtUnit.bottom = rtRect.bottom * rtBKRect.bottom;
			}
			else
			{
				if (!bHorizontal)
				{
					rtBKRect.left = -rtBKRect.left;
					rtBKRect.right = -rtBKRect.right;
				}
				rtUnit = rtBKRect;
				int nAlign = pSpecial->m_nAlignMode;
				switch (nAlign)
				{
				case ALIGN_TL:
					pUnit->pos.x = bHorizontal ? rtRect.left : rtRect.right;
					pUnit->pos.y = -rtRect.top;
					break;
				case ALIGN_ML:
					pUnit->pos.x = bHorizontal ? rtRect.left : (rtRect.left + rtRect.right)/2;
					pUnit->pos.y = bHorizontal ? -(rtRect.top + rtRect.bottom)/2 : -rtRect.top;
					break;
				case ALIGN_BL:
					pUnit->pos.x = rtRect.left;
					pUnit->pos.y = bHorizontal ? -rtRect.bottom : -rtRect.top;
					break;
				case ALIGN_BM:
					pUnit->pos.x = bHorizontal ? (rtRect.left + rtRect.right)/2 : rtRect.left;
					pUnit->pos.y = bHorizontal ? -rtRect.bottom : -(rtRect.top + rtRect.bottom)/2;
					break;
				case ALIGN_BR:
					pUnit->pos.x = bHorizontal ? rtRect.right : rtRect.left;
					pUnit->pos.y = -rtRect.bottom;
					break;
				case ALIGN_MR:
					pUnit->pos.x = bHorizontal ? rtRect.right : (rtRect.left + rtRect.right)/2;
					pUnit->pos.y = bHorizontal ? -(rtRect.top + rtRect.bottom)/2 : -rtRect.bottom;
					break;
				case ALIGN_TR:
					pUnit->pos.x = rtRect.right;
					pUnit->pos.y = bHorizontal ? -rtRect.top : -rtRect.bottom;
					break;
				case ALIGN_TM:
					pUnit->pos.x = bHorizontal ? (rtRect.left + rtRect.right)/2 : rtRect.right;
					pUnit->pos.y = bHorizontal ? -rtRect.top : -(rtRect.top + rtRect.bottom)/2;
					break;
				default:
					pUnit->pos.x = (rtRect.left + rtRect.right)/2;
					pUnit->pos.y = -(rtRect.top + rtRect.bottom)/2;
					break;
				}
			}

			point2f pt[4] = {{rtUnit.left, rtUnit.top}, {rtUnit.right, rtUnit.top}, 
				{rtUnit.right, rtUnit.bottom}, {rtUnit.left, rtUnit.bottom}};
			CreateRegionVectorGraphFromPoints(&pUnit->u.u2d.graph, pt, 4, FALSE);

			BaseGraphStaticEffect_Copy(&pUnit->u.u2d.staticeffect, &pSpecial->m_BKPublicAttr);
			BaseAR_Add(paryUnit, pUnit);

			nSpecialUnitCnt = 1;
		}
	}

	m_pTextObj->nSpecialUnitCnt = nSpecialUnitCnt;
	return nSpecialUnitCnt;
}

BOOL CBaseTypeset::IsNeedUpdateUnitArray()
{
	//用末段计算Glyph总数
	int nWordInfoCnt = 0;
	int nParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	if (nParaCnt > 0)
	{
		CBaseTextParagraph* pDynParag = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaCnt - 1);
		nWordInfoCnt = pDynParag->m_nPreAllWordInfoCnt + BaseAR_GetCount(&pDynParag->m_arWordInfo);
	}

	int nUnitCnt = BaseAR_GetCount(&m_pTextObj->ar2DUnit);
	return nWordInfoCnt != nUnitCnt;
}

BOOL CBaseTypeset::HitTestText(point2f& point, BOOL bCaptureMouse, BOOL& bHorizontal)
{
	bHorizontal = m_pTextObj->bHorizontal;
	return IsHitTextByPoint(point, bCaptureMouse);
}

BOOL CBaseTypeset::IsHitTextByPoint(point2f& pt, BOOL bCaptureMouse)
{
	if (m_pTextObj == NULL)
		return FALSE;

	point2f ptHit;
	ptHit.x = pt.x;
	ptHit.y = pt.y;

	if (!bCaptureMouse)
	{
		rectf rtBorder;
		GetTypesetRect(rtBorder, FALSE);
		if (!BaseRectf_PtInRect(&rtBorder, ptHit))
		{
			return FALSE;
		}
	}

	BOOL bLinearTypeset = IsLinearTypeset();
	BaseARray* parDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(parDynPara);
	for (int i = 0; i < nDynParaCnt; ++i)
	{
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(parDynPara, i);
		if (pParagraph->IsHitTextByPoint(ptHit, bCaptureMouse, bLinearTypeset))
		{
			return TRUE;
		}
	}

	return FALSE;
}

int CBaseTypeset::GetCharPosFromPoint(BOOL& bIsLineEndPos, point2f& pt, BOOL bCaptureMouse)
{
	bIsLineEndPos = FALSE;
	if (m_pTextObj == NULL)
		return -1;

	point2f ptHit;
	ptHit.x = pt.x;
	ptHit.y = pt.y;

	if (!bCaptureMouse)
	{
		rectf rtBorder;
		GetTypesetRect(rtBorder, FALSE);
		if (!BaseRectf_PtInRect(&rtBorder, ptHit))
		{
			return -1;
		}
	}

	BOOL bLinearTypeset = IsLinearTypeset();
	BaseARray* parDynPara = &m_pTypesetPrivate->m_arParagraph;
	int nDynParaCnt = BaseAR_GetCount(parDynPara);
	for (int i = 0; i < nDynParaCnt; ++i)
	{
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(parDynPara, i);
		int nHitIndex = pParagraph->GetCharPosFromPoint(bIsLineEndPos, ptHit, bCaptureMouse, bLinearTypeset);
		if (nHitIndex >= 0)
		{
			//索引调整原则：
			//禁止返回回车和换行两个字符之间的位置；
			//如果是行尾，则索引位置禁止在回车换行后面。
			int nStrLen = (int)_tcslen(m_pTextObj->szText);
			if (IsLeftInput())
			{
				if (nHitIndex >= 1 && BaseGraphISCRLF(m_pTextObj->szText, nHitIndex-1, nStrLen))
				{
					nHitIndex -= 1;
				}
				else if (bIsLineEndPos)
				{
					if (nHitIndex >= 2 && BaseGraphISCRLF(m_pTextObj->szText, nHitIndex-2, nStrLen))
					{
						nHitIndex -= 2;
					}
				}
			}

			return nHitIndex;
		}
	}

	return -1;
}

int CBaseTypeset::GetCharPosFromPoint(BOOL& bIsLineEndPos, 
									point2f& pt, 
									int nParaIdx, 
									int nLineIdx,
									BOOL bCaptureMouse)
{
	bIsLineEndPos = FALSE;
	if (m_pTextObj == NULL)
		return -1;

	int nParaCount = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	if (nParaIdx >=0 && nParaIdx < nParaCount)
	{
		BOOL bLinearTypeset = IsLinearTypeset();
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
		int nHitIndex = pParagraph->GetCharPosFromPoint(bIsLineEndPos, pt, nLineIdx, bCaptureMouse, bLinearTypeset);
		if (nHitIndex >= 0)
		{
			int nStrLen = (int)_tcslen(m_pTextObj->szText);
			if (nHitIndex >= 1 && BaseGraphISCRLF(m_pTextObj->szText, nHitIndex-1, nStrLen))
			{
				nHitIndex -= 1;
			}
			else if (bIsLineEndPos)
			{
				if (nHitIndex >= 2 && BaseGraphISCRLF(m_pTextObj->szText, nHitIndex-2, nStrLen))
				{
					nHitIndex -= 2;
				}
			}
			return nHitIndex;
		}
	}

	return -1;
}

void CBaseTypeset::ConvertCharPosToParaLineIdx(int& nParaIdx, 
											 int& nLineIdx, 
											 int nCharIndex, 
											 BOOL bIsLineEndPos)
{
	BOOL bLineEnd = bIsLineEndPos;
	//字符总数。
	int nStrLen = (int)_tcslen(m_pTextObj->szText);

	int nParaCnt = BaseAR_GetCount(&m_pTextObj->arParagraph);
	for (int i = nParaCnt-1; i >= 0; --i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(&m_pTextObj->arParagraph, i);
		if (nCharIndex >= pParaObj->nBeginIndex || nCharIndex >= nStrLen)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
			pParagraph->ConvertCharPosToLineIdx(nLineIdx, nCharIndex, bLineEnd);
			if (nLineIdx >= 0)
			{
				nParaIdx = i;
			}
			else
			{
				nParaIdx = -1;
			}
			return;
		}
	}

	nParaIdx = -1;
	nLineIdx = -1;
}

void CBaseTypeset::GetParaLineIdxOfNextOrPreLine(int& nParaIdx, int& nLineIdx, BOOL bNextLine)
{
	int nParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	if (nParaIdx >= 0 && nParaIdx < nParaCnt)
	{
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
		int nLineCnt = pParagraph->GetLineCount();
		if (nLineIdx >= 0 && nLineIdx < nLineCnt)
		{
			int nNewLine = bNextLine ? nLineIdx + 1 : nLineIdx - 1;
			if (nNewLine >=0 && nNewLine < nLineCnt)
			{
				//当前段下一行/上一行
				nLineIdx = nNewLine;
				return;
			}
		}

		//有下一段/上一段？
		int nNewPara = bNextLine ? nParaIdx + 1 : nParaIdx - 1;
		if (nNewPara >= 0 && nNewPara < nParaCnt)
		{
			pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nNewPara);
			nLineCnt = pParagraph->GetLineCount();
			if (nLineCnt > 0)
			{
				//下一段/上一段第一行/最后一行
				nParaIdx = nNewPara;
				nLineIdx = bNextLine ? 0 : nLineCnt - 1;
				return;
			}
		}
	}

	nParaIdx = -1;
	nLineIdx = -1;
}

BOOL CBaseTypeset::IsCharPosAtLineEnd(BOOL nCharIndex)
{
	BaseARray* parPara = &m_pTextObj->arParagraph;
	int nParaCnt = BaseAR_GetCount(parPara);
	for (int i = nParaCnt-1; i >= 0; --i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(parPara, i);
		if (nCharIndex >= pParaObj->nBeginIndex)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
			if (pParagraph->IsCharPosAtLineEnd(nCharIndex))
			{
				return TRUE;
			}
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CBaseTypeset::GetCaretGraphByCharPos(int nCharIndex, 
										BOOL bIsLineEndPos, 
										BOOL bFullHeight, 
										vectorgraph* pGraph)
{
	BaseGraphGlyphSpace objCaretInfo;
	if (GetCaretInfoByCharPos(&objCaretInfo, nCharIndex, bIsLineEndPos, bFullHeight, 3))
	{
		point2f pt[4];
		for (int i = 0; i < 4; ++i)
		{
			pt[i].x = objCaretInfo.ptBound[i].x;
			pt[i].y = objCaretInfo.ptBound[i].y;
		}
		CreateRegionVectorGraphFromPoints(pGraph, pt, 4, FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseTypeset::GetSelGraphByCharPos(int nBeginIndex, 
									  int nEndIndex, 
									  vectorgraph* pGraph)
{
	BaseARray arCellRect;
	BaseAR_Init(&arCellRect);
	if (!GetSelSpaceInfoByCharPos(nBeginIndex, nEndIndex, &arCellRect))
	{
		//删除残余
		BaseGraphGlyphSpace_ReleaseAll(arCellRect);
		return FALSE;
	}

	//选区可能包含多行，每行又有可能有多个Run，所以分成多个cell。
	LPCommonGlyphSpace pGlyphSpace;
	int nCount;
	nCount = BaseAR_GetCount(&arCellRect);
	pGraph->ncell = nCount;
	pGraph->cell = new vectorcell[nCount];

	if (IsLinearTypeset())		//线性排版
	{
		for(int i = 0; i < nCount; ++i)
		{
			pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&arCellRect, i);

			vectorcell& pvCell = pGraph->cell[i];
			pvCell.width = 1;
			pvCell.type = close_anticlockwise_region;
			pvCell.npoint = BaseGraphGLYPHSPACEPTs;
			pvCell.pttype = new unsigned long[BaseGraphGLYPHSPACEPTs];
			pvCell.point = new point2f[BaseGraphGLYPHSPACEPTs];

			for (int nP = 0; nP < BaseGraphGLYPHSPACEPTs; ++nP)
			{
				pvCell.pttype[nP] = point_line;
				pvCell.point[nP].x = pGlyphSpace->ptBound[nP].x;
				pvCell.point[nP].y = pGlyphSpace->ptBound[nP].y;
			}

			if (pvCell.point[0].x <= pvCell.point[1].x)
			{
				pvCell.bound.left = pvCell.point[0].x;
				pvCell.bound.right = pvCell.point[1].x;
			}
			else
			{
				pvCell.bound.left = pvCell.point[1].x;
				pvCell.bound.right = pvCell.point[0].x;
			}
			if (pvCell.point[0].y <= pvCell.point[3].y)
			{
				pvCell.bound.top = pvCell.point[0].y;
				pvCell.bound.bottom = pvCell.point[3].y;
			}
			else
			{
				pvCell.bound.top = pvCell.point[3].y;
				pvCell.bound.bottom = pvCell.point[0].y;
			}

			pGraph->bound = pvCell.bound;
			delete pGlyphSpace;
		}
	}
	else
	{
		if (IsHorizontal())		//构造横排选中区域
		{
			for(int i = 0; i < nCount; ++i)
			{
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&arCellRect, i);

				vectorcell& pvCell = pGraph->cell[i];
				pvCell.width = 1;
				pvCell.type = close_anticlockwise_region;
				pvCell.npoint = BaseGraphGLYPHSPACEPTs_EX;
				pvCell.pttype = new unsigned long[BaseGraphGLYPHSPACEPTs_EX];
				pvCell.point = new point2f[BaseGraphGLYPHSPACEPTs_EX];

				pvCell.pttype[0] = point_line;
				pvCell.point[0].x = pGlyphSpace->ptBound[0].x;
				pvCell.point[0].y = pGlyphSpace->ptBound[0].y;

				pvCell.pttype[1] = point_line;
				pvCell.point[1].x = pGlyphSpace->ptBound[1].x;
				pvCell.point[1].y = pGlyphSpace->ptBound[1].y;

				pvCell.pttype[2] = point_line;
				pvCell.point[2].x = pGlyphSpace->ptBound[4].x;
				pvCell.point[2].y = pGlyphSpace->ptBound[4].y;

				pvCell.pttype[3] = point_line;
				pvCell.point[3].x = pGlyphSpace->ptBound[5].x;
				pvCell.point[3].y = pGlyphSpace->ptBound[5].y;

				pvCell.pttype[4] = point_line;
				pvCell.point[4].x = pGlyphSpace->ptBound[2].x;
				pvCell.point[4].y = pGlyphSpace->ptBound[2].y;

				pvCell.pttype[5] = point_line;
				pvCell.point[5].x = pGlyphSpace->ptBound[3].x;
				pvCell.point[5].y = pGlyphSpace->ptBound[3].y;

				if (pvCell.point[0].x <= pvCell.point[4].x)
				{
					pvCell.bound.left = pvCell.point[0].x;
					pvCell.bound.right = pvCell.point[4].x;
				}
				else
				{
					pvCell.bound.left = pvCell.point[4].x;
					pvCell.bound.right = pvCell.point[0].x;
				}
				if (pvCell.point[0].y <= pvCell.point[3].y)
				{
					pvCell.bound.top = pvCell.point[0].y;
					pvCell.bound.bottom = pvCell.point[3].y;
				}
				else
				{
					pvCell.bound.top = pvCell.point[3].y;
					pvCell.bound.bottom = pvCell.point[0].y;
				}

				pGraph->bound = pvCell.bound;
				delete pGlyphSpace;
			}
		}
		else					//构造竖排选中区域
		{
			for(int i = 0; i < nCount; ++i)
			{
				pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&arCellRect, i);

				vectorcell& pvCell = pGraph->cell[i];
				pvCell.width = 1;
				pvCell.type = close_anticlockwise_region;
				pvCell.npoint = BaseGraphGLYPHSPACEPTs_EX;
				pvCell.pttype = new unsigned long[BaseGraphGLYPHSPACEPTs_EX];
				pvCell.point = new point2f[BaseGraphGLYPHSPACEPTs_EX];

				pvCell.pttype[0] = point_line;
				pvCell.point[0].x = pGlyphSpace->ptBound[0].x;
				pvCell.point[0].y = pGlyphSpace->ptBound[0].y;

				pvCell.pttype[1] = point_line;
				pvCell.point[1].x = pGlyphSpace->ptBound[1].x;
				pvCell.point[1].y = pGlyphSpace->ptBound[1].y;

				pvCell.pttype[2] = point_line;
				pvCell.point[2].x = pGlyphSpace->ptBound[2].x;
				pvCell.point[2].y = pGlyphSpace->ptBound[2].y;

				pvCell.pttype[3] = point_line;
				pvCell.point[3].x = pGlyphSpace->ptBound[4].x;
				pvCell.point[3].y = pGlyphSpace->ptBound[4].y;

				pvCell.pttype[4] = point_line;
				pvCell.point[4].x = pGlyphSpace->ptBound[5].x;
				pvCell.point[4].y = pGlyphSpace->ptBound[5].y;

				pvCell.pttype[5] = point_line;
				pvCell.point[5].x = pGlyphSpace->ptBound[3].x;
				pvCell.point[5].y = pGlyphSpace->ptBound[3].y;

				if (pvCell.point[0].x <= pvCell.point[1].x)
				{
					pvCell.bound.left = pvCell.point[0].x;
					pvCell.bound.right = pvCell.point[1].x;
				}
				else
				{
					pvCell.bound.left = pvCell.point[1].x;
					pvCell.bound.right = pvCell.point[0].x;
				}
				if (pvCell.point[0].y <= pvCell.point[5].y)
				{
					pvCell.bound.top = pvCell.point[0].y;
					pvCell.bound.bottom = pvCell.point[5].y;
				}
				else
				{
					pvCell.bound.top = pvCell.point[5].y;
					pvCell.bound.bottom = pvCell.point[0].y;
				}

				pGraph->bound = pvCell.bound;
				delete pGlyphSpace;
			}
		}
	}

	BaseAR_RemoveAll(&arCellRect);
	return TRUE;
}

BOOL CBaseTypeset::GetSelSpaceGraphByCharPos(int nBeginIndex, 
										   int nEndIndex, 
										   vectorgraph* pGraph)
{
	BaseARray arCellRect;
	BaseAR_Init(&arCellRect);
	if (!GetSelSpaceSpaceInfoByCharPos(nBeginIndex, nEndIndex, &arCellRect))
	{
		//删除残余
		BaseGraphGlyphSpace_ReleaseAll(arCellRect);
		return FALSE;
	}

	int nTextLen = (int)_tcslen(m_pTextObj->szText);
	nEndIndex = min(nEndIndex, nTextLen);

	int nCount = BaseAR_GetCount(&arCellRect);
	if (nCount <= 0)
		return FALSE;

	pGraph->ncell = nCount;
	pGraph->cell = new vectorcell[nCount];

	float fEdgeX = 1;
	float fEdgeY = 1;
	if (m_pTextObj->bHorizontal)
		fEdgeX = 2;
	else
		fEdgeY = 2;

	LPCommonGlyphSpace pGlyphSpace;
	for(int i = 0; i < nCount; ++i)
	{
		pGlyphSpace = (LPCommonGlyphSpace)BaseAR_GetAt(&arCellRect, i);

		vectorcell& pvCell = pGraph->cell[i];
		pvCell.width = 1;
		pvCell.type = close_anticlockwise_region;
		pvCell.npoint = BaseGraphGLYPHSPACEPTs;
		pvCell.pttype = new unsigned long[pvCell.npoint];
		pvCell.point = new point2f[pvCell.npoint];

		//三角形
		pvCell.pttype[0] = point_line;
		pvCell.point[0].x = pGlyphSpace->ptBound[0].x + fEdgeX;
		pvCell.point[0].y = pGlyphSpace->ptBound[0].y + fEdgeY;
		pvCell.pttype[1] = point_line;
		pvCell.point[1].x = pGlyphSpace->ptBound[1].x - fEdgeX;
		pvCell.point[1].y = pGlyphSpace->ptBound[1].y + fEdgeY;
		pvCell.pttype[2] = point_line;
		pvCell.point[2].x = pGlyphSpace->ptBound[2].x - fEdgeX;
		pvCell.point[2].y = pGlyphSpace->ptBound[2].y - fEdgeY;
		pvCell.pttype[3] = point_line;
		pvCell.point[3].x = pGlyphSpace->ptBound[3].x + fEdgeX;
		pvCell.point[3].y = pGlyphSpace->ptBound[3].y - fEdgeY;

		if (pvCell.point[0].x <= pvCell.point[1].x)
		{
			pvCell.bound.left = pvCell.point[0].x;
			pvCell.bound.right = pvCell.point[1].x;
		}
		else
		{
			pvCell.bound.left = pvCell.point[1].x;
			pvCell.bound.right = pvCell.point[0].x;
		}
		if (pvCell.point[0].y <= pvCell.point[3].y)
		{
			pvCell.bound.top = pvCell.point[0].y;
			pvCell.bound.bottom = pvCell.point[3].y;
		}
		else
		{
			pvCell.bound.top = pvCell.point[3].y;
			pvCell.bound.bottom = pvCell.point[0].y;
		}

		pGraph->bound = pvCell.bound;
		delete pGlyphSpace;
	}

	BaseAR_RemoveAll(&arCellRect);
	return TRUE;
}

BOOL CBaseTypeset::GetCaretInfoByCharPos(LPCommonGlyphSpace pCaretInfo, 
									   int nCharIndex, 
									   BOOL bIsLineEndPos, 
									   BOOL bFullHeight, 
									   float fCaretHalfWidth)
{
	BOOL bLineEnd = bIsLineEndPos;
	//字符总数。
	int nStrLen = (int)_tcslen(m_pTextObj->szText);
	BOOL bLinearTypeset = IsLinearTypeset();

	BaseARray* parPara = &m_pTextObj->arParagraph;
	int nParaCnt = BaseAR_GetCount(parPara);
	for (int i = nParaCnt-1; i >= 0; --i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(parPara, i);
		if (nCharIndex >= pParaObj->nBeginIndex || nCharIndex >= nStrLen)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
			if (pParagraph->GetCaretInfoByCharPos(pCaretInfo, nCharIndex, bLineEnd, FALSE, bLinearTypeset, fCaretHalfWidth))
			{
				return TRUE;
			}
			memset(pCaretInfo, 0, sizeof(BaseGraphGlyphSpace));
			return FALSE;
		}
	}
	memset(pCaretInfo, 0, sizeof(BaseGraphGlyphSpace));
	return FALSE;
}

BOOL CBaseTypeset::GetCaretInfoByGlyphIndex(LPCommonGlyphSpace pCaretInfo, 
										  int nParaIdx, 
										  int nIndex, 
										  BOOL bIsLineEndPos, 
										  BOOL bFullHeight, 
										  float fCaretHalfWidth)
{
	//bFullHeight：光标矩形是否取全高（包含行距等附加高度）。
	BOOL bLineEnd = bIsLineEndPos;

	CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, nParaIdx);
	if (pParagraph->GetCaretInfoByGlyphIndex(pCaretInfo, nIndex, bLineEnd, bFullHeight, IsLinearTypeset(), fCaretHalfWidth))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseTypeset::GetSelSpaceInfoByCharPos(int nBeginIndex, 
										  int nEndIndex, 
										  BaseARray* pArInfo)
{
	BOOL bLinearTypeset = IsLinearTypeset();
	int nParaCnt = BaseAR_GetCount(&m_pTypesetPrivate->m_arParagraph);
	for (int i = 0; i < nParaCnt; ++i)
	{
		CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
		pParagraph->GetSelSpaceInfoByCharPos(
			nBeginIndex, 
			nEndIndex, 
			pArInfo, 
			bLinearTypeset
			);
	}
	return TRUE;
}

BOOL CBaseTypeset::GetSelSpaceSpaceInfoByCharPos(int nBeginIndex, 
											   int nEndIndex, 
											   BaseARray* pArInfo)
{
	BOOL bRet = FALSE;
	BaseGraphGlyphSpace objSpaceInfo;
	int nTextLen = (int)_tcslen(m_pTextObj->szText);
	nEndIndex = min(nEndIndex, nTextLen);

	for (int nC = nBeginIndex; nC < nEndIndex; ++nC)
	{
		BOOL bDBCCase = TRUE;
		if (m_pTextObj->szText[nC] == _T(' '))
		{
			//半角空格
			if (!GetCharPosSpaceInfo(nC, &objSpaceInfo))
				continue;
		}
		else if (m_pTextObj->szText[nC] == _T('　'))
		{
			//全角空格
			if (!GetCharPosSpaceInfo(nC, &objSpaceInfo))
				continue;
			bDBCCase = FALSE;
		}
		else
			continue;

		LPCommonGlyphSpace pGlyphSpace = new BaseGraphGlyphSpace;
		memcpy(pGlyphSpace, &objSpaceInfo, sizeof(BaseGraphGlyphSpace));
		BaseAR_Add(pArInfo, pGlyphSpace);
		if (bDBCCase)
		{
			if (m_pTextObj->bHorizontal)
			{
				pGlyphSpace->ptBound[0].y = pGlyphSpace->ptBound[3].y - (pGlyphSpace->ptBound[3].y - pGlyphSpace->ptBound[0].y) / 2;
				pGlyphSpace->ptBound[1].y = pGlyphSpace->ptBound[2].y - (pGlyphSpace->ptBound[2].y - pGlyphSpace->ptBound[1].y) / 2;
			}
			else
			{
				pGlyphSpace->ptBound[1].x = pGlyphSpace->ptBound[0].x + (pGlyphSpace->ptBound[1].x - pGlyphSpace->ptBound[0].x) / 2;
				pGlyphSpace->ptBound[2].x = pGlyphSpace->ptBound[3].x + (pGlyphSpace->ptBound[2].x - pGlyphSpace->ptBound[3].x) / 2;
			}
		}
		bRet = TRUE;
	}
	return bRet;
}

int CBaseTypeset::GetEndCaretCharPos()
{
	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	return nTextLength;
}

BOOL CBaseTypeset::GetCharPosSpaceInfo(int nCharIndex, LPCommonGlyphSpace pSpaceInfo)
{
	//字符总数。
	int nStrLen = (int)_tcslen(m_pTextObj->szText);

	BaseARray* parPara = &m_pTextObj->arParagraph;
	int nParaCnt = BaseAR_GetCount(parPara);
	for (int i = nParaCnt-1; i >= 0; --i)
	{
		LPCommonTextParagraph pParaObj = (LPCommonTextParagraph)BaseAR_GetAt(parPara, i);
		if (nCharIndex >= pParaObj->nBeginIndex || nCharIndex >= nStrLen)
		{
			CBaseTextParagraph* pParagraph = (CBaseTextParagraph*)BaseAR_GetAt(&m_pTypesetPrivate->m_arParagraph, i);
			if (pParagraph != NULL && pParagraph->GetCharPosSpaceInfo(nCharIndex, pSpaceInfo))
			{
				return TRUE;
			}
			memset(pSpaceInfo, 0, sizeof(BaseGraphGlyphSpace));
			return FALSE;
		}
	}
	memset(pSpaceInfo, 0, sizeof(BaseGraphGlyphSpace));
	return FALSE;
}

BOOL CBaseTypeset::SelectAllChar(int& nBeginIndex, int& nEndIndex)
{
	nBeginIndex = 0;
	int nTextLen = (int)_tcslen(m_pTextObj->szText);
	nEndIndex = nTextLen;
	return TRUE;
}

BOOL CBaseTypeset::ProcessDirectionKey(UINT uKey, 
									 int& nBeginIndex, 
									 int& nEndIndex,
									 BOOL& bIsLineEndPos,
									 BOOL bPressShift, 
									 BOOL bPressCtrl, 
									 LPVOID lpParam)
{
	//排版方式不同，方向键意义也随之不同。
	switch (uKey)
	{
	case VK_LEFT:
		{
			if (m_pTextObj->bHorizontal)
			{
// 				if (m_pTextObj->bLeftInput)
					return MoveCaretPre(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
// 				else
// 					return MoveCaretNext(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
			}
			else
			{
// 				if (m_pTextObj->bLeftInput)
					return MoveCaretPreLine(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
// 				else
// 					return MoveCaretNextLine(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
			}
		}
	case VK_RIGHT:
		{
			if (m_pTextObj->bHorizontal)
			{
// 				if (m_pTextObj->bLeftInput)
					return MoveCaretNext(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
// 				else
// 					return MoveCaretPre(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
			}
			else
			{
// 				if (m_pTextObj->bLeftInput)
					return MoveCaretNextLine(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
// 				else
// 					return MoveCaretPreLine(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
			}
		}
	case VK_UP:
		{
			if (m_pTextObj->bHorizontal)
				return MoveCaretPreLine(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
			else
				return MoveCaretPre(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
		}
	case VK_DOWN:
		{
			if (m_pTextObj->bHorizontal)
				return MoveCaretNextLine(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
			else
				return MoveCaretNext(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
		}
	case VK_HOME:
		{
			return MoveCaretToHome(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
		}
	case VK_END:
		{
			return MoveCaretToEnd(nBeginIndex, nEndIndex, bIsLineEndPos, bPressShift, bPressCtrl);
		}
	}

	return FALSE;
}

BOOL CBaseTypeset::MoveCaretNext(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl)
{
	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	if (nEndIndex >= 0 && BaseGraphISCRLF(m_pTextObj->szText, nEndIndex, nTextLength))
		nEndIndex += 2;	//确保掠过一个完整的换行符
	else
		++nEndIndex;

	if (nEndIndex > nTextLength)
		nEndIndex = nTextLength;

	if (!bPressShift)
		nBeginIndex = nEndIndex;

	bIsLineEndPos = IsCharPosAtLineEnd(nEndIndex);
	return TRUE;
}

BOOL CBaseTypeset::MoveCaretPre(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl)
{
	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	if (nEndIndex >= 2 && BaseGraphISCRLF(m_pTextObj->szText, nEndIndex-2, nTextLength))
		nEndIndex -= 2;	//确保掠过一个完整的换行符
	else
		--nEndIndex;

	if (nEndIndex < 0)
		nEndIndex = 0;

	if (!bPressShift)
		nBeginIndex = nEndIndex;

	bIsLineEndPos = IsCharPosAtLineEnd(nEndIndex);
	return TRUE;
}

BOOL CBaseTypeset::MoveCaretPreLine(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl)
{
	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	BOOL bLineEnd = bIsLineEndPos;

	BaseGraphGlyphSpace objGlyphSpace;
	if (!GetCaretInfoByCharPos(&objGlyphSpace, nEndIndex, bLineEnd, FALSE, 0))
		return FALSE;
	int nParaIdx;
	int nLineIdx;
	ConvertCharPosToParaLineIdx(nParaIdx, nLineIdx, nEndIndex, bLineEnd);
	GetParaLineIdxOfNextOrPreLine(nParaIdx, nLineIdx, FALSE);
	if (nParaIdx < 0 || nLineIdx < 0)
		return FALSE;

	point2f ptTest;
	if (m_pTextObj->bHorizontal)
	{
		ptTest.x = objGlyphSpace.ptBound[0].x;
		ptTest.y = objGlyphSpace.ptBound[0].y - 4;
	}
	else
	{
		if (m_pTextObj->bLeftInput)
		{
			ptTest.x = objGlyphSpace.ptBound[0].x - 4;
			ptTest.y = objGlyphSpace.ptBound[0].y;
		}
		else
		{
			ptTest.x = objGlyphSpace.ptBound[1].x + 4;
			ptTest.y = objGlyphSpace.ptBound[1].y;
		}
	}

	int nNewIndex = GetCharPosFromPoint(bIsLineEndPos, ptTest, nParaIdx, nLineIdx, TRUE);
	if (nNewIndex < 0 || nNewIndex > nTextLength)
		return FALSE;

	nEndIndex = nNewIndex;

	if (!bPressShift)
		nBeginIndex = nEndIndex;

	return TRUE;
}

BOOL CBaseTypeset::MoveCaretNextLine(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl)
{
	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	BOOL bLineEnd = bIsLineEndPos;

	BaseGraphGlyphSpace objGlyphSpace;
	if (!GetCaretInfoByCharPos(&objGlyphSpace, nEndIndex, bLineEnd, FALSE, 0))
		return FALSE;
	int nParaIdx;
	int nLineIdx;
	ConvertCharPosToParaLineIdx(nParaIdx, nLineIdx, nEndIndex, bLineEnd);
	GetParaLineIdxOfNextOrPreLine(nParaIdx, nLineIdx, TRUE);
	if (nParaIdx < 0 || nLineIdx < 0)
		return FALSE;

	point2f ptTest;
	if (m_pTextObj->bHorizontal)
	{
		ptTest.x = objGlyphSpace.ptBound[3].x;
		ptTest.y = objGlyphSpace.ptBound[3].y + 4;
	}
	else
	{
		if (m_pTextObj->bLeftInput)
		{
			ptTest.x = objGlyphSpace.ptBound[1].x + 4;
			ptTest.y = objGlyphSpace.ptBound[1].y;
		}
		else
		{
			ptTest.x = objGlyphSpace.ptBound[0].x - 4;
			ptTest.y = objGlyphSpace.ptBound[0].y;
		}
	}

	int nNewIndex = GetCharPosFromPoint(bIsLineEndPos, ptTest, nParaIdx, nLineIdx, TRUE);
	if (nNewIndex < 0 || nNewIndex > nTextLength)
		return FALSE;

	nEndIndex = nNewIndex;
	if (!bPressShift)
		nBeginIndex = nEndIndex;

	return TRUE;
}

BOOL CBaseTypeset::MoveCaretToHome(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl)
{
	nEndIndex = 0;
	if (!bPressShift)
		nBeginIndex = nEndIndex;

	bIsLineEndPos = FALSE;
	return TRUE;
}

BOOL CBaseTypeset::MoveCaretToEnd(int& nBeginIndex, int& nEndIndex, BOOL& bIsLineEndPos, BOOL bPressShift, BOOL bPressCtrl)
{
	int nTextLength = (int)_tcslen(m_pTextObj->szText);
	nEndIndex = nTextLength;
	if (!bPressShift)
		nBeginIndex = nEndIndex;

	bIsLineEndPos = TRUE;
	return TRUE;
}

void CBaseTypeset::OnLock()
{
	if( g_TypesetInterface.Lock != NULL )
		g_TypesetInterface.Lock(m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnUnLock()
{
	if( g_TypesetInterface.UnLock != NULL )
		g_TypesetInterface.UnLock(m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnAddUnit(BOOL bKeepBalance)
{
	if( g_TypesetInterface.OnAddUnit != NULL )
		g_TypesetInterface.OnAddUnit(bKeepBalance, m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnInsertUnit(int nIndex, int nCount, BOOL bKeepBalance)
{
	if( g_TypesetInterface.OnInsertUnit != NULL )
		g_TypesetInterface.OnInsertUnit(nIndex, nCount, bKeepBalance, m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnDeleteUnit(int nIndex, int nCount, BOOL bKeepBalance)
{
	if( g_TypesetInterface.OnDeleteUnit != NULL )
		g_TypesetInterface.OnDeleteUnit(nIndex, nCount, bKeepBalance, m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnDeleteAllUnit()
{
	if( g_TypesetInterface.OnDeleteAllUnit != NULL )
		g_TypesetInterface.OnDeleteAllUnit(m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnRefreshUnit(int nIndex, int nCount)
{
	if( g_TypesetInterface.OnRefreshUnit != NULL )
		g_TypesetInterface.OnRefreshUnit(nIndex, nCount, m_pTypesetPrivate->lpParam);
}

void CBaseTypeset::OnRefreshAllUnit(int nLevel)
{
	if( g_TypesetInterface.OnRefreshAllUnit != NULL )
		g_TypesetInterface.OnRefreshAllUnit(m_pTypesetPrivate->lpParam, nLevel);
}


//////////////////////////////////////////////////////////////////////////
// API

int BaseGraphTypeset_GetParagraphCharCount(LPCommonText pTextObj, int nParagIdx)
{
	int nParaLen;

	BaseARray* paryPara = &pTextObj->arParagraph;
	int nParaCnt = BaseAR_GetCount(paryPara);
	LPCommonTextParagraph pParagObj = (LPCommonTextParagraph)BaseAR_GetAt(paryPara, nParagIdx);
	if (nParagIdx < nParaCnt-1)
		nParaLen = ((LPCommonTextParagraph)BaseAR_GetAt(paryPara, nParagIdx+1))->nBeginIndex;
	else
		nParaLen = (int)_tcslen(pTextObj->szText);
	nParaLen -= pParagObj->nBeginIndex;

	return nParaLen;
}

BaseGraphUnit* BaseGraphTypeset_GetUnitByWordInfo(LPCommonText pTextObj, int nWordInfoIdx)
{
	if (nWordInfoIdx >= 0 && nWordInfoIdx < BaseAR_GetCount(&pTextObj->ar2DUnit))
	{
		return (BaseGraphUnit*)BaseAR_GetAt(&pTextObj->ar2DUnit, nWordInfoIdx);
	}
	return NULL;
}

void BaseGraphTypeset_GetTypesetRect(LPCommonText pTextObj, rectf& rtTypeset, BOOL bReCal)
{
	if (bReCal)
	{
		rtTypeset = pTextObj->rtBorder;
		if (BaseRectf_IsRectEmpty(&rtTypeset))
		{
			memset(&rtTypeset, 0, sizeof(rectf));
		}
	}
	else
	{
		LPCommonTypesetPrivate pTypesetPrivate = (LPCommonTypesetPrivate)pTextObj->pTypesetPrivate;
		rtTypeset = pTypesetPrivate->rtTypesetBorder;
	}
}

BaseARray* BaseGraphTypeset_GetTextParagraphArray(LPCommonText pTextObj)
{
	LPCommonTypesetPrivate pTypesetPrivate = (LPCommonTypesetPrivate)pTextObj->pTypesetPrivate;
	return &pTypesetPrivate->m_arParagraph;
}

void BaseGraphTypeset_GenerateWordInfo(LPCommonText pTextObj, LPCommonWordInfo pWordInfo, TCHAR ch, int nFontIndex)
{
	//用指定索引位置的字取ch的字模信息。
	//pWordInfo由主调者申请维护
	BaseGraphWordInfoInit(pWordInfo);
	pWordInfo->wChar = ch;

	BaseGraphFont font;
	LPCommonFont pFont;
	pFont = BaseGraphTextProp_GetFont(&pTextObj->stuTextProp, nFontIndex);
	if (pFont != NULL)
	{
		BaseGraphFont_Copy(&font, pFont);
	}
	else
	{
		BaseGraphFont_Init(&font);
	}

	if (pFont != NULL)
	{
		pWordInfo->bVisible = (bool)BaseGraphIsVisibleChar(pWordInfo->wChar);
		pWordInfo->bGlyphIndex = false;
		pWordInfo->fDistance = pFont->lfDistance;

		BaseGraphWordInfoFill(NULL, pWordInfo, NULL, pFont, pTextObj->bHorizontal);
		if (pWordInfo->wChar == VK_TAB)
		{
			// Tab只占空位置，没有字形
			common_release_vectorgraph(&pWordInfo->vgGraph);
		}
	}
}
