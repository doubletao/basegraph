#include "stdAfx.h"
#include "Common\BezierDef.h"
#include <assert.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////
point2f* SYMaskHandleGetControlPt(
								BaseARray			&HandleArray,	//in
								BOOL				bClose,		//in 
								int				&nCount		//ount control points for rip
								)
{
	int nHandleCount = BaseAR_GetCount(&HandleArray);
	if(nHandleCount==0)
	{
		nCount = 0;
		return NULL;
	}
	else
	{
		int   nHandleC;

		if(bClose)
		{
			nHandleC = nHandleCount;
			nCount   = nHandleCount*3+1;
		}else
		{
			nHandleC = nHandleCount-1;
			nCount   = (nHandleCount-1)*3+1;
		}
		point2f* pCtrl    = new point2f[nCount];
		point2f* pCurCtrl = pCtrl;

		LPCommonMaskHandle pHandle;
		for(int nIndex =0;nIndex<nHandleC;nIndex++)
		{
			pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nIndex);
			pCurCtrl->x = pHandle->m_Pt.x;
			pCurCtrl->y = pHandle->m_Pt.y;
			pCurCtrl++;
			pCurCtrl->x = pHandle->m_HelpPt[1].x;
			pCurCtrl->y = pHandle->m_HelpPt[1].y;
			pCurCtrl++;

			pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, (nIndex+1)%nHandleCount);
			pCurCtrl->x = pHandle->m_HelpPt[0].x;
			pCurCtrl->y = pHandle->m_HelpPt[0].y;
			pCurCtrl++;
		}
		if(bClose)
		{
			pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, 0);
			pCurCtrl->x = pHandle->m_Pt.x;
			pCurCtrl->y = pHandle->m_Pt.y;
		}else
		{
			pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nHandleCount-1);
			pCurCtrl->x = pHandle->m_Pt.x;
			pCurCtrl->y = pHandle->m_Pt.y;
		}
		return pCtrl;
	}
}

//////////////////////////////////////////////////////////////////
struct PtBintreeNode
{
	point2f fPt;
	float  ft;
	PtBintreeNode *lChild,*rChild;
	PtBintreeNode()
	{
		lChild = NULL;
		rChild = NULL;
		ft	   = 0.0f;	
	}
};
typedef PtBintreeNode* LPPtBintree;  

void GetPtBintreeNode(LPPtBintree PtBintree, CBaseBezierSplitArray &PtArray)
{
	if(PtBintree)
	{
		if(PtBintree->lChild)
			GetPtBintreeNode(PtBintree->lChild,PtArray);
		PtArray.Add(SPLITE_BEZIER3_NODE(PtBintree->fPt,PtBintree->ft));
		if(PtBintree->rChild)
			GetPtBintreeNode(PtBintree->rChild,PtArray);
	}
}
void FreePtBintreeNode(LPPtBintree PtBintree)
{
	if(PtBintree)
	{
		if(PtBintree->lChild)
			FreePtBintreeNode(PtBintree->lChild);
		if(PtBintree->rChild)
			FreePtBintreeNode(PtBintree->rChild);
		delete PtBintree;
	}
}

inline LPPtBintree  SplitBezier(point2f  F0,point2f F1,point2f G0,point2f G1,float ft0,float ft1)
{
#define    MIN_SPLIT_SIZE   0.1f
	point2f GMid;
	GMid.x = (G0.x+G1.x)/8.0f;
	GMid.y = (G0.y+G1.y)/8.0f;
	float fMidt = (ft0+ft1)/2.0f;
	LPPtBintree PtBintree = new PtBintreeNode;
	PtBintree->fPt.x = (F0.x+F1.x)/2.0f-GMid.x;
	PtBintree->fPt.y = (F0.y+F1.y)/2.0f-GMid.y;
	PtBintree->ft	 = fMidt;
	if(fabs(GMid.x)>MIN_SPLIT_SIZE  || fabs(GMid.y)>MIN_SPLIT_SIZE)
	{
		G0.x = G0.x/4.0f;
		G0.y = G0.y/4.0f;
		G1.x = G1.x/4.0f;
		G1.y = G1.y/4.0f;
		PtBintree->lChild = SplitBezier(F0,PtBintree->fPt,G0,GMid,ft0,fMidt);
		PtBintree->rChild = SplitBezier(PtBintree->fPt,F1,GMid,G1,fMidt,ft1);
	}
	return PtBintree;
}

