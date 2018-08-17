#ifndef _SYTYPESETDATAAPI_INC_
#define _SYTYPESETDATAAPI_INC_
#include <vector>
#include "DataRender\DrawTextDef.h"
#include "DataRender\MaterialDef.h"

typedef CBaseARray<curCtrl*,curCtrl*> CurveAr;

//////////////////////////////////////////////////////////////////////////
/************************SYText*******************************/
 void	SYText_Init(LPCommonText pText);
 void	SYText_Create(LPCommonText pText, 
					LPCTSTR szText, 
					rectf rtBorder,
					LPCommonFont pFont,					//字体
					LPCommonStaticEffect pFillColor,		//2D色
					SYMaterial* p3DColor,				//3D色
					LPCommonUnderLine pUnderLine,			//下划线
					E_TextAlignHorizontal alignH, 
					E_TextAlignVeritical alignV, 
					BOOL bHorizontal, 
					BOOL bLeftInput, 
					BOOL bVLeftTypeset, 
					BOOL bLineWrap, 
					BOOL bFixedFrame);
 void	BASEGRAPH_DATARENDER_API SYText_Release(LPCommonText pText);
 void	SYText_Copy(LPCommonText pDes, LPCBaseText pSrc, BOOL bCopyNullText);
 BOOL	SYText_Compare(LPCBaseText pText1, LPCBaseText pText2, BOOL bNullTextEq);
 void	SYText_CopyProp(LPCommonTextProp& pDesProp,			//目标SYTextProp，NULL则内部new。
					LPCBaseText pSrcText,				//源SYText
					int nDesLen = -1);				//目标长度
//更新字的3D拉伸属性
 void	SYText_UpdateStretchParam(LPCommonText pText, LPCommonStretchParam pStretch);
//整体渲染属性
 void	SYText_SetWholeRender(LPCommonText pText, BOOL bWholeRender);
 BOOL	SYText_IsWholeRender(LPCommonText pText);
//根据字符串拆分段落
 void	SYText_SplitStringToParagraphArray(std::vector<int>& vParaIndex, LPCTSTR szText);

 void	BASEGRAPH_DATARENDER_API SYText_GetStaticEffectProperty(void* pProp, LPCommonStaticEffect pFillColor, int nIndex);
 void	SYText_GetFontProperty(void* pProp, LPCommonFont pFont, int nIndex);
 //查（翻转）表法计算CRC32，可以分块计算。
 unsigned long	SYCRC32Cal(unsigned long ulCRC32, unsigned char *pBuf, unsigned long ulLen);

 void	BASEGRAPH_DATARENDER_API SYText_SetParagraphParam(LPCommonText pTex, 
							 int nStart, 
							 int nCount, 
							 E_TextPropertyType eType, 
							 LPVOID pPropertyData);
//用指定的大小修改排版矩形
 void	SYText_ModifyTypesetBorderBySize(LPCommonText pText, SYSizef& szRectSize);
//设置固定字框
 void	BASEGRAPH_DATARENDER_API SYText_SetFixedFrame(LPCommonText pText, BOOL bFixedFrame);

/*******************TypesetPrivateData************************/
 void	SYTypesetPrivateData_Init(LPCommonTypesetPrivate pData);
 void	SYTypesetPrivateData_Release(LPCommonTypesetPrivate pData);

/************************SYFont*******************************/
 void	BASEGRAPH_DATARENDER_API SYFont_Init(LPCommonFont pFont);
 void	SYFont_Release(LPCommonFont pFont);
 int	SYFont_Save(LPCBaseFont pFont, LPBYTE pBuffer, int nLen);
 int	SYFont_Load(LPCommonFont pFont, LPBYTE pBuffer, int nLen);
 void	BASEGRAPH_DATARENDER_API SYFont_Copy(LPCommonFont pDes, LPCBaseFont pSrc);
 BOOL	SYFont_Compare(LPCBaseFont pFont1, LPCBaseFont pFont2);
//根据掩码设置字体子属性
 void	SYFont_SetValue(LPCommonFont pDesFont, const LPVOID lpSrcData, const LPVOID lp2ndSrcData, DWORD dwMask);
