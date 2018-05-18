#pragma once
#include <map>
#include "ObjectCalculator.h"

class CalculatorManager
{
private:
	std::map<CString, ObjectCalculator * > m_mapObjCalculator;//物件ID和物件计算器的键值对
	float m_fScale;
	int m_nFrame;
private:
	CalculatorManager();
	~CalculatorManager();
public:
	static CalculatorManager & Instance();

	int GetTexture(IN CString strID);
	DXHalDef32 * GetDesHalDef(IN CString strID);
	void Update2DRenderMatrix(IN float fScale);
	matrix4f Get2DRenderMatrix(IN CString strID);

	//ChangLevel
	void SetChgLevel(CString strObjID, EmObjectChgLevel emChgLevel);//set是赋值
	void AddChgLevel(CString strObjID, EmObjectChgLevel emChgLevel);//add是叠加
	void AddChgLevel(CString strObjID, BaseGraph_DataBase::ParamValueDef emParam);//通过变更的属性设置changlevel
	EmObjectChgLevel GetChgLevel(CString strObjID);

	ObjectCalculator * GetObjCalculator(CString strObjID);
	bool ReleaseCalculator(CString strObjID);
	bool ReleaseCalculator(ObjectCalculator * pCalc);

	bool UpdateRenderBuffer(CString strObjID);
	bool GetBuffer(IN CString strID, OUT StuSimpleBuffer & pBuf);

	bool IsItemSelected(IN CString strID, OUT bool & bSel);
	bool SetItemSelected(IN CString strID, OPTIONAL bool & bSel);//bSel会返回之前的状态
	bool SetItemSelected(IN CString strID, IN const bool &  bSel);
	bool GetSelectObj(OUT std::vector<CString> & vecSelObjID);
	bool ClearAllSelect();

	float GetScale();
	void SetScale(const float & fScale);
	void SetFrame(const int & nFrame);
	int GetFrame();

	bool GetCtrlBound(IN CString strObjID, OUT rectf & rtBoundf);//获取物件原始bound
	bool GetSceneBound(IN CString strObjID, OUT rectf & rtBoundf);//获取物件的场景bound
	bool GetCtrlPoint(IN CString strObjID, OUT std::vector<pointf> & vecCtrlPoint);//获取物件原始bound

	EmHittestRetType HitTest(IN pointf ptHit, OPTIONAL CString & strID, OPTIONAL EmCtrlPointIndex & emCtrlPoint);//根据一次单击查找选中块(),strID为空则选中最上层，否则会查找指定点的重叠物件中指定块的下一个物件
	bool BoxHitTest(IN rectf rtBox, OUT std::vector<CString> & vecStrObjID);//框选，入选的条件是碰着就算
	bool BoxInterTest(IN rectf rtBox, OUT std::vector<CString> & vecStrObjID);//框选，入选的条件是框住了才算

	//根据属性来获取属性对应的changlevel
	static EmObjectChgLevel GetPropertyChanegLevel(IN BaseGraph_DataBase::ParamValueDef emParam);
};

