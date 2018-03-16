#include "stdafx.h"
#include "SceneRender\3DIntersection.h"
#define LINE_SEG_DEVIATION	2.f
#define LINE_CIRCLE_DEVIATION	6.f

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

BOOL IntersectTriangle( const vector3f* orig,const vector3f* dir,const vector3f* v0,const vector3f* v1,const vector3f* v2, vector3f* pOutPt );
int Intersect_Triangle(IN const DX_Triangle_Intersection* pIn , OUT Point_Intersection* pOutPt )
{
	vector3f orig = pIn->ray.startPoint;
	vector3f dir = pIn->ray.Direction;
	vector3f v0 = pIn->triangle.v0;
	vector3f v1 = pIn->triangle.v1;
	vector3f v2 = pIn->triangle.v2;
	vector3f edge1 = v1 - v0, edge2 = v2 - v0, pvec = { 0, 0, 0 };
	float3CrossProduct( &pvec, &dir, &edge2 );

	float det = float3DotProduct( &edge1, &pvec );

	vector3f tvec;
	if( det > 0 )
		tvec = orig - v0;
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if( det < 0.0001f )
		return 0;

	float u = float3DotProduct( &tvec, &pvec );
	if( u < 0.0f || u > det )
		return 0;

	vector3f qvec;
	float3CrossProduct( &qvec, &tvec, &edge1 );

	float v = float3DotProduct( &dir, &qvec );
	if( v < 0.0f || u + v > det )
		return 0;

	if (pOutPt)
	{
		v=v/det;
		u=u/det;
		pOutPt->nearPoint = v0 * (1 - u - v) + v1 * u + v2 * v;
	}

	return INTERSECTION_NEAR;
}

bool PointInCone(IN const vector3f& snapPoint,IN float heightMax, IN float heightMin, IN float f_k, IN float f_C, IN float startAngle, IN float endAngle)
{
	if(snapPoint.y<=heightMax && snapPoint.y>=heightMin)
	{
		double radius=f_k*snapPoint.y+f_C;
		double angle=asin(snapPoint.z/radius);
		if(snapPoint.x < 0)	
			angle=PAI - angle;
		else if(snapPoint.x>0 && snapPoint.z<0) 
			angle=PAI_2+angle;
		if(angle>startAngle && angle<endAngle){
			return true;
		}
	}
	return false;

}

//int Intersection_Cone(IN const SY_Cone_Intersection* pIn,OUT Point_Intersection* pOut)
BASEGRAPH_SCENE_RENDER_API int GetRayConeInt( const SYRay* pRay,const DX_Cone_Parameter* pCone,SYHitPoint* pHitPoint )
{
	int return_result=0;
	const float Radius_max=pCone->fBaseRadius > pCone->fTopRadius ? pCone->fBaseRadius : pCone->fTopRadius;
	const float Radius_min=pCone->fBaseRadius <= pCone->fTopRadius ? pCone->fBaseRadius : pCone->fTopRadius;
	const float Height_max=pCone->fHeight>0?pCone->fHeight:0;//cone的上高
	const float Height_min=pCone->fHeight>0?0:pCone->fHeight;//cone的下高
	const float height=fabs(pCone->fHeight);
	vector3f direction=pRay->vDir;
	vector3f startPoint=pRay->vStart;
	float f_k,f_C;
	if((pCone->fBaseRadius>pCone->fTopRadius&&pCone->fHeight>0)||((pCone->fBaseRadius<pCone->fTopRadius)&&pCone->fHeight<0))
	{
		f_k=(Radius_min-Radius_max)/height;		
		f_C=(Radius_max-Radius_min)*Height_max/height+Radius_min;
	}
	else
	{
		f_k=(Radius_max-Radius_min)/height;		
		f_C=(Radius_min-Radius_max)*Height_min/height+Radius_min;
	}
	double f_b=startPoint.x*direction.x+startPoint.z*direction.z-f_k*direction.y*(f_C+f_k*startPoint.y);
	double f_a=direction.x*direction.x+direction.z*direction.z-f_k*f_k*direction.y*direction.y;
	double f_c=startPoint.x*startPoint.x+startPoint.z*startPoint.z-(f_C+f_k*startPoint.y)*(f_C+f_k*startPoint.y);
	double discrim=f_b*f_b-f_a*f_c;
	//在圆柱很大，离视线很远的时候，捕捉边缘比较困难
	if (fabs(discrim) <= 200) {
		double tmin = - f_b / f_a;
		vector3f snapPoint=startPoint+direction*(float)tmin;
		if (PointInCone(snapPoint,Height_max,Height_min,f_k,f_C,pCone->fStartAngle,pCone->fEndAngle))
		{
			//pOut->nearPoint=snapPoint;
			pHitPoint->AddPoint(&snapPoint);
			return_result=return_result | INTERSECTION_EDGE;
		}
	}
	//if(discrim > 200){
	{
		double tmin=(-f_b-sqrt(discrim))/f_a;
		vector3f snapPoint=startPoint+direction*(float)tmin;
		if (PointInCone(snapPoint,Height_max,Height_min,f_k,f_C,pCone->fStartAngle,pCone->fEndAngle)) 
		{
			//pOut->nearPoint=snapPoint;
			pHitPoint->AddPoint(&snapPoint);
			return_result=return_result | INTERSECTION_NEAR;
		}
		double tmax=(-f_b+sqrt(discrim))/f_a;
		snapPoint=startPoint+direction*(float)tmax;
		if (PointInCone(snapPoint,Height_max,Height_min,f_k,f_C,pCone->fStartAngle,pCone->fEndAngle)) 
		{
			pHitPoint->AddPoint(&snapPoint);
			//pOut->farPoint=snapPoint;
			return_result=return_result | INTERSECTION_FAR;
		}
	}
	return return_result;
}

