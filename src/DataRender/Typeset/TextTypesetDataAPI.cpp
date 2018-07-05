#include "stdafx.h"
#include "TextTypesetDataAPI.h"
//#include "ActionOperator.h"
#include "TextTypeset.h"
#include "TextParagraph.h"
#include "DataRender\DataRenderUtityFunc.h"
#include "..\ObjectCalculator.h"
#include "..\CalculatorTitleWord.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//CRC32翻转表
unsigned int g_uCRC32Table[256] = 
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

//////////////////////////////////////////////////////////////////////////
// SYText
 void SYText_Init(LPSYText pText)
{
	if (pText != NULL)
	{
		pText->dwVersion = 3;
		pText->szText = NULL;
		memset(&pText->rtBorder, 0, sizeof(rectf));
		memset(&pText->rtTypesetReal, 0, sizeof(rectf));
		memset(&pText->rtFixedFrame, 0, sizeof(rectf));
		pText->bUseBorderEditMode = FALSE;
		pText->bOneClusterOneUnit = TRUE;
		pText->bHorizontal = TRUE;
		pText->bLeftInput = TRUE;
		pText->bVLeftTypeset = TRUE;
		pText->bLineWrap = TRUE;
		pText->bFixedFrame = FALSE;
		pText->eAlignVertical = TEXT_ALIGN_V_TOP;

		pText->nSmallFontFactor = 3;
		pText->nSmallFontQuality = PI_Quality_LOW;
		pText->bSmallFontCalcBorder = FALSE;

		SYAR_Init(&pText->arParagraph);
		SYTextProp_Init(&pText->stuTextProp);
		SYAR_Init(&pText->ar2DUnit);
		SYAR_Init(&pText->ar3DUnit);

		pText->vGraphScale.x = 1.0;
		pText->vGraphScale.y = 1.0;

		pText->nLinesLimit = -1;
		pText->bHideBorderCrosserC = FALSE;
		pText->bHideBorderCrosserL = FALSE;

		pText->pStretch = NULL;
		pText->pSpecialEffect = NULL;
		pText->nTextUnitCnt = 0;
		pText->nUnderlineUnitCnt = 0;
		pText->nSpecialUnitCnt = 0;

		pText->wTempFlag = 0;
		pText->pTypesetPrivate = NULL;
		pText->hHandle = NULL;
		pText->lpParam = NULL;

		pText->pArtWord = NULL;
		pText->pTypesetPath = NULL;
	}
}

 void SYText_Create(LPSYText pText, 
	LPCTSTR szText, 
	rectf rtBorder,
	LPSYFont pFont, 
	LPSYStaticEffect pFillColor, 
	SYMaterial* p3DColor, 
	LPSYUnderLine pUnderLine, 
	E_TextAlignHorizontal alignH, 
	E_TextAlignVeritical alignV, 
	BOOL bHorizontal, 
	BOOL bLeftInput, 
	BOOL bVLeftTypeset, 
	BOOL bLineWrap, 
	BOOL bFixedFrame)
{
	//pText已经Init！
	if (pText == NULL)
		return;

	int nTextLen = 0;
	if (szText != NULL)
	{
		if (pText->szText != NULL)
			delete[] pText->szText;

		nTextLen = (int)_tcslen(szText);
		pText->szText = new TCHAR[nTextLen + 1];
		_tcscpy_s(pText->szText, nTextLen + 1, szText);
	}

	//TypesetReal与Border初始相同
	pText->rtBorder = rtBorder;
	pText->rtTypesetReal = rtBorder;
	pText->rtFixedFrame = rtBorder;
	pText->bHorizontal = bHorizontal;
	pText->bLeftInput = bLeftInput;
	pText->bVLeftTypeset = /*bVLeftTypeset*/(!bHorizontal && !bLeftInput) ? FALSE : TRUE;
	pText->bLineWrap = bLineWrap;
	pText->bFixedFrame = bFixedFrame;
	pText->eAlignVertical = alignV;

	SYTextProp_Create(nTextLen, &pText->stuTextProp, pFont, pFillColor, p3DColor, pUnderLine);
	SYText_InitParagraphArray(pText, alignH);
}

 void SYText_Release(LPSYText pText)
{
	if (pText == NULL)
		return;

	if (pText->szText != NULL)
	{
		delete[] pText->szText;
		pText->szText = NULL;
	}

	SYTextProp_Release(&pText->stuTextProp);

	int nParaCnt = SYAR_GetCount(&pText->arParagraph);
	for (int i = 0; i < nParaCnt; ++i)
	{
		LPSYTextParagraph pParagraph = (LPSYTextParagraph)SYAR_GetAt(&pText->arParagraph, i);
		SYText_ReleaseParagraph(pParagraph);
		delete pParagraph;
	}
	SYAR_RemoveAll(&pText->arParagraph);

	int n2DUnitCnt = SYAR_GetCount(&pText->ar2DUnit);
	for (int i = 0; i < n2DUnitCnt; ++i)
	{
		SYUnit* p2DUnit = (SYUnit*)SYAR_GetAt(&pText->ar2DUnit, i);
		sy_release_unit(p2DUnit);
		delete p2DUnit;
	}
	SYAR_RemoveAll(&pText->ar2DUnit);

	int n3DUnitCnt = SYAR_GetCount(&pText->ar3DUnit);
	for (int i = 0; i < n3DUnitCnt; ++i)
	{
		SYUnit* p3DUnit = (SYUnit*)SYAR_GetAt(&pText->ar3DUnit, i);
		sy_release_unit(p3DUnit);
		delete p3DUnit;
	}
	SYAR_RemoveAll(&pText->ar3DUnit);

	if (pText->pStretch != NULL)
	{
		delete pText->pStretch;
		pText->pStretch = NULL;
	}

	pText->pSpecialEffect = NULL;

	if (pText->pTypesetPrivate != NULL)
	{
		SYTypesetPrivateData_Release((LPSYTypesetPrivate)(pText->pTypesetPrivate));
		delete pText->pTypesetPrivate;
		pText->pTypesetPrivate = NULL;
	}

	//释放艺术字
	if (pText->pArtWord != NULL)
	{
		SYArtTypeset_Release(pText->pArtWord);
		delete pText->pArtWord;
		pText->pArtWord = NULL;
	}
	//释放排版路径
	if (pText->pTypesetPath != NULL)
	{
		SYArtTypeset_Release(pText->pTypesetPath);
		delete pText->pTypesetPath;
		pText->pTypesetPath = NULL;
	}
}

 void SYText_Copy(LPSYText pDes, LPCSYText pSrc, BOOL bCopyNullText)
{
	if (pDes == NULL || pSrc == NULL || pSrc == pDes)
		return;

	SYText_Release(pDes);

	//没有文字时的处理
	if (pSrc->szText != NULL)
	{
		size_t nTextLength = _tcslen(pSrc->szText);
		pDes->szText = new TCHAR[nTextLength + 1];
		_tcscpy_s(pDes->szText, nTextLength + 1, pSrc->szText);

		bCopyNullText = TRUE;
	}

	if (bCopyNullText)
	{
		pDes->dwVersion = pSrc->dwVersion;
		pDes->rtBorder = pSrc->rtBorder;
		pDes->rtFixedFrame = pSrc->rtFixedFrame;
		pDes->bUseBorderEditMode = pSrc->bUseBorderEditMode;
		pDes->bOneClusterOneUnit = pSrc->bOneClusterOneUnit;
		pDes->bHorizontal = pSrc->bHorizontal;
		pDes->bLeftInput = pSrc->bLeftInput;
		pDes->bVLeftTypeset = pSrc->bVLeftTypeset;
		pDes->bLineWrap = pSrc->bLineWrap;
		pDes->bFixedFrame = pSrc->bFixedFrame;
		pDes->eAlignVertical = pSrc->eAlignVertical;
		pDes->bPageMode = pSrc->bPageMode;

		pDes->nSmallFontFactor = pSrc->nSmallFontFactor;
		pDes->nSmallFontQuality = pSrc->nSmallFontQuality;
		pDes->bSmallFontCalcBorder = pSrc->bSmallFontCalcBorder;

		pDes->vGraphScale = pSrc->vGraphScale;

		pDes->nLinesLimit = pSrc->nLinesLimit;
		pDes->bHideBorderCrosserC = pSrc->bHideBorderCrosserC;
		pDes->bHideBorderCrosserL = pSrc->bHideBorderCrosserL;

		pDes->nTextUnitCnt = pSrc->nTextUnitCnt;
		pDes->nUnderlineUnitCnt = pSrc->nUnderlineUnitCnt;
		pDes->nSpecialUnitCnt = pSrc->nSpecialUnitCnt;

		if (pSrc->pStretch != NULL)
		{
			pDes->pStretch = new SYStretchParam;
			memcpy(pDes->pStretch, pSrc->pStretch, sizeof(SYStretchParam));
		}

		//特殊效果
		sy_special_effect_copy(pDes->pSpecialEffect, pSrc->pSpecialEffect);

		SYTextProp_Copy(&pDes->stuTextProp, &pSrc->stuTextProp);

		int nParaCnt = SYAR_GetCount(&pSrc->arParagraph);
		for (int i = 0; i < nParaCnt; i++)
		{
			LPSYTextParagraph pParaSrc = (LPSYTextParagraph)SYAR_GetAt(&pSrc->arParagraph, i);
			LPSYTextParagraph pParaDes = new SYTextParagraph;
			SYText_InitParagraph(pParaDes);
			SYText_CopyParagraph(pParaDes, pParaSrc);
			SYAR_Add(&pDes->arParagraph, pParaDes);
		}

		//艺术字拷贝
		if (pSrc->pArtWord != NULL)
		{
			pDes->pArtWord = new SYArtTypeset;
			SYArtTypeset_Init(pDes->pArtWord);
			SYArtTypeset_Copy(pDes->pArtWord, pSrc->pArtWord);
		}

		//排版路径
		if (pSrc->pTypesetPath != NULL)
		{
			pDes->pTypesetPath = new SYArtTypeset;
			SYArtTypeset_Init(pDes->pTypesetPath);
			SYArtTypeset_Copy(pDes->pTypesetPath, pSrc->pTypesetPath);
		}

		//临时数据
		pDes->wTempFlag = pSrc->wTempFlag;
	}
}

 BOOL SYText_Compare(LPCSYText pText1, LPCSYText pText2, BOOL bNullTextEq)
{
	if (pText1 == NULL || pText2 == NULL)
		return FALSE;

	if (pText1 != pText2)
	{
		if (pText1->szText == NULL && pText2->szText == NULL)
		{
			//允许NULL时为等
			if (bNullTextEq)
			{
				return TRUE;
			}
		}
		else if ((pText1->szText != NULL && pText2->szText == NULL) 
			|| (pText2->szText != NULL && pText1->szText == NULL)
			)
		{
			//仅有一个为NULL，则不等。
			return FALSE;
		}

		if (!bNullTextEq && _tcscmp(pText1->szText, pText2->szText) != 0)
			return FALSE;

		if (pText1->dwVersion != pText2->dwVersion 
			|| pText1->bHorizontal != pText2->bHorizontal
			|| pText1->bLeftInput != pText2->bLeftInput
			|| pText1->bVLeftTypeset != pText2->bVLeftTypeset
			|| pText1->bLineWrap != pText2->bLineWrap
			|| pText1->bFixedFrame != pText2->bFixedFrame
			|| pText2->eAlignVertical != pText2->eAlignVertical
			|| !SYRectf_EqualRect((rectf*)(&pText1->rtBorder), (rectf*)(&pText2->rtBorder))
			|| !SYRectf_EqualRect((rectf*)(&pText1->rtFixedFrame), (rectf*)(&pText2->rtFixedFrame))
			)
			return FALSE;

		int nParaCnt1 = SYAR_GetCount(&pText1->arParagraph);
		int nParaCnt2 = SYAR_GetCount(&pText2->arParagraph);
		if (nParaCnt1 != nParaCnt2)
			return FALSE;
		for (int i = 0; i < nParaCnt1; i++)
		{
			LPSYTextParagraph pPara1 = (LPSYTextParagraph)SYAR_GetAt(&pText1->arParagraph, i);
			LPSYTextParagraph pPara2 = (LPSYTextParagraph)SYAR_GetAt(&pText2->arParagraph, i);
			if (!SYText_CompareParagraph(pPara1, pPara2))
				return FALSE;
		}

		//属性比较
		if (!SYTextProp_Compare(&pText1->stuTextProp, &pText2->stuTextProp))
		{
			return FALSE;
		}

		//艺术字
		if (!SYArtTypeset_Compare(pText1->pArtWord, pText2->pArtWord))
		{
			return FALSE;
		}

		//排版路径
		if (!SYArtTypeset_Compare(pText1->pTypesetPath, pText2->pTypesetPath))
		{
			return FALSE;
		}
	}

	return TRUE;
}

 void SYText_CopyProp(LPSYTextProp& pDesProp, LPCSYText pSrcText, int nDesLen/* = -1*/)
{
	if (pDesProp == NULL)
	{
		pDesProp = new SYTextProp;
		SYTextProp_Init(pDesProp);
	}
	SYTextProp_Copy(pDesProp, &pSrcText->stuTextProp, nDesLen);
}

 void SYText_UpdateStretchParam(LPSYText pText, LPSYStretchParam pStretch)
{
	if (pText != NULL && pStretch != NULL)
	{
		if (pStretch->bEnable && pStretch->fThick > ABS_FLOAT_0)
		{
			if (pText->pStretch == NULL)
			{
				pText->pStretch = new SYStretchParam;
			}
			memcpy(pText->pStretch, pStretch, sizeof(SYStretchParam));
		}
		else
		{
			SAFE_DELETE(pText->pStretch);
		}

		LPSYTextPropIndex pPropIdx;
		SYMaterial* p3DColor;

		LPSYTextProp pTextProp = &pText->stuTextProp;
		SYArray* paryIndex = &pTextProp->arIndex;
		int nPropCnt = SYAR_GetCount(paryIndex);
		if (pStretch != NULL && pStretch->bEnable && pStretch->fThick > ABS_FLOAT_0)
		{
			//3D拉伸
			for (int i = 0; i < nPropCnt; ++i)
			{
				pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, i);
				if (pPropIdx->p3DColor[0] == NULL)
				{
					LPSYStaticEffect p2DColor = (LPSYStaticEffect)pPropIdx->p2DColor;
					if (p2DColor != NULL)
					{
						//用2D色转3D色
						assert(false);
						DWORD faceColor = 0;//SYColor_GetRGBColor(&(p2DColor->face.color));
						p3DColor = new SYMaterial;
						SYMaterial_Init(p3DColor, faceColor);
						LPVOID pSame;
						int nSameID = SYTextProp_FindSame3DColor(&pTextProp->ar3DColor, p3DColor, &pSame);
						if (nSameID < 0)
						{
							SYAR_Add(&pTextProp->ar3DColor, p3DColor);
						}
						else
						{
							// 相同配置只用一个即可
							SYMaterial_Release(p3DColor);
							SAFE_DELETE(p3DColor);
							p3DColor = (SYMaterial*)pSame;
						}

						for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
							pPropIdx->p3DColor[n3D] = p3DColor;
					}
				}
			}
		}
		else
		{
			//非拉伸，取消3D属性。
			for (int i = 0; i < nPropCnt; ++i)
			{
				pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, i);
				for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
					pPropIdx->p3DColor[n3D] = NULL;
			}

			int nColorCnt = SYAR_GetCount(&pTextProp->ar3DColor);
			for (int i = 0; i < nColorCnt; ++i)
			{
				p3DColor = (SYMaterial*)SYAR_GetAt(&pTextProp->ar3DColor, i);
				SYMaterial_Release(p3DColor);
				delete p3DColor;
			}
			SYAR_RemoveAll(&pTextProp->ar3DColor);
		}
	}
}

 void SYText_SetWholeRender(LPSYText pText, BOOL bWholeRender)
{
	if (bWholeRender)
		pText->wTempFlag |= SYTEXT_TEMPFLAG_WHOLERENDER;
	else
		pText->wTempFlag &= ~SYTEXT_TEMPFLAG_WHOLERENDER;
}

 BOOL SYText_IsWholeRender(LPSYText pText)
{
	return ((pText->wTempFlag & SYTEXT_TEMPFLAG_WHOLERENDER) == SYTEXT_TEMPFLAG_WHOLERENDER);
}

 void SYText_SplitStringToParagraphArray(std::vector<int>& vParaIndex, LPCTSTR szText)
{
	//根据字符串拆分段落
	if (szText != NULL)
	{
		vParaIndex.push_back(0);
		int nTextLength = (int)_tcslen(szText);
		for (int i = 0; i < nTextLength; i++)
		{
			if (SYISCRLF(szText, i, nTextLength))
			{
				vParaIndex.push_back(i+2);
				++i;
			}
		}
	}
}

 void SYText_GetStaticEffectProperty(void* pProp, LPSYStaticEffect pFillColor, int nIndex)
{
	if (pProp != NULL && pFillColor != NULL)
	{
		LPSYTextProp pTextProp = (LPSYTextProp)pProp;
		LPSYTextPropIndex pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(&pTextProp->arIndex, nIndex);
		if (pPropIdx == NULL)
		{
			int nIdxCnt = SYAR_GetCount(&pTextProp->arIndex);
			pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(&pTextProp->arIndex, nIdxCnt - 1);
		}

		if (pPropIdx != NULL && pPropIdx->p2DColor != NULL)
		{
			SYStaticEffect_Copy(pFillColor, (LPSYStaticEffect)pPropIdx->p2DColor);
		}
	}
}

 void SYText_GetFontProperty(void* pProp, LPSYFont pFont, int nIndex)
{
	if (pProp != NULL && pFont != NULL)
	{
		LPSYTextProp pTextProp = (LPSYTextProp)pProp;
		LPSYTextPropIndex pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(&pTextProp->arIndex, nIndex);
		if (pPropIdx == NULL)
		{
			int nIdxCnt = SYAR_GetCount(&pTextProp->arIndex);
			pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(&pTextProp->arIndex, nIdxCnt - 1);
		}

		if (pPropIdx != NULL && pPropIdx->pFont != NULL)
		{
			SYFont_Copy(pFont, (LPSYFont)pPropIdx->pFont);
		}
	}
}
 unsigned long SYCRC32Cal(unsigned long ulCRC32, unsigned char *pBuf, unsigned long ulLen)
 {
	 if (ulLen > 0)
	 {
		 ulCRC32 = ~ulCRC32;
		 while (ulLen--)
		 {
			 ulCRC32 = g_uCRC32Table[(ulCRC32 ^ *pBuf++) & 0xFFL] ^ (ulCRC32 >> 8);
		 }
		 return (ulCRC32 ^ (~0L));
	 }
	 return ulCRC32;
 }
 void SYText_SetParagraphParam(LPSYText pTex, 
	int nStart, 
	int nCount, 
	E_TextPropertyType eType,
	LPVOID pPropertyData
	)
{
	int nEndIdx;
	int nParagCnt = SYAR_GetCount(&pTex->arParagraph);
	if (nStart < 0)
	{
		nStart = 0;
		nCount = nParagCnt;
	}
	nEndIdx = nStart + nCount - 1;
	if (nStart < nParagCnt && nEndIdx >= 0 && nEndIdx < nParagCnt)
	{
		switch (eType)
		{
		case TEXT_PROPERTY_TYPE_FONT_DISTANCE:
			{
				for (int i=nStart; i<=nEndIdx; ++i)
				{
					LPSYTextParagraph pParaObj = (LPSYTextParagraph)SYAR_GetAt(&pTex->arParagraph, i);
					pParaObj->fLineDistance = *((float*)pPropertyData);
				}
			}
			break;
		}
	}
}

 void SYText_ModifyTypesetBorderBySize(LPSYText pText, SYSizef& szRectSize)
{
	if (pText == NULL || (szRectSize.cx < SYTEXT_DIMENSION_MIN && szRectSize.cy < SYTEXT_DIMENSION_MIN))
		return;

	E_TextAlignHorizontal eAlignH = TEXT_ALIGN_H_LEFT;
	LPSYTextParagraph pPara = (LPSYTextParagraph)SYAR_GetAt(&pText->arParagraph, 0);
	if (pPara != NULL)
		eAlignH = pPara->eAlignHorizontal;

	float fTemp1, fTemp2, fAdjust, fCenter;
	rectf* prtBorder = &pText->rtBorder;
	int nAlignX = 0;
	int nAlignY = 0;
	if (pText->bHorizontal)
	{
		switch (eAlignH)
		{
		case TEXT_ALIGN_H_LEFT:
			break;
		case TEXT_ALIGN_H_RIGHT:
			nAlignX = 1;
			break;
		case TEXT_ALIGN_H_CENTER:
		case TEXT_ALIGN_H_ENDS:
			nAlignX = 2;
			break;
		}

		switch (pText->eAlignVertical)
		{
		case TEXT_ALIGN_V_TOP:
			break;
		case TEXT_ALIGN_V_BOTTOM:
		case TEXT_ALIGN_V_BOTTOMBASE:
			nAlignY = 1;
			break;
		case TEXT_ALIGN_V_CENTER:
		case TEXT_ALIGN_V_ENDS:
			nAlignY = 2;
			break;
		}
	}
	else
	{
		switch (eAlignH)
		{
		case TEXT_ALIGN_H_LEFT:
			break;
		case TEXT_ALIGN_H_RIGHT:
			nAlignY = 1;
			break;
		case TEXT_ALIGN_H_CENTER:
		case TEXT_ALIGN_H_ENDS:
			nAlignY = 2;
			break;
		}

		if (pText->bLeftInput)
		{
			switch (pText->eAlignVertical)
			{
			case TEXT_ALIGN_V_TOP:
				break;
			case TEXT_ALIGN_V_BOTTOM:
			case TEXT_ALIGN_V_BOTTOMBASE:
				nAlignX = 1;
				break;
			case TEXT_ALIGN_V_CENTER:
			case TEXT_ALIGN_V_ENDS:
				nAlignX = 2;
				break;
			}
		}
		else
		{
			switch (pText->eAlignVertical)
			{
			case TEXT_ALIGN_V_TOP:
				nAlignX = 1;
				break;
			case TEXT_ALIGN_V_BOTTOM:
			case TEXT_ALIGN_V_BOTTOMBASE:
				break;
			case TEXT_ALIGN_V_CENTER:
			case TEXT_ALIGN_V_ENDS:
				nAlignX = 2;
				break;
			}
		}
	}

	// X方向
	if (szRectSize.cx >= SYTEXT_DIMENSION_MIN)
	{
		switch (nAlignX)
		{
		case 0:
			{
				fTemp1 = prtBorder->left + szRectSize.cx;
				if (fTemp1 - prtBorder->left < SYTEXT_DIMENSION_MIN)
				{
					fTemp1 = pText->rtBorder.left + SYTEXT_DIMENSION_MIN;
				}
				prtBorder->right = fTemp1;
			}
			break;
		case 1:
			{
				fTemp2 = prtBorder->right - szRectSize.cx;
				if (prtBorder->right - fTemp2 < SYTEXT_DIMENSION_MIN)
				{
					fTemp2 = prtBorder->right - SYTEXT_DIMENSION_MIN;
				}
				prtBorder->left = fTemp2;
			}
			break;
		case 2:
			{
				fCenter = (prtBorder->right + prtBorder->left) / 2;
				fAdjust = (szRectSize.cx - (prtBorder->right - prtBorder->left)) / 2;
				fTemp1 = prtBorder->left - fAdjust;
				fTemp2 = prtBorder->right + fAdjust;
				if (fCenter - fTemp1 < SYTEXT_DIMENSION_MIN / 2)
				{
					fTemp1 = fCenter - SYTEXT_DIMENSION_MIN / 2;
				}
				prtBorder->left = fTemp1;
				if (fTemp2 - fCenter < SYTEXT_DIMENSION_MIN / 2)
				{
					fTemp2 = fCenter + SYTEXT_DIMENSION_MIN / 2;
				}
				prtBorder->right = fTemp2;
			}
			break;
		}
	}

	// Y方向
	if (szRectSize.cy >= SYTEXT_DIMENSION_MIN)
	{
		switch (nAlignY)
		{
		case 0:
			{
				fTemp1 = prtBorder->top + szRectSize.cy;
				if (fTemp1 - prtBorder->top < SYTEXT_DIMENSION_MIN)
				{
					fTemp1 = prtBorder->top + SYTEXT_DIMENSION_MIN;
				}
				prtBorder->bottom = fTemp1;
			}
			break;
		case 1:
			{
				fTemp1 = prtBorder->bottom - szRectSize.cy;
				if (prtBorder->bottom - fTemp1 < SYTEXT_DIMENSION_MIN)
				{
					fTemp1 = prtBorder->bottom - SYTEXT_DIMENSION_MIN;
				}
				prtBorder->top = fTemp1;
			}
			break;
		case 2:
			{
				fCenter = (prtBorder->bottom + prtBorder->top) / 2;
				fAdjust = (szRectSize.cy - (prtBorder->bottom - prtBorder->top)) / 2;
				fTemp1 = prtBorder->top - fAdjust;
				fTemp2 = prtBorder->bottom + fAdjust;
				if (fCenter - fTemp1 < SYTEXT_DIMENSION_MIN / 2)
				{
					fTemp1 = fCenter - SYTEXT_DIMENSION_MIN / 2;
				}
				prtBorder->top = fTemp1;
				if (fTemp2 - fCenter < SYTEXT_DIMENSION_MIN / 2)
				{
					fTemp2 = fCenter + SYTEXT_DIMENSION_MIN / 2;
				}
				prtBorder->bottom = fTemp2;
			}
			break;
		}
	}
}

 void SYText_SetFixedFrame(LPSYText pText, BOOL bFixedFrame)
{
	//原先非固定字框且原先不使用排版矩形，
	//则将实际排版矩形赋给排版矩形。
	if (!pText->bFixedFrame)
	{
		if (bFixedFrame)
		{
			if (!pText->bUseBorderEditMode)
			{
				pText->rtBorder = pText->rtTypesetReal;
			}
		}
	}
	pText->bFixedFrame = bFixedFrame;
}


