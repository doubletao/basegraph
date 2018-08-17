#ifndef _BaseGraphTYPESETDATAAPI_INC_
#define _BaseGraphTYPESETDATAAPI_INC_
#include <vector>
#include "DataRender\DrawTextDef.h"
#include "DataRender\MaterialDef.h"

typedef CBaseARray<curCtrl*,curCtrl*> CurveAr;

//////////////////////////////////////////////////////////////////////////
/************************BaseGraphText*******************************/
 void	BaseGraphText_Init(LPCommonText pText);
 void	BaseGraphText_Create(LPCommonText pText, 
					LPCTSTR szText, 
					rectf rtBorder,
					LPCommonFont pFont,					//字体
					LPCommonStaticEffect pFillColor,		//2D色
					BaseGraphMaterial* p3DColor,				//3D色
					LPCommonUnderLine pUnderLine,			//下划线
					E_TextAlignHorizontal alignH, 
					E_TextAlignVeritical alignV, 
					BOOL bHorizontal, 
					BOOL bLeftInput, 
					BOOL bVLeftTypeset, 
					BOOL bLineWrap, 
					BOOL bFixedFrame);
 void	BASEGRAPH_DATARENDER_API BaseGraphText_Release(LPCommonText pText);
 void	BaseGraphText_Copy(LPCommonText pDes, LPCBaseText pSrc, BOOL bCopyNullText);
 BOOL	BaseGraphText_Compare(LPCBaseText pText1, LPCBaseText pText2, BOOL bNullTextEq);
 void	BaseGraphText_CopyProp(LPCommonTextProp& pDesProp,			//目标BaseGraphTextProp，NULL则内部new。
					LPCBaseText pSrcText,				//源BaseGraphText
					int nDesLen = -1);				//目标长度
//更新字的3D拉伸属性
 void	BaseGraphText_UpdateStretchParam(LPCommonText pText, LPCommonStretchParam pStretch);
//整体渲染属性
 void	BaseGraphText_SetWholeRender(LPCommonText pText, BOOL bWholeRender);
 BOOL	BaseGraphText_IsWholeRender(LPCommonText pText);
//根据字符串拆分段落
 void	BaseGraphText_SplitStringToParagraphArray(std::vector<int>& vParaIndex, LPCTSTR szText);

 void	BASEGRAPH_DATARENDER_API BaseGraphText_GetStaticEffectProperty(void* pProp, LPCommonStaticEffect pFillColor, int nIndex);
 void	BaseGraphText_GetFontProperty(void* pProp, LPCommonFont pFont, int nIndex);
 //查（翻转）表法计算CRC32，可以分块计算。
 unsigned long	BaseGraphCRC32Cal(unsigned long ulCRC32, unsigned char *pBuf, unsigned long ulLen);

 void	BASEGRAPH_DATARENDER_API BaseGraphText_SetParagraphParam(LPCommonText pTex, 
							 int nStart, 
							 int nCount, 
							 E_TextPropertyType eType, 
							 LPVOID pPropertyData);
//用指定的大小修改排版矩形
 void	BaseGraphText_ModifyTypesetBorderBySize(LPCommonText pText, BaseGraphSizef& szRectSize);
//设置固定字框
 void	BASEGRAPH_DATARENDER_API BaseGraphText_SetFixedFrame(LPCommonText pText, BOOL bFixedFrame);

/*******************TypesetPrivateData************************/
 void	BaseGraphTypesetPrivateData_Init(LPCommonTypesetPrivate pData);
 void	BaseGraphTypesetPrivateData_Release(LPCommonTypesetPrivate pData);

/************************BaseGraphFont*******************************/
 void	BASEGRAPH_DATARENDER_API BaseGraphFont_Init(LPCommonFont pFont);
 void	BaseGraphFont_Release(LPCommonFont pFont);
 int	BaseGraphFont_Save(LPCBaseFont pFont, LPBYTE pBuffer, int nLen);
 int	BaseGraphFont_Load(LPCommonFont pFont, LPBYTE pBuffer, int nLen);
 void	BASEGRAPH_DATARENDER_API BaseGraphFont_Copy(LPCommonFont pDes, LPCBaseFont pSrc);
 BOOL	BaseGraphFont_Compare(LPCBaseFont pFont1, LPCBaseFont pFont2);
