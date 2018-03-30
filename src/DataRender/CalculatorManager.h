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

	ObjectCalculator * GetObjCalculator(CString strObjID);
	bool ReleaseCalculator(CString strObjID);
	bool ReleaseCalculator(ObjectCalculator * pCalc);

	bool UpdateRenderBuffer(CString strObjID);
	bool GetBuffer(IN CString strID, OUT StuSimpleBuffer & pBuf);

	bool IsItemSelected(IN CString strID, OUT bool & bSel);
	bool SetItemSelected(IN CString strID, OPTIONAL bool & bSel);//bSel会返回之前的状态
	bool SetItemSelected(IN CString strID, IN const bool &  bSel);
	bool ClearAllSelect();

	float GetScale();
	void SetScale(const float & fScale);
	void SetFrame(const int & nFrame);
	int GetFrame();
};