//////////////////////////////////////////////////////////////////////////
// SYTypesetPrivateData
 void SYTypesetPrivateData_Init(LPSYTypesetPrivate pData)
{
	SYAR_Init(&pData->m_arParagraph);
	pData->lpParam = NULL;
	memset(&pData->rtTypesetBorder, 0, sizeof(rectf));
}

 void SYTypesetPrivateData_Release(LPSYTypesetPrivate pData)
{
	int nParagraphCnt = SYAR_GetCount(&pData->m_arParagraph);
	for (int i = 0; i < nParagraphCnt; ++i)
	{
		CSYTextParagraph* pParagraph = (CSYTextParagraph*)SYAR_GetAt(&pData->m_arParagraph, i);
		delete pParagraph;
	}
	SYAR_RemoveAll(&pData->m_arParagraph);

	pData->lpParam = NULL;
}


//////////////////////////////////////////////////////////////////////////
// SYFont
void SYFont_Init(LPSYFont pFont)
{
	memset(pFont, 0, sizeof(SYFont));
	pFont->lfHeight = 30;
	_tcscpy_s(pFont->lfFaceNameEng, LF_FACESIZE, _T("Arial"));
	_tcscpy_s(pFont->lfFaceName, LF_FACESIZE, _T("宋体"));

	pFont->dwVersion = 1;
}

void SYFont_Release(LPSYFont pFont)
{
}

 int SYFont_Save(LPCSYFont pFont, LPBYTE pBuffer, int nLen)
{
	int nSize = 0;
	LPBYTE pCurBuf = pBuffer;

	LONG lfTemp;
	nSize += sizeof(LONG);
	if (pCurBuf)
	{
		lfTemp = (LONG)pFont->lfHeight;
		memcpy(pCurBuf, &lfTemp, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(LONG);
	if (pCurBuf)
	{
		lfTemp = (LONG)pFont->lfWidth;
		memcpy(pCurBuf, &lfTemp, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(LONG);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfEscapement, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(LONG);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfOrientation, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(LONG);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfWeight, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfItalic, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfUnderline, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfStrikeOut, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfCharSet, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfOutPrecision, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfClipPrecision, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfQuality, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfPitchAndFamily, sizeof(LONG));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(pFont->lfFaceName);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfFaceName, sizeof(pFont->lfFaceName));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(DWORD);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->dwVersion, sizeof(DWORD));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(float);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfDistance, sizeof(float));
		pCurBuf = pBuffer + nSize;
	}

	//兼容字对齐问题
	nSize += sizeof(DWORD);
	if (pCurBuf)
	{
		DWORD dwBold = pFont->bBold;
		memcpy(pCurBuf, &dwBold, sizeof(DWORD));
		pCurBuf = pBuffer + nSize;
	}

	//兼容LPVOID在32位和64位系统中长度不一问题
	nSize += sizeof(__int64);
	if (pCurBuf)
	{
		__int64 n64Temp = (__int64)pFont->lpParam;
		memcpy(pCurBuf, &n64Temp, sizeof(__int64));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(pFont->lfFaceNameEng);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfFaceNameEng, sizeof(pFont->lfFaceNameEng));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfVEmbeding, sizeof(BYTE));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfHEmbeding, sizeof(BYTE));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfVEmbedingAlign, sizeof(BYTE));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfHEmbedingAlign, sizeof(BYTE));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfVGRotate, sizeof(BYTE));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(BYTE);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfScript, sizeof(BYTE));
		pCurBuf = pBuffer + nSize;
	}

	nSize += sizeof(float);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &pFont->lfScriptOffset, sizeof(float));
		pCurBuf = pBuffer + nSize;
	}

	//预留
	BYTE bReserved[32] = {0};
	nSize += sizeof(bReserved);
	if (pCurBuf)
	{
		memcpy(pCurBuf, &bReserved, sizeof(bReserved));
		pCurBuf = pBuffer + nSize;
	}

	return nSize;
}

 int SYFont_Load(LPSYFont pFont, LPBYTE pBuffer, int nLen)
{
	int nSize = 0;
	LPBYTE pCurBuf = pBuffer;

	LONG lfTemp;
	memcpy(&lfTemp, pCurBuf, sizeof(LONG));
	pFont->lfHeight = (float)lfTemp;
	nSize += sizeof(LONG);
	pCurBuf = pBuffer + nSize;

	memcpy(&lfTemp, pCurBuf, sizeof(LONG));
	pFont->lfWidth = (float)lfTemp;
	nSize += sizeof(LONG);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfEscapement, pCurBuf, sizeof(LONG));
	nSize += sizeof(LONG);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfOrientation, pCurBuf, sizeof(LONG));
	nSize += sizeof(LONG);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfWeight, pCurBuf, sizeof(LONG));
	nSize += sizeof(LONG);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfItalic, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfUnderline, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfStrikeOut, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfCharSet, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfOutPrecision, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfClipPrecision, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfQuality, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfPitchAndFamily, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfFaceName, pCurBuf, sizeof(pFont->lfFaceName));
	nSize += sizeof(pFont->lfFaceName);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->dwVersion, pCurBuf, sizeof(DWORD));
	nSize += sizeof(DWORD);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfDistance, pCurBuf, sizeof(float));
	nSize += sizeof(float);
	pCurBuf = pBuffer + nSize;

	DWORD dwBold;
	memcpy(&dwBold, pCurBuf, sizeof(DWORD));
	pFont->bBold = (BYTE)dwBold;
	nSize += sizeof(float);
	pCurBuf = pBuffer + nSize;

	__int64 n64Temp;
	memcpy(&n64Temp, pCurBuf, sizeof(__int64));
	pFont->lpParam = (LPVOID)n64Temp;
	nSize += sizeof(__int64);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfFaceNameEng, pCurBuf, sizeof(pFont->lfFaceNameEng));
	nSize += sizeof(pFont->lfFaceNameEng);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfVEmbeding, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfHEmbeding, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfVEmbedingAlign, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfHEmbedingAlign, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfVGRotate, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfScript, pCurBuf, sizeof(BYTE));
	nSize += sizeof(BYTE);
	pCurBuf = pBuffer + nSize;

	memcpy(&pFont->lfScriptOffset, pCurBuf, sizeof(float));
	nSize += sizeof(float);
	pCurBuf = pBuffer + nSize;

	//预留
	BYTE bReserved[32] = {0};
	memcpy(&bReserved, pCurBuf, sizeof(bReserved));
	nSize += sizeof(bReserved);
	pCurBuf = pBuffer + nSize;

	pFont->dwVersion = 1;
	return nSize;
}