//根据掩码设置字体子属性
 void	BaseGraphFont_SetValue(LPCommonFont pDesFont, const LPVOID lpSrcData, const LPVOID lp2ndSrcData, DWORD dwMask);
//根据掩码拷贝字体子属性
 void	BaseGraphFont_CopyValue(LPCommonFont pDesFont, LPCBaseFont pSrcFont, DWORD dwMask);

/***********************BaseGraphUnderLine***************************/
 void	BASEGRAPH_DATARENDER_API BaseGraphUnderLine_Init(LPCommonUnderLine pUnderLine);
 void	BaseGraphUnderLine_Create(LPCommonUnderLine pUnderLine, E_TextUnderLine eType, float fYPos,float fLineWidth,LPCommonStaticEffect p2DColor);
 void	BaseGraphUnderLine_Release(LPCommonUnderLine pUnderLine);
 void	BaseGraphUnderLine_Copy(LPCommonUnderLine pDes, LPCBaseUnderLine pSrc);
 BOOL	BaseGraphUnderLine_Compare(LPCBaseUnderLine pUnderLine1, LPCBaseUnderLine pUnderLine2);
//根据掩码设置下划线子属性
 void	BaseGraphUnderLine_SetValue(LPCommonUnderLine pDesUnderLine, const LPVOID lpSrcData, DWORD dwMask);
//根据掩码拷贝下划线子属性
 void	BaseGraphUnderLine_CopyValue(LPCommonUnderLine pDesUnderLine, LPCBaseUnderLine pSrcUnderLine, DWORD dwMask);

/************************BaseGraphTextProp***************************/
 void	BaseGraphTextProp_Init(LPCommonTextProp pProp);
 void	BaseGraphTextProp_Create(int nTextLen,LPCommonTextProp pProp,LPCommonFont pFont, LPCommonStaticEffect p2DColor,BaseGraphMaterial* p3DColor,LPCommonUnderLine pUnderLine);
 void	BaseGraphTextProp_Release(LPCommonTextProp pProp);
 void	BaseGraphTextProp_Copy(LPCommonTextProp pDes, LPCBaseTextProp pSrc, int nDesLen = -1);
 BOOL	BaseGraphTextProp_Compare(LPCBaseTextProp pProp1, LPCBaseTextProp pProp2);
//删除/插入字符串时维护BaseGraphTextProp
 void	BaseGraphTextProp_DeleteString(LPCommonTextProp pProp, int nCharIdx, int nDelCnt, int nTextLen, BOOL bWholeRender);
 void	BaseGraphTextProp_InsertString(LPCommonTextProp pProp, int nCharIdx, int nInsCnt, int nTextLen);

//取指定字符的字体、2D颜色、3D颜色或者下划线属性
 LPVOID	BaseGraphTextProp_GetProp(LPCommonTextProp pProp,  int nCharIdx,  E_TextPropType eType);
//取指定字符属性索引结构（索引越界时取边界值）
 LPCommonTextPropIndex	BaseGraphTextProp_GetPropIndex(BaseARray* paryIndex, int nCharIdx);
//删除一个属性索引
 BOOL	BaseGraphTextProp_EmptyPropIndex(LPCommonTextProp pProp, LPCommonTextPropIndex pPropIdx, BOOL bReserve2DClr);
//属性表长度调整
 void	BASEGRAPH_DATARENDER_API BaseGraphTextProp_ResizePropIndexArray(LPCommonTextProp pProp, int nSize, BOOL bKeepProperty);
//取字体属性
 LPCommonFont	BASEGRAPH_DATARENDER_API BaseGraphTextProp_GetFont(LPCommonTextProp pProp, int nCharIdx);
//取2D颜色
 LPCommonStaticEffect	BASEGRAPH_DATARENDER_API BaseGraphTextProp_GetStaticEffect(LPCommonTextProp pProp, int nCharIdx);
//取指定子面的3D颜色
 BASEGRAPH_DATARENDER_API BaseGraphMaterial* BaseGraphTextProp_Get3DColor(LPCommonTextProp pProp, int nCharIdx, int nSubFace = -1);
