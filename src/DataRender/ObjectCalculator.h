#pragma once
#include <map>
#include <vector>
#include "DataBufferManager.h"

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
	EmObjectChgLevel m_emChgLevel;
public:
	ObjectCalculator(CString strObjID);
	virtual ~ObjectCalculator();
	virtual int GetWidth();
	virtual int GetHeight();
	int GetObjBufferID();
	bool GetObjBuffer(byte *& pBuffer);
protected:
	virtual bool NeedRenderBuffer();
public:
	virtual bool UpdateRenderBuffer();
};