int Intersection_Plane(IN const DX_Plane_Intersection* pIn, OUT Point_Intersection* pOut)
{
	int return_result = 0;
	float IsIntersec = float3DotProduct(&(pIn->Plane.normal), &(pIn->Ray.Direction));
	vector3f vDirect = pIn->Plane.point - pIn->Ray.startPoint;
	float t = float3DotProduct(&pIn->Plane.normal,&vDirect) / IsIntersec;
	if (fabs(IsIntersec) < 0.02)
	{
		return_result = NO_INTERSECTION;
	}
	else if (t > 0) 
	{
		return_result = return_result | INTERSECTION_NEAR;
		pOut->nearPoint = pIn->Ray.startPoint +  pIn->Ray.Direction * t;
	}
	else
	{ 
		return_result = INTERSECTION_FAR;
		pOut->farPoint = pIn->Ray.startPoint + pIn->Ray.Direction * t;
	}
	return return_result;
}

int Intersection_3DCircle(IN const DX_3DCircle_Intersection* pIn, OUT Point_Intersection* pOut)
{
	int return_result = NO_INTERSECTION;
	DX_Plane_Intersection CirclePlane;
	CirclePlane.Plane.normal = pIn->Circle.Normal;
	CirclePlane.Plane.point = pIn->Circle.Centre;
	CirclePlane.Ray = pIn->Ray;
	Point_Intersection PointPlane;
	int interType = Intersection_Plane(&CirclePlane,&PointPlane) ;
	if (interType == 0) //射线与平面平行
	{
		vector3f vDirect = CirclePlane.Ray.startPoint - CirclePlane.Plane.point;
		vector3f DirPointToCentre;
		float3Normalize(&DirPointToCentre,&vDirect);
		float IsInt = float3DotProduct(&DirPointToCentre,&CirclePlane.Plane.normal);
		if (fabs(IsInt) < 0.02)
		{
			vector3f v;
			float3CrossProduct( &v ,&(CirclePlane.Plane.normal), &(CirclePlane.Ray.Direction));
			vector3f vRayDirect = CirclePlane.Plane.point - CirclePlane.Ray.startPoint;
			float distance = fabs(float3DotProduct(&v,&vRayDirect));
			if (distance < pIn->Circle.Radius)
			{
				return_result = return_result | INTERSECTION_NEAR;
			}
		}
	}
	else if (interType == INTERSECTION_NEAR) 
	{
		vector3f Sub = pIn->Circle.Centre;
		Sub -= PointPlane.nearPoint ;
		if (fabs(float3Mode(&Sub) / pIn->Circle.Radius - 1) < 0.1 ) 
		{
			return_result = return_result | INTERSECTION_NEAR;
			pOut->nearPoint = PointPlane.nearPoint;
		}
	}
	return return_result;
}

int Intersection_LineSeg(IN const DX_LineSeg_Intersection* pIn,OUT Point_Intersection* pOut)
{
	int return_result = NO_INTERSECTION;
	DX_LineSeg_Intersection Int = *pIn;
	vector3f v ;
	vector3f lineDir, rayDir;
	vector3f vRay = Int.line.start - Int.line.end;
	vector3f vLineRay = Int.line.start - Int.Ray.startPoint;
	float3Normalize(&lineDir,&vRay);
	float3Normalize(&rayDir,&(Int.Ray.Direction));
	float3CrossProduct( &v ,&lineDir, &rayDir);
	float distance = fabs(float3DotProduct(&v,&vLineRay));
	if (distance < LINE_SEG_DEVIATION) 
	{
		//获取相交点找到两条直线的公垂线的垂足
		lineDir = Int.line.end - Int.line.start;
		float a = float3DotProduct(&vLineRay,&(Int.Ray.Direction));
		float c = float3DotProduct(&vLineRay,&lineDir);
		float b = float3DotProduct(&lineDir,&(Int.Ray.Direction));
		float d = float3DotProduct(&lineDir,&lineDir);
		float e = float3DotProduct(&(Int.Ray.Direction),&(Int.Ray.Direction));
		float t = (a/e*b - c)/(d-b/e*b);
		if (t > 0 && t < 1) 
		{
			if (pOut)
				pOut->nearPoint = Int.line.start + lineDir * t;
			return_result = return_result | INTERSECTION_NEAR;
		}
	}
	return return_result;
}

int Intersection_RaySeg( IN const DX_LineSeg_Intersection* pIn, OUT Point_Intersection* pOut )
{	
	int return_result = NO_INTERSECTION;
	DX_LineSeg_Intersection Int = *pIn;
	vector3f v ;
	vector3f lineDir, rayDir;
	vector3f vRay = Int.line.start - Int.line.end;
	vector3f vLineRay = Int.line.start - Int.Ray.startPoint;

	float3Normalize(&lineDir,&vRay);
	float3Normalize(&rayDir,&(Int.Ray.Direction));
	float3CrossProduct( &v ,&lineDir, &rayDir);
	float distance = fabs(float3DotProduct(&v,&vLineRay));
	if (distance < LINE_SEG_DEVIATION) 
	{
		//获取相交点找到两条直线的公垂线的垂足
		lineDir = Int.line.end - Int.line.start;
		float a = float3DotProduct(&vLineRay,&(Int.Ray.Direction));
		float c = float3DotProduct(&vLineRay,&lineDir);
		float b = float3DotProduct(&lineDir,&(Int.Ray.Direction));
		float d = float3DotProduct(&lineDir,&lineDir);
		float e = float3DotProduct(&(Int.Ray.Direction),&(Int.Ray.Direction));
		float t = (a/e*b - c)/(d-b/e*b);
		if (t > 0 ) 
		{
			if (pOut)
				pOut->nearPoint = Int.line.start + lineDir * t;
			return_result = return_result | INTERSECTION_NEAR;
		}
	}
	return return_result;
}