//取所有子面的3D颜色
 LPVOID*		BaseGraphTextProp_Get3DColorArray(LPCommonTextProp pProp, int nCharIdx);
//取下划线
 LPCommonUnderLine	BASEGRAPH_DATARENDER_API BaseGraphTextProp_GetUnderLine(LPCommonTextProp pProp, int nCharIdx);
//设置字体属性，nCount为-1时，设置全部。
 void	BaseGraphTextProp_SetFont(LPCommonTextProp pProp, int nCharIdx, int nCount, LPCBaseFont lpFont, DWORD dwMask = BaseGraphTEXTFONTMASK_ALL);
 void	BaseGraphTextProp_FontProperty(LPCommonTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, const LPVOID lp2ndPropData, DWORD dwMask);
//设置2D颜色，nCount为-1时，设置全部。
 void	BaseGraphTextProp_SetStaticEffect(LPCommonTextProp pProp, int nIndex, int nCharIdx, LPCBaseStaticEffect lpFillColor, DWORD dwMask = BaseGraphTEXTCOLORMASK_ALL);
//设置3D颜色，nCount为-1时，设置全部。
 void	BaseGraphTextProp_Set3DColor(LPCommonTextProp pProp, int nCharIdx, int nCount, BaseGraphMaterial* p3DColor);
//设置子面3D颜色，nCount为-1时，设置全部。
 void	BaseGraphTextProp_SetSubFace3DColor(LPCommonTextProp pProp, int nCharIdx, int nCount, int nSubFace, BaseGraphMaterial* p3DColor);
//设置下划线，nCount为-1时，设置全部。
 void	BaseGraphTextProp_SetUnderLine(LPCommonTextProp pProp, int nCharIdx, int nCount, LPCBaseUnderLine lpUnderLine, DWORD dwMask = BaseGraphTEXTFONTMASK_UNDERLINE_ALL);
 void	BaseGraphTextProp_SetUnderLineProperty(LPCommonTextProp pProp, int nCharIdx, int nCount, const LPVOID lpPropData, DWORD dwMask);
//从前向后查找第1个相同字体（不得与自己比较）
 int	BaseGraphTextProp_FindSameFont(BaseARray* paryFont, LPCBaseFont pCompFont, LPVOID* pSame);
//从前向后查找第1个相同2D颜色（不得与自己比较）
 int	BaseGraphTextProp_FindSame2DColor(BaseARray* paryColor, LPCBaseStaticEffect pCompColor, LPVOID* pSame);
//从前向后查找第1个相同3D颜色（不得与自己比较）
 int	BaseGraphTextProp_FindSame3DColor(BaseARray* paryColor, const BaseGraphMaterial* pCompColor, LPVOID* pSame);
//从前向后查找第1个相同下划线（不得与自己比较）
 int	BaseGraphTextProp_FindSameUnderLine(BaseARray* paryUnderLine, LPCBaseUnderLine pUnderLine, LPVOID* pSame);
//根据掩码拷贝2D颜色子属性
 void	BaseGraphText2DColor_CopyValue(LPCommonStaticEffect pDesColor, LPCBaseStaticEffect pSrcColor, DWORD dwMask);
//从字体表中删除1个
 BOOL	BaseGraphTextProp_RemoveFont(BaseARray* paryFont, int nIndex);
 BOOL	BaseGraphTextProp_RemoveFont(BaseARray* paryFont, LPCommonFont pFont);
//从2D颜色表中删除1个
 BOOL	BaseGraphTextProp_Remove2DColor(BaseARray* pary2DColor, int nIndex);
 BOOL	BaseGraphTextProp_Remove2DColor(BaseARray* pary2DColor, LPCommonStaticEffect p2DColor);
//从3D颜色表中删除1个
 BOOL	BaseGraphTextProp_Remove3DColor(BaseARray* pary3DColor, int nIndex);
 BOOL	BaseGraphTextProp_Remove3DColor(BaseARray* pary3DColor, BaseGraphMaterial* p3DColor);
