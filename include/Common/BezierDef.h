#ifndef __Common_BEZIER_INC__
#define __Common_BEZIER_INC__

#include "VectorDef.h"
#include "Common\CommonDataContainer.h"
#include "PlaneDef.h"
#include "CommonUtility.h"

struct curCtrl
{
	point2f   basePt;
	point2f   LeftPt;
	point2f   RightPt;
};	

enum  eSYCurveType
{
	CURVE_ARC = 0,				//arc curve
	CURVE_BEZIER = 1,			//random BEZIER
	CURVE_B = 2,				//b stripe
	CURVE_LINE = 3,				//line curve
};

struct CBaseMaskHandle
{
	int			m_nVersion;		//version
	point2f		m_Pt;			//Main control point,unity coordinates
	point2f		m_HelpPt[2];	//Previous and next control point of Bezier curve
	BOOL		m_bContinue;	//Wheather C1 is continuous with neighbour point

	float		m_fZ;
	float		m_fHelpZ[2];

	CBaseMaskHandle(point2f point)
	{
		m_nVersion = 1;
		m_Pt		= point;
		m_HelpPt[0] = point;
		m_HelpPt[1] = point;
		m_bContinue = TRUE;
		m_fZ = 0;
		m_fHelpZ[0]	= 0;
		m_fHelpZ[1] = 0;
	}

	CBaseMaskHandle(const point2f &point,
		const point2f &HPt1,
		const point2f &HPt2)
	{
		m_nVersion = 1;
		m_Pt		= point;
		m_HelpPt[0] = HPt1;
		m_HelpPt[1] = HPt2;
		m_bContinue = FALSE;
		m_fZ = 0;
		m_fHelpZ[0]	= 0;
		m_fHelpZ[1] = 0;
	}

	CBaseMaskHandle(const CBaseMaskHandle& other)
	{
		m_nVersion = 1;
		memcpy(this, &other, sizeof(CBaseMaskHandle));
	}

	void Empty(BOOL bInit = TRUE)
	{
		memset(this, 0, sizeof(CBaseMaskHandle));
		m_nVersion = 1;
	}

	CBaseMaskHandle()
	{
		Empty();
	}

	~CBaseMaskHandle()
	{
		Empty(FALSE);
	}

	CBaseMaskHandle & operator = (const CBaseMaskHandle& other)
	{
		Empty(FALSE);
		memcpy(this, &other, sizeof(CBaseMaskHandle));
		return *this ;
	}

	BOOL operator == (const CBaseMaskHandle& other)
	{
		if(m_nVersion == other.m_nVersion &&
			m_Pt		== other.m_Pt &&
			m_HelpPt[0] == other.m_HelpPt[0] &&
			m_HelpPt[1] == other.m_HelpPt[1] &&

			m_fZ == other.m_fZ &&
			m_fHelpZ[0] == other.m_fHelpZ[0] &&
			m_fHelpZ[1] == other.m_fHelpZ[1] &&

			m_bContinue == other.m_bContinue)
			return TRUE;
		else
			return FALSE;
	}

	BOOL operator != (const CBaseMaskHandle& other)
	{
		return !(*this == other);
	}
	void Reset()
	{
		m_nVersion = 0;
		m_Pt.x = 0;
		m_Pt.y = 0;
		m_HelpPt[0].x = 0;;
		m_HelpPt[0].y = 0;
		m_HelpPt[1].x = 0;
		m_HelpPt[1].y = 0;
		m_bContinue = FALSE;
		m_fZ = 0;
		m_fHelpZ[0] = 0;
		m_fHelpZ[1] = 0;
	}
};
typedef CBaseMaskHandle*   LPCommonMaskHandle;
typedef CBaseARray<CBaseMaskHandle, CBaseMaskHandle>  CBaseMaskHandleArray;


point2f*	__cdecl	SYMaskHandleGetControlPt(BaseARray &HandleArray, BOOL bClose, int &nCount);


//////////////////////////////////////////////////////////////////////////
//MICRO DEFIBNE 
#define MIDPT(pt0,pt1,pt)\
	pt.x = (pt0.x+pt1.x)/2.0f;\
	pt.y = (pt0.y+pt1.y)/2.0f;


//////////////////////////////////////////////////////////////////////////
//Bezier split node define
struct SPLITE_BEZIER3_NODE
{
	point2f	fPt;
	float	ft;
	SPLITE_BEZIER3_NODE(){}
	SPLITE_BEZIER3_NODE(point2f fpt0, float  ft0)
	{
		fPt= fpt0;
		ft = ft0;
	}
};

typedef CBaseARray<SPLITE_BEZIER3_NODE, SPLITE_BEZIER3_NODE> CBaseBezierSplitArray;

//Bezier split line info define
struct SBEIZER3_LINE_INFO
{
	float fLen;
	float ft;
};

typedef CBaseARray<SBEIZER3_LINE_INFO, SBEIZER3_LINE_INFO> CBaseBezier3LineArray;

//////////////////////////////////////////////////////////////////////////
void				SYBezierGet3Sp(point2f CtrlPt[], CBaseBezierSplitArray& Bezier_SplitArray);
BASEGRAPH_COMMON_API	void	__cdecl		SYBezierGet3Pt(point2f CtrlPt[], CBasePoint2fArray &PtArray);

void				SYBezier3GetLineInfo(point2f CtrlPt[], CBaseBezier3LineArray& arBezier3Line);

