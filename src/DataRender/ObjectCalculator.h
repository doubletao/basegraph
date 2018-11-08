#pragma once
#include <map>
#include <vector>
#include "DataBufferManager.h"

#define EFFECT_CALC_CONDITIONS_OVERALL	0
#define EFFECT_CALC_CONDITIONS_PER		1

/*
物件计算器
用于物件的中间计算过程，包括建模，矩阵，关键字，2D自绘等
*/

enum Matrix_type
{
	enum_Matrix_Identity = 0x00000000,
	enum_Matrix_Offset = 0x00000001,
	enum_Matrix_Zoom = 0x00000002,
	enum_Matrix_Rotate = 0x00000004,
	enum_Matrix_Title = 0x00000008,
};

struct BaseGraphRenderUnitView
{
	BOOL			bChanged;
	CommonBufferDef32*	pUnitBuffer;
	DXHalDef32*		pUnitHalBuf;
	DXHalDef32*		pUnitDymHalBuffer;
	BOOL			bUseColorAlpha;
	BaseGraphRenderUnitView();
	~BaseGraphRenderUnitView();
};

class ObjectCalculator
{
protected:
	CString m_strObjID;
	int m_nObjBufferID;
	bool m_bSelected;
	DXHalDef32 m_HalDes;
	CRITICAL_SECTION m_Lock;
	int m_nLastFrame;
	int m_nRotateOrder;
	std::map<CString, BaseGraphRenderUnitView *> m_mapRenderUnitView;
protected:
	/*矩阵相关*/
	matrix4f	m_VSelfMatrix;			//只计算自身的矩阵
	matrix4f	m_VGlobalMatrix;		//计算自上而下的的矩阵
	matrix4f	m_ScrollMatrix;			//滚动矩阵
public:
	void		SetRenderMatV(matrix4f* pMat);
	const matrix4f* GetRenderMatV() const;
	void Update2DRenderMatrix(IN float fScale);
	matrix4f Get2DRenderMatrix();
	matrix4f GetScrollMatrix();
	matrix4f GetSelfMatrix();
public:
	ObjectCalculator(CString strObjID);
	virtual ~ObjectCalculator();
	void Release();
	virtual int GetWidth();
	virtual int GetHeight();
	virtual bool UpdateRenderBuffer();
	virtual bool PrepareItemHalBufer(int nDevice);
	virtual bool IsItemNeedRender();
	bool ValidAction();

	virtual BaseGraphItemRenderUnit * GetRenderUnit(int nIndex, const BaseARray& pUnitArray);
	static void GetSceneCenter(float3& pt);

	void SetChgLevel(EmObjectChgLevel emChgLevel);//set是赋值
	void AddChgLevel(EmObjectChgLevel emChgLevel);//add是叠加
	EmObjectChgLevel GetChgLevel();

	ObjectBuffer * GetObjectBuffer();
	int GetObjBufferID();
	bool GetObjSimpBuffer(StuSimpleBuffer & pBuf);
	bool SetSelected(bool bSel);//返回原来的状态
	bool IsSelected();
	DXHalDef32 * GetHalDes();
	CString GetObjectID();
	virtual BaseGraph_DataBase::ITEM_FORMTYPE Get3DType()const;

	virtual bool GetOriBound(OUT rectf & rtBoundf);//获取物件原始bound
	virtual bool GetSceneBound(OUT rectf & rtBoundf);//获取物件原始bound
	virtual bool GetCtrlPoint(OUT std::vector<pointf> & vecCtrlPoint);//获取物件原始bound
	void Lock();
	void Unlock();

	//hittest
	bool HitTest(IN pointf ptHit);//判定是否点在物件内
	bool BoxHitTest(IN rectf rtBox);//框选，条件是碰着就算
	bool BoxInterTest(IN rectf rtBox);//框选，条件是框住了才算
protected:
	virtual bool NeedUpdateBuffer();
	BaseGraphRenderUnitView * GetRenderUnitView(CString strUintID);
	void AddRenderUnitView(CString strUnitID, BaseGraphRenderUnitView * pRenderUnitView);
	void ClearRenderUnitView();
};