//从下划线表中删除1个
 BOOL	BaseGraphTextProp_RemoveUnderLine(BaseARray* paryUnderLine, int nIndex);
 BOOL	BaseGraphTextProp_RemoveUnderLine(BaseARray* paryUnderLine, LPCommonUnderLine pUnderLine);

/*********************BaseGraphTextPropIndex*************************/
 void	BaseGraphTextPropIndex_Init(LPCommonTextPropIndex pPropIdx);
 void	BaseGraphTextPropIndex_Release(LPCommonTextPropIndex pPropIdx);
 void	BaseGraphTextPropIndex_Copy(LPCommonTextPropIndex pDes, LPCBaseTextPropIndex pSrc);
 BOOL	BaseGraphTextPropIndex_Compare(LPCBaseTextPropIndex pPropIdx1, LPCBaseTextPropIndex pPropIdx2);

/***********************BaseARtTypeset**************************/
 void	BaseARtTypeset_Init(LPBaseARtTypeset pArt);
 void	BaseARtTypeset_Create(LPBaseARtTypeset pArt, eBaseARtTypesetPath ePath);
 void	BaseARtTypeset_Create(LPBaseARtTypeset pPath, CurveAr* pCurveAry, BOOL bRotate = TRUE);
 void	BaseARtTypeset_Release(LPBaseARtTypeset pArt);
 void	BaseARtTypeset_Copy(LPBaseARtTypeset pDes, LPCBaseARtTypeset pSrc);
 BOOL	BaseARtTypeset_Compare(LPCBaseARtTypeset pArt1, LPCBaseARtTypeset pArt2);

/*********************BaseGraphGlyphSpace****************************/
 void	BASEGRAPH_DATARENDER_API BaseGraphGlyphSpace_ReleaseAll(BaseARray& artGlyphSpace);

/***********************其它导出******************************/
//字符串按回车换行符拆分
 void	BaseGraphStr_SplitByCRLF(LPCTSTR szString, CBaseStrArray& arString);
//字符串按指定字符拆分
 void	BaseGraphStr_SplitByChar(LPCTSTR szString, TCHAR cSplit, CBaseStrArray& arString);
//创建颜色字体Buffer
 BOOL	BaseGraphCreateTextBuffer(LPDWORD pdwBuffer, int* pnWidth, int* pnHeight, 
						LPCTSTR szText, LPCommonFont pFont, LPCommonStaticEffect pColor,double dScale);

/*************************************************************/
 void	BaseGraphText_InitParagraphArray(LPCommonText pText, E_TextAlignHorizontal eAlignHorizontal);
 void	BaseGraphText_InitParagraph(LPCommonTextParagraph pParagraph);
 void	BaseGraphText_ReleaseParagraph(LPCommonTextParagraph pParagraph);
 int	BaseGraphText_SaveParagraph(LPCommonTextParagraph pParagraph, LPBYTE pBuffer, int nLen);
 int	BaseGraphText_LoadParagraph(LPCommonTextParagraph pParagraph, LPBYTE pBuffer, int nLen);
 void	BaseGraphText_CopyParagraph(LPCommonTextParagraph pDes, LPCommonTextParagraph pSrc);
 BOOL	BaseGraphText_CompareParagraph(LPCommonTextParagraph pParagraph1, LPCommonTextParagraph pParagraph2);
 void	BaseGraphText_InitSymbol(LPCommonTextSymbol pSymbol);
 void	BaseGraphText_ReleaseSymbol(LPCommonTextSymbol pSymbol);
 int	BaseGraphText_SaveSymbol(LPCommonTextSymbol pSymbol, LPBYTE pBuffer, int nLen);
 int	BaseGraphText_LoadSymbol(LPCommonTextSymbol pSymbol, LPBYTE pBuffer, int nLen);
 void	BaseGraphText_CopySymbol(LPCommonTextSymbol pDes, LPCommonTextSymbol pSrc);
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
 BASEGRAPH_DATARENDER_API void BaseGraphTypesetInterface_InitItemInterface();
 //初始化排版器接口环境
 BASEGRAPH_DATARENDER_API void BaseGraphTypeset_InitInterfaceEnv();
 //////////////////////////////////////////////////////////////////////////
#endif //_BaseGraphTYPESETDATAAPI_INC_