//根据掩码拷贝字体子属性
 void	SYFont_CopyValue(LPCommonFont pDesFont, LPCBaseFont pSrcFont, DWORD dwMask);

/***********************SYUnderLine***************************/
 void	BASEGRAPH_DATARENDER_API SYUnderLine_Init(LPCommonUnderLine pUnderLine);
 void	SYUnderLine_Create(LPCommonUnderLine pUnderLine, E_TextUnderLine eType, float fYPos,float fLineWidth,LPCommonStaticEffect p2DColor);
 void	SYUnderLine_Release(LPCommonUnderLine pUnderLine);
 void	SYUnderLine_Copy(LPCommonUnderLine pDes, LPCBaseUnderLine pSrc);
 BOOL	SYUnderLine_Compare(LPCBaseUnderLine pUnderLine1, LPCBaseUnderLine pUnderLine2);
//根据掩码设置下划线子属性
 void	SYUnderLine_SetValue(LPCommonUnderLine pDesUnderLine, const LPVOID lpSrcData, DWORD dwMask);
//根据掩码拷贝下划线子属性
 void	SYUnderLine_CopyValue(LPCommonUnderLine pDesUnderLine, LPCBaseUnderLine pSrcUnderLine, DWORD dwMask);

/************************SYTextProp***************************/
 void	SYTextProp_Init(LPCommonTextProp pProp);
 void	SYTextProp_Create(int nTextLen,LPCommonTextProp pProp,LPCommonFont pFont, LPCommonStaticEffect p2DColor,SYMaterial* p3DColor,LPCommonUnderLine pUnderLine);
 void	SYTextProp_Release(LPCommonTextProp pProp);
 void	SYTextProp_Copy(LPCommonTextProp pDes, LPCBaseTextProp pSrc, int nDesLen = -1);
 BOOL	SYTextProp_Compare(LPCBaseTextProp pProp1, LPCBaseTextProp pProp2);
//删除/插入字符串时维护SYTextProp
 void	SYTextProp_DeleteString(LPCommonTextProp pProp, int nCharIdx, int nDelCnt, int nTextLen, BOOL bWholeRender);
 void	SYTextProp_InsertString(LPCommonTextProp pProp, int nCharIdx, int nInsCnt, int nTextLen);

//取指定字符的字体、2D颜色、3D颜色或者下划线属性
 LPVOID	SYTextProp_GetProp(LPCommonTextProp pProp,  int nCharIdx,  E_TextPropType eType);
//取指定字符属性索引结构（索引越界时取边界值）
 LPCommonTextPropIndex	SYTextProp_GetPropIndex(BaseARray* paryIndex, int nCharIdx);
//删除一个属性索引
 BOOL	SYTextProp_EmptyPropIndex(LPCommonTextProp pProp, LPCommonTextPropIndex pPropIdx, BOOL bReserve2DClr);
//属性表长度调整
 void	BASEGRAPH_DATARENDER_API SYTextProp_ResizePropIndexArray(LPCommonTextProp pProp, int nSize, BOOL bKeepProperty);
//取字体属性
 LPCommonFont	BASEGRAPH_DATARENDER_API SYTextProp_GetFont(LPCommonTextProp pProp, int nCharIdx);
//取2D颜色
 LPCommonStaticEffect	BASEGRAPH_DATARENDER_API SYTextProp_GetStaticEffect(LPCommonTextProp pProp, int nCharIdx);
//取指定子面的3D颜色
 BASEGRAPH_DATARENDER_API SYMaterial* SYTextProp_Get3DColor(LPCommonTextProp pProp, int nCharIdx, int nSubFace = -1);
//取所有子面的3D颜色
 LPVOID*		SYTextProp_Get3DColorArray(LPCommonTextProp pProp, int nCharIdx);
//取下划线
 LPCommonUnderLine	BASEGRAPH_DATARENDER_API SYTextProp_GetUnderLine(LPCommonTextProp pProp, int nCharIdx);
