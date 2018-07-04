#include "stdafx.h"
#include "DataRender\ShapeForm.h"
#include "DataRender\AlgorithmRip.h"

/////////////////////////////////////////////////////////////////////////
//implement of CMatrixForm 
CMatrixForm::CMatrixForm()
{
	Init();
}
void CMatrixForm::Init()
{
	m_Mat[0][0]  = 1.0;   m_Mat[0][1]    = 0.0f;   m_Mat[0][2]  = 0.0f;
	m_Mat[1][0]  = 0.0f;  m_Mat[1][1]    = 1.0f;   m_Mat[1][2]  = 0.0f;
	m_Mat[2][0]  = 0.0f;  m_Mat[2][1]    = 0.0f;   m_Mat[2][2]  = 1.0f;
}

CMatrixForm& CMatrixForm::operator =(CMatrixForm &Other)
{
	memcpy(m_Mat,Other.m_Mat,SYSIZE_OF_FMAT3);
	return *this;
}
CMatrixForm&  CMatrixForm::operator *=(CMatrixForm &Other)
{
	SYFMAT3 Mat;
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
		{
			Mat[i][j] =0;
			for(int m=0;m<3;m++)
				Mat[i][j]+= m_Mat[i][m]*Other.m_Mat[m][j];
		}
	memcpy(m_Mat,Mat,SYSIZE_OF_FMAT3);
	return *this;
}
point2f CMatrixForm::FPt(const point2f &fPt)
{
	point2f fRet;
	fRet.x = fPt.x*m_Mat[0][0]+fPt.y*m_Mat[1][0]+m_Mat[2][0];
	fRet.y = fPt.x*m_Mat[0][1]+fPt.y*m_Mat[1][1]+m_Mat[2][1];
	return fRet;
}
//end of implement of CMatrixForm 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//implement of class CRotateXYForm
void CRotateForm::Create(float fRotate[])
{
	float fXSin =(float)sin(fRotate[0]);
	float fXCos =(float)cos(fRotate[0]);
	float fYSin =(float)sin(fRotate[1]);
	float fYCos =(float)cos(fRotate[1]);
	float fZSin =(float)sin(fRotate[2]);
	float fZCos =(float)cos(fRotate[2]);
	m_Mat[0][0] = fZCos*fYCos; m_Mat[0][1]= fZSin*fXCos+fZCos*fYSin*fXSin;
	m_Mat[1][0] = -fZSin*fYCos;m_Mat[1][1]= fZCos*fXCos-fZSin*fYSin*fXSin;
}
//end of implement class CRotateXYForm
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//implement of class CRotateXYForm
void CRotateXYForm::Create(int nAngleX,int nAngleY)
{
	nAngleX%=360;
	nAngleY%=360;
	if(nAngleX==90)
		nAngleX++;
	if(nAngleY==90)
		nAngleY++;
	float fx = (float)cos(nAngleX*PAIF/180.0f); 
	float fy = (float)cos(nAngleY*PAIF/180.0f); 

	float t1 = (float)sin(nAngleX*PAIF/180.0f); 
	float t2 = (float)sin(nAngleY*PAIF/180.0f); 

	m_Mat[0][0]  = fx;   
	m_Mat[0][1]  = t1*t2;   

	m_Mat[1][1]  = fy;
}
//end of implement of class CRotateXYForm
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//implement of class CRotateZForm
void CRotateZForm::Create(float fDegree)
{
	double dDegree = fDegree * PAI / 180.0;
	float	cosine = (float)cos(dDegree);
	float	sine   = (float)sin(dDegree);
	Create(cosine,sine);
}

void CRotateZForm::CreateUseRadian(float fRadian)
{
	float	cosine = (float)cos(fRadian);
	float	sine   = (float)sin(fRadian);
	Create(cosine,sine);
}

void CRotateZForm::Create(float	cosine,float sine)
{
	m_Mat[0][0]  = cosine;   m_Mat[0][1]  = sine;     m_Mat[0][2]  = 0.0f;
	m_Mat[1][0]  = -sine;    m_Mat[1][1]  = cosine;     m_Mat[1][2]  = 0.0f;
}

