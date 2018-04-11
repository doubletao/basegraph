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
	: m_fScale(1.0f)
	, m_nFrame(0)
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

void CalculatorManager::Update2DRenderMatrix(IN float fScale)
{
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		ObjectCalculator * pCalc = it->second;
		if (pCalc)
		{
			pCalc->Update2DRenderMatrix(fScale);
		}
	}
}

matrix4f CalculatorManager::Get2DRenderMatrix(IN CString strID)
{
	ObjectCalculator * pCalc = GetObjCalculator(strID);
	if (pCalc)
	{
		return pCalc->Get2DRenderMatrix();
	}
	//没找到直接给一个单位矩阵
	matrix4f matRet;
	matrix4f_identity(&matRet);
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
		bSel = pCalc->IsSelected();
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

bool CalculatorManager::GetSelectObj(OUT std::vector<CString> & vecSelObjID)
{
	bool bRet = false;
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		ObjectCalculator * pCalc = it->second;
		if (pCalc && pCalc->IsSelected())
		{
			vecSelObjID.push_back(pCalc->GetObjectID());
			bRet = true;
		}
	}
	return bRet;
}

bool CalculatorManager::ClearAllSelect()
{
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		ObjectCalculator * pCalc = it->second;
		if (pCalc && pCalc->IsSelected())
		{
			pCalc->SetSelected(false);
		}
	}
	return true;
}

float CalculatorManager::GetScale()
{
	return m_fScale;
}

void CalculatorManager::SetScale(const float & fScale)
{
	if (fabs(m_fScale - fScale) > FLOAT_0)
	{
		//如果不相等，则所有物件都需要设置变动级别
		for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
		{
			if (it->second)
			{
				it->second->SetChgLevel(ChgLevel_Figure);
			}
		}
	}
	m_fScale = fScale;
}

void CalculatorManager::SetFrame(const int & nFrame)
{
	m_nFrame = nFrame;
	Update2DRenderMatrix(m_fScale);
}

int CalculatorManager::GetFrame()
{
	return m_nFrame;
}