//////////////////////////////////////////////////////////////////////////
void CommonBezierGet3Sp(point2f CtrlPt[], CBaseBezierSplitArray& arBezierSplit)
{
	point2f G0,G1;
	G0.x = (CtrlPt[0].x-2*CtrlPt[1].x+CtrlPt[2].x)*3.0f;
	G0.y = (CtrlPt[0].y-2*CtrlPt[1].y+CtrlPt[2].y)*3.0f;

	G1.x = (CtrlPt[1].x-2*CtrlPt[2].x+CtrlPt[3].x)*3.0f;
	G1.y = (CtrlPt[1].y-2*CtrlPt[2].y+CtrlPt[3].y)*3.0f;
	LPPtBintree  PtBintree = SplitBezier(CtrlPt[0],CtrlPt[3],G0,G1,0.0f,1.0f);
	GetPtBintreeNode(PtBintree, arBezierSplit);
	FreePtBintreeNode(PtBintree);
}

void CommonBezierGet3Pt(point2f CtrlPt[], CBasePoint2fArray &PtArray)
{
	CBaseBezierSplitArray arBezierSplit;
	CommonBezierGet3Sp(CtrlPt, arBezierSplit);

	int nSplitCnt = arBezierSplit.GetCount();
	for(int nIndex =0;nIndex<nSplitCnt;nIndex++)
	{
		PtArray.Add(arBezierSplit.GetAt(nIndex).fPt);
	}
}

void GetBezier3_Split_Info(point2f CtrlPt[], CBaseBezierSplitArray& arBezierSplit)
{
	arBezierSplit.Add(SPLITE_BEZIER3_NODE(CtrlPt[0], 0.0f));
	CommonBezierGet3Sp(CtrlPt, arBezierSplit);
	arBezierSplit.Add(SPLITE_BEZIER3_NODE(CtrlPt[3], 1.0f));
}

void GetBezier3_Line_Info2(CBaseBezierSplitArray& arBezierSplit, CBaseBezier3LineArray& arBezier3Line)
{
	int nSize = arBezierSplit.GetCount();
	arBezier3Line.SetSize(nSize);

	arBezier3Line[0].fLen = 0.0f;
	arBezier3Line[0].ft   = arBezierSplit.GetAt(0).ft;

	for(int i = 1; i<nSize; ++i)
	{
		arBezier3Line[i].fLen = SYPt2fDistance(arBezierSplit[i].fPt, arBezierSplit[i-1].fPt) + arBezier3Line[i-1].fLen;
		arBezier3Line[i].ft   = arBezierSplit[i].ft;
	}
}

void CommonBezier3GetLineInfo(point2f CtrlPt[], CBaseBezier3LineArray& arBezier3Line)
{
	CBaseBezierSplitArray arBezierSplit;
	GetBezier3_Split_Info(CtrlPt, arBezierSplit);
	GetBezier3_Line_Info2(arBezierSplit, arBezier3Line);
}

float CommonBezier3CalcParat(CBaseBezier3LineArray& arBezier3Line, float fCurveLen)
{
	int nB3LineCnt = arBezier3Line.GetCount();

	int nS = 0, nE = nB3LineCnt-1, nMid;
	while (nS+1<nE)
	{
		nMid = (nS+nE)/2;
		if (fCurveLen <= arBezier3Line.GetAt(nMid).fLen)
			nE = nMid;
		else
			nS = nMid;
	}

	if (fCurveLen < arBezier3Line.GetAt(nS).fLen && nS>0)
		nS--;

	SBEIZER3_LINE_INFO* pB3Info = (SBEIZER3_LINE_INFO*)&(arBezier3Line.GetAt(nS));
	SBEIZER3_LINE_INFO* pB3InfoNext = (SBEIZER3_LINE_INFO*)&(arBezier3Line.GetAt(nS+1));
	float ft = pB3Info->ft + (pB3InfoNext->ft - pB3Info->ft)*(pB3Info->fLen)
				/(pB3InfoNext->fLen - pB3Info->fLen);
	return ft;
}