int Intersection_Sphere(IN const DX_Sphere_Intersection* pIn, OUT Point_Intersection* pOut)
{
	int return_result = NO_INTERSECTION;
	DX_Sphere_Intersection Int = *pIn;
	vector3f rayDir = pIn->Ray.Direction;
	vector3f stPointToCenter;
	stPointToCenter = - Int.Ray.startPoint;
	float projection=float3DotProduct(&stPointToCenter,&rayDir);
	float length=float3DotProduct(&stPointToCenter,&stPointToCenter);
	float radius_2 = pIn->fSphereRadius * pIn->fSphereRadius;
	if (fabs((length-projection*projection) - radius_2) < 50) 
	{
		pOut->nearPoint = Int.Ray.startPoint + rayDir * projection;
		return_result = INTERSECTION_EDGE;
	}
	if(length< radius_2 || (projection >= 0 && (length-projection*projection) < radius_2 - 500))
	{
		return_result = INTERSECTION_NEAR | INTERSECTION_FAR;
	}
	return return_result;
}

int Intersection_3DPoint(IN const DX_3DPoint_Intersection* pIn)
{
	DX_3DPoint_Intersection Int = *pIn;
	vector3f pointToStart = Int.vPoint - Int.ray.startPoint;
	float projection = float3DotProduct(&pointToStart,&Int.ray.Direction);
	float length = float3DotProduct(&pointToStart, &pointToStart);
	if (length < POINT_DEVIATION * POINT_DEVIATION || (projection > 0 && (length - projection * projection) <= POINT_DEVIATION * POINT_DEVIATION ))
	{
		return INTERSECTION_NEAR;
	}
	return NO_INTERSECTION;
}

int Intersection_Tetrahedron(IN const DX_Tetrahedron_Intersection* pIn,OUT Point_Intersection* pOut)
{
	int return_result = NO_INTERSECTION; 
	DX_Triangle_Intersection triangle_Int;
	triangle_Int.triangle.v0 = pIn->firstPoint;
	triangle_Int.triangle.v1 = pIn->secondPoint;
	triangle_Int.triangle.v2 = pIn->forthPoint;
	triangle_Int.ray = pIn->Ray;
	return_result = Intersect_Triangle(&triangle_Int,pOut);
	if(return_result )
		return return_result;

	triangle_Int.triangle.v0 = pIn->firstPoint;
	triangle_Int.triangle.v1 = pIn->secondPoint;
	triangle_Int.triangle.v2 = pIn->thirdPoint;
	triangle_Int.ray = pIn->Ray;
	return_result = Intersect_Triangle(&triangle_Int,pOut);
	if(return_result )
		return return_result;
	triangle_Int.triangle.v0 = pIn->firstPoint;
	triangle_Int.triangle.v1 = pIn->thirdPoint;
	triangle_Int.triangle.v2 = pIn->forthPoint;
	triangle_Int.ray = pIn->Ray;
	return_result = Intersect_Triangle(&triangle_Int,pOut);
	if(return_result )
		return return_result;
	triangle_Int.triangle.v0 = pIn->secondPoint;
	triangle_Int.triangle.v1 = pIn->forthPoint;
	triangle_Int.triangle.v2 = pIn->thirdPoint;
	return_result = Intersect_Triangle(&triangle_Int,pOut);
	return return_result;
}
BOOL GetTriangleInt( const SYRay* pRay, const float3*  v0, const float3*  v1, const float3*  v2, SYHitPoint* pHitPoint )
{
	float3 point;
	if(IntersectTriangle( &pRay->vStart,&pRay->vDir,v0,v1,v2, &point ))
	{
		pHitPoint->AddPoint(&point);
		return TRUE;
	}
	return FALSE;
}
//射线与面片相交
BOOL GetRaySegment(const SYRay* pRay,const float3& v1,const float3& v2,const float3& v3,const float3& v4,SYHitPoint* pHitPoint)//顺时针
{
	if(!GetTriangleInt( pRay, &v1, &v2, &v3, pHitPoint ))
	{
		if(!GetTriangleInt( pRay, &v1, &v3, &v4, pHitPoint ))
			return FALSE;
	}
	return TRUE;
}
int Intersection_Segment(IN const DX_Segment_Intersection* pIn,OUT Point_Intersection* pOut)
{
	int return_result = NO_INTERSECTION; 
	DX_Triangle_Intersection triangle_Int;
	triangle_Int.triangle.v0 = pIn->firstPoint;
	triangle_Int.triangle.v1 = pIn->secondPoint;
	triangle_Int.triangle.v2 = pIn->forthPoint;
	triangle_Int.ray = pIn->Ray;
	return_result = Intersect_Triangle(&triangle_Int,pOut);
	if(return_result )
		return return_result;

	triangle_Int.triangle.v0 = pIn->secondPoint;
	triangle_Int.triangle.v1 = pIn->forthPoint;
	triangle_Int.triangle.v2 = pIn->thirdPoint;
	return_result = Intersect_Triangle(&triangle_Int,pOut);
	return return_result;
}

