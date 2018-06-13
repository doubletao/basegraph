#include "stdafx.h"
#include "CalculatorManager.h"
#include "CalculatorFactory.h"
#include "Calculator2DLayer.h"
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CalculatorManager::CalculatorManager()
	: m_fScale(1.0f)
	, m_nField(0)
	, m_bStaticMode(true)
	, m_strCurParentCalcID()
{
	m_strCurParentCalcID = Calculator2DLayer::strCalculator2DLayer;
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

void CalculatorManager::SetChgLevel(CString strObjID, EmObjectChgLevel emChgLevel)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		pCalc->SetChgLevel(emChgLevel);
	}
}

void CalculatorManager::AddChgLevel(CString strObjID, EmObjectChgLevel emChgLevel)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		pCalc->AddChgLevel(emChgLevel);
	}
}

void CalculatorManager::AddChgLevel(CString strObjID, BaseGraph_DataBase::ParamValueDef emParam)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		pCalc->AddChgLevel(GetPropertyChanegLevel(emParam));
	}
}

EmObjectChgLevel CalculatorManager::GetChgLevel(CString strObjID)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->GetChgLevel();
	}
	return ChgLevel_NULL;
}

ObjectCalculator * CalculatorManager::GetObjCalculator(CString strObjID)
{
	if (m_mapObjCalculator.count(strObjID) > 0)
	{
		return m_mapObjCalculator[strObjID];
	}
	else if (strObjID == Calculator2DLayer::strCalculator2DLayer)
	{
		BaseGraph_DataBase::ObjectType emObjType = BaseGraph_DataBase::ObjectType_2DLayer;
		m_mapObjCalculator[strObjID] = CalculatorFactory::CreateCalculator(emObjType, strObjID);
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

void CalculatorManager::SetFrame(const int & nField)
{
	m_nField = nField;
	Update2DRenderMatrix(m_fScale);
}

int CalculatorManager::GetFrame()
{
	return m_nField;
}

bool CalculatorManager::GetOriBound(IN CString strObjID, OUT rectf & rtBoundf)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->GetOriBound(rtBoundf);
	}
	return false;
}

bool CalculatorManager::GetSceneBound(IN CString strObjID, OUT rectf & rtBoundf)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->GetSceneBound(rtBoundf);
	}
	return false;
}

bool CalculatorManager::GetCtrlPoint(IN CString strObjID, OUT std::vector<pointf>& vecCtrlPoint)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->GetCtrlPoint(vecCtrlPoint);
	}
	return false;
}

bool CalculatorManager::IsItemNeedRender(IN CString strObjID)
{
	ObjectCalculator * pCalc = GetObjCalculator(strObjID);
	if (pCalc)
	{
		return pCalc->IsItemNeedRender();
	}
	return false;
}

EmHittestRetType CalculatorManager::HitTest(IN pointf ptHit, OPTIONAL CString & strID, OPTIONAL EmCtrlPointIndex & emCtrlPoint)
{
	//遍历物件执行hittest点测试，如果strID不为空，则从strID的下一个开始执行遍历到strID的上一个结束
	if (strID.IsEmpty())
	{
		//指定ID为空，直接遍历calculate就可以
		for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
		{
			if (it->second)
			{
				if (it->second->HitTest(ptHit))
				{
					strID = it->first;
					return Hittest_Object;
				}
			}
		}
	} 
	else
	{
		//指定ID不为空，则需要考虑层次，此时需要遍历物件集合
		std::vector<CString> vecObjID;
		if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
		{
			std::vector<CString>::iterator itFind = std::find(vecObjID.begin(), vecObjID.end(), strID);
			if (itFind == vecObjID.end())
			{
				//如果没找到，则视为无指定查找
				strID.Empty();
				return HitTest(ptHit, strID, emCtrlPoint);
			}
			else
			{
				//先从指定物件的下一个物件开始，查找到结尾
				for (std::vector<CString>::iterator it = itFind + 1; it != vecObjID.end(); it++)
				{
					if (m_mapObjCalculator.count(*it) > 0)
					{
						if (m_mapObjCalculator[*it])
						{
							if (m_mapObjCalculator[*it]->HitTest(ptHit))
							{
								strID = *it;
								return Hittest_Object;
							}
						}
					}
				}
				//然后回到开头一直找到指定物件为止
				for (std::vector<CString>::iterator it = vecObjID.begin(); it != itFind; it++)
				{
					if (m_mapObjCalculator.count(*it) > 0)
					{
						if (m_mapObjCalculator[*it])
						{
							if (m_mapObjCalculator[*it]->HitTest(ptHit))
							{
								strID = *it;
								return Hittest_Object;
							}
						}
					}
				}
			}
		}
	}
	strID.Empty();
	emCtrlPoint = CtrlPoint_NULL;
	return Hittest_NULL;
}