/*********************************************************************/
/*Name:		CommonBezierGetValue
/*function: calculate three degree Bezier value
/*Paremeter:p0--p3 is control point 
/*          t---bezier parameter
/*********************************************************************/
float CommonBezierGetValue2( float p0,float p1,float p2,
						float  p3,float t)
{
	float  t1  = 1.0f-t;
	float  t11 = t1*t1;
	float  tt  = t*t;
	return (t1*p0+3.0f*t*p1)*t11+(3.0f*t1*p2+t*p3)*tt;
}
point2f CommonBezierGetValue(point2f Pt[4], float t)
{
	// 根据时间来计算，曲线不是均匀增长的
	point2f tPt;
	tPt.x = CommonBezierGetValue2(Pt[0].x,Pt[1].x,Pt[2].x,Pt[3].x,t);
	tPt.y = CommonBezierGetValue2(Pt[0].y,Pt[1].y,Pt[2].y,Pt[3].y,t);
	return  tPt;
}

float CommonBezierGetCutValue2( float p0,float p1,float p2,
						float  p3,float t)
{ 
	float  t1  = 1.0f-t;
	float  t11 = t1*t1;
	float  tt  = t*t;
	return  3.0f*(-p0*t11+p1*(t11-t1*2.0f*t)+p2*(2.0f*t*t1-tt)+p3*tt);
}

point2f CommonBezierGetCutValue(point2f Pt[4], float t)
{
	point2f tPt;
	tPt.x = CommonBezierGetCutValue2(Pt[0].x,Pt[1].x,Pt[2].x,Pt[3].x,t);
	tPt.y = CommonBezierGetCutValue2(Pt[0].y,Pt[1].y,Pt[2].y,Pt[3].y,t);
	return  tPt;
}

/////////////////////////////////////////////////////////////////////
//Cut Bezier Curver two part
//pt[4]--------Original thress degree Bezier Curver control points
//pt1[4]-------First cutted part of thress degree Bezier Curver control points
//pt2[4]-------Second cutted part of thress degree Bezier Curver control points
//t-----------cut point parameter 
/////////////////////////////////////////////////////////////////////
void CommonBezierCut(point2f pt[4],	//In
			   point2f pt1[4],	//Out
			   point2f pt2[4],	//Out
			   float  t			//In	
			   )
{
	assert(t>=0.0f && t<=1.0f); 
	float P01x=(1.0f-t)*pt[0].x+t*pt[1].x;
	float P01y=(1.0f-t)*pt[0].y+t*pt[1].y;

	float P11x=(1.0f-t)*pt[1].x+t*pt[2].x;
	float P11y=(1.0f-t)*pt[1].y+t*pt[2].y;

	float P21x=(1.0f-t)*pt[2].x+t*pt[3].x;
	float P21y=(1.0f-t)*pt[2].y+t*pt[3].y;

	float P02x=(1.0f-t)*P01x+t*P11x;
	float P02y=(1.0f-t)*P01y+t*P11y;

	float P12x=(1.0f-t)*P11x+t*P21x;
	float P12y=(1.0f-t)*P11y+t*P21y;

	float P03x=(1.0f-t)*P02x+t*P12x;
	float P03y=(1.0f-t)*P02y+t*P12y;

	pt1[0]=pt[0];
	pt1[1].x = P01x;
	pt1[1].y = P01y;
	pt1[2].x = P02x;
	pt1[2].y = P02y;
	pt1[3].x = P03x;
	pt1[3].y = P03y;

	pt2[0]=pt1[3];
	pt2[1].x = P12x;
	pt2[1].y = P12y;
	pt2[2].x = P21x;
	pt2[2].y = P21y;
	pt2[3]=pt[3];
}