void SYFont_Copy(LPSYFont pDes, LPCSYFont pSrc)
{
	memcpy(pDes, pSrc, sizeof(SYFont));
}

 BOOL SYFont_Compare(LPCSYFont pFont1, LPCSYFont pFont2)
{
	if (pFont1 == pFont2)
		return TRUE;
	if ((pFont1 == NULL && pFont2 != NULL) || (pFont2 == NULL && pFont1 != NULL))
		return FALSE;

	if (pFont1->lfHeight != pFont2->lfHeight
		|| pFont1->lfWidth != pFont2->lfWidth
		|| pFont1->lfItalic != pFont2->lfItalic
		|| pFont1->bBold != pFont2->bBold
		|| pFont1->lfDistance != pFont2->lfDistance
		|| pFont1->lfEscapement != pFont2->lfEscapement
		|| pFont1->lfVEmbeding != pFont2->lfVEmbeding
		|| pFont1->lfHEmbeding != pFont2->lfHEmbeding
		|| pFont1->lfVEmbedingAlign != pFont2->lfVEmbedingAlign
		|| pFont1->lfHEmbedingAlign != pFont2->lfHEmbedingAlign
		|| pFont1->lfVGRotate != pFont2->lfVGRotate
		|| pFont1->lfScript != pFont2->lfScript
		|| pFont1->lfScriptOffset != pFont2->lfScriptOffset
		|| _tcscmp(pFont1->lfFaceName, pFont2->lfFaceName) != 0
		|| _tcscmp(pFont1->lfFaceNameEng, pFont2->lfFaceNameEng) != 0)
	{
		return FALSE;
	}
	return TRUE;
}

 void SYFont_SetValue(LPSYFont pDesFont, const LPVOID lpSrcData, const LPVOID lp2ndSrcData, DWORD dwMask)
{
	//参数有效性由调用者检查
	switch (dwMask)
	{
	case SYTEXTFONTMASK_HEIGHT:
		pDesFont->lfHeight = *(float*)lpSrcData;
		break;
	case SYTEXTFONTMASK_WIDTH:
		pDesFont->lfWidth = *(float*)lpSrcData;
		break;
	case SYTEXTFONTMASK_FONTSIZE:
		pDesFont->lfWidth = ((SYSizef*)lpSrcData)->cx;
		pDesFont->lfHeight = ((SYSizef*)lpSrcData)->cy;
		break;
	case SYTEXTFONTMASK_BOLD:
		pDesFont->bBold = *(BYTE*)lpSrcData;
		break;
	case SYTEXTFONTMASK_ITALIC:
		pDesFont->lfItalic = *(BYTE*)lpSrcData;
		break;
	case SYTEXTFONTMASK_FACENAME:
		memcpy(pDesFont->lfFaceName, lpSrcData, sizeof(TCHAR) * LF_FACESIZE);
		break;
	case SYTEXTFONTMASK_FACENAMEENG:
		memcpy(pDesFont->lfFaceNameEng, lpSrcData, sizeof(TCHAR) * LF_FACESIZE);
		break;
	case SYTEXTFONTMASK_DISTANCE:
		pDesFont->lfDistance = *(float*)lpSrcData;
		break;
	case SYTEXTFONTMASK_ESCAPEMENT:
		pDesFont->lfEscapement = *(LONG*)lpSrcData;
		break;
	case SYTEXTFONTMASK_VGROTATE:
		pDesFont->lfVGRotate = *(BYTE*)lpSrcData;
		break;
	case SYTEXTFONTMASK_SCRIPT:
		pDesFont->lfScript = *(BYTE*)lpSrcData;
		//第二个参数
		if (lp2ndSrcData != NULL)
			pDesFont->lfScriptOffset = *(float*)lp2ndSrcData;
		break;
	}
}

 void SYFont_CopyValue(LPSYFont pDesFont, LPCSYFont pSrcFont, DWORD dwMask)
{
	//参数有效性由调用者检查
	if (dwMask & SYTEXTFONTMASK_HEIGHT)
	{
		pDesFont->lfHeight = pSrcFont->lfHeight;
	}
	if (dwMask & SYTEXTFONTMASK_WIDTH)
	{
		pDesFont->lfWidth = pSrcFont->lfWidth;
	}
	if (dwMask & SYTEXTFONTMASK_BOLD)
	{
		pDesFont->bBold = pSrcFont->bBold;
	}
	if (dwMask & SYTEXTFONTMASK_ITALIC)
	{
		pDesFont->lfItalic = pSrcFont->lfItalic;
	}
	if (dwMask & SYTEXTFONTMASK_FACENAME)
	{
		memcpy(pDesFont->lfFaceName, pSrcFont->lfFaceName, sizeof(TCHAR) * LF_FACESIZE);
	}
	if (dwMask & SYTEXTFONTMASK_FACENAMEENG)
	{
		memcpy(pDesFont->lfFaceNameEng, pSrcFont->lfFaceNameEng, sizeof(TCHAR) * LF_FACESIZE);
	}
	if (dwMask & SYTEXTFONTMASK_DISTANCE)
	{
		pDesFont->lfDistance = pSrcFont->lfDistance;
	}
	if (dwMask & SYTEXTFONTMASK_ESCAPEMENT)
	{
		pDesFont->lfEscapement = pSrcFont->lfEscapement;
	}
	if (dwMask & SYTEXTFONTMASK_VGROTATE)
	{
		pDesFont->lfVGRotate = pSrcFont->lfVGRotate;
	}
	if (dwMask & SYTEXTFONTMASK_SCRIPT)
	{
		pDesFont->lfScript = pSrcFont->lfScript;
		pDesFont->lfScriptOffset = pSrcFont->lfScriptOffset;
	}
}


//////////////////////////////////////////////////////////////////////////
// SYUnderLine
 void SYUnderLine_Init(LPSYUnderLine pUnderLine)
{
	if (pUnderLine != NULL)
	{
		pUnderLine->dwVersion = 1;
		pUnderLine->eType = UNDERLINE_SOLID;
		pUnderLine->fDistance = 0;
		pUnderLine->fHeight = 0;
		SYStaticEffect_Init(&pUnderLine->se2DColor);
	}
}

 void SYUnderLine_Create(LPSYUnderLine pUnderLine,
	E_TextUnderLine eType, 
	float fDistance,
	float fLineHeight,
	LPSYStaticEffect p2DColor)
{
	if (pUnderLine != NULL)
	{
		pUnderLine->eType = eType;
		pUnderLine->fDistance = fDistance;
		pUnderLine->fHeight = fLineHeight;
		if (p2DColor != NULL)
		{
			SYStaticEffect_Copy(&pUnderLine->se2DColor, p2DColor);
		}
		else
		{
			SYStaticEffect_Init(&pUnderLine->se2DColor);
		}
	}
}

 void SYUnderLine_Release(LPSYUnderLine pUnderLine)
{
	if (pUnderLine != NULL)
	{
		SYStaticEffect_Release(&pUnderLine->se2DColor);
	}
}

 void SYUnderLine_Copy(LPSYUnderLine pDes, LPCSYUnderLine pSrc)
{
	if (pDes != NULL && pSrc != NULL)
	{
		SYUnderLine_Release(pDes);

		pDes->dwVersion = pSrc->dwVersion;
		pDes->eType = pSrc->eType;
		pDes->fDistance = pSrc->fDistance;
		pDes->fHeight = pSrc->fHeight;
		SYStaticEffect_Copy(&pDes->se2DColor, &pSrc->se2DColor);
	}
}

 BOOL SYUnderLine_Compare(LPCSYUnderLine pUnderLine1, LPCSYUnderLine pUnderLine2)
{
	if (pUnderLine1 == pUnderLine2)
		return TRUE;
	if ((pUnderLine1 == NULL && pUnderLine2 != NULL) || (pUnderLine2 == NULL && pUnderLine1 != NULL))
		return FALSE;

	if (pUnderLine1->eType != pUnderLine2->eType
		|| pUnderLine1->fDistance != pUnderLine2->fDistance
		|| pUnderLine1->fHeight != pUnderLine2->fHeight)
	{
		return FALSE;
	}

	if (!sy_staticeffect_compare(&pUnderLine1->se2DColor, &pUnderLine2->se2DColor))
	{
		return FALSE;
	}

	return TRUE;
}

 void SYUnderLine_SetValue(LPSYUnderLine pDesUnderLine, const LPVOID lpSrcData, DWORD dwMask)
{
	switch (dwMask)
	{
	case SYTEXTFONTMASK_UNDERLINE_TYPE:
		pDesUnderLine->eType = *(E_TextUnderLine*)lpSrcData;
		break;
	case SYTEXTFONTMASK_UNDERLINE_HEIGHT:
		pDesUnderLine->fHeight = *(float*)lpSrcData;
		break;
	case SYTEXTFONTMASK_UNDERLINE_DISTANCE:
		pDesUnderLine->fDistance = *(float*)lpSrcData;
		break;
	case SYTEXTFONTMASK_UNDERLINE_2DCOLOR:
		SYStaticEffect_Copy(&pDesUnderLine->se2DColor, (LPSYStaticEffect)lpSrcData);
		break;
	}
}

 void SYUnderLine_CopyValue(LPSYUnderLine pDesUnderLine, LPCSYUnderLine pSrcUnderLine, DWORD dwMask)
{
	if (dwMask & SYTEXTFONTMASK_UNDERLINE_TYPE)
	{
		pDesUnderLine->eType = pSrcUnderLine->eType;
	}
	if (dwMask & SYTEXTFONTMASK_UNDERLINE_HEIGHT)
	{
		pDesUnderLine->fHeight = pSrcUnderLine->fHeight;
	}
	if (dwMask & SYTEXTFONTMASK_UNDERLINE_DISTANCE)
	{
		pDesUnderLine->fDistance = pSrcUnderLine->fDistance;
	}
	if (dwMask & SYTEXTFONTMASK_UNDERLINE_2DCOLOR)
	{
		SYStaticEffect_Copy(&pDesUnderLine->se2DColor, &pSrcUnderLine->se2DColor);
	}
}


