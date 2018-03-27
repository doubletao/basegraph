#pragma once
#include "UIControl\UICombinedCtrl.h"
#include "UIControl\UIEdit.h"

class BASEGRAPH_UICONTROL_API UIEditNum
	: public UICombinedCtrl
{
protected:
	UIEdit * m_pUIEdit;
	int m_nPrecision;
	int m_nMin;
	int m_nMax;
public:
	UIEditNum();
	virtual ~UIEditNum();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, int nEditID, HWND hParentWnd, UIElement * pParent);
	void SetPrecision(int nPrecision);
	void SetLimit(int nMin, int nMax);
	virtual BaseGraph_Common::Value GetValue();
	virtual void SetValue(BaseGraph_Common::Value value, bool bRefresh = true);
	virtual void UpdateValueToUI();
	virtual void UpdateUIToValue();
};
