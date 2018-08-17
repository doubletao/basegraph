/************************************************************************/
//    文件：   BaseGraphLineAPI                                                   
//    描述：   线算法，包含直线、线段、射线等
//			   直线标识：Line
//			   线段标识：Segment
//			   射线标识：Ray                                   
/************************************************************************/
#pragma once
//////////////////////////////////////////////////////////////////////////
// 宏定义
#ifndef FLOAT_0
#define FLOAT_0		1e-6
#endif

#define FLOAT_EQUAL(f1,f2)		(fabs((f1) - (f2)) < FLOAT_0)
#define POINTF_EQUAL(p1,p2)		(FLOAT_EQUAL((p1).x,(p2).x) && FLOAT_EQUAL((p1).y,(p2).y))
//////////////////////////////////////////////////////////////////////////
//    功能： 求两点之间距离                                                        
//    返回： 距离                                                               
//    输入： 两点坐标p1,p2                                               
//    输出： 无                                        
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_DATARENDER_API float PointToPointDis(IN point2f p1, IN point2f p2);
//////////////////////////////////////////////////////////////////////////
//    功能： 求点pt到两点所在直线的距离                                                         
//    返回： 距离                                                               
//    输入： 点坐标pt，直线上两点坐标pLS,pLE                                               
//    输出： 无                                        
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_DATARENDER_API double PointToLineDis(IN point2f pt, IN point2f ptLS, IN point2f ptLE); 
//////////////////////////////////////////////////////////////////////////
//    功能： 判断点pt是否位于线段上                                                        
//    返回： 在则返回TRUE，否则FALSE                                                               
//    输入： 点坐标pt，线段两点坐标pLS,pLE                                                  
//    输出： 无                                        
BASEGRAPH_DATARENDER_API BOOL IsPointOnSegment(IN point2f pt, IN point2f ptLS, IN point2f ptLE);
//////////////////////////////////////////////////////////////////////////
//    功能： 判断两直线是否相交                                                     
//    返回： 相交则返回TRUE，否则FALSE                                                                   
//    输入： 直线1两点坐标 ptL1S，ptL1E；  直线2两点坐标 ptL2S，ptL2E；                                            
//    输出： 直线交点 pPtOut                                       
BASEGRAPH_DATARENDER_API BOOL IsLineIntersection(IN point2f ptL1S,IN point2f ptL1E,IN point2f ptL2S,IN point2f ptL2E,OUT point2f* pPtOut);
//////////////////////////////////////////////////////////////////////////
//    功能： 判断两线段是否相交                                                     
//    返回： 相交则返回TRUE，否则FALSE                                                                   
//    输入： 线段1两点坐标 ptL1S，ptL1E；  线段2两点坐标 ptL2S，ptL2E；                                            
//    输出： 线段交点 pPtOut                                       
BASEGRAPH_DATARENDER_API BOOL IsSegmentIntersection(IN point2f ptL1S,IN point2f ptL1E,IN point2f ptL2S,IN point2f ptL2E,OUT point2f* pPtOut);


//////////////////////////////////////////////////////////////////////////
//    功能： 判断射线与线段是否相交                                                     
//    返回： 相交则返回TRUE，否则FALSE                                                                   
//    输入： 射线两点坐标 ptRay0，ptRay1；  线段2两点坐标 pt0，pt1；                                            
//    输出： 射线与线段交点 pPtOut                                       
BASEGRAPH_DATARENDER_API BOOL IsRaySegmentIntersection(IN point2f ptRay0,IN point2f ptRay1,IN point2f pt0,IN point2f pt1,OUT point2f* pPtOut);
//////////////////////////////////////////////////////////////////////////
//    功能： 判断线段是否与区域（线段组）相交                                                 
//    返回： 返回相交点的个数                                                                   
//    输入： 线段两点坐标 pt0，pt1；  线段数组pArSeg（数组元素为point2lf*）                                         
//    输出： 输出相交点数组arOut（数组元素为point2f*） ，输出数组按离线段第一个点pt0的距离从近到远排序                                    
BASEGRAPH_DATARENDER_API int IsSegmentRegionIntersection(IN point2f pt0,IN point2f pt1,IN BaseARray* pArSeg,OUT BaseARray* pArOut);
//////////////////////////////////////////////////////////////////////////
//    功能： 判断射线是否与区域（线段组）相交                                                 
//    返回： 返回相交点的个数                                                                   
//    输入： 射线两点坐标 ptRay0，ptRay1；  线段数组pArSeg（数组元素为point2lf*）                                         
//    输出： 输出相交点数组arOut（数组元素为point2f*） ，输出数组按离线段第一个点的距离从近到远排序                                    
BASEGRAPH_DATARENDER_API BOOL IsRayRegionIntersection(IN point2f ptRay0,IN point2f ptRay1,IN BaseARray* pArSeg,OUT point2f* pPtOut);
//////////////////////////////////////////////////////////////////////////
//    功能： 判断点pt是否位于区域（线段组）上                                                        
//    返回： 在则返回TRUE，否则FALSE                                                               
//    输入： 点坐标pt，线段数组pArSeg（数组元素为point2lf*）                                                     
//    输出： 无   
BASEGRAPH_DATARENDER_API BOOL IsPointOnRegion(IN point2f pt,IN BaseARray* pArSeg);
//////////////////////////////////////////////////////////////////////////
//    功能： 判断点pt是否位于区域（线段组）内                                                        
//    返回： 在则返回TRUE，否则FALSE                                                               
//    输入： 点坐标pt，线段数组pArSeg（数组元素为point2lf*）                                                     
//    输出： 无   
BASEGRAPH_DATARENDER_API BOOL IsPointInRegionLF(IN point2f pt,IN BaseARray* pArSeg);

//////////////////////////////////////////////////////////////////////////
//    功能： 判断点pt是否位于区域（线段组）内                                                        
//    返回： 在则返回TRUE，否则FALSE                                                               
//    输入： 点坐标pt，线段数组pArSeg（数组元素为point2f*）                                                     
//    输出： 无   
BASEGRAPH_DATARENDER_API BOOL IsPointInRegionF(IN point2f pt,IN BaseARray* pArSeg);
//////////////////////////////////////////////////////////////////////////
//    功能： 给一条连续的线段和一个连续的封闭区域（线段组），求出线段在区域（线段组）内的部分                                                    
//    返回： 无                                                             
//    输入： 原始线段数组pArLine（数组元素为point2f*），区域线段数组pArSeg（数组元素为point2lf*）                                                     
//    输出： 原始线段在区域（线段组）内的部分，有可能被截为多段线段数组，即返回线段数组的数组pArOut（数组元素为BaseARray*）；
//			 数组元素的元素为（point2f*）
BASEGRAPH_DATARENDER_API void GetSegmentInRegion(IN BaseARray* pArLine,IN BaseARray* pArSeg,OUT BaseARray* pArOut);
//////////////////////////////////////////////////////////////////////////