int Intersection_Quad(IN const DX_QuadRang_Intersection* pIn)
{
	int return_result = NO_INTERSECTION; 
	DX_Segment_Intersection segmentInt;
	segmentInt.firstPoint	= pIn->firstPoint	;
	segmentInt.secondPoint	= pIn->secondPoint	;
	segmentInt.thirdPoint	= pIn->thirdPoint	;
	segmentInt.forthPoint	= pIn->forthPoint	;
	segmentInt.Ray = pIn->Ray;
	return_result = Intersection_Segment(&segmentInt,NULL);
	if (return_result)
	{
		return return_result;
	}

	segmentInt.firstPoint	= pIn->fifthPoint	;
	segmentInt.secondPoint	= pIn->sixthPoint	;
	segmentInt.thirdPoint	= pIn->seventhPoint	;
	segmentInt.forthPoint	= pIn->eighthPoint	;
	return_result = Intersection_Segment(&segmentInt,NULL);
	if (return_result)
	{
		return return_result;
	}

	segmentInt.firstPoint	= pIn->firstPoint	;
	segmentInt.secondPoint	= pIn->secondPoint;
	segmentInt.thirdPoint	= 	pIn->sixthPoint;
	segmentInt.forthPoint	= pIn->fifthPoint	;
	return_result = Intersection_Segment(&segmentInt,NULL);
	if (return_result)
	{
		return return_result;
	}

	segmentInt.firstPoint	= pIn->thirdPoint	;
	segmentInt.secondPoint	= pIn->seventhPoint	;
	segmentInt.thirdPoint	= pIn->eighthPoint	;
	segmentInt.forthPoint	= 	pIn->forthPoint;
	return_result = Intersection_Segment(&segmentInt,NULL);
	if (return_result)
	{
		return return_result;
	}

	segmentInt.firstPoint	= pIn->secondPoint	;
	segmentInt.secondPoint	= pIn->sixthPoint	;
	segmentInt.thirdPoint	= pIn->seventhPoint	;
	segmentInt.forthPoint	= 	pIn->thirdPoint;
	return_result = Intersection_Segment(&segmentInt,NULL);
	if (return_result)
	{
		return return_result;
	}

	segmentInt.firstPoint	= pIn->firstPoint	;
	segmentInt.secondPoint	= pIn->fifthPoint	;
	segmentInt.thirdPoint	= pIn->eighthPoint	;
	segmentInt.forthPoint	= 	pIn->forthPoint;
	return_result = Intersection_Segment(&segmentInt,NULL);
	if (return_result )
	{
		return return_result;
	}
	return return_result;

}
int GetPoint_Intersection(IN const void* paramin,OUT Point_Intersection* pOut,const int type)
{
	int return_result=NO_INTERSECTION;
	switch(type) 
	{
	case RAY_TO_3DCIRCLE:
		return_result = Intersection_3DCircle((DX_3DCircle_Intersection*)paramin, pOut);
		break;
	case RAY_TO_LINE_SEG:
		return_result = Intersection_LineSeg((DX_LineSeg_Intersection*)paramin,pOut);
		break;
	case RAY_TO_SPHERA:
		return_result = Intersection_Sphere((DX_Sphere_Intersection*)paramin,pOut);
		break;
	case RAY_TO_PLANE:
		return_result = Intersection_Plane((DX_Plane_Intersection*)paramin,pOut);
		break;
	case RAY_TO_TRIANGLE:
		return_result = Intersect_Triangle((DX_Triangle_Intersection*)paramin,pOut);
		break;
	case RAY_TO_3DPOINT:
		return_result = Intersection_3DPoint((DX_3DPoint_Intersection*)paramin);
		break;
	case RAY_TO_RAY_SEG:
		return_result = Intersection_RaySeg( (DX_LineSeg_Intersection*)paramin, pOut );
		break;
	case RAY_TO_QUADRANGULAR:
		return_result = Intersection_Quad( (DX_QuadRang_Intersection*)paramin );
		break;
	case RAY_TO_SEGMENT:
		return_result = Intersection_Segment((DX_Segment_Intersection*)paramin,pOut);
		break;
	case RAY_TO_TETRAHEDRON:
		return_result = Intersection_Tetrahedron((DX_Tetrahedron_Intersection*)paramin,pOut);
		break;
	default:
		break;
	}
	return return_result;
}

BOOL IntersectTriangle( const vector3f* orig,const vector3f* dir,const vector3f* v0,const vector3f* v1,const vector3f* v2, vector3f* pOutPt )
{
	DX_Triangle_Intersection Int;
	Int.ray.startPoint = *orig;
	Int.ray.Direction = *dir;
	Int.triangle.v0 = *v0;
	Int.triangle.v1 = *v1;
	Int.triangle.v2 = *v2;
	Point_Intersection pointOut;
	if(GetPoint_Intersection(&Int,&pointOut,RAY_TO_TRIANGLE) == INTERSECTION_NEAR)
	{
		if (pOutPt)
		{
			*pOutPt = pointOut.nearPoint;
		}
		return TRUE;
	}
	return FALSE;
}


void Generate_Quatn_BetweenDir( OUT SYQuatn* pqQuatn, IN vector3f* pvStartDir, IN vector3f* pvEndDir )
{
	vector3f vAxis = { 0.f, 0.f, 0.f };
	float3CrossProduct( &vAxis, pvStartDir, pvEndDir );
	if ( fabs(vAxis.x) < ABS_FLOAT_0 && fabs(vAxis.y) < ABS_FLOAT_0 && fabs(vAxis.z) < ABS_FLOAT_0 ) 
	{
	}
	else
	{
		float3Normalize( &vAxis, &vAxis );
	}
	
	double fAngle = 0.f;
	double fdot = float3DotProduct( pvStartDir, pvEndDir );
	double mod1 = sqrt(((double)pvStartDir->x) * pvStartDir->x + ((double)pvStartDir->y) * pvStartDir->y + ((double)pvStartDir->z) * pvStartDir->z);
	double mod2 = sqrt(((double)pvEndDir->x) * pvEndDir->x + ((double)pvEndDir->y) * pvEndDir->y + ((double)pvEndDir->z) * pvEndDir->z);
	//double mod1 = float3Mode(pvStartDir);
	//double mod2 = float3Mode(pvEndDir);

	double fresult = fdot/(mod1 * mod2);
	if (fresult > 1.0)
	{
		fresult = 1.0;
	}
	else if (fresult < -1.0)
	{
		fresult = -1.0;
	}
	fAngle = acos(fresult);
//	fAngle = acos( float3DotProduct( pvStartDir, pvEndDir ) / ( float3Mode(pvStartDir) * float3Mode(pvEndDir) ) );
	////////////////////////////////////////////////////////////////////////// begin
	// test: 四元数弄反了旋转轴向量: 可能的原因是左右手坐标系弄混了
	// 暂时简单将vAxis反向
	vAxis = -vAxis;
	//////////////////////////////////////////////////////////////////////////  end
	SYQuatn_FromAngleAxis( pqQuatn, &vAxis, (float)fAngle );
}

