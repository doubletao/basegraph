#ifndef __Common_COMMONUTILITY_20120605_INC__
#define __Common_COMMONUTILITY_20120605_INC__
#include "Common\VectorDef.h"
#include "Common\CommonDataContainer.h"
#include "Common\PlaneDef.h"
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void	__cdecl	BaseGraphPtCopyPt2f(point2 &P, point2f &FP);
BASEGRAPH_COMMON_API void	__cdecl	BaseGraphPt2fCopyPt(point2f &FP, point2 &P);
BASEGRAPH_COMMON_API void	__cdecl	BaseGraphOffsetPt2fs(point2f VerPt[], point2f &pt, int nCount);
BASEGRAPH_COMMON_API void	__cdecl	BaseGraphOffsetPt3fs(point3f VerPt[], point3f &pt, int nCount);
BASEGRAPH_COMMON_API float	__cdecl	BaseGraphPt2fDistance(point2f &pt0, point2f &pt1);
//释放一个point2f数组
BASEGRAPH_COMMON_API void	__cdecl	BaseAR_ReleasePt2fAry(BaseARray& ary);
//释放一个point3f数组
BASEGRAPH_COMMON_API void	__cdecl	BaseAR_ReleasePt3fAry(BaseARray& ary);
//在指针数组中用指针查找索引号
BASEGRAPH_COMMON_API int	__cdecl	BaseAR_GetIndexByPtr(const BaseARray* pArray, void* pPtr);
BASEGRAPH_COMMON_API rectf	__cdecl	BaseGraphGetBoundfFromPt2fs(point2f *pt, int nCount=4);
//////////////////////////////////////////////////////////////////////////
typedef CBaseARray<point2, point2>		CBasePoint2Array;
typedef CBaseARray<pointf, pointf>		CBasePointfArray;
typedef CBaseARray<point2f, point2f>		CBasePoint2fArray;
typedef CBaseARray<point2lf, point2lf>	CBasePoint2lfArray;

typedef CBaseARray<point3, point3>		CBasePoint3Array;
typedef CBaseARray<point3f, point3f>		CBasePoint3fArray;
typedef CBaseARray<vector3f, vector3f>	CBaseVector3fArray;
typedef CBaseARray<point3lf, point3lf>	CBasePoint3lfArray;
typedef CBaseARray<vector3lf, vector3lf>	CBaseVector3lfArray;

typedef CBaseARray<point2*, point2*>		CBasePoint2PtrArray;
typedef CBaseARray<pointf*, pointf*>		CBasePointfPtrArray;
typedef CBaseARray<point2f*, point2f*>	CBasePoint2fPtrArray;
typedef CBaseARray<point2lf*, point2lf*>	CBasePoint2lfPtrArray;

typedef CBaseARray<point3*, point3*>		CBasePoint3PtrArray;
typedef CBaseARray<point3f*, point3f*>	CBasePoint3fPtrArray;
typedef CBaseARray<vector3f*, vector3f*>	CBaseVector3fPtrArray;
typedef CBaseARray<point3lf*, point3lf*>	CBasePoint3lfPtrArray;
typedef CBaseARray<vector3lf*, vector3lf*>	 CBaseVector3lfPtrArray;
#endif		// #ifndef __Common_COMMONUTILITY_20120605_INC__
