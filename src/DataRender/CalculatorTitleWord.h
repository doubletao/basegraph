#pragma once
#include "ObjectCalculator.h"
#include "Typeset\TextTypeset.h"

class CalculatorTitleword :
	public ObjectCalculator
{
public:
	CalculatorTitleword(CString strObjID);
	virtual ~CalculatorTitleword();
protected:
	CSYTypeset m_TypeSet;
	SYText m_text;
	rectf m_rtUnitBound;//根据unit算出来的静态bound
	SYArray m_2DTextRenderUnitArray;
	SYArray m_arWholeRenderTextGraph;	//文字整体渲染时的临时Graph（可以从零散Graph重建得到）
protected:
	///文字属性
	//基本属性
	CString m_strText;
	CString m_strFontName;
	BOOL m_bUnderline;
	int m_nFontDistance;
	BOOL m_bHorizontal;
	BOOL m_bLineWrap;
	int m_nFontWidth;
	int m_nFontHeight;
	E_TextAlignHorizontal m_TextAlignH;
	E_TextAlignVeritical m_TextAlignV;
	BOOL m_bLeftInput;
	BOOL m_bVLeftTypeset;
	BOOL m_bFixedFrame;
	//字色
	SYStaticEffect m_TextStaitcEffect;
	//下划线
	SYUnderLine m_TextUnderLine;
protected:
	bool PrepareObjBuffer(int nWidth, int nHeight);
public:
	virtual bool NeedUpdateBuffer();
	virtual bool UpdateRenderBuffer();

	virtual bool GetCurrentParam();//获取模型参数(如果有关键帧动作，应该在这里边干完)
	virtual bool Update2DUnit();//更新2D模型
	virtual bool UpdateToHalBuf();//更新显存buf

	virtual rectf CalUnitBound();
	virtual bool RefreshRenderUnit(int nFrame, BOOL bInherit);
	//计算图形单元,源为punitin，目标为prenderunit,vmatrix,nmatrix为当前变换矩阵
	//对于不同的形变可能处理方式不同,比如对于一般的2D图形,将可能是轮廓和矩阵相乘得到结果
	//而对于特殊的2D图形则可能是把原始数据进行形变,然后根据形变后的数据进行轮廓生成
	//目前的计算都是针对于动态状态而言的,对于静态状态改变的可能是源.
	virtual BOOL _trans_calcunit(int nFrame, int nUnitIndex, int nOrderIndex, BOOL bInherit = TRUE);
	virtual BOOL _trans_calcunit_matrix(matrix4f * pVMatrix, int* pdwMatrixType, double dFrame, int nUnitIndex, int nOrderIndex);
	virtual void GetRenderUnitOrder(CSYIntArray& arOrder);
	virtual void DeleteWholeRenderGraph();
	virtual BOOL UpdateWholeRenderGraph(SYArray* pRenderUnitArray);
	virtual void UpdateItemGpuBuffer(BOOL bLock = FALSE);
	SYItemRenderUnit * GetRenderUnit(int nIndex);
	virtual int Get2DTextUnitCount();
	virtual void GetRenderUnitBound(int nIndex, rectf& rtBoundf);
	virtual int GetWidth();
	virtual int GetHeight();
	virtual SYText * GetText();
	virtual SYArray * Get2DTextUnitArray();
	virtual SYUnit*	GetUnit(int nIndex);
	virtual void DeleteRenderUnitView(LPCTSTR szUnitID);
	virtual void RenderUnitBufferForGpu();
	virtual bool PrepareItemHalBufer(int nDevice);
	virtual bool GetOriBound(OUT rectf & rtBoundf);//获取物件原始bound
};
