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

class ObjectCalculator
{
protected:
	CString m_strObjID;
	int m_nObjBufferID;
	bool m_bSelected;
	EmObjectChgLevel m_emChgLevel;
	DXHalDef32 m_HalDes;
	CRITICAL_SECTION m_Lock;
public:
	ObjectCalculator(CString strObjID);
	virtual ~ObjectCalculator();
	virtual int GetWidth();
	virtual int GetHeight();
	virtual bool UpdateRenderBuffer();
	virtual bool PrepareItemHalBufer(int nDevice);

	ObjectBuffer * GetObjectBuffer();
	int GetObjBufferID();
	bool GetObjSimpBuffer(StuSimpleBuffer & pBuf);
	bool SetSelected(bool bSel);//返回原来的状态
	bool GetSelected();
	DXHalDef32 * GetHalDes();
	void Lock();
	void Unlock();
protected:
	virtual bool NeedRenderBuffer();
};