void GetCubeIntersection(const vector3f* center,float fHeight,DX_QuadRang_Intersection* pInt,const matrix4f* pMatrix /*= NULL*/)
{
	matrix4f worldMatrix = *pMatrix;
	pInt->firstPoint.x = center->x - fHeight / 2; //0:x- y+ z-
	pInt->firstPoint.y = center->y + fHeight / 2;
	pInt->firstPoint.z = center->z - fHeight / 2;
	pInt->firstPoint *= worldMatrix;

	pInt->secondPoint.x = center->x - fHeight / 2; //1:x- y+ z+
	pInt->secondPoint.y = center->y + fHeight / 2;
	pInt->secondPoint.z = center->z + fHeight / 2;
	pInt->secondPoint *= worldMatrix;

	pInt->thirdPoint.x = center->x - fHeight / 2; //2:x- y- z-
	pInt->thirdPoint.y = center->y - fHeight / 2;
	pInt->thirdPoint.z = center->z - fHeight / 2;
	pInt->thirdPoint *= worldMatrix;

	pInt->forthPoint.x = center->x - fHeight / 2; //3:x- y- z+
	pInt->forthPoint.y = center->y - fHeight / 2;
	pInt->forthPoint.z = center->z + fHeight / 2;
	pInt->forthPoint *= worldMatrix;

	pInt->fifthPoint.x = center->x + fHeight / 2; //4:x+ y+ z-
	pInt->fifthPoint.y = center->y + fHeight / 2;
	pInt->fifthPoint.z = center->z - fHeight / 2;
	pInt->fifthPoint *= worldMatrix;

	pInt->sixthPoint.x = center->x + fHeight / 2; //5:x+ y+ z+
	pInt->sixthPoint.y = center->y + fHeight / 2;
	pInt->sixthPoint.z = center->z + fHeight / 2;
	pInt->sixthPoint *= worldMatrix;

	pInt->seventhPoint.x = center->x + fHeight / 2; //6:x+ y- z-
	pInt->seventhPoint.y = center->y - fHeight / 2;
	pInt->seventhPoint.z = center->z - fHeight / 2;
	pInt->seventhPoint *= worldMatrix;

	pInt->eighthPoint.x = center->x + fHeight / 2; //7:x+ y- z+
	pInt->eighthPoint.y = center->y - fHeight / 2;
	pInt->eighthPoint.z = center->z + fHeight / 2;
	pInt->eighthPoint *= worldMatrix;
}

SYHitPoint::~SYHitPoint()
{
	int nCount = GetPointCount();
	for (int i = 0; i < nCount; i++)
	{
		float3* pPoint = (float3*)SYAR_GetAt(&m_arHitPoint,i);
		SAFE_DELETE(pPoint);
	}
	SYAR_RemoveAll(&m_arHitPoint);
}

int SYHitPoint::AddPoint( const float3* pHitPoint )
{
	float3* newPoint = new float3;
	memcpy(newPoint,pHitPoint,sizeof(float3));
	return (int)SYAR_Add(&m_arHitPoint,newPoint);
}

bool SYHitPoint::DelPoint( int nIndex )
{
	float3* removePoint = (float3*)SYAR_GetAt(&m_arHitPoint,nIndex);
	SAFE_DELETE(removePoint);
	SYAR_RemoveAt(&m_arHitPoint,nIndex,1);
	return true;
}

const float3& SYHitPoint::operator[]( int nIndex )const
{
	return *GetPoint(nIndex);
}

BASEGRAPH_SCENE_RENDER_API int GetRayPlaneInt( const SYRay* pRay,const float3* pCenter,const float3* pNormal,SYHitPoint* pHitPoint )
{
	int return_result = SY_NO_HIT;
	float IsIntersec = float3DotProduct(pNormal, &pRay->vDir);
	float3 vDirect = *pCenter - pRay->vStart;
	float t = float3DotProduct(pNormal,&vDirect) / IsIntersec;
	if (fabs(IsIntersec) < 1e-7)
	{
		return_result = SY_NO_HIT;
	}
	else if (t > 0) 
	{
		return_result = SY_NORMAL_HIT;
		float3 hitPoint = pRay->vStart +  pRay->vDir * t;
		if(pHitPoint)
			pHitPoint->AddPoint(&hitPoint);
	}
	return return_result;
}

BASEGRAPH_SCENE_RENDER_API int GetSegmentCircleInt(const float3* pSegStart,const float3* pSegEnd,float fRadius,const float3* pCenter,const float3* pNormal,SYHitPoint* pHitPoint)
{
	int return_result = SY_NO_HIT;
	float3 vDirect = *pCenter - *pSegStart;
	float3 SegDir = *pSegEnd  - *pSegStart;
	float planeAngle = float3DotProduct(pNormal,&SegDir);
	float IsInt = float3DotProduct(&vDirect,pNormal);
	if (fabs(planeAngle) <= 5e-6f) //线段与平面平行
	{
		if (fabs(IsInt ) <= 5e-6f)//线段是否在平面上
		{
			float3 v;
			float3CrossProduct( &v ,pNormal, &SegDir);
			float3Normalize(&v,&v);
			float fDistance = float3DotProduct(&v,&vDirect);
			if (fabs(fDistance) <= fRadius)
			{
				float fSegLength = float3Mode(&SegDir);
				float fHitLength = float3DotProduct(&SegDir,&vDirect) / fSegLength;
				float d =  (fHitLength - sqrt(fRadius * fRadius - fDistance * fDistance)) / fSegLength;
				//近点
				if (d >= 0 && d <= 1)
				{
					float3 hitPoint = *pSegStart + d * SegDir;
					pHitPoint->AddPoint(&hitPoint);
					return_result = SY_NORMAL_HIT;
				}
				//远点
				d =  (fHitLength + sqrt(fRadius * fRadius - fDistance * fDistance)) / fSegLength;
				if (d >= 0 && d <= 1)
				{
					float3 hitPoint = *pSegStart + d * SegDir;
					pHitPoint->AddPoint(&hitPoint);
					return_result = SY_NORMAL_HIT;
				}
			}
		}
	}
	else
	{
		float t = IsInt / planeAngle;
		if(t >= 0 && t <= 1)
		{
			float3 hitPoint = *pSegStart + t * SegDir;
			if(float3Mode(&(hitPoint - *pCenter)) == fRadius)
			{
				pHitPoint->AddPoint(&hitPoint);
				return_result = SY_NORMAL_HIT;
			}
		}
	}
	return return_result;
}

