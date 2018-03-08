//////////////////////////////////////////////////////////////////////////////////
#ifndef __SYSHAPEFORM_H_
#define __SYSHAPEFORM_H_
///////////////////////////////////////////////////////////////////////////////
//form 
class BASEGRAPH_DATARENDER_API CForm
{
public:
	virtual void Create(){};
	virtual point2f FPt(const point2f &fPt)=0;
};

class BASEGRAPH_DATARENDER_API CTwistForm:public CForm
{
//data
protected:
	float m_fx;
	float m_fy;
	rectf m_RtBound;
public:
	virtual void Create(float fx,float fy,rectf* lpBound);
	virtual point2f FPt(const point2f &fPt);
};

typedef float (*LPCURVE_FUNC)(float fx);
class BASEGRAPH_DATARENDER_API CCurveForm:public CForm
{
protected:
	 int m_nTop;
	 int m_nBottom;
	 LPCURVE_FUNC m_CurveFunc[2];
public:
	virtual void Create(int nTop,int nBottom,LPCURVE_FUNC CurveFunc[2]);
	virtual point2f FPt(const point2f &fPt);
};

class BASEGRAPH_DATARENDER_API CFourPtForm:public CForm
{
protected:
	 rectf		m_Rt;
	 point2f	m_FormPt[4];
public:
	virtual void Create(const rectf &Rt,const point2f FormPt[]);
	virtual point2f FPt(const point2f &fPt);
};

class BASEGRAPH_DATARENDER_API CMatrixForm:public CForm
{
//Attributes
public:
	SYFMAT3		m_Mat;
public:
	CMatrixForm();
//operator
public:
	void Init();
	CMatrixForm& operator =(CMatrixForm &Other);
	CMatrixForm& operator *=(CMatrixForm &Other);
	virtual point2f FPt(const point2f &fPt);
};

class  BASEGRAPH_DATARENDER_API CRotateForm : public CMatrixForm
{
public:
	virtual void Create(float fRotate[]);
};

class BASEGRAPH_DATARENDER_API CRotateZForm:public CMatrixForm
{
public:
	virtual void Create(float fDegree);
	void CreateUseRadian(float fRadian);
	void Create(float cosine,float sine);
};

class BASEGRAPH_DATARENDER_API CRotateXYForm:public CMatrixForm
{
public:
	virtual void Create(int nAngleX,int nAngleY);
};

class  BASEGRAPH_DATARENDER_API CKirlForm:public CMatrixForm
{
public:
	virtual void Create(float fx,float fy);
};

class BASEGRAPH_DATARENDER_API CBallForm:public CForm
{
	point2f	m_fCenter;
	float	m_fKx;
	float	m_fKy;
	float	m_fR;
public:
	void Create(point2f &fCenter,float fKx,float fKy,float fR);
	virtual point2f FPt(const point2f &fPt);
};

class  BASEGRAPH_DATARENDER_API CScaleForm:public CMatrixForm
{
public:
	virtual void Create(float fx,float fy);
};
//////////////////////////////////////////////////////////////////////////
class BASEGRAPH_DATARENDER_API CSYPolgonRip
{
public:
	virtual void Form(CForm &objFm, vectorgraph* pGraph);
	virtual void Form(CForm &objFm, rectf* pRect, point3f* pPt);
	virtual void Form(CForm &objFm, point3f* paryPt, int nPtCnt);
};
//////////////////////////////////////////////////////////////////////////
//void		DrawForm(CDC &dc,const rectf* lpRect,CForm &FmObj,point2f*  lpOrg);
BASEGRAPH_DATARENDER_API int	CalLineDegree(point2f &Pt1,point2f &Pt2);
//////////////////////////////////////////////////////////////////////////
#endif//define __SYSHAPEFORM_H_