/////////////////////////////////////////////////////////////////////
//Judge wheather pt0  is on Bezier Curver
//pt[4]--------thress degree Bezier Curver control points
//r------------Difference limit radiaus
//return value:  
//			TRUE:	pt0  is on Bezier Curver,Ptv is VERIFYED POINT
//			FALSE:  pt0  is not on Bezier Curver
/////////////////////////////////////////////////////////////////////
float CommonBezierPtIn(const point2f &pt0,	//In
				 point2f Pt[4],			//In
				 float  r,				//In
				 float  t1,				//Out
				 float  t2				//Out
				 )
{
	float fMinDis = r*2;
	float fRet    = -1.0f;
	int   nti     = -1;
	float fMid    = (t1+t2)/2.0f;
	float t[]     = {0.0f,0.5f,1.0f},fDis;
	point2f tPt;
	rectf Rtf = SYGetBoundfFromPt2fs(Pt);
	for(int i=0;i<3;i++)
	{
		tPt.x = CommonBezierGetValue2(Pt[0].x,Pt[1].x,Pt[2].x,Pt[3].x,t[i]);
		tPt.y = CommonBezierGetValue2(Pt[0].y,Pt[1].y,Pt[2].y,Pt[3].y,t[i]);
		fDis  = SYPt2fDistance(tPt,*(point2f*)&pt0);

		//Find Nearest point In curver
		if(fDis<fMinDis)   
		{
			fMinDis = fDis;
			nti		= i;
		}
	}

	//Find In other Segments
	if(nti>=0)
	{
		switch(nti)
		{
		case 0:
			return  t1;
		case 1:
			return  fMid;
		case 2:
			return  t2;
		}
	}
	else
	{
		float fW = BaseRectf_Width(&Rtf);
		float fH = BaseRectf_Height(&Rtf);
		if (fW<r*2.0f && fH<r*2.0f && BaseRectf_PtInRect(&Rtf, pt0))
			return fMid;

		if(fW<r*2.0f)
		{
			Rtf.left  -= r-fW;
			Rtf.right += r-fW;
		}
		if(fH<r*2.0f)
		{
			Rtf.top    -= r-fH;
			Rtf.bottom += r-fH;
		}
		if(BaseRectf_PtInRect(&Rtf, pt0))
		{
			point2f p1[4],p2[4];
			CommonBezierCut(Pt,p1,p2,0.5f);
			fRet = CommonBezierPtIn(pt0,p1,r,t1,fMid);
			if(fRet<0.0f)           
				return CommonBezierPtIn(pt0,p2,r,fMid,t2);
			else
				return fRet;
		}
	}
	return -1.0f;
}

void  CommonBezierGetPtArray(BaseARray &HandleArray,int nIndex,point2f CtrlPt[4])
{
	LPCommonMaskHandle pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nIndex);
	int nVer=0;
	CtrlPt[nVer].x = pHandle->m_Pt.x;
	CtrlPt[nVer].y = pHandle->m_Pt.y;
	nVer++;
	CtrlPt[nVer].x = pHandle->m_HelpPt[1].x;
	CtrlPt[nVer].y = pHandle->m_HelpPt[1].y;
	nVer++;

	pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nIndex+1);
	CtrlPt[nVer].x = pHandle->m_HelpPt[0].x;
	CtrlPt[nVer].y = pHandle->m_HelpPt[0].y;
	nVer++;
	CtrlPt[nVer].x = pHandle->m_Pt.x;
	CtrlPt[nVer].y = pHandle->m_Pt.y;
}

point2f* CommonBezierGetPtList(BaseARray &HandleArray)
{
	int nHandleCount = BaseAR_GetCount(&HandleArray);
	if(nHandleCount==0)
	{
		return NULL;
	}
	else
	{
		int nCount = (nHandleCount-1)*3+1;
		point2f* pCtrl    = new point2f[nCount];
		point2f* pCurCtrl = pCtrl;
		LPCommonMaskHandle pHandle;
		for(int nIndex =0;nIndex<nHandleCount-1;nIndex++)
		{
			pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nIndex);
			pCurCtrl->x = pHandle->m_Pt.x;
			pCurCtrl->y = pHandle->m_Pt.y;
			pCurCtrl++;
			pCurCtrl->x = pHandle->m_HelpPt[1].x;
			pCurCtrl->y = pHandle->m_HelpPt[1].y;
			pCurCtrl++;

			pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nIndex+1);
			pCurCtrl->x = pHandle->m_HelpPt[0].x;
			pCurCtrl->y = pHandle->m_HelpPt[0].y;
			pCurCtrl++;
		}
		pHandle = (LPCommonMaskHandle)BaseAR_GetAt(&HandleArray, nHandleCount-1);
		pCurCtrl->x = pHandle->m_Pt.x;
		pCurCtrl->y = pHandle->m_Pt.y;
		return pCtrl;
	}
}

