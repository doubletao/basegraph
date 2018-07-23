#include "stdAfx.h"
#include <assert.h>
#include "Common\CommonUtility.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
void SYPtCopyPt2f(point2 &P,point2f &FP)
{
	FP.x = (float)P.x;
	FP.y = (float)P.y;
}

void SYPt2fCopyPt(point2f &FP,point2 &P)
{
	P.x = (long)FP.x;
	P.y = (long)FP.y;
}

void SYOffsetPt2fs(point2f VerPt[],point2f &pt,int nCount)
{
	for (int i=0; i<nCount; ++i)
	{
		VerPt[i].x += pt.x;
		VerPt[i].y += pt.y;
	}
}

void SYOffsetPt3fs(point3f VerPt[], point3f &pt, int nCount)
{
	for (int i=0; i<nCount; ++i)
	{
		VerPt[i].x += pt.x;
		VerPt[i].y += pt.y;
		VerPt[i].z += pt.z;
	}
}

float SYPt2fDistance(point2f &pt0,point2f &pt1)
{
	float dtx = (pt1.x-pt0.x);
	float dty = (pt1.y-pt0.y);

	return sqrt(dtx*dtx+dty*dty);
}

void SYAR_ReleasePt2fAry(SYArray& ary)
{
	int nCnt = SYAR_GetCount(&ary);
	for(int i=0; i<nCnt; ++i)
	{
		point2f* pPt = (point2f*)SYAR_GetAt(&ary, i);
		delete pPt;
	}
	SYAR_RemoveAll(&ary);
}

void SYAR_ReleasePt3fAry(SYArray& ary)
{
	int nCnt = SYAR_GetCount(&ary);
	for(int i=0; i<nCnt; ++i)
	{
		point3f* pPt = (point3f*)SYAR_GetAt(&ary, i);
		delete pPt;
	}
	SYAR_RemoveAll(&ary);
}

int SYAR_GetIndexByPtr(const SYArray* pArray, void* pPtr)
{
	int nCnt = SYAR_GetCount(pArray);
	for(int i=0; i<nCnt; ++i)
	{
		if (SYAR_GetAt(pArray, i) == pPtr)
			return i;
	}
	return -1;
}

rectf SYGetBoundfFromPt2fs(point2f *pt,int nCount)
{
	assert(nCount>0);

	rectf Rtf;
	Rtf.left = pt[0].x;
	Rtf.top = pt[0].y;
	Rtf.right = pt[0].x;
	Rtf.bottom = pt[0].y;
	for(int i=1;i<nCount;i++)
	{
		if(Rtf.left > pt[i].x)
			Rtf.left = pt[i].x;
		if(Rtf.right < pt[i].x)
			Rtf.right = pt[i].x;
		if(Rtf.top > pt[i].y)
			Rtf.top = pt[i].y;
		if(Rtf.bottom < pt[i].y)
			Rtf.bottom = pt[i].y;
	}
	return Rtf;
}