BASEGRAPH_SCENE_RENDER_API int GetRaySphereInt( const SYRay* pRay,float fRadius,SYHitPoint* pHitPoint )
{
	float3 stPointToCenter = pRay->vStart;
	float projection = float3DotProduct(&stPointToCenter,&pRay->vDir);
	float length = float3DotProduct(&stPointToCenter,&stPointToCenter);
	float radius_2 = fRadius * fRadius;
	float fA = float3DotProduct(&pRay->vDir,&pRay->vDir);
	//float fB = 2 * projection;
	float fC = length - radius_2;
	if ( fabs(projection * projection / (fA * fC) - 1) <= ABS_FLOAT_0)
	{
		float3 hitpoint = pRay->vStart + pRay->vDir * projection / fA;
		pHitPoint->AddPoint(&hitpoint);
		return SY_HIT_EDGE;
	}

	float delt = projection * projection - fA * fC;
	int ret = SY_NO_HIT;
	if (delt >  0)
	{
		float t = (-projection - sqrtf(projection*projection - fA * fC)) / fA;
		if(t >= 0)
		{
			float3 hitpoint = pRay->vStart + pRay->vDir * t;
			pHitPoint->AddPoint(&hitpoint);
			ret = SY_NORMAL_HIT;
		}
		t = (-projection + sqrtf(projection*projection - fA * fC)) / fA;
		if (t >= 0)
		{
			float3 hitpoint = pRay->vStart + pRay->vDir * t;
			pHitPoint->AddPoint(&hitpoint);
			ret = SY_NORMAL_HIT;
		}
	}
	return ret;
}

int SY3DModelHitTest( int nSubModel,
					  const DXRenderSubModel* pSubModelDes,
					  const DWORD* pIndex,
					  const DXVertex3D* pVertex,          //物件顶点
					  const float3* pvScreenPt,	//屏幕坐标
					  const SYSpaceConvertParam* pParam,//其中的WORLDMatrix应为物件的世界矩阵
					  float3* pOutPt,BOOL isHitReturn)
{
	//float3 desVector,eyeDir;
	SYRay ray;
	GetRayDirection(&ray,pvScreenPt,pParam);
	return SY3DRayHitModel( nSubModel, pSubModelDes, pIndex, pVertex, &ray, pParam, pOutPt, isHitReturn );
}


 BASEGRAPH_SCENE_RENDER_API int SY3DRayHitModel( int nSubModel,
						  const DXRenderSubModel* pSubModelDes,
						  const DWORD* pIndex,
						  const DXVertex3D* pVertex,          //物件顶点
						  const SYRay* pRay,	
						  const SYSpaceConvertParam* pParam,//其中的WORLDMatrix应为物件的世界矩阵
						  float3* pOutPt,BOOL isHitReturn)
 {
	 const SYRay& ray = *pRay;

	 SYHitPoint HitPoint;
	 SYArray arHitModel;
	 SYAR_Init(&arHitModel);
	 float3 vPoint;
	 for (int j = 0; j < nSubModel; j++)
	 {
		 int nTri_Num = pSubModelDes[j].nTri_Num;
		 DWORD dwStartIndex = pSubModelDes[j].istart;
		 int nFirPtIndex = 0, nSedPtIndex = 0, nThrPtIndex = 0;
		 float3 FirstPoint = { 0, 0, 0 }, SecondPoint = { 0, 0, 0 }, ThirdPoint = { 0, 0, 0 };
		 if ( pSubModelDes[j].nTri_Type == DX_TRIANGLE_LIST )
		 {
			 for ( int i = 0; i < nTri_Num; i++ )
			 {
				 nFirPtIndex   = pIndex[ 3*i + dwStartIndex ];
				 nSedPtIndex   = pIndex[ 3*i + 1 + dwStartIndex ];
				 nThrPtIndex   = pIndex[ 3*i + 2 + dwStartIndex ];

				 if( IntersectTriangle( &ray.vStart, &ray.vDir,
					 &pVertex[nFirPtIndex].vDyn,&pVertex[nSedPtIndex].vDyn,
					 &pVertex[nThrPtIndex].vDyn, &vPoint ) == TRUE )
				 {
					 //printf("%d,%d,%d,Index:%d\n",nFirPtIndex,nSedPtIndex,nThrPtIndex,i);
					 if (isHitReturn)
						 return j;
					 HitPoint.AddPoint(&vPoint);
					 SYAR_Add(&arHitModel,(SYAR_ELEMENT)j);
				 }
			 }
		 }
		 else if ( pSubModelDes[j].nTri_Type == DX_TRIANGLE_FAN )
		 {
			 nFirPtIndex   = pIndex[dwStartIndex];
			 for ( int i = 0; i < nTri_Num; i++ )
			 {
				 nSedPtIndex   = pIndex[ i + 1 + dwStartIndex ];
				 nThrPtIndex   = pIndex[ i + 2 + dwStartIndex ];
				 if( IntersectTriangle( &ray.vStart, &ray.vDir, 
					 &pVertex[nFirPtIndex].vDyn,&pVertex[nSedPtIndex].vDyn,
					 &pVertex[nThrPtIndex].vDyn, &vPoint ) == TRUE )
				 {
					 if (isHitReturn)
						 return j;
					 HitPoint.AddPoint(&vPoint);
					 SYAR_Add(&arHitModel,(SYAR_ELEMENT)j);
				 }
			 }
		 }
		 else if ( pSubModelDes[j].nTri_Type == DX_TRIANGLE_STRIP )
		 {
			 for ( int i = 0; i < nTri_Num; i++ )
			 {
				 nFirPtIndex   = pIndex[ i + dwStartIndex ];
				 nSedPtIndex   = pIndex[ i + 1 + dwStartIndex ];
				 nThrPtIndex   = pIndex[ i + 2 + dwStartIndex ];

				 if( IntersectTriangle( &ray.vStart, &ray.vDir, 
					 &pVertex[nFirPtIndex].vDyn,&pVertex[nSedPtIndex].vDyn,
					 &pVertex[nThrPtIndex].vDyn, &vPoint ) == TRUE )
				 {
					 if (isHitReturn)
						 return j;
					 HitPoint.AddPoint(&vPoint);
					 SYAR_Add(&arHitModel,(SYAR_ELEMENT)j);
				 }
			 }
		 }
	 }
	 int nHitCount = HitPoint.GetPointCount();
	 if (nHitCount > 0)
	 {
		 matrix4f mat = *pParam->pWorldMat * *pParam->pViewMat;
		 int nearestPoint = HitPoint.GetNearestPt( &mat );

		 if(pOutPt)
			 *pOutPt = HitPoint[nearestPoint];
		 int nHitSubModel = (int)SYAR_GetAt(&arHitModel,nearestPoint);
		 SYAR_RemoveAll(&arHitModel);
		 return nHitSubModel;
	 }
	 return -1;
 }