float CommonBezierGetYValueFromX(point2f CtrlPt[], float fX)
{
	rectf Rtf = SYGetBoundfFromPt2fs(CtrlPt);
	if (BaseRectf_Width(&Rtf) < 0.0005f || BaseRectf_Height(&Rtf) < 0.0005f)
	{
		point2f ptCent;
		BaseRectf_CenterPoint(&ptCent, &Rtf);
		return ptCent.y;
	}
	else
	{
		point2f fptMid,fptMid0,fptMid1,fpt0,fpt1,fpt2;
		MIDPT(CtrlPt[0],CtrlPt[1],fpt0);
		MIDPT(CtrlPt[1],CtrlPt[2],fpt1);
		MIDPT(CtrlPt[2],CtrlPt[3],fpt2);
		MIDPT(fpt0,fpt1,fptMid0);
		MIDPT(fpt1,fpt2,fptMid1);
		MIDPT(fptMid0,fptMid1,fptMid);
		point2f SubCtrlPt[4];
		if(fX==fptMid.x && 0)
		{
			point2f ptCent;
			BaseRectf_CenterPoint(&ptCent, &Rtf);
			return ptCent.y;
		}
		else if(fX<fptMid.x)
		{
			SubCtrlPt[0]=CtrlPt[0];
			SubCtrlPt[1]=fpt0;
			SubCtrlPt[2]=fptMid0;
			SubCtrlPt[3]=fptMid;
		}
		else
		{
			SubCtrlPt[0]=fptMid;
			SubCtrlPt[1]=fptMid1;
			SubCtrlPt[2]=fpt2;
			SubCtrlPt[3]=CtrlPt[3];
		}
		return CommonBezierGetYValueFromX(SubCtrlPt,fX);
	}
}

BOOL bInLine(point2f &pt0,point2f &pt1,point2f &pt2)
{
	return FALSE;
	float fDis0 = SYPt2fDistance(pt0,pt1);
	float fDis1 = SYPt2fDistance(pt1,pt2);
	float fDis2 = SYPt2fDistance(pt2,pt1);
	if(fDis0+fDis1<fDis2+0.5f)
		return TRUE;
	if(fDis1+fDis2<fDis0+0.5f)
		return TRUE;
	if(fDis0+fDis2<fDis1+0.5f)
		return TRUE;
	return FALSE;
}

rectf  SYCacArcRect(point2f& pt0,point2f& pt1, point2f& pt2,float* pfR,point2f* pOrg)
{
	rectf Rt;
	memset(&Rt, 0, sizeof(rectf));

	if(!bInLine(pt0,pt1,pt2))
	{
		point2f PtDiff0,PtMid0,PtOther0;
		PtDiff0.x  =pt1.x-pt0.x;
		PtDiff0.y  =pt1.y-pt0.y;
		PtMid0.x   =(pt1.x+pt0.x)/2.0f;
		PtMid0.y   =(pt1.y+pt0.y)/2.0f;

		PtOther0.x =PtMid0.x-PtDiff0.y;
		PtOther0.y =PtMid0.y+PtDiff0.x;


		point2f PtDiff1,PtMid1,PtOther1;
		PtDiff1.x =pt2.x-pt1.x;
		PtDiff1.y =pt2.y-pt1.y;

		PtMid1.x  =(pt1.x+pt2.x)/2.0f;
		PtMid1.y  =(pt1.y+pt2.y)/2.0f;
		PtOther1.x =PtMid1.x-PtDiff1.y;
		PtOther1.y =PtMid1.y+PtDiff1.x;

		point2f Org;
		if(!SYGetTwoLineInterPt(PtMid0,PtOther0,PtMid1,PtOther1,Org))
			return Rt;
		if(pOrg)
			*pOrg = Org;

		float fR =SYPt2fDistance(Org,pt0);
		if(pfR)
			*pfR = fR; 

		//if(fR>360)
		//	return  CRect(0,0,0,0);
		BaseRectf_OffsetRect1(&Rt, Org.x, Org.y);
		BaseRectf_InflateRect1(&Rt, fR, fR);
	}
	return Rt;
}