//////////////////////////////////////////////////////////////////////////
// SYTextProp
 void SYTextProp_Init(LPSYTextProp pProp)
{
	if (pProp != NULL)
	{
		pProp->dwVersion = 2;
		SYAR_Init(&pProp->arIndex);
		SYAR_Init(&pProp->arFont);
		SYAR_Init(&pProp->ar2DColor);
		SYAR_Init(&pProp->ar3DColor);
		SYAR_Init(&pProp->arUnderLine);
	}
}

 void SYTextProp_Create(int nTextLen,
	LPSYTextProp pProp,
	LPSYFont pFont, 
	LPSYStaticEffect p2DColor,
	SYMaterial* p3DColor,
	LPSYUnderLine pUnderLine)
{
	if (pProp != NULL)
	{
		//字体
		LPSYFont pNewFont = new SYFont;
		SYFont_Init(pNewFont);
		if (pFont != NULL)
		{
			SYFont_Copy(pNewFont, pFont);
		}
		SYAR_Add(&pProp->arFont, pNewFont);

		//2D Color
		LPSYStaticEffect pNew2DColor = new SYStaticEffect;
		SYStaticEffect_Init(pNew2DColor);
		if (p2DColor != NULL)
		{
			SYStaticEffect_Copy(pNew2DColor, p2DColor);
		}
		SYAR_Add(&pProp->ar2DColor, pNew2DColor);

		//3D Color（允许没有）
		SYMaterial* pNew3DColor = NULL;
		if (p3DColor != NULL)
		{
			pNew3DColor = new SYMaterial;
			SYMaterial_Init(pNew3DColor);
			SYMaterial_Copy(pNew3DColor, p3DColor);
			SYAR_Add(&pProp->ar3DColor, pNew3DColor);
		}

		//下划线（允许没有）
		LPSYUnderLine pNewUnderLine = NULL;
		if (pUnderLine != NULL)
		{
			pNewUnderLine = new SYUnderLine;
			SYUnderLine_Init(pNewUnderLine);
			SYUnderLine_Copy(pNewUnderLine, pUnderLine);
			SYAR_Add(&pProp->arUnderLine, pNewUnderLine);
		}

		//索引表
		if (nTextLen < 1)
			nTextLen = 1;
		for (int i = 0; i < nTextLen; ++i)
		{
			LPSYTextPropIndex pPropIdx = new SYTextPropIndex;
			pPropIdx->pFont = pNewFont;
			pPropIdx->p2DColor = pNew2DColor;
			for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
				pPropIdx->p3DColor[n3D] = pNew3DColor;
			pPropIdx->pUnderLine = pNewUnderLine;
			SYAR_Add(&pProp->arIndex, pPropIdx);
		}
	}
}

 void SYTextProp_Release(LPSYTextProp pProp)
{
	if (pProp != NULL)
	{
		int nCnt, i;
		//字体
		nCnt = SYAR_GetCount(&pProp->arFont);
		for (i=0; i<nCnt; ++i)
		{
			LPSYFont pFont = (LPSYFont)SYAR_GetAt(&pProp->arFont, i);
			SYFont_Release(pFont);
			delete pFont;
		}
		SYAR_RemoveAll(&pProp->arFont);

		//2D Color
		nCnt = SYAR_GetCount(&pProp->ar2DColor);
		for (i=0; i<nCnt; ++i)
		{
			LPSYStaticEffect p2DColor = (LPSYStaticEffect)SYAR_GetAt(&pProp->ar2DColor, i);
			SYStaticEffect_Release(p2DColor);
			delete p2DColor;
		}
		SYAR_RemoveAll(&pProp->ar2DColor);

		//3D Color
		nCnt = SYAR_GetCount(&pProp->ar3DColor);
		for (i=0; i<nCnt; ++i)
		{
			SYMaterial* p3DColor = (SYMaterial*)SYAR_GetAt(&pProp->ar3DColor, i);
			SYMaterial_Release(p3DColor);
			delete p3DColor;
		}
		SYAR_RemoveAll(&pProp->ar3DColor);

		//下划线
		nCnt = SYAR_GetCount(&pProp->arUnderLine);
		for (i=0; i<nCnt; ++i)
		{
			LPSYUnderLine pUnderLine = (LPSYUnderLine)SYAR_GetAt(&pProp->arUnderLine, i);
			SYUnderLine_Release(pUnderLine);
			delete pUnderLine;
		}
		SYAR_RemoveAll(&pProp->arUnderLine);

		//索引表
		nCnt = SYAR_GetCount(&pProp->arIndex);
		for (i=0; i<nCnt; ++i)
		{
			LPSYTextPropIndex pPropIdx = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, i);
			SYTextPropIndex_Release(pPropIdx);
			delete pPropIdx;
		}
		SYAR_RemoveAll(&pProp->arIndex);
	}
}

 void SYTextProp_Copy(LPSYTextProp pDes, LPCSYTextProp pSrc, int nDesLen/* = -1*/)
{
	if (pDes == NULL || pSrc == NULL || pSrc == pDes)
		return;

	SYTextProp_Release(pDes);

	int nAryCnt;
	nAryCnt = SYAR_GetCount(&pSrc->arFont);
	for (int i = 0; i < nAryCnt; ++i)
	{
		LPSYFont pSrcFont = (LPSYFont)SYAR_GetAt(&pSrc->arFont, i);
		LPSYFont pNewFont = new SYFont;
		SYFont_Copy(pNewFont, pSrcFont);
		SYAR_Add(&pDes->arFont, pNewFont);
	}

	nAryCnt = SYAR_GetCount(&pSrc->ar2DColor);
	for (int i = 0; i < nAryCnt; ++i)
	{
		LPSYStaticEffect pSrc2DColor = (LPSYStaticEffect)SYAR_GetAt(&pSrc->ar2DColor, i);
		LPSYStaticEffect pNew2DColor = new SYStaticEffect;
		SYStaticEffect_Init(pNew2DColor);
		SYStaticEffect_Copy(pNew2DColor, pSrc2DColor);
		SYAR_Add(&pDes->ar2DColor, pNew2DColor);
	}

	nAryCnt = SYAR_GetCount(&pSrc->ar3DColor);
	for (int i = 0; i < nAryCnt; ++i)
	{
		SYMaterial* pSrc3DColor = (SYMaterial*)SYAR_GetAt(&pSrc->ar3DColor, i);
		SYMaterial* pNew3DColor = new SYMaterial;
		SYMaterial_Init(pNew3DColor);
		SYMaterial_Copy(pNew3DColor, pSrc3DColor);
		SYAR_Add(&pDes->ar3DColor, pNew3DColor);
	}

	nAryCnt = SYAR_GetCount(&pSrc->arUnderLine);
	for (int i = 0; i < nAryCnt; ++i)
	{
		LPSYUnderLine pSrcUnderLine = (LPSYUnderLine)SYAR_GetAt(&pSrc->arUnderLine, i);
		LPSYUnderLine pNewUnderLine = new SYUnderLine;
		SYUnderLine_Init(pNewUnderLine);
		SYUnderLine_Copy(pNewUnderLine, pSrcUnderLine);
		SYAR_Add(&pDes->arUnderLine, pNewUnderLine);
	}

	LPSYTextPropIndex pSrcPropIdx, pNewPropIdx;
	nAryCnt = SYAR_GetCount(&pSrc->arIndex);
	for (int i = 0; i < nAryCnt; ++i)
	{
		pSrcPropIdx = (LPSYTextPropIndex)SYAR_GetAt(&pSrc->arIndex, i);
		pNewPropIdx = new SYTextPropIndex;

		int nID;
		nID = SYAR_GetIndexByPtr(&pSrc->arFont, pSrcPropIdx->pFont);
		pNewPropIdx->pFont = SYAR_GetAt(&pDes->arFont, nID);

		nID = SYAR_GetIndexByPtr(&pSrc->ar2DColor, pSrcPropIdx->p2DColor);
		pNewPropIdx->p2DColor = SYAR_GetAt(&pDes->ar2DColor, nID);

		for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
		{
			nID = SYAR_GetIndexByPtr(&pSrc->ar3DColor, pSrcPropIdx->p3DColor[n3D]);
			pNewPropIdx->p3DColor[n3D] = SYAR_GetAt(&pDes->ar3DColor, nID);
		}

		nID = SYAR_GetIndexByPtr(&pSrc->arUnderLine, pSrcPropIdx->pUnderLine);
		pNewPropIdx->pUnderLine = SYAR_GetAt(&pDes->arUnderLine, nID);

		SYAR_Add(&pDes->arIndex, pNewPropIdx);
	}

	//调整目标属性个数
	if (nDesLen >= 0)
	{
		SYTextProp_ResizePropIndexArray(pDes, nDesLen, TRUE);
	}
}

 BOOL SYTextProp_Compare(LPCSYTextProp pProp1, LPCSYTextProp pProp2)
{
	return FALSE;
}

 void SYTextProp_DeleteString(LPSYTextProp pProp, int nCharIdx, int nDelCnt, int nTextLen, BOOL bWholeRender)
{
	if (nDelCnt <= 0 || nCharIdx < 0)
		return;

	SYArray* paryIndex = &pProp->arIndex;
	int nPropCnt = SYAR_GetCount(paryIndex);

	//确保属性索引至少存在一个
	if (nTextLen < 1)
		nTextLen = 1;
	//防止删除多了
	if (nPropCnt - nDelCnt < nTextLen)
		nDelCnt = nPropCnt - nTextLen;

	BOOL b1StWholeRender;
	for (int nP = nCharIdx + nDelCnt - 1; nP >= nCharIdx; --nP)
	{
		LPSYTextPropIndex pTempIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
		b1StWholeRender = (bWholeRender && nP == 0);
		if (SYTextProp_EmptyPropIndex(pProp, pTempIdx, b1StWholeRender))
		{
			if (b1StWholeRender)
			{
				//转移2D颜色索引
				LPSYTextPropIndex pNextIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP + 1);
				if (pNextIdx != NULL && pTempIdx->p2DColor)
					pNextIdx->p2DColor = pTempIdx->p2DColor;
			}
			SYTextPropIndex_Release(pTempIdx);
			delete pTempIdx;
			SYAR_RemoveAt(paryIndex, nP, 1);
		}
	}
}

 void SYTextProp_InsertString(LPSYTextProp pProp, int nCharIdx, int nInsCnt, int nTextLen)
{
	if (nInsCnt > 0)
	{
		if (nCharIdx < 0)
			nCharIdx = 0;
		int nSrc = nCharIdx - 1;
		if (nSrc < 0)
			nSrc = 0;

		SYArray* paryIndex = &pProp->arIndex;
		int nPropCnt = SYAR_GetCount(paryIndex);
		//确保属性索引至少存在一个
		if (nTextLen < 1)
			nTextLen = 1;
		//防止插入超出
		if (nPropCnt + nInsCnt > nTextLen)
			nInsCnt = nTextLen - nPropCnt;

		if (nInsCnt > 0)
		{
			//复制
			LPSYTextPropIndex pSrcIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nSrc);
			if (pSrcIdx != NULL)
			{
				for (int i = 0; i < nInsCnt; ++i)
				{
					LPSYTextPropIndex pNewIdx = new SYTextPropIndex;
					SYTextPropIndex_Copy(pNewIdx, pSrcIdx);
					SYAR_InsertAt(paryIndex, nCharIdx, pNewIdx, 1);
				}
			}
		}
	}
}

 LPVOID SYTextProp_GetProp(LPSYTextProp pProp, int nCharIdx, E_TextPropType eType)
{
	LPSYTextPropIndex pIndex = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, nCharIdx);
	if (pIndex != NULL)
	{
		switch (eType)
		{
		case TEXT_PROP_TYPE_FONT:
			return pIndex->pFont;
		case TEXT_PROP_TYPE_2DCOLOR:
			return pIndex->p2DColor;
		case TEXT_PROP_TYPE_3DCOLOR:
			return pIndex->p3DColor;
		case TEXT_PROP_TYPE_UNDERLINE:
			return pIndex->pUnderLine;
		}
	}
	return NULL;
}

 LPSYTextPropIndex	SYTextProp_GetPropIndex(SYArray* paryIndex, int nCharIdx)
{
	int nCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx >= nCnt)
		nCharIdx = nCnt - 1;
	else if (nCharIdx < 0)
		nCharIdx = 0;

	return (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nCharIdx);
}

 BOOL SYTextProp_EmptyPropIndex(LPSYTextProp pProp, LPSYTextPropIndex pPropIdx, BOOL bReserve2DClr)
{
	SYArray* paryIndex = &pProp->arIndex;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (pPropIdx != NULL)
	{
		//删除没有被引用的属性
		WORD wLogic = 0x00;
		WORD w3DLogic[SYTEXT_SUBFACE_CNT] = {0x10, 0x20, 0x40};

		for (int i = 0; i < nPropCnt; ++i)
		{
			LPSYTextPropIndex pTempIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, i);
			if (pTempIdx != pPropIdx)
			{
				if (pTempIdx->pFont == pPropIdx->pFont)
					wLogic |= 0x01;
				if (pTempIdx->p2DColor == pPropIdx->p2DColor)
					wLogic |= 0x02;
				if (pTempIdx->pUnderLine == pPropIdx->pUnderLine)
					wLogic |= 0x04;

				for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
				{
					LPVOID pTemp3DClr = pTempIdx->p3DColor[n3D];
					if (pTemp3DClr != NULL)
					{
						for (int i3 = 0; i3 < SYTEXT_SUBFACE_CNT; ++i3)
						{
							LPVOID pProp3DClr = pPropIdx->p3DColor[i3];
							if (pTemp3DClr == pProp3DClr)
							{
								wLogic |= w3DLogic[i3];
							}
						}
					}
				}

				if (wLogic == 0x77)
					break;
			}
		}

		if ((wLogic & 0x01) == 0x00)
		{
			//禁止空表
			if (SYAR_GetCount(&pProp->arFont) > 1)
				SYTextProp_RemoveFont(&pProp->arFont, (LPSYFont)pPropIdx->pFont);
		}
		if (!bReserve2DClr && (wLogic & 0x02) == 0x00)
		{
			//禁止空表
			if (SYAR_GetCount(&pProp->ar2DColor) > 1)
				SYTextProp_Remove2DColor(&pProp->ar2DColor, (LPSYStaticEffect)pPropIdx->p2DColor);
		}
		if ((wLogic & 0x04) == 0x00)
		{
			//允许空表
			if (SYAR_GetCount(&pProp->arUnderLine) > 0)
				SYTextProp_RemoveUnderLine(&pProp->arUnderLine, (LPSYUnderLine)pPropIdx->pUnderLine);
		}

		//允许空表
		if (SYAR_GetCount(&pProp->ar3DColor) > 0)
		{
			for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
			{
				if ((wLogic & w3DLogic[n3D]) == 0x00)
				{
					SYTextProp_Remove3DColor(&pProp->ar3DColor, (SYMaterial*)(pPropIdx->p3DColor[n3D]));
				}
			}
		}

		return TRUE;
	}
	return FALSE;
}

 void SYTextProp_ResizePropIndexArray(LPSYTextProp pProp, int nSize, BOOL bKeepProperty)
{
	LPSYTextPropIndex pSrcIdx, pTempIdx;
	int nNewPropCnt = nSize;
	SYArray* paryIndex = &pProp->arIndex;
	int nOldPropCnt = SYAR_GetCount(paryIndex);
	if (bKeepProperty)
	{
		if (nNewPropCnt >= nOldPropCnt)
		{
			pSrcIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nOldPropCnt - 1);
			if (pSrcIdx != NULL)
			{
				while (nNewPropCnt > nOldPropCnt)
				{
					pTempIdx = new SYTextPropIndex;
					SYTextPropIndex_Copy(pTempIdx, pSrcIdx);
					SYAR_Add(paryIndex, pTempIdx);
					++nOldPropCnt;
				}
			}
			return;
		}
	}
	else
	{
		nNewPropCnt = 1;
	}

	while (nNewPropCnt < nOldPropCnt)
	{
		--nOldPropCnt;
		pSrcIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nOldPropCnt);
		if (SYTextProp_EmptyPropIndex(pProp, pSrcIdx, FALSE))
		{
			SYTextPropIndex_Release(pSrcIdx);
			delete pSrcIdx;
			SYAR_RemoveAt(paryIndex, nOldPropCnt, 1);
		}
	}
}

 LPSYFont SYTextProp_GetFont(LPSYTextProp pProp, int nCharIdx)
{
	int nCnt = SYAR_GetCount(&pProp->arIndex);
	if (nCharIdx >= nCnt)
		nCharIdx = nCnt - 1;
	else if (nCharIdx < 0)
		nCharIdx = 0;

	if (nCharIdx < 0)
	{
		return (LPSYFont)SYAR_GetAt(&pProp->arFont, 0);
	}

	LPSYTextPropIndex pIndex = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, nCharIdx);
	if (pIndex != NULL)
	{
		return (LPSYFont)pIndex->pFont;
	}
	return (LPSYFont)SYAR_GetAt(&pProp->arFont, 0);
}

 LPSYStaticEffect SYTextProp_GetStaticEffect(LPSYTextProp pProp, int nCharIdx)
{
	int nCnt = SYAR_GetCount(&pProp->arIndex);
	if (nCharIdx >= nCnt)
		nCharIdx = nCnt - 1;
	else if (nCharIdx < 0)
		nCharIdx = 0;

	LPSYTextPropIndex pIndex = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, nCharIdx);
	if (pIndex != NULL)
	{
		return (LPSYStaticEffect)pIndex->p2DColor;
	}
	return (LPSYStaticEffect)SYAR_GetAt(&pProp->ar2DColor, 0);
}

 SYMaterial* SYTextProp_Get3DColor(LPSYTextProp pProp, int nCharIdx, int nSubFace)
{
	int nCnt = SYAR_GetCount(&pProp->arIndex);
	if (nCharIdx >= nCnt)
		nCharIdx = nCnt - 1;
	else if (nCharIdx < 0)
		nCharIdx = 0;

	LPSYTextPropIndex pIndex = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, nCharIdx);
	if (pIndex != NULL)
	{
		if (nSubFace < 0 || nSubFace > SYTEXT_SUBFACE_CNT - 1)
			nSubFace = 0;
		return (SYMaterial*)(pIndex->p3DColor[nSubFace]);
	}
	return (SYMaterial*)SYAR_GetAt(&pProp->ar3DColor, 0);
}

 LPVOID* SYTextProp_Get3DColorArray(LPSYTextProp pProp, int nCharIdx)
{
	int nCnt = SYAR_GetCount(&pProp->arIndex);
	if (nCharIdx >= nCnt)
		nCharIdx = nCnt - 1;
	else if (nCharIdx < 0)
		nCharIdx = 0;

	LPSYTextPropIndex pIndex = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, nCharIdx);
	if (pIndex != NULL)
	{
		return &(pIndex->p3DColor[0]);
	}
	return NULL;
}

 LPSYUnderLine SYTextProp_GetUnderLine(LPSYTextProp pProp, int nCharIdx)
{
	int nCnt = SYAR_GetCount(&pProp->arIndex);
	if (nCharIdx >= nCnt)
		nCharIdx = nCnt - 1;
	else if (nCharIdx < 0)
		nCharIdx = 0;

	LPSYTextPropIndex pIndex = (LPSYTextPropIndex)SYAR_GetAt(&pProp->arIndex, nCharIdx);
	if (pIndex != NULL)
	{
		return (LPSYUnderLine)pIndex->pUnderLine;
	}
	return NULL;
}

 void SYTextProp_SetFont(LPSYTextProp pProp, int nCharIdx, int nCount, LPCSYFont lpFont, DWORD dwMask)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryFont = &pProp->arFont;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;

	LPSYTextPropIndex pTempPropIdx;
	LPSYFont pCurFont;
	LPSYFont pTempFont;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		//查找当前字符的字体属性索引
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);
		if (pCurIndex != NULL)
		{
			pCurFont = (LPSYFont)pCurIndex->pFont;

			//是否还使用该字体者
			BOOL bRef = FALSE;
			for (int nP = 0; nP < nPropCnt; ++nP)
			{
				pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
				if (pTempPropIdx != pCurIndex && pTempPropIdx->pFont == pCurFont)
				{
					bRef = TRUE;
					break;
				}
			}

			if (bRef)
			{
				pTempFont = new SYFont;
				SYFont_Init(pTempFont);
				SYFont_Copy(pTempFont, pCurFont);
				SYFont_CopyValue(pTempFont, lpFont, dwMask);

				LPVOID pSame;
				int nSameID = SYTextProp_FindSameFont(paryFont, pTempFont, &pSame);
				//无相同字体，则在字体表尾追加。
				if (nSameID < 0)
				{
					SYAR_Add(paryFont, pTempFont);
					//属性表引用项重定向
					pCurIndex->pFont = pTempFont;
				}
				else
				{
					pCurIndex->pFont = pSame;
					SYFont_Release(pTempFont);
					delete pTempFont;
				}
			}
			else
			{
				SYFont_CopyValue(pCurFont, lpFont, dwMask);
				LPVOID pSame;
				int nSameID = SYTextProp_FindSameFont(paryFont, pCurFont, &pSame);
				if (nSameID >= 0)
				{
					for (int nP = 0; nP < nPropCnt; ++nP)
					{
						pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
						if (pTempPropIdx->pFont == pSame)
							pTempPropIdx->pFont = pCurFont;
					}
					SYTextProp_RemoveFont(paryFont, nSameID);
				}
			}
		}
	}
}

 void SYTextProp_FontProperty(LPSYTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, const LPVOID lp2ndPropData, DWORD dwMask)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryFont = &pProp->arFont;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;

	LPSYTextPropIndex pTempPropIdx;
	LPSYFont pCurFont;
	LPSYFont pTempFont;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		//查找当前字符的字体属性索引
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);
		if (pCurIndex != NULL)
		{
			pCurFont = (LPSYFont)pCurIndex->pFont;

			//是否还使用该字体者
			BOOL bRef = FALSE;
			for (int nP = 0; nP < nPropCnt; ++nP)
			{
				pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
				if (pTempPropIdx != pCurIndex && pTempPropIdx->pFont == pCurFont)
				{
					bRef = TRUE;
					break;
				}
			}

			if (bRef)
			{
				pTempFont = new SYFont;
				SYFont_Init(pTempFont);
				SYFont_Copy(pTempFont, pCurFont);
				SYFont_SetValue(pTempFont, lpPropData, lp2ndPropData, dwMask);

				LPVOID pSame;
				int nSameID = SYTextProp_FindSameFont(paryFont, pTempFont, &pSame);
				//无相同字体，则在字体表尾追加。
				if (nSameID < 0)
				{
					SYAR_Add(paryFont, pTempFont);
					//属性表引用项重定向
					pCurIndex->pFont = pTempFont;
				}
				else
				{
					pCurIndex->pFont = pSame;
					SYFont_Release(pTempFont);
					delete pTempFont;
				}
			}
			else
			{
				SYFont_SetValue(pCurFont, lpPropData, lp2ndPropData, dwMask);
				LPVOID pSame;
				int nSameID = SYTextProp_FindSameFont(paryFont, pCurFont, &pSame);
				if (nSameID >= 0)
				{
					for (int nP = 0; nP < nPropCnt; ++nP)
					{
						pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
						if (pTempPropIdx->pFont == pSame)
							pTempPropIdx->pFont = pCurFont;
					}
					SYTextProp_RemoveFont(paryFont, nSameID);
				}
			}
		}
	}
}

 void SYTextProp_SetStaticEffect(LPSYTextProp pProp, int nCharIdx, int nCount, LPCSYStaticEffect lpFillColor, DWORD dwMask)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryColor = &pProp->ar2DColor;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;

	LPSYTextPropIndex pTempPropIdx;
	LPSYStaticEffect pCurColor;
	LPSYStaticEffect pTempColor;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);
		if (pCurIndex != NULL)
		{
			pCurColor = (LPSYStaticEffect)pCurIndex->p2DColor;

			BOOL bRef = FALSE;
			for (int nP = 0; nP < nPropCnt; ++nP)
			{
				pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
				if (pTempPropIdx != pCurIndex && pTempPropIdx->p2DColor == pCurColor)
				{
					bRef = TRUE;
					break;
				}
			}

			if (bRef)
			{
				pTempColor = new SYStaticEffect;
				SYStaticEffect_Init(pTempColor);
				SYStaticEffect_Copy(pTempColor, pCurColor);
				SYText2DColor_CopyValue(pTempColor, lpFillColor, dwMask);

				LPVOID pSame;
				int nSameID = SYTextProp_FindSame2DColor(paryColor, pTempColor, &pSame);
				if (nSameID < 0)
				{
					SYAR_Add(paryColor, pTempColor);
					pCurIndex->p2DColor = pTempColor;
				}
				else
				{
					pCurIndex->p2DColor = pSame;
					SYStaticEffect_Release(pTempColor);
					delete pTempColor;
				}
			}
			else
			{
				SYText2DColor_CopyValue(pCurColor, lpFillColor, dwMask);
				LPVOID pSame;
				int nSameID = SYTextProp_FindSame2DColor(paryColor, pCurColor, &pSame);
				if (nSameID >= 0)
				{
					for (int nP = 0; nP < nPropCnt; ++nP)
					{
						pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
						if (pTempPropIdx->p2DColor == pSame)
							pTempPropIdx->p2DColor = pCurColor;
					}
					SYTextProp_Remove2DColor(paryColor, nSameID);
				}
			}
		}
	}
}

 void SYTextProp_Set3DColor(LPSYTextProp pProp, int nCharIdx, int nCount, SYMaterial* p3DColor)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryColor = &pProp->ar3DColor;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;

	LPSYTextPropIndex pTempPropIdx;
	SYMaterial* pNewColor = NULL;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);

		LPVOID pSame;
		int nSameID = SYTextProp_FindSame3DColor(paryColor, p3DColor, &pSame);
		if (nSameID < 0)
		{
			pNewColor = new SYMaterial;
			SYMaterial_Init(pNewColor);
			SYMaterial_Copy(pNewColor, p3DColor);
			SYAR_Add(paryColor, pNewColor);
		}
		else
		{
			pNewColor = (SYMaterial*)pSame;
		}

		LPVOID pRemoved[SYTEXT_SUBFACE_CNT] = {NULL};
		for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
		{
			LPVOID p3DColorSrc = pCurIndex->p3DColor[n3D];
			if (p3DColorSrc == NULL)
			{
				pCurIndex->p3DColor[n3D] = pNewColor;
				continue;
			}

			BOOL bDeal = FALSE;
			for (int i3 = 0; i3 < SYTEXT_SUBFACE_CNT; ++i3)
			{
				if (pRemoved[i3] == p3DColorSrc)
				{
					bDeal = TRUE;
					break;
				}
			}
			if (bDeal)
			{
				pCurIndex->p3DColor[n3D] = pNewColor;
				continue;
			}

			BOOL bRef = FALSE;
			for (int nP = 0; nP < nPropCnt; ++nP)
			{
				pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
				if (pTempPropIdx != pCurIndex)
				{
					for (int i3 = 0; i3 < SYTEXT_SUBFACE_CNT; ++i3)
					{
						if (pTempPropIdx->p3DColor[i3] == p3DColorSrc)
						{
							bRef = TRUE;
							break;
						}
					}
				}
				if (bRef)
					break;
			}

			if (!bRef)
			{
				pRemoved[n3D] = p3DColorSrc;
				SYTextProp_Remove3DColor(paryColor, (SYMaterial*)p3DColorSrc);
			}
			pCurIndex->p3DColor[n3D] = pNewColor;
		}
	}
}

 void SYTextProp_SetSubFace3DColor(LPSYTextProp pProp, int nCharIdx, int nCount, int nSubFace, SYMaterial* p3DColor)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryColor = &pProp->ar3DColor;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;
	if (nSubFace < 0 || nSubFace > SYTEXT_SUBFACE_CNT - 1)
		nSubFace = 0;

	LPSYTextPropIndex pTempPropIdx;
	SYMaterial* pNewColor = NULL;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);

		LPVOID pSame;
		int nSameID = SYTextProp_FindSame3DColor(paryColor, p3DColor, &pSame);
		if (nSameID < 0)
		{
			pNewColor = new SYMaterial;
			SYMaterial_Init(pNewColor);
			SYMaterial_Copy(pNewColor, p3DColor);
			SYAR_Add(paryColor, pNewColor);
		}
		else
		{
			pNewColor = (SYMaterial*)pSame;
		}

		LPVOID p3DColorSrc = pCurIndex->p3DColor[nSubFace];
		if (p3DColorSrc == NULL)
		{
			pCurIndex->p3DColor[nSubFace] = pNewColor;
			continue;
		}

		BOOL bRef = FALSE;
		for (int nP = 0; nP < nPropCnt; ++nP)
		{
			pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
			if (pTempPropIdx != pCurIndex)
			{
				for (int i3 = 0; i3 < SYTEXT_SUBFACE_CNT; ++i3)
				{
					if (pTempPropIdx->p3DColor[i3] == p3DColorSrc)
					{
						bRef = TRUE;
						break;
					}
				}
			}
			else
			{
				for (int i3 = 0; i3 < SYTEXT_SUBFACE_CNT; ++i3)
				{
					if (i3 != nSubFace && pTempPropIdx->p3DColor[i3] == p3DColorSrc)
					{
						bRef = TRUE;
						break;
					}
				}
			}
			if (bRef)
				break;
		}

		if (!bRef)
		{
			SYTextProp_Remove3DColor(paryColor, (SYMaterial*)p3DColorSrc);
		}
		pCurIndex->p3DColor[nSubFace] = pNewColor;
	}
}

 void SYTextProp_SetUnderLine(LPSYTextProp pProp, int nCharIdx, int nCount, LPCSYUnderLine lpUnderLine, DWORD dwMask)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryUL = &pProp->arUnderLine;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;

	LPSYTextPropIndex pTempPropIdx;
	LPSYUnderLine pCurUL;
	LPSYUnderLine pTempUL;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);
		if (pCurIndex != NULL)
		{
			pCurUL = (LPSYUnderLine)pCurIndex->pUnderLine;

			BOOL bRef;
			if (pCurUL != NULL)
			{
				bRef = FALSE;
				for (int nP = 0; nP < nPropCnt; ++nP)
				{
					pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
					if (pTempPropIdx != pCurIndex && pTempPropIdx->pUnderLine == pCurUL)
					{
						bRef = TRUE;
						break;
					}
				}
			}
			else
			{
				bRef = TRUE;
			}

			if (bRef)
			{
				pTempUL = new SYUnderLine;
				SYUnderLine_Init(pTempUL);
				if (pCurUL != NULL)
					SYUnderLine_Copy(pTempUL, pCurUL);
				SYUnderLine_CopyValue(pTempUL, lpUnderLine, dwMask);

				LPVOID pSame;
				int nSameID = SYTextProp_FindSameUnderLine(paryUL, pTempUL, &pSame);
				//无相同字体，则在字体表尾追加。
				if (nSameID < 0)
				{
					SYAR_Add(paryUL, pTempUL);
					//属性表引用项重定向
					pCurIndex->pUnderLine = pTempUL;
				}
				else
				{
					pCurIndex->pUnderLine = pSame;
					SYUnderLine_Release(pTempUL);
					delete pTempUL;
				}
			}
			else
			{
				SYUnderLine_CopyValue(pCurUL, lpUnderLine, dwMask);
				LPVOID pSame;
				int nSameID = SYTextProp_FindSameUnderLine(paryUL, pCurUL, &pSame);
				if (nSameID >= 0)
				{
					for (int nP = 0; nP < nPropCnt; ++nP)
					{
						pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
						if (pTempPropIdx->pUnderLine == pSame)
							pTempPropIdx->pUnderLine = pCurUL;
					}
					SYTextProp_RemoveUnderLine(paryUL, nSameID);
				}
			}
		}
	}
}

 void SYTextProp_SetUnderLineProperty(LPSYTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, DWORD dwMask)
{
	SYArray* paryIndex = &pProp->arIndex;
	SYArray* paryUL = &pProp->arUnderLine;
	int nPropCnt = SYAR_GetCount(paryIndex);
	if (nCharIdx < 0)
	{
		nCharIdx = 0;
		nCount = nPropCnt;
	}
	if (nCount <= 0)
		return;

	LPSYTextPropIndex pTempPropIdx;
	LPSYUnderLine pCurUL;
	LPSYUnderLine pTempUL;

	for (int nC = nCharIdx; nC < nCharIdx + nCount; ++nC)
	{
		LPSYTextPropIndex pCurIndex = SYTextProp_GetPropIndex(paryIndex, nC);
		if (pCurIndex != NULL)
		{
			pCurUL = (LPSYUnderLine)pCurIndex->pUnderLine;

			BOOL bRef;
			if (pCurUL != NULL)
			{
				bRef = FALSE;
				for (int nP = 0; nP < nPropCnt; ++nP)
				{
					pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
					if (pTempPropIdx != pCurIndex && pTempPropIdx->pUnderLine == pCurUL)
					{
						bRef = TRUE;
						break;
					}
				}
			}
			else
			{
				bRef = TRUE;
			}

			if (bRef)
			{
				pTempUL = new SYUnderLine;
				SYUnderLine_Init(pTempUL);
				if (pCurUL != NULL)
					SYUnderLine_Copy(pTempUL, pCurUL);
				SYUnderLine_SetValue(pTempUL, lpPropData, dwMask);

				LPVOID pSame;
				int nSameID = SYTextProp_FindSameUnderLine(paryUL, pTempUL, &pSame);
				//无相同字体，则在字体表尾追加。
				if (nSameID < 0)
				{
					SYAR_Add(paryUL, pTempUL);
					//属性表引用项重定向
					pCurIndex->pUnderLine = pTempUL;
				}
				else
				{
					pCurIndex->pUnderLine = pSame;
					SYUnderLine_Release(pTempUL);
					delete pTempUL;
				}
			}
			else
			{
				SYUnderLine_SetValue(pCurUL, lpPropData, dwMask);
				LPVOID pSame;
				int nSameID = SYTextProp_FindSameUnderLine(paryUL, pCurUL, &pSame);
				if (nSameID >= 0)
				{
					for (int nP = 0; nP < nPropCnt; ++nP)
					{
						pTempPropIdx = (LPSYTextPropIndex)SYAR_GetAt(paryIndex, nP);
						if (pTempPropIdx->pUnderLine == pSame)
							pTempPropIdx->pUnderLine = pCurUL;
					}
					SYTextProp_RemoveUnderLine(paryUL, nSameID);
				}
			}
		}
	}
}

 int SYTextProp_FindSameFont(SYArray* paryFont, LPCSYFont pCompFont, LPVOID* pSame)
{
	int nFontCnt = SYAR_GetCount(paryFont);
	for (int i = 0; i < nFontCnt; ++i)
	{
		LPSYFont pFont = (LPSYFont)SYAR_GetAt(paryFont, i);
		//查找相同者
		if (pCompFont != pFont && SYFont_Compare(pFont, pCompFont))
		{
			if (pSame != NULL)
				*pSame = pFont;
			return i;
		}
	}
	if (pSame != NULL)
		*pSame = NULL;
	return -1;
}

 int SYTextProp_FindSame2DColor(SYArray* paryColor, LPCSYStaticEffect pCompColor, LPVOID* pSame)
{
	int nFontCnt = SYAR_GetCount(paryColor);
	for (int i = 0; i < nFontCnt; ++i)
	{
		LPSYStaticEffect pColor = (LPSYStaticEffect)SYAR_GetAt(paryColor, i);
		//查找相同者
		if (pCompColor != pColor && sy_staticeffect_compare(pCompColor, pColor))
		{
			if (pSame != NULL)
				*pSame = pColor;
			return i;
		}
	}
	if (pSame != NULL)
		*pSame = NULL;
	return -1;
}

 int SYTextProp_FindSame3DColor(SYArray* paryColor, const SYMaterial* pCompColor, LPVOID* pSame)
{
	int nFontCnt = SYAR_GetCount(paryColor);
	for (int i = 0; i < nFontCnt; ++i)
	{
		SYMaterial* pColor = (SYMaterial*)SYAR_GetAt(paryColor, i);
		//查找相同者
		if (pCompColor != pColor && SYMaterial_Compare(pCompColor, pColor))
		{
			if (pSame != NULL)
				*pSame = pColor;
			return i;
		}
	}
	if (pSame != NULL)
		*pSame = NULL;
	return -1;
}

 int SYTextProp_FindSameUnderLine(SYArray* paryUnderLine, LPCSYUnderLine pCompUnderLine, LPVOID* pSame)
{
	int nUnderLineCnt = SYAR_GetCount(paryUnderLine);
	for (int i = 0; i < nUnderLineCnt; ++i)
	{
		LPSYUnderLine pUnderLine = (LPSYUnderLine)SYAR_GetAt(paryUnderLine, i);
		//查找相同者
		if (pCompUnderLine != pUnderLine && SYUnderLine_Compare(pCompUnderLine, pUnderLine))
		{
			if (pSame != NULL)
				*pSame = pUnderLine;
			return i;
		}
	}
	if (pSame != NULL)
		*pSame = NULL;
	return -1;
}

 void SYText2DColor_CopyValue(LPSYStaticEffect pDesColor, LPCSYStaticEffect pSrcColor, DWORD dwMask)
{
	//参数有效性由调用者检查
	if (dwMask & SYTEXTCOLORMASK_FACE)
	{
		memcpy(&pDesColor->face, &pSrcColor->face, sizeof(BaseGraph_DataBase::faceinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_BORDER)
	{
		memcpy(&pDesColor->border, &pSrcColor->border, sizeof(BaseGraph_DataBase::borderinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_SHADOW)
	{
		memcpy(&pDesColor->shadow, &pSrcColor->shadow, sizeof(BaseGraph_DataBase::shadowinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_CUBE)
	{
		memcpy(&pDesColor->cube, &pSrcColor->cube, sizeof(BaseGraph_DataBase::cubeinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_BORDERCUBE)
	{
		memcpy(&pDesColor->bordercube, &pSrcColor->bordercube, sizeof(BaseGraph_DataBase::cubeinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_BORDERSHADOW)
	{
		memcpy(&pDesColor->bordershadow, &pSrcColor->bordershadow, sizeof(BaseGraph_DataBase::shadowinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_FACELIGHT)
	{
		memcpy(&pDesColor->facelight, &pSrcColor->facelight, sizeof(BaseGraph_DataBase::lightinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_CUBELIGHT)
	{
		memcpy(&pDesColor->cubelight, &pSrcColor->cubelight, sizeof(BaseGraph_DataBase::lightinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_CHANNEL)
	{
		memcpy(&pDesColor->channel, &pSrcColor->channel, sizeof(BaseGraph_DataBase::channelinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_MATERIAL)
	{
		memcpy(&pDesColor->material, &pSrcColor->material, sizeof(BaseGraph_DataBase::materialinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_APPENDMATERIAL)
	{
		memcpy(&pDesColor->appendmaterial, &pSrcColor->appendmaterial, sizeof(BaseGraph_DataBase::materialinfo));
	}
	if (dwMask & SYTEXTCOLORMASK_RENDERLINE)
	{
		pDesColor->renderline = pSrcColor->renderline;
	}
	if (dwMask & SYTEXTCOLORMASK_WIDTH)
	{
		pDesColor->fwidth = pSrcColor->fwidth;
	}
	if (dwMask & SYTEXTCOLORMASK_INDEX)
	{
		pDesColor->index = pSrcColor->index;
	}

	if (dwMask & SYTEXTCOLORMASK_FACELIGHTCHANNEL)
	{
		sy_lightinfo_copy(&pDesColor->facelight, &pSrcColor->facelight);
	}
	if (dwMask & SYTEXTCOLORMASK_CUBELIGHTCHANNEL)
	{
		sy_lightinfo_copy(&pDesColor->cubelight, &pSrcColor->cubelight);
	}
	if (dwMask & SYTEXTCOLORMASK_ACTIVEATTRIBLE)
	{
		sy_dynamictexture_copy(&pDesColor->dyntex, &pSrcColor->dyntex);
	}
	if (dwMask & SYTEXTCOLORMASK_FACEADORNCHANNEL)
	{
		sy_lightinfo_copy(&pDesColor->facelight, &pSrcColor->facelight);
	}
	if (dwMask & SYTEXTCOLORMASK_CUBEADORNCHANNEL)
	{
		sy_lightinfo_copy(&pDesColor->cubelight, &pSrcColor->cubelight);
	}
	if (dwMask & SYTEXTCOLORMASK_MASKADORNCHANNEL)
	{
		sy_channelinfo_copy(&pDesColor->channel, &pSrcColor->channel);
	}
	if (dwMask & SYTEXTCOLORMASK_PATTANADORNCHANNEL)
	{
		sy_materialinfo_copy(&pDesColor->material, &pSrcColor->material);
	}
	if (dwMask & SYTEXTCOLORMASK_APPENDADORNCHANNEL)
	{
		sy_materialinfo_copy(&pDesColor->appendmaterial, &pSrcColor->appendmaterial);
	}
}

 BOOL SYTextProp_RemoveFont(SYArray* paryFont, int nIndex)
{
	LPSYFont pFont = (LPSYFont)SYAR_GetAt(paryFont, nIndex);
	if (pFont != NULL)
	{
		SYFont_Release(pFont);
		delete pFont;
		SYAR_RemoveAt(paryFont, nIndex, 1);
		return TRUE;
	}
	return FALSE;
}

 BOOL SYTextProp_RemoveFont(SYArray* paryFont, LPSYFont pFont)
{
	if (pFont != NULL)
	{
		int nCnt = SYAR_GetCount(paryFont);
		for (int i = 0; i < nCnt; ++i)
		{
			if (SYAR_GetAt(paryFont, i) == pFont)
			{
				SYFont_Release(pFont);
				delete pFont;
				SYAR_RemoveAt(paryFont, i, 1);
				return TRUE;
			}
		}
	}
	return FALSE;
}

 BOOL SYTextProp_Remove2DColor(SYArray* pary2DColor, int nIndex)
{
	LPSYStaticEffect p2DColor = (LPSYStaticEffect)SYAR_GetAt(pary2DColor, nIndex);
	if (p2DColor != NULL)
	{
		SYStaticEffect_Release(p2DColor);
		delete p2DColor;
		SYAR_RemoveAt(pary2DColor, nIndex, 1);
		return TRUE;
	}
	return FALSE;
}

 BOOL SYTextProp_Remove2DColor(SYArray* pary2DColor, LPSYStaticEffect p2DColor)
{
	if (p2DColor != NULL)
	{
		int nCnt = SYAR_GetCount(pary2DColor);
		for (int i = 0; i < nCnt; ++i)
		{
			if (SYAR_GetAt(pary2DColor, i) == p2DColor)
			{
				SYStaticEffect_Release(p2DColor);
				delete p2DColor;
				SYAR_RemoveAt(pary2DColor, i, 1);
				return TRUE;
			}
		}
	}
	return FALSE;
}

 BOOL SYTextProp_Remove3DColor(SYArray* pary3DColor, int nIndex)
{
	SYMaterial* p3DColor = (SYMaterial*)SYAR_GetAt(pary3DColor, nIndex);
	if (p3DColor != NULL)
	{
		SYMaterial_Release(p3DColor);
		delete p3DColor;
		SYAR_RemoveAt(pary3DColor, nIndex, 1);
		return TRUE;
	}
	return FALSE;
}

 BOOL SYTextProp_Remove3DColor(SYArray* pary3DColor, SYMaterial* p3DColor)
{
	if (p3DColor != NULL)
	{
		int nCnt = SYAR_GetCount(pary3DColor);
		for (int i = 0; i < nCnt; ++i)
		{
			if (SYAR_GetAt(pary3DColor, i) == p3DColor)
			{
				SYMaterial_Release(p3DColor);
				delete p3DColor;
				SYAR_RemoveAt(pary3DColor, i, 1);
				return TRUE;
			}
		}
	}
	return FALSE;
}

 BOOL SYTextProp_RemoveUnderLine(SYArray* paryUnderLine, int nIndex)
{
	LPSYUnderLine pUnderLine = (LPSYUnderLine)SYAR_GetAt(paryUnderLine, nIndex);
	if (pUnderLine != NULL)
	{
		SYUnderLine_Release(pUnderLine);
		delete pUnderLine;
		SYAR_RemoveAt(paryUnderLine, nIndex, 1);
		return TRUE;
	}
	return FALSE;
}

 BOOL SYTextProp_RemoveUnderLine(SYArray* paryUnderLine, LPSYUnderLine pUnderLine)
{
	if (pUnderLine != NULL)
	{
		int nCnt = SYAR_GetCount(paryUnderLine);
		for (int i = 0; i < nCnt; ++i)
		{
			if (SYAR_GetAt(paryUnderLine, i) == pUnderLine)
			{
				SYUnderLine_Release(pUnderLine);
				delete pUnderLine;
				SYAR_RemoveAt(paryUnderLine, i, 1);
				return TRUE;
			}
		}
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// SYTextPropIndex
 void SYTextPropIndex_Init(LPSYTextPropIndex pPropIdx)
{
	pPropIdx->pFont = NULL;
	pPropIdx->p2DColor = NULL;
	for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
		pPropIdx->p3DColor[n3D] = NULL;
	pPropIdx->pUnderLine = NULL;
}

 void SYTextPropIndex_Release(LPSYTextPropIndex pPropIdx)
{
}

 void SYTextPropIndex_Copy(LPSYTextPropIndex pDes, LPCSYTextPropIndex pSrc)
{
	pDes->pFont = pSrc->pFont;
	pDes->p2DColor = pSrc->p2DColor;
	for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
		pDes->p3DColor[n3D] = pSrc->p3DColor[n3D];
	pDes->pUnderLine = pSrc->pUnderLine;
}

 BOOL SYTextPropIndex_Compare(LPCSYTextPropIndex pPropIdx1, LPCSYTextPropIndex pPropIdx2)
{
	if (pPropIdx1 == pPropIdx2)
		return TRUE;
	if ((pPropIdx1 == NULL && pPropIdx2 != NULL) || (pPropIdx2 == NULL && pPropIdx1 != NULL))
		return FALSE;

	if (pPropIdx1->pFont != pPropIdx2->pFont
		|| pPropIdx1->p2DColor != pPropIdx2->p2DColor
		|| pPropIdx1->pUnderLine != pPropIdx2->pUnderLine)
	{
		return FALSE;
	}
	for (int n3D = 0; n3D < SYTEXT_SUBFACE_CNT; ++n3D)
	{
		if (pPropIdx1->p3DColor[n3D] != pPropIdx2->p3DColor[n3D])
			return FALSE;
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// SYArtTypeset
 void SYArtTypeset_Init(LPSYArtTypeset pArtWord)
{
	if (pArtWord != NULL)
	{
		pArtWord->dwVersion = 1;
		pArtWord->ePath = SYPATH_NULL;
		pArtWord->bRotate = TRUE;
		pArtWord->fPitch = 0.08f;
		SYAR_Init(&pArtWord->arHandlePointUp);
		SYAR_Init(&pArtWord->arHandlePointDown);
	}
}

 // 艺术字
 void SYArtTypeset_Create(LPSYArtTypeset pArt, eSYArtTypesetPath ePath)
{
	if (pArt == NULL)
		return;

	pArt->ePath = ePath;

	SYArray* parPoint[ARTPATHTYPECNT] = {&pArt->arHandlePointUp,
										&pArt->arHandlePointDown};
	for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
	{
		//控制点
		int nPointCnt = SYAR_GetCount(parPoint[ptIndex]);
		for (int i = 0; i < nPointCnt; ++i)
		{
			LPSYMaskHandle pHP = (LPSYMaskHandle)SYAR_GetAt(parPoint[ptIndex], i);
			delete pHP;
		}
		SYAR_RemoveAll(parPoint[ptIndex]);

		point2f ptCtrl;
		point2f ptCtrl_L, ptCtrl_R;
		LPSYMaskHandle pHandle;
		switch (ePath)
		{
		case SYPATH_ARC:
			{
				ptCtrl.x = -0.5;
				ptCtrl.y = 0/*0.5*/;
				pHandle = new CSYMaskHandle(ptCtrl);
				SYAR_Add(parPoint[ptIndex], pHandle);
				ptCtrl.x = 0;
				ptCtrl.y = -0.5;
				pHandle = new CSYMaskHandle(ptCtrl);
				SYAR_Add(parPoint[ptIndex], pHandle);
				ptCtrl.x = 0.5;
				ptCtrl.y = 0/*0.5*/;
				pHandle = new CSYMaskHandle(ptCtrl);
				SYAR_Add(parPoint[ptIndex], pHandle);
				break;
			}
		case SYPATH_BEZIER:
			{
				ptCtrl.x = -0.5;
				ptCtrl.y = 0/*0.5*/;
				pHandle = new CSYMaskHandle(ptCtrl);
				SYAR_Add(parPoint[ptIndex], pHandle);
				ptCtrl.x = 0.5;
				ptCtrl.y = 0/*0.5*/;
				pHandle = new CSYMaskHandle(ptCtrl);
				SYAR_Add(parPoint[ptIndex], pHandle);
				break;
			}
		case SYPATH_FLAT:
			{
				int iPos = 1;
				if (ptIndex == 1)
				{
					// 上下两条直线对称
					iPos = -1;
				}
				ptCtrl.x = -0.5f;
				ptCtrl.y = -0.5f * iPos;
				ptCtrl_L.x = -0.5f;
				ptCtrl_L.y = -0.5f * iPos;
				ptCtrl_R.x = -0.5f;
				ptCtrl_R.y = -0.5f * iPos;
				pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
				pHandle->m_bContinue = TRUE;
				SYAR_Add(parPoint[ptIndex], pHandle);
				ptCtrl.x = 0.5f;
				ptCtrl.y = -0.5f * iPos;
				ptCtrl_L.x = 0.5f;
				ptCtrl_L.y = -0.5f * iPos;
				ptCtrl_R.x = 0.5f;
				ptCtrl_R.y = -0.5f * iPos;
				pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
				pHandle->m_bContinue = TRUE;
				SYAR_Add(parPoint[ptIndex], pHandle);
				break;
			}
		case SYPATH_UPBEZIER:
			{
				// 上曲线下直线
				if (ptIndex == 0)
				{
					ptCtrl.x = -0.5f;
					ptCtrl.y = -0.5f;
					ptCtrl_L.x = -0.8f;
					ptCtrl_L.y = -0.2f;
					ptCtrl_R.x = -0.2f;
					ptCtrl_R.y = -0.8f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
					ptCtrl.x = 0.5f;
					ptCtrl.y = -0.5f;
					ptCtrl_L.x = 0.2f;
					ptCtrl_L.y = -0.8f;
					ptCtrl_R.x = 0.8f;
					ptCtrl_R.y = -0.2f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
				}
				else if (ptIndex == 1)
				{
					ptCtrl.x = -0.5f;
					ptCtrl.y = 0.5f;
					ptCtrl_L.x = -0.5f;
					ptCtrl_L.y = 0.5f;
					ptCtrl_R.x = -0.5f;
					ptCtrl_R.y = 0.5f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
					ptCtrl.x = 0.5f;
					ptCtrl.y = 0.5f;
					ptCtrl_L.x = 0.5f;
					ptCtrl_L.y = 0.5f;
					ptCtrl_R.x = 0.5f;
					ptCtrl_R.y = 0.5f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
				}
				break;
			}
		case SYPATH_DOWNBEZIER:
			{
				//上直线下曲线
				if (ptIndex == 0)
				{
					ptCtrl.x = -0.5f;
					ptCtrl.y = -0.5f;
					ptCtrl_L.x = -0.5f;
					ptCtrl_L.y = -0.5f;
					ptCtrl_R.x = -0.5f;
					ptCtrl_R.y = -0.5f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
					ptCtrl.x = 0.5f;
					ptCtrl.y = -0.5f;
					ptCtrl_L.x = 0.5f;
					ptCtrl_L.y = -0.5f;
					ptCtrl_R.x = 0.5f;
					ptCtrl_R.y = -0.5f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
				}
				else if (ptIndex == 1)
				{
					ptCtrl.x = -0.5f;
					ptCtrl.y = 0.5f;
					ptCtrl_L.x = -0.8f;
					ptCtrl_L.y = 0.8f;
					ptCtrl_R.x = -0.2f;
					ptCtrl_R.y = 0.2f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
					ptCtrl.x = 0.5f;
					ptCtrl.y = 0.5f;
					ptCtrl_L.x = 0.2f;
					ptCtrl_L.y = 0.2f;
					ptCtrl_R.x = 0.8f;
					ptCtrl_R.y = 0.8f;
					pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
					pHandle->m_bContinue = TRUE;
					SYAR_Add(parPoint[ptIndex], pHandle);
				}
				break;
			}
		case SYPATH_SYMMETRYBEZIER:
			{
				int iPos = 1;
				if (ptIndex == 1)
				{
					// 上下两条曲线对称
					iPos = -1;
				}
				ptCtrl.x = -0.5f;
				ptCtrl.y = -0.5f * iPos;
				ptCtrl_L.x = -0.8f;
				ptCtrl_L.y = -0.8f * iPos;
				ptCtrl_R.x = -0.2f;
				ptCtrl_R.y = -0.2f * iPos;
				pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
				pHandle->m_bContinue = TRUE;
				SYAR_Add(parPoint[ptIndex], pHandle);
				ptCtrl.x = 0.5f;
				ptCtrl.y = -0.5f * iPos;
				ptCtrl_L.x = 0.2f;
				ptCtrl_L.y = -0.2f * iPos;
				ptCtrl_R.x = 0.8f;
				ptCtrl_R.y = -0.8f * iPos;
				pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
				pHandle->m_bContinue = TRUE;
				SYAR_Add(parPoint[ptIndex], pHandle);
				break;
			}
		case SYPATH_SAMEBEZIER:
			{
				int iPos = 0;
				if (ptIndex == 1)
				{
					// 上下两条曲线平移
					iPos = 1;
				}
				ptCtrl.x = -0.5f;
				ptCtrl.y = -0.5f + iPos;
				ptCtrl_L.x = -0.8f;
				ptCtrl_L.y = -0.8f + iPos;
				ptCtrl_R.x = -0.2f;
				ptCtrl_R.y = -0.2f + iPos;
				pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
				pHandle->m_bContinue = TRUE;
				SYAR_Add(parPoint[ptIndex], pHandle);
				ptCtrl.x = 0.5f;
				ptCtrl.y = -0.5f + iPos;
				ptCtrl_L.x = 0.2f;
				ptCtrl_L.y = -0.2f + iPos;
				ptCtrl_R.x = 0.8f;
				ptCtrl_R.y = -0.8f + iPos;
				pHandle = new CSYMaskHandle(ptCtrl, ptCtrl_L, ptCtrl_R);
				pHandle->m_bContinue = TRUE;
				SYAR_Add(parPoint[ptIndex], pHandle);
				break;
			}
		}
	}
}

// 曲线文字
void SYArtTypeset_Create(LPSYArtTypeset pPath, CurveAr* pCurveAry, BOOL bRotate/* = TRUE*/)
{
	if (pPath == NULL || pCurveAry == NULL)
		return;

	pPath->bRotate = bRotate;

	SYArray* parPoint = &pPath->arHandlePointDown;
	//控制点
	int nPointCnt = SYAR_GetCount(parPoint);
	for (int i = 0; i < nPointCnt; ++i)
	{
		LPSYMaskHandle pHP = (LPSYMaskHandle)SYAR_GetAt(parPoint, i);
		SAFE_DELETE(pHP);
	}
	SYAR_RemoveAll(parPoint);

	int nCurvePtCnt = pCurveAry->GetCount();
	for (int i=0; i<nCurvePtCnt; i++)
	{
		curCtrl* pCtrl = pCurveAry->GetAt(i);
		LPSYMaskHandle pHandle = new CSYMaskHandle(pCtrl->basePt);
		pHandle->m_HelpPt[0] = pCtrl->LeftPt;
		pHandle->m_HelpPt[1] = pCtrl->RightPt;
		pHandle->m_Pt.y = -pHandle->m_Pt.y;
		pHandle->m_HelpPt[0].y = -pHandle->m_HelpPt[0].y;
		pHandle->m_HelpPt[1].y = -pHandle->m_HelpPt[1].y;
		SYAR_Add(parPoint, pHandle);
	}
}

 void SYArtTypeset_Release(LPSYArtTypeset pArt)
{
	if (pArt == NULL)
		return;

	SYArray* parPoint[ARTPATHTYPECNT] = {&pArt->arHandlePointUp,
										&pArt->arHandlePointDown};
	for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
	{
		int nPointCnt = SYAR_GetCount(parPoint[ptIndex]);
		for (int i = 0; i < nPointCnt; ++i)
		{
			LPSYMaskHandle pHP = (LPSYMaskHandle)SYAR_GetAt(parPoint[ptIndex], i);
			delete pHP;
		}
		SYAR_RemoveAll(parPoint[ptIndex]);
	}
}

 void SYArtTypeset_Copy(LPSYArtTypeset pDes, LPCSYArtTypeset pSrc)
{
	if (pDes == NULL || pSrc == NULL || pSrc == pDes)
		return;

	SYArtTypeset_Release(pDes);

	pDes->dwVersion = pSrc->dwVersion;
	pDes->ePath = pSrc->ePath;
	pDes->bRotate = pSrc->bRotate;
	pDes->fPitch = pSrc->fPitch;

	SYArray* parPointDes[ARTPATHTYPECNT] = {&pDes->arHandlePointUp,
											&pDes->arHandlePointDown};
	LPSYArtTypeset pSrc1 = const_cast<LPSYArtTypeset>(pSrc);
	SYArray* parPointSrc[ARTPATHTYPECNT] = {&pSrc1->arHandlePointUp,
											&pSrc1->arHandlePointDown};
	for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
	{
		int nHandleCnt = SYAR_GetCount(parPointSrc[ptIndex] );
		for (int i = 0; i < nHandleCnt; ++i)
		{
			LPSYMaskHandle pSrcHandle = (LPSYMaskHandle)SYAR_GetAt(parPointSrc[ptIndex], i);
			LPSYMaskHandle pDesHandle = new CSYMaskHandle(*pSrcHandle);
			SYAR_Add(parPointDes[ptIndex], pDesHandle);
		}
	}
}

 BOOL SYArtTypeset_Compare(LPCSYArtTypeset pArt1, LPCSYArtTypeset pArt2)
{
	if (pArt1 == pArt2)
		return TRUE;
	if ((pArt1 == NULL && pArt2 != NULL) || (pArt2 == NULL && pArt1 != NULL))
		return FALSE;

	if (pArt1->dwVersion != pArt2->dwVersion
		|| pArt1->bRotate != pArt2->bRotate
		|| pArt1->ePath != pArt2->ePath
		|| pArt1->fPitch != pArt2->fPitch)
	{
		return FALSE;
	}

	LPSYArtTypeset pArt11 = const_cast<LPSYArtTypeset>(pArt1);
	SYArray* parPoint1[ARTPATHTYPECNT] = {&pArt11->arHandlePointUp,
										&pArt11->arHandlePointDown};
	LPSYArtTypeset pArt21 = const_cast<LPSYArtTypeset>(pArt2);
	SYArray* parPoint2[ARTPATHTYPECNT] = {&pArt21->arHandlePointUp,
										&pArt21->arHandlePointDown};
	for (int ptIndex=0; ptIndex<ARTPATHTYPECNT; ptIndex++)
	{
		int nHandleCnt1 = SYAR_GetCount(parPoint1[ptIndex]);
		int nHandleCnt2 = SYAR_GetCount(parPoint2[ptIndex]);
		if (nHandleCnt1 != nHandleCnt2)
			return FALSE;
		for (int i = 0; i < nHandleCnt1; ++i)
		{
			LPSYMaskHandle pSrcHandle = (LPSYMaskHandle)SYAR_GetAt(parPoint1[ptIndex], i);
			LPSYMaskHandle pDesHandle = (LPSYMaskHandle)SYAR_GetAt(parPoint2[ptIndex], i);
			if (*pSrcHandle != *pDesHandle)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// SYGlyphSpace
 void SYGlyphSpace_ReleaseAll(SYArray& artGlyphSpace)
{
	int nCount = SYAR_GetCount(&artGlyphSpace);
	for(int i = 0; i < nCount; ++i)
	{
		LPSYGlyphSpace pSpace = (LPSYGlyphSpace)SYAR_GetAt(&artGlyphSpace, i);
		delete pSpace;
	}
	SYAR_RemoveAll(&artGlyphSpace);
}


//////////////////////////////////////////////////////////////////////////
 void SYStr_SplitByCRLF(LPCTSTR szString, CSYStrArray& arString)
{
	if (szString != NULL)
	{
		LPTSTR szText;
		int nLen = (int)_tcslen(szString);
		int nLastCRLF = 0;
		int nCharCnt;

		for (int nC = 0; nC < nLen; ++nC)
		{
			if (SYISCRLF(szString, nC, nLen))
			{
				nCharCnt = nC - nLastCRLF;
				szText = new TCHAR[nCharCnt + 1];
				if (nCharCnt > 0)
					memcpy(szText, szString + nLastCRLF, sizeof(TCHAR) * nCharCnt);
				szText[nCharCnt] = _T('\0');
				arString.Add(szText);
				++nC;
				nLastCRLF = nC + 1;
			}
		}

		nCharCnt = nLen - nLastCRLF;
		if (nCharCnt > 0)
		{
			szText = new TCHAR[nCharCnt + 1];
			memcpy(szText, szString + nLastCRLF, sizeof(TCHAR) * nCharCnt);
			szText[nCharCnt] = _T('\0');
			arString.Add(szText);
		}
	}
}

 void SYStr_SplitByChar(LPCTSTR szString, TCHAR cSplit, CSYStrArray& arString)
{
	if (szString != NULL)
	{
		LPTSTR szText;
		int nLen = (int)_tcslen(szString);
		int nLastSplit = 0;
		int nCharCnt;

		for (int nC = 0; nC < nLen; ++nC)
		{
			if (szString[nC] == cSplit)
			{
				nCharCnt = nC - nLastSplit;
				szText = new TCHAR[nCharCnt + 1];
				if (nCharCnt > 0)
					memcpy(szText, szString + nLastSplit, sizeof(TCHAR) * nCharCnt);
				szText[nCharCnt] = _T('\0');
				arString.Add(szText);
				nLastSplit = nC + 1;
			}
		}

		nCharCnt = nLen - nLastSplit;
		if (nCharCnt > 0)
		{
			szText = new TCHAR[nCharCnt + 1];
			memcpy(szText, szString + nLastSplit, sizeof(TCHAR) * nCharCnt);
			szText[nCharCnt] = _T('\0');
			arString.Add(szText);
		}
	}
}

//创建颜色字体Buffer
 BOOL SYCreateTextBuffer(LPDWORD pdwBuffer, int* pnWidth, int* pnHeight, 
									LPCTSTR szText, LPSYFont pFont, LPSYStaticEffect pColor, 
									double dScale)
{
	assert(szText && pFont && pColor);
	SYText text;
	SYText_Init(&text);
	rectf rtBorder;
	rtBorder.left = 0;
	rtBorder.right = 10;
	rtBorder.top = 0;
	rtBorder.bottom = 10;
	float fFontHeight = pFont->lfHeight;
	float fFontWidth = pFont->lfWidth;
	pFont->lfHeight = (float)(pFont->lfHeight * dScale);
	pFont->lfWidth = pFont->lfHeight / 2;
	SYText_Create(&text, 
		szText, 
		rtBorder, 
		pFont, 
		pColor, 
		NULL, 
		NULL, 
		TEXT_ALIGN_H_LEFT, 
		TEXT_ALIGN_V_BOTTOM, 
		TRUE, 
		TRUE, 
		TRUE, 
		FALSE, 
		FALSE);
	pFont->lfHeight = fFontHeight;
	pFont->lfWidth = fFontWidth;
	text.bUseBorderEditMode = TRUE;
	CSYTypeset typesetor;
	typesetor.Attach(&text);
	typesetor.Typeset(CSYTypeset::TTLReTypeset);

	rectf rtReal = text.rtTypesetReal;
	int nWidth = (int)(rtReal.right - rtReal.left);
	int nHeight = (int)(rtReal.bottom - rtReal.top);
	if (pnWidth)
		*pnWidth = nWidth;
	if (pnHeight)
		*pnHeight = nHeight;

	if (pdwBuffer)
	{
		int nCount = SYAR_GetCount(&text.ar2DUnit);
		for (int i = 0; i < nCount; i++)
		{
			SYUnit *pUnit = (SYUnit *)SYAR_GetAt(&text.ar2DUnit, i);

			CSYBuffer *pOutBuffer = NULL;
			RECT rtOutBound;
			int nOutWidth;
			int nOutHeight;
			int nOutPitch;
			pointf ptOutOrg;

			SYRenderGraphToOpBuff(&pUnit->u.u2d.graph, pColor, pOutBuffer, rtOutBound, nOutWidth, nOutHeight, nOutPitch, 1, ptOutOrg, 0);

			if (!pOutBuffer)
			{
				continue;
			}

			int nXOffset = (int)(pUnit->pos.x - ptOutOrg.x);
			int nYOffset = (int)(-pUnit->pos.y - ptOutOrg.y + (rtBorder.top - rtReal.top));

			SY_BitBlt(
				pdwBuffer, 
				nXOffset, 
				nYOffset, 
				nOutWidth, 
				nOutHeight, 
				nWidth, 
				nHeight, 
				pOutBuffer->m_dwBuffer, 
				0, 
				0, 
				nOutWidth, 
				nOutHeight, 
				nOutWidth, 
				nOutHeight, 
				BUFFER_MIX, 
				BUFFER_PARITY_BOTH
				);

			SYData_ReleaseCalcBuffer(pOutBuffer);
		}
	}

	SYText_Release(&text);
	//SY_WriteBimap(_T("D:\\1234.bmp"), nWidth, nHeight, 32, (BYTE *)pdwBuffer);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
void SYText_InitParagraphArray(LPSYText pText, E_TextAlignHorizontal eAlignHorizontal)
{
	assert(pText && pText->szText);
	std::vector<int> vParaIndex;
	SYText_SplitStringToParagraphArray(vParaIndex, pText->szText);

	LPSYTextParagraph pParagraph = NULL;
	int nParaCnt = (int)vParaIndex.size();
	for (int i = 0; i < nParaCnt; ++i)
	{
		pParagraph = new SYTextParagraph;
		pParagraph->dwVersion = 1;
		pParagraph->nBeginIndex = vParaIndex[i];
		pParagraph->eAlignHorizontal = eAlignHorizontal;
		pParagraph->fLineDistance = 0;
		pParagraph->fBeforeDistance = 0;
		pParagraph->fAfterDistance = 0;
		pParagraph->nLevel = 0;
		pParagraph->fIndent = 0;
		pParagraph->lpSymbol = new SYTextSymbol;
		SYText_InitSymbol(pParagraph->lpSymbol);
		pParagraph->hHandle = NULL;
		pParagraph->lpParam = NULL;
		SYAR_Add(&pText->arParagraph, pParagraph);
	}
}

void SYText_InitParagraph(LPSYTextParagraph pParagraph)
{
	assert(pParagraph);
	pParagraph->dwVersion = 1;
	pParagraph->nBeginIndex = 0;
	pParagraph->eAlignHorizontal = TEXT_ALIGN_H_LEFT;
	pParagraph->fLineDistance = 0;
	pParagraph->fBeforeDistance = 0;
	pParagraph->fAfterDistance = 0;
	pParagraph->nLevel = 0;
	pParagraph->fIndent = 0;
	pParagraph->lpSymbol = new SYTextSymbol;
	SYText_InitSymbol(pParagraph->lpSymbol);

	pParagraph->hHandle = NULL;
	pParagraph->lpParam = NULL;
}

void SYText_ReleaseParagraph(LPSYTextParagraph pParagraph)
{
	assert(pParagraph);
	if (pParagraph->lpSymbol)
	{
		SYText_ReleaseSymbol(pParagraph->lpSymbol);
		delete pParagraph->lpSymbol;
		pParagraph->lpSymbol = NULL;
	}
}

int SYText_SaveParagraph(LPSYTextParagraph pParagraph, LPBYTE pBuffer, int nLen)
{
	assert(pParagraph);

	int nBufferSize = 0;
	LPBYTE pCurBuf = pBuffer;

	nBufferSize += sizeof(DWORD);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->dwVersion, sizeof(DWORD));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(int);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->nBeginIndex, sizeof(int));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(E_TextAlignHorizontal);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->eAlignHorizontal, sizeof(E_TextAlignHorizontal));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(float);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->fLineDistance, sizeof(float));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(float);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->fBeforeDistance, sizeof(float));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(float);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->fAfterDistance, sizeof(float));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(int);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pParagraph->nLevel, sizeof(int));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(BOOL);
	BOOL bHasSymbol = (pParagraph->lpSymbol != NULL);
	if (pBuffer)
	{
		memcpy(pCurBuf, &bHasSymbol, sizeof(BOOL));
		pCurBuf = pBuffer + nBufferSize;
	}

	if (bHasSymbol)
	{
		nBufferSize += SYText_SaveSymbol(pParagraph->lpSymbol, pCurBuf, nLen - nBufferSize);
		if (pBuffer)
			pCurBuf = pBuffer + nBufferSize;
	}

	return nBufferSize;
}

int SYText_LoadParagraph(LPSYTextParagraph pParagraph, LPBYTE pBuffer, int nLen)
{
	assert(pParagraph && pBuffer);

	int nBufferSize = 0;
	LPBYTE pCurBuf = pBuffer;

	memcpy(&pParagraph->dwVersion, pCurBuf, sizeof(DWORD));
	nBufferSize += sizeof(DWORD);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pParagraph->nBeginIndex, pCurBuf, sizeof(int));
	nBufferSize += sizeof(int);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pParagraph->eAlignHorizontal, pCurBuf, sizeof(E_TextAlignHorizontal));
	nBufferSize += sizeof(E_TextAlignHorizontal);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pParagraph->fLineDistance, pCurBuf, sizeof(float));
	nBufferSize += sizeof(float);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pParagraph->fBeforeDistance, pCurBuf, sizeof(float));
	nBufferSize += sizeof(float);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pParagraph->fAfterDistance, pCurBuf, sizeof(float));
	nBufferSize += sizeof(float);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pParagraph->nLevel, pCurBuf, sizeof(int));
	nBufferSize += sizeof(int);
	pCurBuf = pBuffer + nBufferSize;

	BOOL bHasSymbol;
	memcpy(&bHasSymbol, pCurBuf, sizeof(BOOL));
	nBufferSize += sizeof(BOOL);
	pCurBuf = pBuffer + nBufferSize;
	if (bHasSymbol)
	{
		SAFE_DELETE(pParagraph->lpSymbol);
		pParagraph->lpSymbol = new SYTextSymbol;
		SYText_InitSymbol(pParagraph->lpSymbol);
		nBufferSize += SYText_LoadSymbol(pParagraph->lpSymbol, pCurBuf, nLen - nBufferSize);
		pCurBuf = pBuffer + nBufferSize;
	}

	return nBufferSize;
}

void SYText_CopyParagraph(LPSYTextParagraph pDes, LPSYTextParagraph pSrc)
{
	if (pDes->lpSymbol != NULL)
	{
		SYText_ReleaseSymbol(pDes->lpSymbol);
		delete pDes->lpSymbol;
		pDes->lpSymbol = NULL;
	}
	memcpy(pDes, pSrc, sizeof(SYTextParagraph));
	pDes->lpSymbol = new SYTextSymbol;
	SYText_InitSymbol(pDes->lpSymbol);
	SYText_CopySymbol(pDes->lpSymbol, pSrc->lpSymbol);
}

BOOL SYText_CompareParagraph(LPSYTextParagraph pParagraph1, LPSYTextParagraph pParagraph2)
{
	if (pParagraph1->dwVersion != pParagraph2->dwVersion
		|| pParagraph1->nBeginIndex != pParagraph2->nBeginIndex
		|| pParagraph1->eAlignHorizontal != pParagraph2->eAlignHorizontal
		|| pParagraph1->fLineDistance != pParagraph2->fLineDistance
		|| pParagraph1->fBeforeDistance != pParagraph2->fLineDistance
		|| pParagraph1->nLevel != pParagraph2->nLevel
		|| pParagraph1->fIndent != pParagraph1->fIndent)
		return FALSE;

	return TRUE;
}

void SYText_InitSymbol(LPSYTextSymbol pSymbol)
{
	assert(pSymbol);
	pSymbol->dwVersion = 1;
	SYStaticEffect_Init(&pSymbol->seFillColor);
	pSymbol->rtSize.left = 0;
	pSymbol->rtSize.top = 0;
	pSymbol->rtSize.right = 0;
	pSymbol->rtSize.bottom = 0;
	pSymbol->fScale = 1;

	pSymbol->hHandle = NULL;
	pSymbol->lpParam = NULL;
}

void SYText_ReleaseSymbol(LPSYTextSymbol pSymbol)
{
	assert(pSymbol);
	SYStaticEffect_Release(&pSymbol->seFillColor);
}

int SYText_SaveSymbol(LPSYTextSymbol pSymbol, LPBYTE pBuffer, int nLen)
{
	assert(pSymbol);

	int nBufferSize = 0;
	LPBYTE pCurBuf = pBuffer;

	nBufferSize += sizeof(DWORD);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pSymbol->dwVersion, sizeof(DWORD));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(static_effect);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pSymbol->seFillColor, sizeof(static_effect));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(rectf);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pSymbol->rtSize, sizeof(rectf));
		pCurBuf = pBuffer + nBufferSize;
	}

	nBufferSize += sizeof(float);
	if (pBuffer)
	{
		memcpy(pCurBuf, &pSymbol->fScale, sizeof(float));
		pCurBuf = pBuffer + nBufferSize;
	}

	return nBufferSize;
}

int SYText_LoadSymbol(LPSYTextSymbol pSymbol, LPBYTE pBuffer, int nLen)
{
	assert(pSymbol && pBuffer);

	int nBufferSize = 0;
	LPBYTE pCurBuf = pBuffer;

	memcpy(&pSymbol->dwVersion, pCurBuf, sizeof(DWORD));
	nBufferSize += sizeof(DWORD);
	pCurBuf = pBuffer + nBufferSize;
	
	if(0)
	{
		typedef struct tag_mydynamictexture
		{
			TCHAR szPath[512];
			float speed;
			float offsetx;
			float offsety;
			float vRunDirx;
			float vRunDiry;
			float scalex;
			float scaley;
			float centerx;
			float centery;
			float rotate;
			float alpha;
			BOOL bRepeat;
		}mydynamictexture;
		typedef struct tag_mystaticeffect
		{
			BaseGraph_DataBase::faceinfo			face;
			BaseGraph_DataBase::borderinfo	        border;
			BaseGraph_DataBase::shadowinfo	        shadow;
			BaseGraph_DataBase::cubeinfo			cube;
			BaseGraph_DataBase::cubeinfo			bordercube;
			BaseGraph_DataBase::shadowinfo			bordershadow;

			BaseGraph_DataBase::lightinfo			facelight;
			BaseGraph_DataBase::lightinfo			cubelight;

			BaseGraph_DataBase::channelinfo			channel;
			BaseGraph_DataBase::materialinfo		material;
			BaseGraph_DataBase::materialinfo		appendmaterial;
			mydynamictexture	dyntex;
			unsigned long		renderline;
			float		        fwidth;
			int			        index;
		}mystatic_effect;
		mystatic_effect seFillColor;
		memcpy(&seFillColor, pCurBuf, sizeof(mystatic_effect));
		nBufferSize += sizeof(mystatic_effect);
		pCurBuf = pBuffer + nBufferSize;
		pSymbol->seFillColor.face=seFillColor.face;
		pSymbol->seFillColor.border=seFillColor.border;
		pSymbol->seFillColor.shadow=seFillColor.shadow;
		pSymbol->seFillColor.cube=seFillColor.cube;
		pSymbol->seFillColor.bordercube=seFillColor.bordercube;
		pSymbol->seFillColor.bordershadow=seFillColor.bordershadow;
		pSymbol->seFillColor.facelight=seFillColor.facelight;
		pSymbol->seFillColor.cubelight=seFillColor.cubelight;
		pSymbol->seFillColor.channel=seFillColor.channel;
		pSymbol->seFillColor.material=seFillColor.material;
		pSymbol->seFillColor.renderline=seFillColor.renderline;
		pSymbol->seFillColor.fwidth=seFillColor.fwidth;
		pSymbol->seFillColor.index=seFillColor.index;
	}
	else
	{
		memcpy(&pSymbol->seFillColor, pCurBuf, sizeof(static_effect));
		nBufferSize += sizeof(static_effect);
		pCurBuf = pBuffer + nBufferSize;
	}

	memcpy(&pSymbol->rtSize, pCurBuf, sizeof(rectf));
	nBufferSize += sizeof(rectf);
	pCurBuf = pBuffer + nBufferSize;

	memcpy(&pSymbol->fScale, pCurBuf, sizeof(float));
	nBufferSize += sizeof(float);
	pCurBuf = pBuffer + nBufferSize;

	return nBufferSize;
}

void SYText_CopySymbol(LPSYTextSymbol pDes, LPSYTextSymbol pSrc)
{
	pDes->dwVersion = pSrc->dwVersion;
	SYStaticEffect_Copy(&pDes->seFillColor, &pSrc->seFillColor);
	pDes->rtSize = pSrc->rtSize;
	pDes->fScale = pSrc->fScale;

	pDes->hHandle = pSrc->hHandle;
	pDes->lpParam = pSrc->lpParam;
}
////////////////////////////////////////////////////////////////////////
void TypesetInterface_Lock(LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		ObjectCalculator * pCalc = (ObjectCalculator *)pInterfaceParam;
		pCalc->Lock();
	}
}

void TypesetInterface_UnLock(LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		ObjectCalculator * pCalc = (ObjectCalculator *)pInterfaceParam;
		pCalc->Unlock();
	}
}

void TypesetInterface_OnAddUnit(BOOL bKeepBalance, LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		CalculatorTitleword* pTitleCalc = (CalculatorTitleword*)pInterfaceParam;
		pTitleCalc->Lock();

		LPSYText pText = pTitleCalc->GetText();
		SYItemRenderUnit* pRenderUnit = CreateRenderUnit(FALSE);
		SYAR_Add(pTitleCalc->Get2DTextUnitArray(), pRenderUnit);

		if (pText->pStretch != NULL && pText->pStretch->bEnable && pText->pStretch->fThick > ABS_FLOAT_0)
		{
			assert(false);
			//djt 3D先不管
			//SYItemRenderUnit* pRenderUnit = CreateRenderUnit(TRUE);
			//int nIndex = pItem->Get3DTextUnitCount(pEnv) - 1;
			//if (nIndex >= 0)
			//{
			//	SYUnit* pTextUnit = pItem->Get3DTextUnit(nIndex, pEnv);
			//	pRenderUnit->m_pUnit = new SYUnit;
			//	memset(pRenderUnit->m_pUnit,0,sizeof(SYUnit));
			//	sy_copy_unit(pRenderUnit->m_pUnit, pTextUnit);
			//	CopyVertexFromItemToEnv(&pRenderUnit->unit3D, pEnv->GetDevice(), sizeof(SYVertex3D), pTextUnit->u.u3d.pmod, pEnv->GetChangedLevel());

			//	SYAR_Add(pEnv->Get3DTextRenderUnitArray(), pRenderUnit);
			//	RefreshUnitMaterial(pRenderUnit, pEnv->GetDevice(), pEnv, pEnv->m_dCurFrame, nIndex, FALSE);
			//	pRenderUnit->m_bChanged = TRUE;
			//}
		}

		pTitleCalc->Unlock();
	}
}

void TypesetInterface_OnInsertUnit(int nIndex, int nCount, BOOL bKeepBalance, LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		CalculatorTitleword* pTitleCalc = (CalculatorTitleword*)pInterfaceParam;
		pTitleCalc->Lock();

		int i;
		int nPreIndex = nIndex - 1;
		LPSYText pText = pTitleCalc->GetText();
		BOOL bStretch = pText->pStretch != NULL && pText->pStretch->bEnable && pText->pStretch->fThick > ABS_FLOAT_0;

#if 0
		//djt action先不管
		//Action维护
		if (nPreIndex >= 0)
		{
			SYArray* paryActor = &(pItem->m_ClipInfo.ActorArray);
			int nActorCnt = SYAR_GetCount(paryActor);
			for (i=0; i<nActorCnt; ++i)
			{
				CSYCGActor* pActor = (CSYCGActor*)SYAR_GetAt(paryActor, i);
				int nActSize = pActor->GetActionCount();
				SYList* pactList = &(pActor->GetActorData()->actions);

				LISTPOSITION cur_pos = SYList_GetHeadPosition(pactList);
				while(cur_pos)
				{
					CSYAction* pAction = (CSYAction*)SYList_GetNext(pactList, &cur_pos);
					SYUnitParam* pPreUnitParam = pAction->GetUnitParam(nPreIndex);
					//如何区分这个Unit是属于哪种Unit的？
					//插入位置的Unit有关键帧
					if (pPreUnitParam != NULL)
					{
						//复制
						if (bStretch)
						{
							SYArray* pary3DText = pItem->Get3DTextUnitArray();
							for (int j=0; j<nCount; ++j)
							{
								SYUnitParam* param = new SYUnitParam;
								pAction->CopyUnitParam(param, pPreUnitParam);
								SYUnit* pTextUnit = (SYUnit*)SYAR_GetAt(pary3DText, nIndex + j);
								SYGuid_To_Str(param->UnitID, pTextUnit->UnitID);
								pAction->AddUnitParam(param, nIndex + j);
							}
						}
						else
						{
							SYArray* pary2DText = pItem->Get2DTextUnitArray();
							for (int j=0; j<nCount; ++j)
							{
								SYUnitParam* param = new SYUnitParam;
								pAction->CopyUnitParam(param, pPreUnitParam);
								SYUnit* pTextUnit = (SYUnit*)SYAR_GetAt(pary2DText, nIndex + j);
								SYGuid_To_Str(param->UnitID, pTextUnit->UnitID);
								pAction->AddUnitParam(param, nIndex + j);
							}
						}
					}
				}
			}
		}
#endif

		SYArray* pary2DTextRenderUnit = pTitleCalc->Get2DTextUnitArray();
		for (i = 0; i < nCount; i++)
		{
			SYItemRenderUnit* pRenderUnit = CreateRenderUnit(FALSE);
			pRenderUnit->m_bChanged = TRUE;
			SYAR_InsertAt(pary2DTextRenderUnit, nIndex + i, pRenderUnit, 1);
		}

#if 0
		//djt 3D先不管
		if (bStretch)
		{
			int nDevice = pEnv->GetDevice();
			int nEnvChgLevel = pEnv->GetChangedLevel();
			SYArray* pary3DTextRenderUnit = pEnv->Get3DTextRenderUnitArray();
			for (i = 0; i < nCount; i++)
			{
				SYItemRenderUnit* pRenderUnit = CreateRenderUnit(TRUE);
				SYUnit* pTextUnit = pItem->Get3DTextUnit(nIndex + i, pEnv);
				if (pTextUnit != NULL)
				{
					pRenderUnit->m_pUnit = new SYUnit;
					sy_init_unit(pRenderUnit->m_pUnit,TRUE);
					sy_copy_unit(pRenderUnit->m_pUnit, pTextUnit);
					CopyVertexFromItemToEnv(&pRenderUnit->unit3D, nDevice, sizeof(SYVertex3D), pTextUnit->u.u3d.pmod, nEnvChgLevel);

					SYAR_InsertAt(pary3DTextRenderUnit, nIndex + i, pRenderUnit, 1);
					RefreshUnitMaterial(pRenderUnit, nDevice, pEnv, pEnv->m_dCurFrame, nIndex + i, FALSE);
					pRenderUnit->m_bChanged = TRUE;
				}
			}
		}
#endif

		pTitleCalc->Unlock();
	}
}

void TypesetInterface_OnDeleteUnit(int nIndex, int nCount, BOOL bKeepBalance, LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		CalculatorTitleword* pTitleCalc = (CalculatorTitleword*)pInterfaceParam;
		pTitleCalc->Lock();
#if 0
		//djt action先不管
		//Action维护
		SYArray* paryActor = &(pItem->m_ClipInfo.ActorArray);
		int nActorCnt = SYAR_GetCount(paryActor);
		for (i=0; i<nActorCnt; ++i)
		{
			CSYCGActor* pActor = (CSYCGActor*)SYAR_GetAt(paryActor, i);
			int nActSize = pActor->GetActionCount();
			SYList* pactList = &(pActor->GetActorData()->actions);

			LISTPOSITION cur_pos = SYList_GetHeadPosition(pactList);
			while(cur_pos)
			{
				CSYAction* pAction = (CSYAction*)SYList_GetNext(pactList, &cur_pos);
				//删除Action
				for (j = nCount-1; j >= 0; --j)
				{
					pAction->DelUnitParam(nIndex + j);
				}
			}
		}
#endif

		LPSYText pText = pTitleCalc->GetText();
		SYArray* paryRenderUnit = pTitleCalc->Get2DTextUnitArray();
		for (int i = 0; i < nCount; i++)
		{
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, nIndex + i);
			if (pRenderUnit)
			{
				ReleaseRenderUnit(pRenderUnit, FALSE, DX3DEngine_GetMainDevice(), pTitleCalc);
				SAFE_DELETE(pRenderUnit);
			}
		}
		SYAR_RemoveAt(paryRenderUnit, nIndex, nCount);

		if (pText->pStretch != NULL && pText->pStretch->bEnable && pText->pStretch->fThick > ABS_FLOAT_0)
		{
			assert(false);
#if 0
			//djt 3D先不管
			paryRenderUnit = pEnv->Get3DTextRenderUnitArray();
			for (i = 0; i < nCount; i++)
			{
				SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, nIndex + i);
				ReleaseRenderUnit(pRenderUnit, TRUE, pEnv->GetDevice(), pEnv);
				delete pRenderUnit;
			}
			SYAR_RemoveAt(paryRenderUnit, nIndex, nCount);
#endif
		}

		pTitleCalc->Unlock();
	}
}

void TypesetInterface_OnDeleteAllUnit(LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		CalculatorTitleword* pTitleCalc = (CalculatorTitleword*)pInterfaceParam;
		pTitleCalc->Lock();

		int i;
#if 0
		//djt
		//Action维护
		SYArray* paryActor = &(pItem->m_ClipInfo.ActorArray);
		int nActorCnt = SYAR_GetCount(paryActor);
		for (i=0; i<nActorCnt; ++i)
		{
			CSYCGActor* pActor = (CSYCGActor*)SYAR_GetAt(paryActor, i);
			int nActSize = pActor->GetActionCount();
			SYList* pactList = &(pActor->GetActorData()->actions);

			LISTPOSITION cur_pos = SYList_GetHeadPosition(pactList);
			while(cur_pos)
			{
				CSYAction* pAction = (CSYAction*)SYList_GetNext(pactList, &cur_pos);
				//删除Action
				pAction->DelAllUnitParam();
			}
		}
#endif

		LPSYText pText = pTitleCalc->GetText();
		SYArray* paryRenderUnit = pTitleCalc->Get2DTextUnitArray();
		int nRenderUnitCount = SYAR_GetCount(paryRenderUnit);
		for (i = 0; i < nRenderUnitCount; i++)
		{
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, i);
			ReleaseRenderUnit(pRenderUnit, FALSE, DX3DEngine_GetMainDevice(), pTitleCalc);
			delete pRenderUnit;
		}
		SYAR_RemoveAll(paryRenderUnit);

		if (pText->pStretch  && pText->pStretch->bEnable && pText->pStretch->fThick > ABS_FLOAT_0)
		{
			assert(false);
#if 0
			//djt
			paryRenderUnit = pEnv->Get3DTextRenderUnitArray();
			nRenderUnitCount = SYAR_GetCount(paryRenderUnit);
			for (i = 0; i < nRenderUnitCount; i++)
			{
				SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, i);
				ReleaseRenderUnit(pRenderUnit, TRUE, pEnv->GetDevice(), pEnv);
				delete pRenderUnit;
			}
			SYAR_RemoveAll(paryRenderUnit);
#endif
		}

		pTitleCalc->Unlock();
	}
}

void TypesetInterface_OnRefreshUnit(int nIndex, int nCount, LPVOID pInterfaceParam)
{
	if (pInterfaceParam != NULL)
	{
		CalculatorTitleword* pTitleCalc = (CalculatorTitleword*)pInterfaceParam;
		pTitleCalc->Lock();

		int nEndIdx = nIndex + nCount;
		LPSYText pText = pTitleCalc->GetText();
		SYArray* paryRenderUnit = pTitleCalc->Get2DTextUnitArray();
		for (int i = nIndex; i < nEndIdx; ++i)
		{
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, i);
			if (pRenderUnit != NULL)
				pRenderUnit->m_bChanged = TRUE;
		}

		if (pText->pStretch != NULL  && pText->pStretch->bEnable && pText->pStretch->fThick > ABS_FLOAT_0)
		{
			assert(false);
#if 0
			//djt
			paryRenderUnit = pEnv->Get3DTextRenderUnitArray();
			for (int i = nIndex; i < nEndIdx; ++i)
			{
				SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, i);
				if (pRenderUnit != NULL)
				{
					pRenderUnit->m_bChanged = TRUE;

					int nModelCnt = pRenderUnit->unit3D.nSubModelCount;
					for(int nSub = 0; nSub < nModelCnt; nSub++)
					{
						SYVideoMaterial* pVideoMat = (SYVideoMaterial*)pRenderUnit->unit3D.pSubModel[nSub].pVideoMaterial;
						SYMaterial_SetChangeType(pVideoMat, SYMaterial_PhyDataChange);
					}
				}
			}
#endif
		}

		pTitleCalc->Unlock();
	}
}

