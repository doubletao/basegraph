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
enum EmObjectChgLevel
{
	ChgLevel_NULL,
	ChgLevel_Matrix,//矩阵变动（旋转，位移，缩放等），无需更新bufer
	ChgLevel_Buffer,//Buffer变动（动画等），需要重画bufer
	ChgLevel_Figure,//形状变化，需要重新建模
	ChgLevel_Create,//最高级别，刚创建，什么都要从头
};

enum Matrix_type
{
	enum_Matrix_Identity = 0x00000000,
	enum_Matrix_Offset = 0x00000001,
	enum_Matrix_Zoom = 0x00000002,
	enum_Matrix_Rotate = 0x00000004,
	enum_Matrix_Title = 0x00000008,
};

struct SYRenderUnitView
{
	BOOL			bChanged;
	SYBufferDef32*	pUnitBuffer;
	DXHalDef32*		pUnitHalBuf;
	DXHalDef32*		pUnitDymHalBuffer;
	BOOL			bUseColorAlpha;
	SYRenderUnitView();
	~SYRenderUnitView();
};

class ObjectCalculator
{
protected:
	CString m_strObjID;
	int m_nObjBufferID;
	bool m_bSelected;
	EmObjectChgLevel m_emChgLevel;
	DXHalDef32 m_HalDes;
	CRITICAL_SECTION m_Lock;
	int m_nLastFrame;
	int m_nRotateOrder;
	std::map<CString, SYRenderUnitView *> m_mapRenderUnitView;
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
	virtual SYItemRenderUnit * GetRenderUnit(int nIndex, const SYArray& pUnitArray);
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
	BaseGraph_DataBase::ITEM_FORMTYPE Get3DType()const;

	virtual bool GetCtrlBound(OUT rectf & rtBoundf);//获取物件原始bound
	virtual bool GetSceneBound(OUT rectf & rtBoundf);//获取物件原始bound
	virtual bool GetCtrlPoint(OUT std::vector<pointf> & vecCtrlPoint);//获取物件原始bound
	void Lock();
	void Unlock();
protected:
	virtual bool NeedRenderBuffer();
	SYRenderUnitView * GetRenderUnitView(CString strUintID);
	void AddRenderUnitView(CString strUnitID, SYRenderUnitView * pRenderUnitView);
	void ClearRenderUnitView();
};
