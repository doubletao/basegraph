#ifndef __SY_COMMONUTILITY_20120605_INC__
#define __SY_COMMONUTILITY_20120605_INC__
#include "Common\VectorDef.h"
#include "Common\CommonDataContainer.h"
#include "Common\PlaneDef.h"
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void	__cdecl	SYPtCopyPt2f(point2 &P, point2f &FP);
BASEGRAPH_COMMON_API void	__cdecl	SYPt2fCopyPt(point2f &FP, point2 &P);
BASEGRAPH_COMMON_API void	__cdecl	SYOffsetPt2fs(point2f VerPt[], point2f &pt, int nCount);
BASEGRAPH_COMMON_API void	__cdecl	SYOffsetPt3fs(point3f VerPt[], point3f &pt, int nCount);
BASEGRAPH_COMMON_API float	__cdecl	SYPt2fDistance(point2f &pt0, point2f &pt1);
//释放一个point2f数组
BASEGRAPH_COMMON_API void	__cdecl	SYAR_ReleasePt2fAry(SYArray& ary);
//释放一个point3f数组
BASEGRAPH_COMMON_API void	__cdecl	SYAR_ReleasePt3fAry(SYArray& ary);
//在指针数组中用指针查找索引号
BASEGRAPH_COMMON_API int	__cdecl	SYAR_GetIndexByPtr(const SYArray* pArray, void* pPtr);
BASEGRAPH_COMMON_API rectf	__cdecl	SYGetBoundfFromPt2fs(point2f *pt, int nCount=4);
//////////////////////////////////////////////////////////////////////////
typedef CSYArray<point2, point2>		CSYPoint2Array;
typedef CSYArray<pointf, pointf>		CSYPointfArray;
typedef CSYArray<point2f, point2f>		CSYPoint2fArray;
typedef CSYArray<point2lf, point2lf>	CSYPoint2lfArray;

typedef CSYArray<point3, point3>		CSYPoint3Array;
typedef CSYArray<point3f, point3f>		CSYPoint3fArray;
typedef CSYArray<vector3f, vector3f>	CSYVector3fArray;
typedef CSYArray<point3lf, point3lf>	CSYPoint3lfArray;
typedef CSYArray<vector3lf, vector3lf>	CSYVector3lfArray;

typedef CSYArray<point2*, point2*>		CSYPoint2PtrArray;
typedef CSYArray<pointf*, pointf*>		CSYPointfPtrArray;
typedef CSYArray<point2f*, point2f*>	CSYPoint2fPtrArray;
typedef CSYArray<point2lf*, point2lf*>	CSYPoint2lfPtrArray;

typedef CSYArray<point3*, point3*>		CSYPoint3PtrArray;
typedef CSYArray<point3f*, point3f*>	CSYPoint3fPtrArray;
typedef CSYArray<vector3f*, vector3f*>	CSYVector3fPtrArray;
typedef CSYArray<point3lf*, point3lf*>	CSYPoint3lfPtrArray;
typedef CSYArray<vector3lf*, vector3lf*>	 CSYVector3lfPtrArray;
#endif		// #ifndef __SY_COMMONUTILITY_20120605_INC__