//设置字体属性，nCount为-1时，设置全部。
 void	SYTextProp_SetFont(LPCommonTextProp pProp, int nCharIdx, int nCount, LPCBaseFont lpFont, DWORD dwMask = SYTEXTFONTMASK_ALL);
 void	SYTextProp_FontProperty(LPCommonTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, const LPVOID lp2ndPropData, DWORD dwMask);
//设置2D颜色，nCount为-1时，设置全部。
 void	SYTextProp_SetStaticEffect(LPCommonTextProp pProp, int nIndex, int nCharIdx, LPCBaseStaticEffect lpFillColor, DWORD dwMask = SYTEXTCOLORMASK_ALL);
//设置3D颜色，nCount为-1时，设置全部。
 void	SYTextProp_Set3DColor(LPCommonTextProp pProp, int nCharIdx, int nCount, SYMaterial* p3DColor);
//设置子面3D颜色，nCount为-1时，设置全部。
 void	SYTextProp_SetSubFace3DColor(LPCommonTextProp pProp, int nCharIdx, int nCount, int nSubFace, SYMaterial* p3DColor);
//设置下划线，nCount为-1时，设置全部。
 void	SYTextProp_SetUnderLine(LPCommonTextProp pProp, int nCharIdx, int nCount, LPCBaseUnderLine lpUnderLine, DWORD dwMask = SYTEXTFONTMASK_UNDERLINE_ALL);
 void	SYTextProp_SetUnderLineProperty(LPCommonTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, DWORD dwMask);
//从前向后查找第1个相同字体（不得与自己比较）
 int	SYTextProp_FindSameFont(BaseARray* paryFont, LPCBaseFont pCompFont, LPVOID* pSame);
//从前向后查找第1个相同2D颜色（不得与自己比较）
 int	SYTextProp_FindSame2DColor(BaseARray* paryColor, LPCBaseStaticEffect pCompColor, LPVOID* pSame);
//从前向后查找第1个相同3D颜色（不得与自己比较）
 int	SYTextProp_FindSame3DColor(BaseARray* paryColor, const SYMaterial* pCompColor, LPVOID* pSame);
//从前向后查找第1个相同下划线（不得与自己比较）
 int	SYTextProp_FindSameUnderLine(BaseARray* paryUnderLine, LPCBaseUnderLine pUnderLine, LPVOID* pSame);
//根据掩码拷贝2D颜色子属性
 void	SYText2DColor_CopyValue(LPCommonStaticEffect pDesColor, LPCBaseStaticEffect pSrcColor, DWORD dwMask);
//从字体表中删除1个
 BOOL	SYTextProp_RemoveFont(BaseARray* paryFont, int nIndex);
 BOOL	SYTextProp_RemoveFont(BaseARray* paryFont, LPCommonFont pFont);
//从2D颜色表中删除1个
 BOOL	SYTextProp_Remove2DColor(BaseARray* pary2DColor, int nIndex);
 BOOL	SYTextProp_Remove2DColor(BaseARray* pary2DColor, LPCommonStaticEffect p2DColor);
//从3D颜色表中删除1个
 BOOL	SYTextProp_Remove3DColor(BaseARray* pary3DColor, int nIndex);
 BOOL	SYTextProp_Remove3DColor(BaseARray* pary3DColor, SYMaterial* p3DColor);
//从下划线表中删除1个
 BOOL	SYTextProp_RemoveUnderLine(BaseARray* paryUnderLine, int nIndex);
 BOOL	SYTextProp_RemoveUnderLine(BaseARray* paryUnderLine, LPCommonUnderLine pUnderLine);

/*********************SYTextPropIndex*************************/
 void	SYTextPropIndex_Init(LPCommonTextPropIndex pPropIdx);
 void	SYTextPropIndex_Release(LPCommonTextPropIndex pPropIdx);
 void	SYTextPropIndex_Copy(LPCommonTextPropIndex pDes, LPCBaseTextPropIndex pSrc);
 BOOL	SYTextPropIndex_Compare(LPCBaseTextPropIndex pPropIdx1, LPCBaseTextPropIndex pPropIdx2);

