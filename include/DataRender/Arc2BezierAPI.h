#ifndef _BaseARC2BEZIER_H
#define _BaseARC2BEZIER_H
// 象限. 注意: 并非屏幕坐标系的象限.
enum ARCLOC
{
	ARCLOC_RIGHTTOP    =   1,
    ARCLOC_LEFTTOP     =   2,
    ARCLOC_LEFTBOTTOM  =   3,
	ARCLOC_RIGHTBOTTOM =   4,
};
//////////////////////////////////////////////////////////////////////////
// 对外接口函数

// 用三次Bezier逼近圆弧:圆弧要等分成多少段
BASEGRAPH_DATARENDER_API int Arc2Bezier_SplitNum(IN const point2f ptfCenter, // 圆心
						IN const float fRadius, // 半径
						IN const BOOL bCircle, // 是否整圆
						IN const float fAngleStartIN, // 起始角度 
						IN const float fAngleEndIN, // 终止角度 默认终止角度大于起始角度,且都在0-2*PI之间
						IN const float fThreshold, // 误差阈值
						IN OUT int & nArcs // 圆弧等分段数
						);

// 用三次Bezier逼近圆弧: 得到控制顶点数组
BASEGRAPH_DATARENDER_API void Arc2Bezier_GetCtrlPts(IN const point2f ptfCenter, // 圆心
						   IN const float fRadius, // 半径
						   IN const float fAngleStart, // 起始角度 
						   IN const float fAngleEnd, // 终止角度 默认终止角度大于起始角度,且都在0-2*PI之间
						   IN const int nArcs, // 圆弧等分段数
						   OUT point2f* ptfCtrlPts // 逼近结果 控制顶点数组 数组大小：nArcs * 3 + 1 
						   );

//----------------------------------
// 用三次Bezier逼近椭圆
//----------------------------------
BASEGRAPH_DATARENDER_API void DividCircle2Bezier(
    IN const point2f ptCenter,       // 圆心
    IN const float fRadius_a,       // 沿x轴半径
    IN const float fRadius_b,       // 沿y轴半径
    OUT BaseARray &aBezier，          // 逼近结果 4个point2f型顶点 
    );

//----------------------------------
// 用三次Bezier逼近1/4椭圆，点为逆时针
//----------------------------------
BASEGRAPH_DATARENDER_API void DividQuarterArc2Bezier(
    IN  const point2f ptCenter,      // 圆心
    IN  const float  fRadius_a,     // 半径
    IN  const float  fRadius_b,     // 半径
    IN  ARCLOC       loc,           // 圆弧位置
    OUT BaseARray&     arPt
    );

//----------------------------------
// 用三次Bezier逼近椭圆弧
//----------------------------------
BASEGRAPH_DATARENDER_API void DividArc2Bezier(
    IN const point2f ptCenter,       //圆心
    IN const float fRadius_a,       // 沿x轴半径
    IN const float fRadius_b,       // 沿y轴半径
	IN const float fStartRadIn,		// 起始弧度
	IN const float fEndRad,			// 终止弧度
    OUT BaseARray &aBezier			// 逼近结果 4个point2f型顶点 
    );

//////////////////////////////////////////////////////////////////////////
// 如下为内部实现函数

// 用三次Bezier逼近圆弧片段
BASEGRAPH_DATARENDER_API void DividedArc2Bezier(IN const point2f ptfCenter, // 圆心
					   IN const float fRadius, // 半径
					   IN const float fAngleStart, // 起始角度 
					   IN const float fAngleEnd, // 终止角度 默认终止角度大于起始角度,且都在0-2*PI之间
					   IN const float fThreshold, // 误差阈值
					   //IN OUT point2f * ptfCtrlPts, // 单段逼近结果 4个控制顶点
					   //OUT CPtrArray *  pArrBezier, // 逼近结果 控制顶点数组
					   IN OUT int & nArcs // 圆等分段数
					   );

// 得到逼近一段圆弧的Bezier曲线
BASEGRAPH_DATARENDER_API void Get4CtrlPts(IN const point2f ptfCenter, // 圆心
				 IN const float fRadius, // 半径
				 //IN int nArcs, // 圆等分段数
				 IN const float fAngleStart, // 起始角度
				 IN const float fAngleEnd, // 终止角度
				 OUT point2f * ptfCtrl // 输出: 四个控制顶点
				 );

// 计算最大误差
BASEGRAPH_DATARENDER_API float GetMaxDis(IN point2f ptfCenter,  // 圆心
				IN float fRadius, //半径
				IN point2f * ptfCtrl // Bezier控制顶点
				);

//创建椭圆矢量图
BASEGRAPH_DATARENDER_API BOOL	CreateEllipse(float fStartRad,float fEndRad,float a,float b,CSYFloatArray* pArray);
//得到椭圆边界点坐标(相对于椭圆中心点)
BASEGRAPH_DATARENDER_API void	GetEllipsePtPos(float fRad, float a, float b, point2f* ptPos);
//根据边界点坐标(相对于椭圆中心点)得到等比缩放的椭圆长短轴以及边界点角度
BASEGRAPH_DATARENDER_API void	CalcRadAndAngle(float3 ptEdg,float& fRad_A, float& fRad_B, float &fAngle, float fMinA, float fMaxA, float fMinB, float fMaxB);

//创建圆的矢量图
BASEGRAPH_DATARENDER_API BOOL  CreateCircle(float fStartRad,double fEndRad,double fRad,CSYFloatArray*	pArray);
BASEGRAPH_DATARENDER_API BOOL  ReleaseCircleArray(CSYFloatArray*	pArray);


BASEGRAPH_DATARENDER_API void	line_vertical_normalizing(OUT point2f* ptout,IN point2f* ptinstart,point2f* ptinend,IN int ndirect);

BASEGRAPH_DATARENDER_API int	line_extend(OUT point2f& ptoutstart,point2f& ptoutend,
								 IN point2f& ptinstart,point2f& ptinend,
								 IN int ndirect,IN float fwidth);
BASEGRAPH_DATARENDER_API int	arc_near_line(const point2f& ptcenter,const point2f& ptstart,const point2f& ptend,float frad);
int BASEGRAPH_DATARENDER_API	cell_makearc(BaseARray* parray,const point2f& ptcenter,const point2f& ptstart,const point2f& ptend,float rad);
//-----------------------------------------------------------------------------
// 计算三次Bezier曲线的最小外包矩形
//-----------------------------------------------------------------------------
//pt: 四个控制顶点// rt:外包矩形
BASEGRAPH_DATARENDER_API void	CalBezier3Bound(IN const point2f *pt, OUT rectf &rt );

BASEGRAPH_DATARENDER_API float SYGetBezierLength(const pointf &Pt1,const pointf &Pt2,const pointf &Pt3,const pointf &Pt4);

#endif