BOOL  SYGetTwoLineInterPt(
						const point2f &Pt1,
						const point2f &Pt2,
						const point2f &Pt3,
						const point2f &Pt4,
						point2f		 &InterPt
						)
{
	float  X21 = Pt2.x-Pt1.x;
	float  Y12 = Pt1.y-Pt2.y;

	float  X43 = Pt4.x-Pt3.x;
	float  Y34 = Pt3.y-Pt4.y;
	float  b1  = Pt1.y*X21+Pt1.x*Y12;
	float  b2  = Pt3.y*X43+Pt3.x*Y34;
	float  A   = X21*Y34-X43*Y12;

	if(fabs(A)<ABS_FLOAT_0)
	{
		InterPt.x = (Pt2.x+Pt3.x+Pt4.x+Pt1.x)/4.0f;
		InterPt.y = (Pt2.y+Pt3.y+Pt4.y+Pt1.y)/4.0f;
		return FALSE;
	}else 
	{
		InterPt.y = (b1*Y34-b2*Y12)/A;
		InterPt.x = (b2*X21-b1*X43)/A;
		return TRUE;
	}
}

bool SYIsPoint2fIn2DPolygon(point2f ptSpace[], int nPtCnt, point2f& pt)
{
	float x = pt.x;
	float y = pt.y;
	int nInterCnt = 0;		//交点计数器

	for (int i = 0; i < nPtCnt; ++i)
	{
		point2f* pCurPt = &ptSpace[i];	//当前点
		point2f* pNextPt;				//下一点，与当前点构成当前线段。
		if (i < nPtCnt - 1)
			pNextPt = &ptSpace[i+1];
		else
			pNextPt = &ptSpace[0];

		//只处理与射线相交的线段
		float fCurSubX  = pCurPt->x - x;
		float fNextSubX = pNextPt->x - x;
		if ( (fCurSubX >= 0.0f && fNextSubX <= 0.0f) || (fCurSubX <= 0.0 && fNextSubX >= 0.0f) )
		{
			bool y_flag = false;
			if ( (pCurPt->y >= y && pNextPt->y <= y) || (pCurPt->y <= y && pNextPt->y >= y) )
			{
				y_flag = true;
			}

			bool bCaledyc = false;
			float yc;					//x在当前线段上的y值
			float fNextSubCurX = pNextPt->x - pCurPt->x;
			if (fNextSubCurX == 0.0f && y_flag)
			{
				nInterCnt = 1;
				break;
			}
			else if (fNextSubCurX == 0.0f)
			{
				break;
			}
			else if (y_flag)
			{
				yc = (pCurPt->y - pNextPt->y) / fNextSubCurX*fCurSubX + pCurPt->y;
				if (yc == y)
				{
					nInterCnt = 1;
					break;
				}
				bCaledyc = true;
			}

			if (!bCaledyc)
			{
				yc = (pCurPt->y - pNextPt->y) / fNextSubCurX*fCurSubX + pCurPt->y;
			}
			if ( (yc > y) && ((fCurSubX > 0.0f && fNextSubX <= 0.0f) || (fCurSubX <= 0 && fNextSubX > 0.0f)) )
			{
				++nInterCnt;
			}
		}
	}

	//如果有偶数个交点则为外部点
	if (nInterCnt % 2 == 0)
		return false;
	else
		return true;
}