//end of implement of class CRotateZForm
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//implement of class CKirlForm
void CKirlForm::Create(float fx,float fy)
{
	{
		CKirlForm KirlForm;
		KirlForm.m_Mat[1][0]  = fx; 
		*this *=KirlForm;
	}
	{
		CKirlForm KirlForm;
		KirlForm.m_Mat[0][1]  = fy; 
		*this *=KirlForm;
	}
}
//end of implement of class CKirlForm
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//implement of class CBallForm
void CBallForm::Create(point2f &fCenter,float fKx,float fKy,float fR)
{
	assert(fKx<1.0f && fKy<1.0f); 

	m_fCenter = fCenter;
	m_fKx = fKx;
	m_fKy = fKy;
	m_fR = fR;
}
point2f CBallForm::FPt(const point2f &fPt)
{
	point2f fRet;
	float fx = fPt.x-m_fCenter.x;
	float fy = fPt.y-m_fCenter.y;

	float fxy2 =fx*fx+fy*fy;

	fRet.x =fPt.x+fx/(fxy2*m_fKx+1.0f)*2.0f;
	fRet.y =fPt.y+fy/(fxy2*m_fKy+1.0f)*2.0f;
	return fRet;
}
//end of implement of class CBallForm
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//implement of class CScaleForm
void CScaleForm::Create(float fx,float fy)
{
	m_Mat[0][0] = fx;
	m_Mat[1][1] = fy;
}
//end of implement class CScaleForm
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//implement of class CFourPtForm
void CFourPtForm::Create(const rectf &Rt,const point2f FormPt[])
{
	m_Rt		= Rt;
	memcpy(m_FormPt,FormPt,sizeof(point2f)*4);
}
point2f CFourPtForm::FPt(const point2f &fPt)
{
	point2f RtPt;
	RtPt.x = 0;
	RtPt.y = 0;

	float fL[2];
	float fTemp = SYRectf_Width(&m_Rt);
	if (fTemp > ABS_FLOAT_0)
	{
		fL[0] = (fPt.x-m_Rt.left) / fTemp;
	}
	else
	{
		fL[0] = fPt.x-m_Rt.left >= 0 ? 1 : -1;
	}
	fTemp = SYRectf_Height(&m_Rt);
	if (fTemp > ABS_FLOAT_0)
	{
		fL[1] = (fPt.y-m_Rt.top) / fTemp;
	}
	else
	{
		fL[1] = fPt.y-m_Rt.top >= 0 ? 1 : -1;
	}

	point2f Pt[4];
	Pt[0].x = m_FormPt[0].x+fL[1]*(m_FormPt[1].x-m_FormPt[0].x);
	Pt[0].y = m_FormPt[0].y+fL[1]*(m_FormPt[1].y-m_FormPt[0].y);

	Pt[1].x = m_FormPt[1].x+fL[0]*(m_FormPt[2].x-m_FormPt[1].x);
	Pt[1].y = m_FormPt[1].y+fL[0]*(m_FormPt[2].y-m_FormPt[1].y);

	Pt[2].x = m_FormPt[3].x+fL[1]*(m_FormPt[2].x-m_FormPt[3].x);
	Pt[2].y = m_FormPt[3].y+fL[1]*(m_FormPt[2].y-m_FormPt[3].y);

	Pt[3].x = m_FormPt[0].x+fL[0]*(m_FormPt[3].x-m_FormPt[0].x);
	Pt[3].y = m_FormPt[0].y+fL[0]*(m_FormPt[3].y-m_FormPt[0].y);
	SYGetTwoLineInterPt(Pt[0],Pt[2],Pt[1],Pt[3],RtPt);

	return RtPt;
}
//end of implement of class CFourPtForm
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//implement of class CTwistForm
void CTwistForm::Create(float fx,float fy,rectf* lpBound)
{
	m_fx		= fx;
	m_fy		= fy;
	m_RtBound   = *lpBound;
}
point2f CTwistForm::FPt(const point2f &fPt)
{
	point2f fRet;

	fRet.x = fPt.x+(m_RtBound.right+m_RtBound.left-2.0f*fPt.x)*m_fx
				  *(2.0f*(fPt.y-m_RtBound.top)/SYRectf_Height(&m_RtBound)-1.0f);

	fRet.y = fPt.y+(m_RtBound.top+m_RtBound.bottom-2.0f*fPt.y)*m_fy
				  *(2.0f*(fRet.x-m_RtBound.left)/SYRectf_Width(&m_RtBound)-1.0f);
	return fRet;
}
//implement of class CTwistForm
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void CCurveForm::Create(int nTop,int nBottom,LPCURVE_FUNC CurveFunc[2])
{
	m_nTop		 = nTop;
	m_nBottom	 = nBottom;
	m_CurveFunc[0] = CurveFunc[0];
	m_CurveFunc[1] = CurveFunc[1];
}
point2f CCurveForm::FPt(const point2f &fPt)
{
	point2f fRet;
	fRet.x =fPt.x;
	float fk = (m_CurveFunc[1](fPt.x)-m_CurveFunc[0](fPt.x))/(m_nBottom-m_nTop);
	fRet.y   = (fPt.y-m_nTop)*fk+m_CurveFunc[0]((float)m_nTop);
	return fRet;
}

