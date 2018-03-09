#include "stdafx.h"
#include "CalculatorManager.h"
#include "CalculatorFactory.h"
#include <assert.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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

int CalculatorManager::GetTexture(IN CString strID)
{
	ObjectCalculator * pCalc = GetObjCalculator(strID);
	if (pCalc)
	{
		DXHalDef32 * pHalDes = pCalc->GetHalDes();
		if (pHalDes)
		{
			return pHalDes->m_hTex;
		}
	}
	return 0;
}

DXHalDef32 * CalculatorManager::GetDesHalDef(IN CString strID)
{
	ObjectCalculator * pCalc = GetObjCalculator(strID);
	if (pCalc)
	{
		DXHalDef32 * pHalDes = pCalc->GetHalDes();
		return pHalDes;
	}
	return nullptr;
}

matrix4f CalculatorManager::Get2DRenderMatrix(IN CString strID, IN float fScale)
{
	matrix4f matRet;
	matrix4f_identity(&matRet);
	calc_trans_vector_tomatrix(matRet, DataBase_GetItemCenter(strID), DataBase_GetItemScale(strID), DataBase_GetItemAngle(strID), DataBase_GetItemPos(strID), ROTATE_ORDER_YXZ);
	matRet._41 *= fScale;
	matRet._42 *= fScale;
	matRet._43 *= fScale;
	return matRet;
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

bool CalculatorManager::GetBuffer(IN CString strObjID, OUT StuSimpleBuffer & pBuf)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->GetObjSimpBuffer(pBuf);
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