BASEGRAPH_SCENE_RENDER_API void GetRayDirection(SYRay* pRay,const float3* pScreenPoint,const SYSpaceConvertParam* pParam)
{
	if(!pRay)
		return;
	float3 vec3DPoint;
	matrix4f viewInv,worldInv;
	matrix4f_inverse(&viewInv,pParam->pViewMat);
	matrix4f_inverse(&worldInv,pParam->pWorldMat);

	if(pParam->nProjType == SY_PERSPECTIVE_PROJ)
	{
		SpaceConvert(&pRay->vStart,pScreenPoint,pParam,SY2DVEC_TO_3D_WORLD);
		//vec3DPoint -= *pParam->m_pEyePos;
		pRay->vDir = pRay->vStart - *pParam->pEyePos * worldInv;
	}
	else if (pParam->nProjType == SY_ORTHO_PROJ)
	{
		vec3DPoint.x = 0;
		vec3DPoint.y = 0;
		vec3DPoint.z = 1;
		SpaceConvert(&pRay->vStart,pScreenPoint,pParam,SY2DVEC_TO_3D_WORLD);
		pRay->vDir = vec3DPoint * viewInv * worldInv;
		pRay->vDir = pRay->vDir - *pParam->pEyePos * worldInv;
	}
	float3Normalize(&pRay->vDir,&pRay->vDir);
}

BASEGRAPH_SCENE_RENDER_API float SYMouseMoveProjToDir( IN const SYSpaceConvertParam* pSpaceParam,IN const float3* p2DStart, IN const float3* p2DEnd, const SYLine* pLine,const float3* pNormal)
{
	if(!pNormal)
	{
		float3 vecSt2DPt, vecEnd2DPt;
		float3 vecLineEndPt = pLine->vStart + pLine->vDir;
		SpaceConvert(&vecSt2DPt,&pLine->vStart,pSpaceParam,SY3DVEC_TO_2D);
		SpaceConvert(&vecEnd2DPt,&vecLineEndPt,pSpaceParam,SY3DVEC_TO_2D);
		float3 vecProjToDir = vecEnd2DPt - vecSt2DPt, vecMouseMoveDis = *p2DEnd - *p2DStart;
		if(float3Mode(&vecProjToDir) <= ABS_FLOAT_0)
		{
			return vecMouseMoveDis.y;
		}
		float3Normalize( &vecProjToDir, &vecProjToDir );
		return float3DotProduct( &vecMouseMoveDis, &vecProjToDir );
	}
	else
	{
		float3 vecSt3DPt, vecEnd3DPt;
		float3 vecLineEndPt = pLine->vStart + pLine->vDir;
		BOOL bHIT = TRUE;
		bHIT &= SpaceConvert(&vecSt3DPt,p2DStart,pSpaceParam,SY2DVEC_TO_3D_PLANE,NULL,pNormal,&pLine->vStart);
		bHIT &= SpaceConvert(&vecEnd3DPt,p2DEnd,pSpaceParam,SY2DVEC_TO_3D_PLANE,NULL,pNormal,&pLine->vStart);
		if (!bHIT)
			return 0;
		float3 vecProjToDir, vecMouseMoveDis = vecEnd3DPt - vecSt3DPt;
		float3Normalize( &vecProjToDir, &pLine->vDir );
		return float3DotProduct( &vecMouseMoveDis, &vecProjToDir );
	}
	return 0;
}