/***********************BaseARtTypeset**************************/
 void	BaseARtTypeset_Init(LPBaseARtTypeset pArt);
 void	BaseARtTypeset_Create(LPBaseARtTypeset pArt, eBaseARtTypesetPath ePath);
 void	BaseARtTypeset_Create(LPBaseARtTypeset pPath, CurveAr* pCurveAry, BOOL bRotate = TRUE);
 void	BaseARtTypeset_Release(LPBaseARtTypeset pArt);
 void	BaseARtTypeset_Copy(LPBaseARtTypeset pDes, LPCBaseARtTypeset pSrc);
 BOOL	BaseARtTypeset_Compare(LPCBaseARtTypeset pArt1, LPCBaseARtTypeset pArt2);

/*********************SYGlyphSpace****************************/
 void	BASEGRAPH_DATARENDER_API SYGlyphSpace_ReleaseAll(BaseARray& artGlyphSpace);

/***********************其它导出******************************/
//字符串按回车换行符拆分
 void	SYStr_SplitByCRLF(LPCTSTR szString, CBaseStrArray& arString);
//字符串按指定字符拆分
 void	SYStr_SplitByChar(LPCTSTR szString, TCHAR cSplit, CBaseStrArray& arString);
//创建颜色字体Buffer
 BOOL	SYCreateTextBuffer(LPDWORD pdwBuffer, int* pnWidth, int* pnHeight, 
						LPCTSTR szText, LPCommonFont pFont, LPCommonStaticEffect pColor,double dScale);

/*************************************************************/
 void	SYText_InitParagraphArray(LPCommonText pText, E_TextAlignHorizontal eAlignHorizontal);
 void	SYText_InitParagraph(LPCommonTextParagraph pParagraph);
 void	SYText_ReleaseParagraph(LPCommonTextParagraph pParagraph);
 int	SYText_SaveParagraph(LPCommonTextParagraph pParagraph, LPBYTE pBuffer, int nLen);
 int	SYText_LoadParagraph(LPCommonTextParagraph pParagraph, LPBYTE pBuffer, int nLen);
 void	SYText_CopyParagraph(LPCommonTextParagraph pDes, LPCommonTextParagraph pSrc);
 BOOL	SYText_CompareParagraph(LPCommonTextParagraph pParagraph1, LPCommonTextParagraph pParagraph2);
 void	SYText_InitSymbol(LPCommonTextSymbol pSymbol);
 void	SYText_ReleaseSymbol(LPCommonTextSymbol pSymbol);
 int	SYText_SaveSymbol(LPCommonTextSymbol pSymbol, LPBYTE pBuffer, int nLen);
 int	SYText_LoadSymbol(LPCommonTextSymbol pSymbol, LPBYTE pBuffer, int nLen);
 void	SYText_CopySymbol(LPCommonTextSymbol pDes, LPCommonTextSymbol pSrc);
 //////////////////////////////////////////////////////////////////////////
 void	TypesetInterface_Lock(LPVOID pInterfaceParam);
 void	TypesetInterface_UnLock(LPVOID pInterfaceParam);
 void	TypesetInterface_OnAddUnit(BOOL bKeepBalance, LPVOID pInterfaceParam);
 void	TypesetInterface_OnInsertUnit(int nIndex, int nCount, BOOL bKeepBalance, LPVOID pInterfaceParam);
 void	TypesetInterface_OnDeleteUnit(int nIndex, int nCount, BOOL bKeepBalance, LPVOID pInterfaceParam);
 void	TypesetInterface_OnDeleteAllUnit(LPVOID pInterfaceParam);
 void	TypesetInterface_OnRefreshUnit(int nIndex, int nCount, LPVOID pInterfaceParam);
 void	TypesetInterface_OnRefreshAllUnit(LPVOID pInterfaceParam, int nLevel);
 //Item类的接口
 BASEGRAPH_DATARENDER_API void SYTypesetInterface_InitItemInterface();
 //初始化排版器接口环境
 BASEGRAPH_DATARENDER_API void SYTypeset_InitInterfaceEnv();
 //////////////////////////////////////////////////////////////////////////
#endif //_SYTYPESETDATAAPI_INC_