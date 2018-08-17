#ifndef _BaseGraphOFFSETCURVE_H
#define _BaseGraphOFFSETCURVE_H

//////////////////////////////////////////////////////////////////////////
// 对外接口函数

//------------------------------------------------------------------
//函数 BaseGraphOffsetCurve 说明：
//功能：输入曲线pGraphIn 向外offset距离fDis后得到的点列
//输入：ptSrcArr: 给定点列。
//输出：ptDesArr: 向外offset距离fDis后得到的点列
//注意！
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API void BaseGraphOffsetCurve(IN vectorgraph* pGraphIn,  // 输入曲线
				   IN float fDis,  // 偏移距离
				   OUT vectorgraph* pGraphOffset // 输出偏移曲线
				   );
BASEGRAPH_DATARENDER_API BOOL BaseGraphReleaseOffsetCell(vectorcell* pcell);
//------------------------------------------------------------------
//函数 BaseGraphOffsetCell 说明：
//功能：输入曲线pCellIn 向外offset距离fDis后得到的cell
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API void BaseGraphOffsetCell(IN vectorcell* pCellIn,  // 输入曲线
									IN vector2f* vOffset,
									IN float fDis,  // 偏移距离
									OUT vectorcell* pCellOffset, // 输出偏移曲线
									IN const int ndirect
									);

BASEGRAPH_DATARENDER_API void BaseGraphOffsetCellCube(IN vectorcell* pCellIn,  // 输入曲线
										vector2f* vOffset,
										IN float fCubOffset,  // 偏移距离
										IN float fAngle,
										OUT vectorcell* pCellOffset, // 输出偏移曲线
										IN const int ndirect
										);

//------------------------------------------------------------------
//函数 BaseGraphOffsetCell 说明：
//功能：输入曲线pCellIn 向外offset距离fDis后得到的cell
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API void BaseGraphOffsetCell(IN vectorcell* pCellIn,  // 输入曲线
									IN float fDis,  // 偏移距离
									OUT BaseARray &aptOrig, //原始边离散点
									OUT BaseARray &aptOrigNorm, //原始边离散点
									OUT CBasePoint2fPtrArray &aptShift, //偏移边离散点
									OUT CBasePoint2fPtrArray &aptShiftNorm, //偏移边离散点
									OUT CBaseDWORDPtrArray &aOrigIndex,
									IN const int ndirect
									);

////////////////////////////////////////////////////////////////////////////
//// 内部实现
//BASEGRAPH_DATARENDER_API int SolveSqure(double a, double b, double c, double *xx);
BASEGRAPH_DATARENDER_API BOOL BaseGraphOC_PtOnRight(IN point2f pt0,IN point2f pt1,IN point2f pt2);//判断左侧是否外侧
BASEGRAPH_DATARENDER_API void BaseGraphOC_Cell2PtfPoly(IN vectorcell * pCurveCell, OUT BaseARray * pptrPoly,OUT BaseARray * pptrPolyNorm,IN OUT int* pMidPtIndex);
BASEGRAPH_DATARENDER_API void BaseGraphOC_Cell2PtlfPoly(IN vectorcell * pCurveCell, OUT BaseARray * pptrPoly,OUT BaseARray * pptrPolyNorm,int* pMidPtIndex);
BASEGRAPH_DATARENDER_API void BaseGraphOC_DelColine(IN OUT BaseARray *pptfArr,int* pMidPtIndex);
BASEGRAPH_DATARENDER_API void BaseGraphOC_DelOverlap(IN OUT BaseARray *pptrOutlineArray,int* pMidPtIndex);
//////////////////////////////////////////////////////////////////////////
//为提高效率采取的牺牲部分展示效果的开关
BASEGRAPH_DATARENDER_API void BaseGraphOC_CloseNearLine();//关闭NearLine函数
BASEGRAPH_DATARENDER_API void BaseGraphOC_OpenNearLine();//打开NearLine函数
#endif