BASEGRAPH_SCENE_RENDER_API float SY3DProjAngle( const SYSpaceConvertParam* pSpaceParam,
									  IN const float3* p2DStart, /*2D平面起始点 */ 
									  IN const float3* p2DEnd, /*2D平面结束点 */ 
									  const float3* pCenter,
									  const float3* pNormal )
{
	SYRay rayStart,rayEnd;
	GetRayDirection(&rayStart,p2DStart,pSpaceParam);
	GetRayDirection(&rayEnd,p2DEnd,pSpaceParam);

	SYHitPoint HitPoint1,HitPoint2;
	if(GetRayPlaneInt(&rayStart,pCenter,pNormal,&HitPoint1) == SY_NO_HIT ||
		GetRayPlaneInt(&rayEnd,pCenter,pNormal,&HitPoint2) == SY_NO_HIT)
	{
		float3 newAxis;
		float3CrossProduct(&newAxis,&rayStart.vDir,pNormal);
		return 10.f * float3DotProduct(&rayEnd.vDir,&newAxis);
	}

	float3 vFirst = HitPoint1[0] - *pCenter;
	float3 vSecond = HitPoint2[0] - *pCenter;
	double fAngle = float3AngleBetVec(&vFirst,&vSecond,pNormal);
	float3NormalMulMat(&vFirst,&vFirst,pSpaceParam->pWorldMat);
	float3NormalMulMat(&vSecond,&vSecond,pSpaceParam->pWorldMat);
	float3 normal;
	float3NormalMulMat(&normal,pNormal,pSpaceParam->pWorldMat);

	float fAngle2 = (float)float3AngleBetVec(&vFirst,&vSecond,&normal); 
	return fAngle2;
}

BASEGRAPH_SCENE_RENDER_API BOOL GetCircleInt( const SYRay* pRay,const float3* pCenter,const float3* pNormal,float Radius,SYHitPoint* pHitPoint )
{
	int return_result = SY_NO_HIT;
	if (GetRayPlaneInt(pRay,pCenter,pNormal,pHitPoint) == SY_NO_HIT) //射线与平面平行
	{
		vector3f vDirect = pRay->vStart - *pCenter;
		float3Normalize(&vDirect,&vDirect);
		float IsInt = float3DotProduct(&vDirect,pNormal);
		if (fabs(IsInt) < ABS_FLOAT_0)
		{
			vector3f v;
			float3CrossProduct( &v ,pNormal, &pRay->vDir);
			//vector3f vRayDirect = CirclePlane.Plane.point - CirclePlane.Ray.startPoint;
			float distance = fabs(float3DotProduct(&v,&vDirect));
			if (distance < Radius)
			{
				return_result = SY_NORMAL_HIT;
				pHitPoint->AddPoint(pCenter);
			}
		}
	}
	else
	{
		vector3f Sub ;
		Sub = *(pHitPoint->GetPoint(0))- *pCenter ;
		if (fabs(float3Mode(&Sub) / Radius - 1) < ABS_FLOAT_0)
		{
			return_result = SY_HIT_EDGE;
		}
		else if(float3Mode(&Sub) < Radius)
			return_result = SY_NORMAL_HIT;
	}
	return return_result;
}

BASEGRAPH_SCENE_RENDER_API BOOL GetPointInt( const SYRay* pRay,const float3* pPoint,float fDeviation /*= POINT_DEVIATION*/ )
{
	vector3f pointToStart = *pPoint - pRay->vStart;
	float projection = float3DotProduct(&pointToStart,&pRay->vDir);
	float length = float3DotProduct(&pointToStart, &pointToStart);
	if (length < fDeviation * fDeviation || (projection > 0 && (length - projection * projection) <= fDeviation * fDeviation ))
	{
		return SY_NORMAL_HIT;
	}
	return SY_NO_HIT;

}

void CreateBox( float3* pEightPt,const float3& vMin,const float3& vMax,float fWidth /*= 0*/,float fHeight /*= 0*/,float fThick /*= 0*/ )
{
	pEightPt[0].x = vMin.x - fWidth;
	pEightPt[0].y = vMax.y + fHeight;
	pEightPt[0].z = vMax.z + fThick;

	pEightPt[1].x = vMax.x + fWidth;
	pEightPt[1].y = vMax.y + fHeight;
	pEightPt[1].z = vMax.z + fThick;
	
	pEightPt[2].x = vMax.x + fWidth;
	pEightPt[2].y = vMax.y + fHeight;
	pEightPt[2].z = vMin.z - fThick;
	
	pEightPt[3].x = vMin.x - fWidth;
	pEightPt[3].y = vMax.y + fHeight;
	pEightPt[3].z = vMin.z - fThick;
	
	pEightPt[4].x = vMin.x - fWidth;
	pEightPt[4].y = vMin.y - fHeight;
	pEightPt[4].z = vMax.z + fThick;
	
	pEightPt[5].x = vMax.x + fWidth;
	pEightPt[5].y = vMin.y - fHeight;
	pEightPt[5].z = vMax.z + fThick;
	
	pEightPt[6].x = vMax.x + fWidth;
	pEightPt[6].y = vMin.y - fHeight;
	pEightPt[6].z = vMin.z - fThick;
	
	pEightPt[7].x = vMin.x - fWidth;
	pEightPt[7].y = vMin.y - fHeight;
	pEightPt[7].z = vMin.z - fThick;

}

int GetRayBoxInt( const SYRay* pRay,const float3* pMin,const float3* pMax,SYHitPoint* pHitPoint )
{
	float3 bound[8];
	CreateBox(bound,*pMin,*pMax);
	BOOL bHitPt = FALSE;
	bHitPt |= GetRaySegment(pRay,bound[0],bound[1],bound[2],bound[3],pHitPoint);
	bHitPt |= GetRaySegment(pRay,bound[4],bound[5],bound[6],bound[7],pHitPoint);
	bHitPt |= GetRaySegment(pRay,bound[2],bound[6],bound[7],bound[3],pHitPoint);
	bHitPt |= GetRaySegment(pRay,bound[1],bound[5],bound[6],bound[2],pHitPoint);
	bHitPt |= GetRaySegment(pRay,bound[0],bound[4],bound[7],bound[3],pHitPoint);
	bHitPt |= GetRaySegment(pRay,bound[0],bound[4],bound[5],bound[1],pHitPoint);

	return bHitPt;
}
