#include "stdafx.h"
#include "CalculatorManager.h"
#include "CalculatorFactory.h"
#include <assert.h>

CalculatorManager::CalculatorManager()
{
}


CalculatorManager::~CalculatorManager()
{
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		delete it->second;
		it->second = nullptr;
	}
	m_mapObjCalculator.clear();
}

CalculatorManager & CalculatorManager::Instance()
{
	static CalculatorManager inst;
	return inst;
}

ObjectCalculator * CalculatorManager::GetObjCalculator(CString strObjID)
{
	if (m_mapObjCalculator.count(strObjID) > 0)
	{
		return m_mapObjCalculator[strObjID];
	}
	else
	{
		BaseGraph_DataBase::ObjectType emObjType = BaseGraph_DataBase::ObjectType_Err;
		if (DataBase_GetItemType(strObjID, emObjType))
		{
			m_mapObjCalculator[strObjID] = CalculatorFactory::CreateCalculator(emObjType, strObjID);
			return m_mapObjCalculator[strObjID];
		}
	}
	return nullptr;
}

bool CalculatorManager::ReleaseCalculator(CString strObjID)
{
	if (m_mapObjCalculator.count(strObjID) > 0)
	{
		delete m_mapObjCalculator[strObjID];
		m_mapObjCalculator[strObjID] = nullptr;
		m_mapObjCalculator.erase(strObjID);
		return true;
	}
	return false;
}

bool CalculatorManager::ReleaseCalculator(ObjectCalculator * pCalc)
{
	assert(false);
	return false;
}

bool CalculatorManager::UpdateRenderBuffer(CString strObjID)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->UpdateRenderBuffer();
	}
	return false;
}

bool CalculatorManager::GetBuffer(IN CString strObjID, OUT byte *& pBuffer, OUT int & nWidth, OUT int & nHeight)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->GetObjBuffer(pBuffer, nWidth, nHeight);
	}
	return false;
}

bool CalculatorManager::IsItemSelected(IN CString strID, OUT bool & bSel)
{
	ObjectCalculator * pCalc = GetObjCalculator(strID);
	if (pCalc)
	{
		bSel = pCalc->GetSelected();
		return true;
	}
	return false;
}

bool CalculatorManager::SetItemSelected(IN CString strID, OPTIONAL bool & bSel)
{
	ObjectCalculator * pCalc = GetObjCalculator(strID);
	if (pCalc)
	{
		bSel = pCalc->SetSelected(bSel);
		return true;
	}
	return false;
}

bool CalculatorManager::SetItemSelected(IN CString strID, IN const bool & bSel)
{
	ObjectCalculator * pCalc = GetObjCalculator(strID);
	if (pCalc)
	{
		pCalc->SetSelected(bSel);
		return true;
	}
	return false;
}

bool CalculatorManager::ClearAllSelect()
{
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		ObjectCalculator * pCalc = it->second;
		if (pCalc->GetSelected())
		{
			pCalc->SetSelected(false);
		}
	}
	return true;
}
