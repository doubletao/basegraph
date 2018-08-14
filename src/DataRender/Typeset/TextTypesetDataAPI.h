#ifndef _SYTYPESETDATAAPI_INC_
#define _SYTYPESETDATAAPI_INC_
#include <vector>
#include "DataRender\DrawTextDef.h"
#include "DataRender\MaterialDef.h"

typedef CBaseARray<curCtrl*,curCtrl*> CurveAr;

//////////////////////////////////////////////////////////////////////////
/************************SYText*******************************/
 void	SYText_Init(LPSYText pText);
 void	SYText_Create(LPSYText pText, 
					LPCTSTR szText, 
					rectf rtBorder,
					LPSYFont pFont,					//字体
					LPSYStaticEffect pFillColor,		//2D色
					SYMaterial* p3DColor,				//3D色
					LPSYUnderLine pUnderLine,			//下划线
					E_TextAlignHorizontal alignH, 
					E_TextAlignVeritical alignV, 
					BOOL bHorizontal, 
					BOOL bLeftInput, 
					BOOL bVLeftTypeset, 
					BOOL bLineWrap, 
					BOOL bFixedFrame);
 void	BASEGRAPH_DATARENDER_API SYText_Release(LPSYText pText);
 void	SYText_Copy(LPSYText pDes, LPCSYText pSrc, BOOL bCopyNullText);
 BOOL	SYText_Compare(LPCSYText pText1, LPCSYText pText2, BOOL bNullTextEq);
 void	SYText_CopyProp(LPSYTextProp& pDesProp,			//目标SYTextProp，NULL则内部new。
					LPCSYText pSrcText,				//源SYText
					int nDesLen = -1);				//目标长度
//更新字的3D拉伸属性
 void	SYText_UpdateStretchParam(LPSYText pText, LPSYStretchParam pStretch);
//整体渲染属性
 void	SYText_SetWholeRender(LPSYText pText, BOOL bWholeRender);
 BOOL	SYText_IsWholeRender(LPSYText pText);
//根据字符串拆分段落
 void	SYText_SplitStringToParagraphArray(std::vector<int>& vParaIndex, LPCTSTR szText);

 void	BASEGRAPH_DATARENDER_API SYText_GetStaticEffectProperty(void* pProp, LPSYStaticEffect pFillColor, int nIndex);
 void	SYText_GetFontProperty(void* pProp, LPSYFont pFont, int nIndex);
 //查（翻转）表法计算CRC32，可以分块计算。
 unsigned long	SYCRC32Cal(unsigned long ulCRC32, unsigned char *pBuf, unsigned long ulLen);

 void	BASEGRAPH_DATARENDER_API SYText_SetParagraphParam(LPSYText pTex, 
							 int nStart, 
							 int nCount, 
							 E_TextPropertyType eType, 
							 LPVOID pPropertyData);
//用指定的大小修改排版矩形
 void	SYText_ModifyTypesetBorderBySize(LPSYText pText, SYSizef& szRectSize);
//设置固定字框
 void	BASEGRAPH_DATARENDER_API SYText_SetFixedFrame(LPSYText pText, BOOL bFixedFrame);

/*******************TypesetPrivateData************************/
 void	SYTypesetPrivateData_Init(LPSYTypesetPrivate pData);
 void	SYTypesetPrivateData_Release(LPSYTypesetPrivate pData);

/************************SYFont*******************************/
 void	BASEGRAPH_DATARENDER_API SYFont_Init(LPSYFont pFont);
 void	SYFont_Release(LPSYFont pFont);
 int	SYFont_Save(LPCSYFont pFont, LPBYTE pBuffer, int nLen);
 int	SYFont_Load(LPSYFont pFont, LPBYTE pBuffer, int nLen);
 void	BASEGRAPH_DATARENDER_API SYFont_Copy(LPSYFont pDes, LPCSYFont pSrc);
 BOOL	SYFont_Compare(LPCSYFont pFont1, LPCSYFont pFont2);
//根据掩码设置字体子属性
 void	SYFont_SetValue(LPSYFont pDesFont, const LPVOID lpSrcData, const LPVOID lp2ndSrcData, DWORD dwMask);
//根据掩码拷贝字体子属性
 void	SYFont_CopyValue(LPSYFont pDesFont, LPCSYFont pSrcFont, DWORD dwMask);