bool CalculatorManager::BoxHitTest(IN rectf rtBox, OUT std::vector<CString>& vecStrObjID)
{
	bool bRet = false;
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		if (it->second)
		{
			if (it->second->BoxHitTest(rtBox))
			{
				vecStrObjID.push_back(it->first);
				bRet = true;
			}
		}
	}
	return bRet;
}

bool CalculatorManager::BoxInterTest(IN rectf rtBox, OUT std::vector<CString>& vecStrObjID)
{
	bool bRet = false;
	for (std::map<CString, ObjectCalculator * >::iterator it = m_mapObjCalculator.begin(); it != m_mapObjCalculator.end(); it++)
	{
		if (it->second)
		{
			if (it->second->BoxInterTest(rtBox))
			{
				vecStrObjID.push_back(it->first);
				bRet = true;
			}
		}
	}
	return bRet;
}

EmObjectChgLevel CalculatorManager::GetPropertyChanegLevel(IN BaseGraph_DataBase::ParamValueDef emParam)
{
	switch (emParam)
	{
	case BaseGraph_DataBase::ObjParam:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Obj_ID_Str:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Obj_Type_Enum:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Obj_Name_Str:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Obj_Width_Int:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Obj_Height_Int:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Obj_ActionBegin_Int:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Obj_ActionEnd_Int:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Obj_3DType:
		return ChgLevel_Create;
	case BaseGraph_DataBase::ObjParam_End:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::SpaceParam:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::SpacePosition_X_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpacePosition_Y_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpacePosition_Z_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceRotate_X_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceRotate_Y_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceRotate_Z_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceScale_X_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceScale_Y_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceScale_Z_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceCenter_X_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceCenter_Y_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceCenter_Z_Float:
		return ChgLevel_Matrix;
	case BaseGraph_DataBase::SpaceParam_End:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::ColorParam:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::ColorParam_Face_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Border_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Shadow_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Cube_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Bordercube_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Bordershadow_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Facelight_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Cubelight_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Channel_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Material_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Appendmaterial_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Dyntex_Stu:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Renderline_Int:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Width_Float:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_Index_Int:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ColorParam_End:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::TitlewordParam:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Titleword_Text_Str:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Titleword_FontName_Str:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Titleword_UnderLine_Bool:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Titleword_LineWrap_Bool:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Titleword_FontDistance_Int:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::Titleword_Horizontal_Bool:
		return ChgLevel_Figure;
	case BaseGraph_DataBase::TitlewordParam_End:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::ScrollParam:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::ScrollParam_End:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::ImageParam:
		return ChgLevel_NULL;
	case BaseGraph_DataBase::Image_Path_Str:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::Image_SrcWidth_Int:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::Image_SrcHeight_Int:
		return ChgLevel_Buffer;
	case BaseGraph_DataBase::ImageParam_End:
		return ChgLevel_NULL;
	default:
		break;
	}
	return ChgLevel_NULL;
}

bool CalculatorManager::IsStaticMode()
{
	return m_bStaticMode;
}

void CalculatorManager::SetStaticMode(bool bStatic)
{
	m_bStaticMode = bStatic;
}

CString CalculatorManager::GetCurParentObjID()
{
	return m_strCurParentCalcID;
}

void CalculatorManager::SetCurParentObjID(CString strParentObjID)
{
	m_strCurParentCalcID = strParentObjID;
}

void CalculatorManager::DeleteCalculator(CString strID)
{
	if (m_mapObjCalculator.count(strID) > 0)
	{
		delete m_mapObjCalculator[strID];
		m_mapObjCalculator[strID] = nullptr;
		m_mapObjCalculator.erase(strID);
	}
}
