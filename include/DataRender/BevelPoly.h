#pragma once
//////////////////////////////////////////////////////////////////////////
typedef struct tag_Index3
{
	int nPolyIndex, nStartIndex, nNumCounts;
}Index3;

typedef struct tag_IntersectNode
{
	pointf ptf; 
	int nIndex1; 
	int nIndex2; 
}IntersectNode;

typedef struct tag_ResultNode
{
	pointf ptf; 
	pointf ptfP; 
	float fParam; 
}ResultNode;

typedef CBaseARray<Index3*> CBaseIndex3Array;
typedef CBaseARray<IntersectNode*> CBaseIntersectNodeArray;
typedef CBaseARray<ResultNode*> CBaseResultNodeArray;
class BevelPoly
{
public:
	BevelPoly(void);
	~BevelPoly(void);
	void BevelPolyAPI(IN vectorcell * pCurveCell, IN float fWidth, IN float fHeight, OUT point3f* &ptf3DRet, OUT int &nTriPtSize, IN BOOL bAntiClockwise ) ;
protected:
	BOOL NearLine(IN pointf* ptf ) ;
	void AddCurve2PtfPoly(OUT CBasePoint2fPtrArray * pptrPoly, OUT CBasePoint2fPtrArray * pptrPolyNorm, IN pointf * ctrlPoints);//AddCurve2PtfPoly(OUT CBasePoint2fPtrArray * pptrPoly, IN pointf * ctrlPoints);
	void GenerateRudeBevel4(IN BaseARray * pptrPoly, IN float fWidth, IN float fHeight, 
		OUT CBasePoint3fPtrArray * pptrArrRude4/*, OUT CPtrArray * pptrArrBox4*/ );
	void GenerateRudeBevel3(IN BaseARray * pptrPoly, IN float fWidth, IN float fHeight, 
		OUT CBasePoint3fPtrArray * pptrArrRude3, /*OUT CPtrArray * pptrArrBox3, */OUT CBaseIndex3Array *pptrArrIndex3 );
	void CutToTri4(IN BaseARray * pptrPoly, IN BaseARray * pptrPolyNorm, IN  CBasePoint3fPtrArray * pptrArrRude4, /*IN CPtrArray * pptrArrBox4,*/
		IN CBasePoint3fPtrArray * pptrArrRude3, /*IN CPtrArray * pptrArrBox3, */IN CBaseIndex3Array *pptrArrIndex3,
		IN float fWidth, IN float fHeight, OUT CBasePoint3fPtrArray * pptrTri ) ;
	void CutToTri3(IN BaseARray * pptrPoly, IN BaseARray * pptrPolyNorm, IN  CBasePoint3fPtrArray * pptrArrRude4, /*IN CPtrArray * pptrArrBox4,*/
		IN CBasePoint3fPtrArray * pptrArrRude3, /*IN CPtrArray * pptrArrBox3, */IN CBaseIndex3Array *pptrArrIndex3,
		IN float fWidth, IN float fHeight, OUT CBasePoint3fPtrArray * pptrTri ) ;
	int MakeASlope(OUT CBasePoint3fPtrArray *ptArr,IN pointf* pCenter, IN pointf* pStartPt,IN pointf* pEndPt,IN float fDis, IN float fHeight,IN int nDirect);
	void CutUnSeenByOne(IN point3f* ptfFish, IN point3f* ptfKnife, IN int nFishSize, IN int nKnifeSize,IN OUT CBasePoint2fPtrArray * pptrCutLineSegArr);
	// pptrLineSegArr: ‰»Îœﬂ∂Œ∂•µ„Array
	void IntersectEachOther(IN CBasePoint2fPtrArray * pptrLineSegArr,  OUT CBaseIntersectNodeArray * pptrIntersectPtArr ) ;
	void GetResultTri4(IN pointf ptfFormer0, IN pointf ptfFormer1, IN pointf ptfConeHeadNorm0, IN pointf ptfConeHeadNorm1, IN CBasePoint2fPtrArray *pptrCutLineSegArr, IN CBaseIntersectNodeArray *ptrIntersectPtArr, IN float fWidth, IN float fHeight, OUT CBasePoint3fPtrArray *pptrTri ); 
	void GetResultTri3(IN pointf ptfConeHead, IN pointf ptfConeHeadNorm, IN pointf ptfFormer0, IN pointf ptfFormer1, IN CBasePoint2fPtrArray *pptrCutLineSegArr, IN CBaseIntersectNodeArray *ptrIntersectPtArr, IN float fWidth, IN float fHeight, OUT CBasePoint3fPtrArray *pptrTri ); 
	void GetEnvelop(IN pointf *ptf, IN int nSize, OUT rectf &rt);
	BOOL RectOverlap(IN rectf rt1, IN rectf rt2);
	void ProjCrossLine(IN point3f ptf1, IN point3f ptf2, IN point3f ptf3, IN point3f ptf4, IN point3f ptf5, IN point3f ptf6,OUT float &fA, OUT float &fB, OUT float &fC);
	BOOL CutLine(IN float fA, IN float fB, IN float fC,IN pointf* ptf, IN int nSize,OUT pointf &ptfS, OUT pointf &ptfE );
	BOOL LineCrossLineSeg(IN float fA, IN float fB, IN float fC,IN pointf ptfS, IN pointf ptfE, OUT pointf & ptfCross );
	void GetMinMaxXY(OUT float &fMinX, OUT float &fMaxX, OUT float &fMinY, OUT float &fMaxY,IN pointf ptfInS, IN pointf ptfInE, IN pointf ptfS, IN pointf ptfE);
	BOOL SamePtf(pointf ptf1, pointf ptf2);
	BOOL SingleLineIntersection(IN pointf startPt1, IN pointf endPt1, IN pointf startPt2, IN pointf endPt2,OUT pointf &ptCross);
	void Perpendicular(IN pointf  a, IN pointf b, IN pointf c, OUT pointf &ptfP, OUT float & fParam );
	void InsertNode(IN OUT CBaseResultNodeArray *pptrResultNodeArr, IN ResultNode * pNode);
	void GetAngle(IN pointf a, IN pointf b, OUT float & angle);
};
//////////////////////////////////////////////////////////////////////////