/***********************SYUnderLine***************************/
 void	BASEGRAPH_DATARENDER_API SYUnderLine_Init(LPSYUnderLine pUnderLine);
 void	SYUnderLine_Create(LPSYUnderLine pUnderLine, E_TextUnderLine eType, float fYPos,float fLineWidth,LPSYStaticEffect p2DColor);
 void	SYUnderLine_Release(LPSYUnderLine pUnderLine);
 void	SYUnderLine_Copy(LPSYUnderLine pDes, LPCSYUnderLine pSrc);
 BOOL	SYUnderLine_Compare(LPCSYUnderLine pUnderLine1, LPCSYUnderLine pUnderLine2);
//根据掩码设置下划线子属性
 void	SYUnderLine_SetValue(LPSYUnderLine pDesUnderLine, const LPVOID lpSrcData, DWORD dwMask);
//根据掩码拷贝下划线子属性
 void	SYUnderLine_CopyValue(LPSYUnderLine pDesUnderLine, LPCSYUnderLine pSrcUnderLine, DWORD dwMask);

/************************SYTextProp***************************/
 void	SYTextProp_Init(LPSYTextProp pProp);
 void	SYTextProp_Create(int nTextLen,LPSYTextProp pProp,LPSYFont pFont, LPSYStaticEffect p2DColor,SYMaterial* p3DColor,LPSYUnderLine pUnderLine);
 void	SYTextProp_Release(LPSYTextProp pProp);
 void	SYTextProp_Copy(LPSYTextProp pDes, LPCSYTextProp pSrc, int nDesLen = -1);
 BOOL	SYTextProp_Compare(LPCSYTextProp pProp1, LPCSYTextProp pProp2);
//删除/插入字符串时维护SYTextProp
 void	SYTextProp_DeleteString(LPSYTextProp pProp, int nCharIdx, int nDelCnt, int nTextLen, BOOL bWholeRender);
 void	SYTextProp_InsertString(LPSYTextProp pProp, int nCharIdx, int nInsCnt, int nTextLen);

//取指定字符的字体、2D颜色、3D颜色或者下划线属性
 LPVOID	SYTextProp_GetProp(LPSYTextProp pProp,  int nCharIdx,  E_TextPropType eType);
//取指定字符属性索引结构（索引越界时取边界值）
 LPSYTextPropIndex	SYTextProp_GetPropIndex(BaseARray* paryIndex, int nCharIdx);
//删除一个属性索引
 BOOL	SYTextProp_EmptyPropIndex(LPSYTextProp pProp, LPSYTextPropIndex pPropIdx, BOOL bReserve2DClr);
//属性表长度调整
 void	BASEGRAPH_DATARENDER_API SYTextProp_ResizePropIndexArray(LPSYTextProp pProp, int nSize, BOOL bKeepProperty);
//取字体属性
 LPSYFont	BASEGRAPH_DATARENDER_API SYTextProp_GetFont(LPSYTextProp pProp, int nCharIdx);
//取2D颜色
 LPSYStaticEffect	BASEGRAPH_DATARENDER_API SYTextProp_GetStaticEffect(LPSYTextProp pProp, int nCharIdx);
//取指定子面的3D颜色
 BASEGRAPH_DATARENDER_API SYMaterial* SYTextProp_Get3DColor(LPSYTextProp pProp, int nCharIdx, int nSubFace = -1);
//取所有子面的3D颜色
 LPVOID*		SYTextProp_Get3DColorArray(LPSYTextProp pProp, int nCharIdx);
//取下划线
 LPSYUnderLine	BASEGRAPH_DATARENDER_API SYTextProp_GetUnderLine(LPSYTextProp pProp, int nCharIdx);
//设置字体属性，nCount为-1时，设置全部。
 void	SYTextProp_SetFont(LPSYTextProp pProp, int nCharIdx, int nCount, LPCSYFont lpFont, DWORD dwMask = SYTEXTFONTMASK_ALL);
 void	SYTextProp_FontProperty(LPSYTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, const LPVOID lp2ndPropData, DWORD dwMask);
//设置2D颜色，nCount为-1时，设置全部。
 void	SYTextProp_SetStaticEffect(LPSYTextProp pProp, int nIndex, int nCharIdx, LPCSYStaticEffect lpFillColor, DWORD dwMask = SYTEXTCOLORMASK_ALL);
//设置3D颜色，nCount为-1时，设置全部。
 void	SYTextProp_Set3DColor(LPSYTextProp pProp, int nCharIdx, int nCount, SYMaterial* p3DColor);
//设置子面3D颜色，nCount为-1时，设置全部。
 void	SYTextProp_SetSubFace3DColor(LPSYTextProp pProp, int nCharIdx, int nCount, int nSubFace, SYMaterial* p3DColor);