void TypesetInterface_OnRefreshAllUnit(LPVOID pInterfaceParam, int nLevel)
{
	if (pInterfaceParam != NULL)
	{
		CalculatorTitleword* pTitleCalc = (CalculatorTitleword*)pInterfaceParam;
		pTitleCalc->Lock();

		LPSYText pText = pTitleCalc->GetText();
		SYArray* paryRenderUnit = pTitleCalc->Get2DTextUnitArray();
		int nRenderUnitCount = SYAR_GetCount(paryRenderUnit);

		for (int i = 0; i < nRenderUnitCount; i++)
		{
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, i);
			if (pRenderUnit != NULL)
			{
				pRenderUnit->m_bChanged = TRUE;
				ReleaseRenderUnit(pRenderUnit, FALSE, DX3DEngine_GetMainDevice(), pTitleCalc);
			}
		}

		if (pText->pStretch != NULL && pText->pStretch->fThick > ABS_FLOAT_0)
		{
			assert(false);
#if 0
			//djt
			paryRenderUnit = pEnv->Get3DTextRenderUnitArray();
			nRenderUnitCount = SYAR_GetCount(paryRenderUnit);
			for (int i = 0; i < nRenderUnitCount; i++)
			{
				SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)SYAR_GetAt(paryRenderUnit, i);
				ReleaseRenderUnit(pRenderUnit, TRUE, pEnv->GetDevice(),pEnv);
				pRenderUnit->m_bChanged = TRUE;
			}
#endif
		}

		pTitleCalc->Unlock();
	}
}

void SYTypesetInterface_InitItemInterface()
{
	g_TypesetInterface.Lock = TypesetInterface_Lock;
	g_TypesetInterface.UnLock = TypesetInterface_UnLock;
	g_TypesetInterface.OnAddUnit = TypesetInterface_OnAddUnit;
	g_TypesetInterface.OnInsertUnit = TypesetInterface_OnInsertUnit;
	g_TypesetInterface.OnDeleteUnit = TypesetInterface_OnDeleteUnit;
	g_TypesetInterface.OnDeleteAllUnit = TypesetInterface_OnDeleteAllUnit;
	g_TypesetInterface.OnRefreshUnit = TypesetInterface_OnRefreshUnit;
	g_TypesetInterface.OnRefreshAllUnit = TypesetInterface_OnRefreshAllUnit;

	g_TypesetInterface.Modify2DUnitTo3DUnit = Modify3DUnitFrom2DUnit;

	g_TypesetInterface.bInitialized = TRUE;
}

void SYTypeset_InitInterfaceEnv()
{
	//USP初始化
	CSYUsp::Initialize(LOCALE_USER_DEFAULT, SYIsEnableFeedBackFont());
	//Item类的接口
	SYTypesetInterface_InitItemInterface();
}