BASEGRAPH_COMMON_API	float	__cdecl		SYBezier3CalcParat(CBaseBezier3LineArray& arBezier3Line, float fCurveLen);

float			SYBezierGetValue2( float p0,float p1,float p2,float p3,float t);
BASEGRAPH_COMMON_API	point2f	__cdecl		SYBezierGetValue(point2f Pt[4], float t);

///////////////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API point2f	__cdecl		SYBezierGetCutValue( point2f Pt[4], float t);
BASEGRAPH_COMMON_API void	__cdecl		SYBezierCut(point2f pt[4], point2f pt1[4], point2f pt2[4], float t);
BASEGRAPH_COMMON_API float	__cdecl		SYBezierPtIn(const point2f  &pt0, point2f pt[4], float r, float t1, float t2);
BASEGRAPH_COMMON_API void	__cdecl		SYBezierGetPtArray(BaseARray &HandleArray, int nIndex, point2f CtrlPt[4]);
point2f*			SYBezierGetPtList(BaseARray &HandleArray);

BASEGRAPH_COMMON_API float	__cdecl		SYBezierGetYValueFromX(point2f CtrlPt[], float fX);

BOOL				bInLine(point2f &pt0, point2f &pt1, point2f &pt2);
BASEGRAPH_COMMON_API rectf	__cdecl		SYCacArcRect(point2f& pt0, point2f& pt1, point2f& pt2, float* pfR=NULL, point2f* pOrg=NULL);

BASEGRAPH_COMMON_API BOOL	__cdecl		SYGetTwoLineInterPt(const point2f &Pt1,const point2f &Pt2,const point2f &Pt3,const point2f &Pt4,point2f &InterPt);

////////////////判断一个2D点是否在任意2D多变形内部////////////////////////
// 算  法：从待判断点出发的任意射线（这里取Y轴正向）如果与多变形边界交点数
//         为奇数，则点在多边形内部，反之在外部。边界点视作在内部。
// 返回值：内部则返回true。
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool	__cdecl		SYIsPoint2fIn2DPolygon(point2f ptSpace[], int nPtCnt, point2f& pt);

//////////////////////////////////////////////////////////////////////////
enum  IP_KEY_RESULT_TYPE
{
	XY,
	XYZ,
	ONLYX,
	ONLYY,
	ONLYZ
};

struct InterPolateData
{
	int m_nVersion;
	IP_KEY_RESULT_TYPE	m_eResultType;
	double              m_dValue1[4];	//X In 0:m_dPreValue,1:m_dPreHelp1 2:m_dNextHelp0,3:m_dNextValue
	double              m_dValue2[4];	//Y In 	
	double              m_dValue3[4];	//Z In 	
	double				m_dCurValue[3]; //Out 0:X 1:Y 2:Z   
	void	Empty(BOOL bInit = TRUE)
	{
		m_nVersion = 1;
		m_eResultType = XYZ;
		for(int i=0;i<4;i++)
		{
			m_dValue1[i] = 0;
			m_dValue2[i] = 0;
			m_dValue3[i] = 0;
		}
		for(int i=0;i<3;i++)
		{
			m_dCurValue[i] = 0;
		}

	};
	InterPolateData()
	{
		Empty();
	}
	~InterPolateData()
	{
		Empty(FALSE);
	}
};

//interpolation type
enum EnumInterPolationType {
	Hold = 0,					//holding till the next keyframe or till stop
	Linear = 1,                 //till linear interpolation of next keyframe 
	Bezier = 2,                 //till bezier interpolation of next keyframe
	ContinousBezier = 3,        //continous bezier curve interpolation
	AutoBezier = 4,             //automatic bezier curve interpolation
	BezierL = 5,
	BezierR = 6,
};

struct  KeyInterPolationMode
{
	int						m_nVersion;
	EnumInterPolationType	m_nInterPolationType;		//interpolation method
	BOOL					m_bContinue;				//continue
	double                  m_dInSpeed;				    //in speed
	double                  m_dInInfluence;				//influence range of in speed
	double                  m_dOutSpeed;				//out speed
	double                  m_dOutInfluence;			//influence range of out speed
	int                     m_nIgnore;                  //1 ignore，0 effective

	void  Empty(BOOL bInit = TRUE)
	{
		m_nVersion = 1;  
		m_nInterPolationType = Linear;
		m_bContinue = TRUE;
		m_dInSpeed = 0.5;
		m_dInInfluence = 0.5;
		m_dOutSpeed = 0.5;
		m_dOutInfluence = 0.5;
		m_nIgnore = 0;
	}
};
typedef KeyInterPolationMode* LpKeyInterPolationMode;

struct BaseKeyStru
{
	int 					m_nBaseVersion;				//Version number
	double					m_dTime;					//timecode, second(unit)
	KeyInterPolationMode    m_ValueInterPolation;       //ket interpolation method 

	//for scale time
	double		m_dPreciseTime;	//精确时间
} ;
typedef BaseKeyStru* LpBaseKeyStru;

struct VelocityValueStruct : public BaseKeyStru
{
	int 					m_nVersion;					//版本号

	BOOL					m_bMajor;
	double					m_dValue;

	double					m_dHelpValue[2];			//插值辅助
	double					m_dHelpTime[2];
} ;
typedef	VelocityValueStruct* LpVelocityValueStruct;

BASEGRAPH_COMMON_API BOOL   __cdecl		SYKeyInterPolateValue(LpBaseKeyStru lpPreKey, LpBaseKeyStru lpNextKey,double dCurTime,InterPolateData* pData);

#endif