//设置下划线，nCount为-1时，设置全部。
 void	SYTextProp_SetUnderLine(LPSYTextProp pProp, int nCharIdx, int nCount, LPCSYUnderLine lpUnderLine, DWORD dwMask = SYTEXTFONTMASK_UNDERLINE_ALL);
 void	SYTextProp_SetUnderLineProperty(LPSYTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, DWORD dwMask);
//从前向后查找第1个相同字体（不得与自己比较）
 int	SYTextProp_FindSameFont(BaseARray* paryFont, LPCSYFont pCompFont, LPVOID* pSame);
//从前向后查找第1个相同2D颜色（不得与自己比较）
 int	SYTextProp_FindSame2DColor(BaseARray* paryColor, LPCSYStaticEffect pCompColor, LPVOID* pSame);
//从前向后查找第1个相同3D颜色（不得与自己比较）
 int	SYTextProp_FindSame3DColor(BaseARray* paryColor, const SYMaterial* pCompColor, LPVOID* pSame);
//从前向后查找第1个相同下划线（不得与自己比较）
 int	SYTextProp_FindSameUnderLine(BaseARray* paryUnderLine, LPCSYUnderLine pUnderLine, LPVOID* pSame);
//根据掩码拷贝2D颜色子属性
 void	SYText2DColor_CopyValue(LPSYStaticEffect pDesColor, LPCSYStaticEffect pSrcColor, DWORD dwMask);
//从字体表中删除1个
 BOOL	SYTextProp_RemoveFont(BaseARray* paryFont, int nIndex);
 BOOL	SYTextProp_RemoveFont(BaseARray* paryFont, LPSYFont pFont);
//从2D颜色表中删除1个
 BOOL	SYTextProp_Remove2DColor(BaseARray* pary2DColor, int nIndex);
 BOOL	SYTextProp_Remove2DColor(BaseARray* pary2DColor, LPSYStaticEffect p2DColor);
//从3D颜色表中删除1个
 BOOL	SYTextProp_Remove3DColor(BaseARray* pary3DColor, int nIndex);
 BOOL	SYTextProp_Remove3DColor(BaseARray* pary3DColor, SYMaterial* p3DColor);
//从下划线表中删除1个
 BOOL	SYTextProp_RemoveUnderLine(BaseARray* paryUnderLine, int nIndex);
 BOOL	SYTextProp_RemoveUnderLine(BaseARray* paryUnderLine, LPSYUnderLine pUnderLine);

/*********************SYTextPropIndex*************************/
 void	SYTextPropIndex_Init(LPSYTextPropIndex pPropIdx);
 void	SYTextPropIndex_Release(LPSYTextPropIndex pPropIdx);
 void	SYTextPropIndex_Copy(LPSYTextPropIndex pDes, LPCSYTextPropIndex pSrc);
 BOOL	SYTextPropIndex_Compare(LPCSYTextPropIndex pPropIdx1, LPCSYTextPropIndex pPropIdx2);

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
 void	SYStr_SplitByCRLF(LPCTSTR szString, CSYStrArray& arString);
//字符串按指定字符拆分
 void	SYStr_SplitByChar(LPCTSTR szString, TCHAR cSplit, CSYStrArray& arString);
//创建颜色字体Buffer
 BOOL	SYCreateTextBuffer(LPDWORD pdwBuffer, int* pnWidth, int* pnHeight, 
						LPCTSTR szText, LPSYFont pFont, LPSYStaticEffect pColor,double dScale);

/*************************************************************/
 void	SYText_InitParagraphArray(LPSYText pText, E_TextAlignHorizontal eAlignHorizontal);
 void	SYText_InitParagraph(LPSYTextParagraph pParagraph);
 void	SYText_ReleaseParagraph(LPSYTextParagraph pParagraph);
 int	SYText_SaveParagraph(LPSYTextParagraph pParagraph, LPBYTE pBuffer, int nLen);
 int	SYText_LoadParagraph(LPSYTextParagraph pParagraph, LPBYTE pBuffer, int nLen);
 void	SYText_CopyParagraph(LPSYTextParagraph pDes, LPSYTextParagraph pSrc);
 BOOL	SYText_CompareParagraph(LPSYTextParagraph pParagraph1, LPSYTextParagraph pParagraph2);
 void	SYText_InitSymbol(LPSYTextSymbol pSymbol);
 void	SYText_ReleaseSymbol(LPSYTextSymbol pSymbol);
 int	SYText_SaveSymbol(LPSYTextSymbol pSymbol, LPBYTE pBuffer, int nLen);
 int	SYText_LoadSymbol(LPSYTextSymbol pSymbol, LPBYTE pBuffer, int nLen);
 void	SYText_CopySymbol(LPSYTextSymbol pDes, LPSYTextSymbol pSrc);
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