////////////////////////////////////////////////////////////////////////////////
void CSYPolgonRip::Form(CForm &objFm, vectorgraph* pGraph)
{
	rectf* prtGraph = nullptr;
	if (pGraph->ncell > 0)
	{
		prtGraph = &pGraph->bound;
		prtGraph->left   = 65536;
		prtGraph->top    = 65536;
		prtGraph->right  = -65536;
		prtGraph->bottom = -65536;
	}

	for(int nC=0; nC<pGraph->ncell; ++nC)
	{
		vectorcell* pCell = &(pGraph->cell[nC]);
		rectf& rtCell = pCell->bound;
		point2f*& pPT = pCell->point;

		pPT[0] = objFm.FPt(pPT[0]);
		rtCell.left   = pPT[0].x;
		rtCell.right  = rtCell.left;
		rtCell.top    = pPT[0].y;
		rtCell.bottom = rtCell.top;

		for(int nP = 1; nP<pCell->npoint; ++nP)
		{
			pPT[nP] = objFm.FPt(pPT[nP]);
			UPDATE_BOUNDF(rtCell, pPT[nP]);
		}

		pGraph->bound.left   = min(prtGraph->left, rtCell.left);
		pGraph->bound.top    = min(prtGraph->top, rtCell.top);
		pGraph->bound.right  = max(prtGraph->right, rtCell.right);
		pGraph->bound.bottom = max(prtGraph->bottom, rtCell.bottom);
	}
}

void CSYPolgonRip::Form(CForm &objFm, rectf* pRect, point3f* pPt)
{
	point2f pt2;
	if (pPt != NULL)
	{
		pt2.x = pPt->x;
		pt2.y = pPt->y;
		pt2 = objFm.FPt(pt2);
		pPt->x = pt2.x;
		pPt->y = pt2.y;
	}

	if (pRect != NULL)
	{
		rectf rtTemp;
		pt2.x = pRect->left;
		pt2.y = pRect->top;
		pt2 = objFm.FPt(pt2);
		rtTemp.right = rtTemp.left = pt2.x;
		rtTemp.bottom = rtTemp.top = pt2.y;

		pt2.x = pRect->right;
		pt2.y = pRect->top;
		pt2 = objFm.FPt(pt2);
		UPDATE_BOUNDF(rtTemp, pt2);

		pt2.x = pRect->left;
		pt2.y = pRect->bottom;
		pt2 = objFm.FPt(pt2);
		UPDATE_BOUNDF(rtTemp, pt2);

		pt2.x = pRect->right;
		pt2.y = pRect->bottom;
		pt2 = objFm.FPt(pt2);
		UPDATE_BOUNDF(rtTemp, pt2);

		*pRect = rtTemp;
	}
}

void CSYPolgonRip::Form(CForm &objFm, point3f* paryPt, int nPtCnt)
{
	if (paryPt != NULL)
	{
		point2f pt2;
		for (int i = 0; i < nPtCnt; ++i)
		{
			pt2.x = paryPt[i].x;
			pt2.y = paryPt[i].y;
			pt2 = objFm.FPt(pt2);
			paryPt[i].x = pt2.x;
			paryPt[i].y = pt2.y;
		}
	}
}
