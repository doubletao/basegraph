#pragma once

#include "DataRender\BevelPoly.h"

typedef struct tag_PtNode
{
	point3f pt;
	BOOL    bStart; 
}PtNode;//输入多边形顶点

typedef struct tag_LineNode
{
	int nStartPt;//起点 在输入多边形顶点数组中的  序号
	int nEndPt;  //终点 在输入多边形顶点数组中的  序号
}LineNode;//线段

typedef struct tag_Triangle
{
	int nPt1;//点1 在输入多边形顶点数组中的  序号
	int nPt2;//点2 在输入多边形顶点数组中的  序号
	int nPt3;//点3 在输入多边形顶点数组中的  序号
}Triangle;//三角形(三个顶点序号)

typedef CBaseARray<PtNode*> CPtNodeArray;
typedef CBaseARray<LineNode*> CLineNodeArray;
typedef CBaseARray<Triangle*> CTriangleArray;

class SYTriangulateApp
{
public:
	SYTriangulateApp(void);
	~SYTriangulateApp(void);
	void SYTriangulateLine(IN vectorgraph* pGraph, OUT OUT CSYPoint3fPtrArray *pptrTri, OUT CSYPoint3fPtrArray *pptrUV, IN BOOL bAntiClockwise);
	void SYTriangulate(IN vectorgraph* pGraph, OUT OUT CSYPoint3fPtrArray *pptrTri, OUT CSYPoint3fPtrArray *pptrUV, IN BOOL bAntiClockwise);
	void SYTriangulate(IN vectorgraph* pGraph, pointf* pt, int nPtSize, int* nUnitStart, int nUnit, int nCurUnit, 
		int* nCellStart, int nCell, OUT CSYPoint3fPtrArray *pptrTri, IN BOOL bAntiClockwise);
	void SYTriangulateGL(IN vectorgraph* pGraph, OUT CSYPoint3fPtrArray *pptrTri,OUT CSYPoint3fPtrArray *pptrUV, IN BOOL bAntiClockwise);
protected:
	//--------------------------------------------------------------------------------
	//功能：判断两点是否在直线两侧
	//--------------------------------------------------------------------------------
	BOOL PtsOnDiffSide(
		IN pointf pt1, 
		IN pointf pt2, 
		IN pointf ptLine1, 
		IN pointf ptLine2
		);

	BOOL PtsOnDiffSide(
		IN point3f p1, 
		IN point3f p2, 
		IN point3f ptLine1, 
		IN point3f ptLine2
		);
	//--------------------------------------------------------------------------------
	//功能：打断并加入内环上的顶点maikefeng
	//--------------------------------------------------------------------------------
	void BreakAndAddCurCell(CSYPoint3fPtrArray *pptrTri, 
		CPtNodeArray &ptPolyArray, //ptfArray中的顶点加入到ptPolyArray
		BaseARray &ptfArray, 
		int nMinXIndex, // ptfArray中最小x值点的序号
		float fMinX, // ptfArray中最小x值
		CSYPoint2fPtrArray &ptfArrAddedMinPts);
	void Triangulate(CPtNodeArray* pptPolyArray, CLineNodeArray* pLineArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise);
	void Triangulate2(CPtNodeArray* pptPolyArray, CLineNodeArray* pLineArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise);
	//--------------------------------------------------------------------------------
	//功能：采用割角法对平面非自交无洞多边形Triangulation
	//输入：多边形顶点数组
	//输出：三角形数组（每个元素含三个顶点）
	//说明: 没有洞
	//--------------------------------------------------------------------------------
	void Triangulate3WithPoly(CPtNodeArray* pptPolyArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise);
	void Triangulate3(CPtNodeArray* pptPolyArray, CLineNodeArray* pLineArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise);
	float cosine(pointf s1, pointf e1, pointf s2, pointf e2);
	BOOL PtOnLeft(IN pointf pt0, IN pointf pt1, IN pointf pt2, BOOL bAntiClockWise);
	BOOL PtOnLeft2(IN pointf pt0, IN pointf pt1, IN pointf pt2, BOOL bAntiClockWise);
	BOOL InitLines( IN CPtNodeArray* pptPolyArray, OUT CLineNodeArray* pLineArray );
	BOOL SingleLineIntersection(
		IN pointf startPt1, 
		IN pointf endPt1, 
		IN pointf startPt2, 
		IN pointf endPt2
		);
	BOOL LineIntersection(
		IN pointf startPt1, 
		IN pointf endPt1, 
		IN CLineNodeArray* pLineArray,
		IN CPtNodeArray* pptPolyArray 
		);

	//////////////////////////////////////////////////////////
	// 求点p到线段l所在直线的距离 
	//////////////////////////////////////////////////////////
	double PToLDist(pointf* p, pointf* pLS, pointf* pLE);

	//------------------------------------------------------------------
	//功能：判断点pt是否位于线段上
	//------------------------------------------------------------------
	BOOL PtOnLineSeg(IN pointf pt, IN pointf line1, IN pointf line2 );

	//------------------------------------------------------------------
	//功能：判断两线段是否相交
	//输入：两线段的起讫点
	//输出：
	//返回值：若相交，则返回TRUE;否则，返回FALSE
	//------------------------------------------------------------------
	BOOL SingleLineIntersection2(
		IN pointf startPt1, 
		IN pointf endPt1, 
		IN pointf startPt2, 
		IN pointf endPt2
		);
	//------------------------------------------------------------------
	//功能：判断线段是否与线段组相交
	//输入：
	//输出：
	//返回值：若相交，则返回TRUE;否则，返回FALSE
	//------------------------------------------------------------------
	BOOL LineIntersection2(
		IN pointf startPt1, 
		IN pointf endPt1, 
		IN CPtNodeArray &ptPolyArray
		);
	int CountLineInArray(IN int nPt1, IN int nPt2, IN int nTheOne, CLineNodeArray* pLineArray, OUT int& m, OUT int& n);
	void InitOutLoopArr(OUT BOOL *bOutLoop, IN const vectorgraph* pGraph, IN const BOOL bAntiClockwise);
	int GetMinYPt(IN vectorcell* pCurveCell );//得到最小y值点 //多边形顶点数组　顶点数据类型：ShiftNode*
	BOOL OutIsLeft(IN pointf pt0,IN pointf pt1,IN pointf pt2);//判断左侧是否外侧
	BOOL IsAntiClockwise(IN vectorcell* pCurveCell );
};