//////////////////////////////////////////////////////////////////////////
BOOL SYKeyInterPolateValue(LpBaseKeyStru lpPreKey, LpBaseKeyStru lpNextKey,double dCurTime,InterPolateData* pData)
{
	point2f	KeyCtrlPt[4];
	for(int nVer=0;nVer<4;nVer++)
	{
		KeyCtrlPt[nVer].x = (float)pData->m_dValue1[nVer];
		KeyCtrlPt[nVer].y = (float)pData->m_dValue2[nVer];
	}

	double dTotalTime = lpNextKey->m_dTime-lpPreKey->m_dTime;
	if(fabs(dTotalTime)<ABS_DOUBLE_0)
	{
		pData->m_dCurValue[0]=pData->m_dValue1[0];
		pData->m_dCurValue[1]=pData->m_dValue2[0];
		pData->m_dCurValue[2]=pData->m_dValue3[0];
		return TRUE;
	}
	else if(dTotalTime<-ABS_DOUBLE_0)
		return FALSE;


	BOOL bLiner    = (lpPreKey->m_ValueInterPolation.m_nInterPolationType==Linear
						&&lpNextKey->m_ValueInterPolation.m_nInterPolationType==Linear);
	float t		   = (float)((dCurTime-lpPreKey->m_dTime)/dTotalTime);
	switch(pData->m_eResultType)
	{
	case XYZ:
		pData->m_dCurValue[2] = pData->m_dValue3[0]+(pData->m_dValue3[3]-pData->m_dValue3[0])*t;
	case XY:
		{
			CBaseBezier3LineArray arBezier3Line;
			CommonBezier3GetLineInfo(KeyCtrlPt, arBezier3Line);

			int nSize       = arBezier3Line.GetCount();
			float fST       = arBezier3Line.GetAt(nSize-1).fLen;
			float fInPara   = (float)lpPreKey->m_ValueInterPolation.m_dOutInfluence;
			float fOutPara  = (float)lpNextKey->m_ValueInterPolation.m_dInInfluence;
			if(fInPara<0.1f)
				fInPara=0.1f;
			if(fOutPara<0.1f)
				fOutPara=0.1f;
			float fInSpeed  = (float)lpPreKey->m_ValueInterPolation.m_dOutSpeed;
			float fOutSpeed = (float)lpNextKey->m_ValueInterPolation.m_dInSpeed;

			if( fabs(KeyCtrlPt[0].x-KeyCtrlPt[1].x) < ABS_DOUBLE_0 && 
				fabs(KeyCtrlPt[2].x-KeyCtrlPt[3].x) < ABS_DOUBLE_0 &&
				fabs(KeyCtrlPt[0].y-KeyCtrlPt[1].y) < ABS_DOUBLE_0 && 
				fabs(KeyCtrlPt[2].y-KeyCtrlPt[3].y) < ABS_DOUBLE_0 )  //by zheng 060818
			{
				point2f CtrlPt[4];
				CtrlPt[0].x = 0.0f;		 CtrlPt[0].y = 0.0f;
				CtrlPt[1].x = fInPara;   CtrlPt[2].x = fOutPara;
				CtrlPt[1].y = fInSpeed;  CtrlPt[2].y = fOutSpeed;
				CtrlPt[3].x = 1.0f;		 CtrlPt[3].y = 1.0f;
				t = CommonBezierGetYValueFromX(CtrlPt,t);
				pData->m_dCurValue[0]=(double)(KeyCtrlPt[0].x+(KeyCtrlPt[3].x-KeyCtrlPt[0].x)*t);
				pData->m_dCurValue[1]=(double)(KeyCtrlPt[0].y+(KeyCtrlPt[3].y-KeyCtrlPt[0].y)*t);
			}
			else
			{
				point2f CtrlPt[4];
				CtrlPt[0].x = 0.0f;
				CtrlPt[0].y = 0.0f;

				CtrlPt[1].x =fInPara;
				CtrlPt[2].x =fOutPara; 

				CtrlPt[1].y = fST*fInSpeed;
				CtrlPt[2].y = fST*fOutSpeed;		

				CtrlPt[3].x = 1.0f;
				CtrlPt[3].y = fST;

				float fS = CommonBezierGetYValueFromX(CtrlPt,t);
				assert(fS<=fST);
				float fparat = CommonBezier3CalcParat(arBezier3Line, fS);

				point2f fPt = CommonBezierGetValue(KeyCtrlPt,fparat);
				pData->m_dCurValue[0]=(double)fPt.x;
				pData->m_dCurValue[1]=(double)fPt.y;
			}
		}
		break;
	case ONLYX:
		assert(0);
		break;
	case ONLYY:
		
		if( fabs(KeyCtrlPt[0].x-KeyCtrlPt[1].x) < ABS_DOUBLE_0 && 
			fabs(KeyCtrlPt[2].x-KeyCtrlPt[3].x) < ABS_DOUBLE_0  &&
			fabs(KeyCtrlPt[0].y-KeyCtrlPt[1].y) < ABS_DOUBLE_0  && 
			fabs(KeyCtrlPt[2].y-KeyCtrlPt[3].y) < ABS_DOUBLE_0 ) 
		{
			pData->m_dCurValue[1]=(double)(KeyCtrlPt[0].y+(KeyCtrlPt[3].y-KeyCtrlPt[0].y)*t);
		}
		else
		{
			assert(KeyCtrlPt[0].x<=((float)dCurTime) && ((float)dCurTime)<=KeyCtrlPt[3].x);
			pData->m_dCurValue[1] = CommonBezierGetYValueFromX(KeyCtrlPt,(float)dCurTime);
		}
		break;
	case ONLYZ:
		assert(0);
		break;
	}
	return TRUE;
}
