#include "stdafx.h"
#include "BaseObject.h"
#include "IntIDGenerator.h"
#include "ObjectFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
BaseGraph_DataBase::BaseObject::BaseObject()
	: m_strParentID(_T(""))
{
	m_mapParam[Obj_ID_Str].SetValue(CommonFunction::CreateGUID());
}


BaseGraph_DataBase::BaseObject::~BaseObject()
{
}

void BaseGraph_DataBase::BaseObject::InitialObjectParam()
{
	//物件属性
	for (int i = ObjParam + 1; i < ObjParam_End; i++)
	{
		AddDefaultParam((ParamValueDef)i);
	}

	//空间值
	for (int i = SpaceParam + 1; i < SpaceParam_End; i++)
	{
		AddDefaultParam((ParamValueDef)i);
	}

	//颜色值
	for (int i = ColorParam + 1; i < ColorParam_End; i++)
	{
		AddDefaultParam((ParamValueDef)i);
	}
}

void BaseGraph_DataBase::BaseObject::AddDefaultParam(ParamValueDef emParamValueDef)
{
	if (m_mapParam.count(emParamValueDef) < 1)
	{
		m_mapParam[emParamValueDef] = GetValueDescriptor(emParamValueDef).stuDefault;
	}
}

bool BaseGraph_DataBase::BaseObject::Serialize(CArchive & archive)
{
	if (archive.IsStoring())
	{
		int nTmp = 0;
		nTmp = (int)m_mapParam.size();
		archive << nTmp;
		for (std::map<ParamValueDef, Value>::iterator it = m_mapParam.begin(); it != m_mapParam.end(); it++)
		{
			nTmp = it->first;
			archive << nTmp;
			it->second.Serialize(archive);
		}
		archive << m_strParentID;
		nTmp = (int)m_vecChildID.size();
		archive << nTmp;
		CString strTmp;
		for (size_t i = 0; i < m_vecChildID.size(); i++)
		{
			strTmp = m_vecChildID[i];
			archive << strTmp;
		}
	}
	else
	{
		int nSize = 0;
		archive >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			int nTmp = 0;
			archive >> nTmp;
			ParamValueDef emParamValueDef = (ParamValueDef)nTmp;
			Value valueTmp;
			valueTmp.Serialize(archive);
			m_mapParam[emParamValueDef] = valueTmp;
		}
		archive >> m_strParentID;
		archive >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			CString strTmp;
			archive >> strTmp;
			m_vecChildID.push_back(strTmp);
		}
	}
	return true;
}

BaseGraph_DataBase::ObjectType BaseGraph_DataBase::BaseObject::GetObjectType()
{
	BaseGraph_DataBase::ObjectType emObjType = BaseGraph_DataBase::ObjectType_Err;
	if (m_mapParam.count(Obj_Type_Enum) > 0)
	{
		emObjType = (BaseGraph_DataBase::ObjectType)m_mapParam[Obj_Type_Enum].ToInt();
	}
	return emObjType;
}

std::map<BaseGraph_DataBase::ParamValueDef, Value> BaseGraph_DataBase::BaseObject::GetProperty()
{
	return m_mapParam;
}

void BaseGraph_DataBase::BaseObject::SetProperty(std::map<ParamValueDef, Value> mapParam)
{
	for (std::map<ParamValueDef, Value>::iterator it = mapParam.begin(); it != mapParam.end(); it++)
	{
		SetValue(it->first, it->second);
	}
}

Value BaseGraph_DataBase::BaseObject::GetValue(ParamValueDef emValueDef)
{
	if (m_mapParam.count(emValueDef) < 1)
	{
		return GetValueDescriptor(emValueDef).stuDefault;
	}
	return m_mapParam[emValueDef];
}

void BaseGraph_DataBase::BaseObject::SetValue(ParamValueDef emValueDef, Value stuValue)
{
	m_mapParam[emValueDef] = stuValue;
}

CString BaseGraph_DataBase::BaseObject::GetItemID()
{
	CString strRet = 0;
	strRet = GetValue(Obj_ID_Str).ToCString();
	return strRet;
}

ObjectType BaseGraph_DataBase::BaseObject::GetItemType()
{
	ObjectType emRet = ObjectType_Err;
	emRet = (ObjectType)GetValue(Obj_Type_Enum).ToInt();
	return emRet;
}

point3f BaseGraph_DataBase::BaseObject::GetItemPos()
{
	point3f ptRet = {0};
	ptRet.x = GetValue(SpacePosition_X_Float).ToFloat();
	ptRet.y = GetValue(SpacePosition_Y_Float).ToFloat();
	ptRet.z = GetValue(SpacePosition_Z_Float).ToFloat();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemPos(point3f ptValue)
{
	SetValue(SpacePosition_X_Float, ptValue.x);
	SetValue(SpacePosition_Y_Float, ptValue.y);
	SetValue(SpacePosition_Z_Float, ptValue.z);
}

point3f BaseGraph_DataBase::BaseObject::GetItemRotate()
{
	point3f ptRet = { 0 };
	ptRet.x = GetValue(SpaceRotate_X_Float).ToFloat();
	ptRet.y = GetValue(SpaceRotate_Y_Float).ToFloat();
	ptRet.z = GetValue(SpaceRotate_Z_Float).ToFloat();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemRotate(point3f ptValue)
{
	SetValue(SpaceRotate_X_Float, ptValue.x);
	SetValue(SpaceRotate_Y_Float, ptValue.y);
	SetValue(SpaceRotate_Z_Float, ptValue.z);
}

point3f BaseGraph_DataBase::BaseObject::GetItemRotateRadian()
{
	point3f ptRotate = GetItemRotate();
	ptRotate *= PAIF / 180;
	return ptRotate;
}

void BaseGraph_DataBase::BaseObject::SetItemRotateRadian(point3f ptValue)
{
	ptValue /= PAIF / 180;
	SetItemRotate(ptValue);
}

point3f BaseGraph_DataBase::BaseObject::GetItemScale()
{
	point3f ptRet = { 0 };
	ptRet.x = GetValue(SpaceScale_X_Float).ToFloat();
	ptRet.y = GetValue(SpaceScale_Y_Float).ToFloat();
	ptRet.z = GetValue(SpaceScale_Z_Float).ToFloat();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemScale(point3f ptValue)
{
	SetValue(SpaceScale_X_Float, ptValue.x);
	SetValue(SpaceScale_Y_Float, ptValue.y);
	SetValue(SpaceScale_Z_Float, ptValue.z);
}

point3f BaseGraph_DataBase::BaseObject::GetItemCenter()
{
	point3f ptRet = { 0 };
	ptRet.x = GetValue(SpaceCenter_X_Float).ToFloat();
	ptRet.y = GetValue(SpaceCenter_Y_Float).ToFloat();
	ptRet.z = GetValue(SpaceCenter_Z_Float).ToFloat();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemCenter(point3f ptValue)
{
	SetValue(SpaceCenter_X_Float, ptValue.x);
	SetValue(SpaceCenter_Y_Float, ptValue.y);
	SetValue(SpaceCenter_Z_Float, ptValue.z);
}

BaseGraph_DataBase::ITEM_FORMTYPE BaseGraph_DataBase::BaseObject::Get3DType()
{
	return (BaseGraph_DataBase::ITEM_FORMTYPE)GetValue(Obj_3DType).ToInt();
}

CString BaseGraph_DataBase::BaseObject::GetParentID()
{
	return m_strParentID;
}

std::vector<CString> BaseGraph_DataBase::BaseObject::GetChildID()
{
	return m_vecChildID;
}

void BaseGraph_DataBase::BaseObject::SetParentID(CString strParentID)
{
	m_strParentID = strParentID;
}

void BaseGraph_DataBase::BaseObject::AddChildObject(CString strChildID)
{
	if (IsChild(strChildID))
	{
		m_vecChildID.push_back(strChildID);
	}
}

bool BaseGraph_DataBase::BaseObject::IsChild(CString strChildID)
{
	if (std::find(m_vecChildID.begin(), m_vecChildID.end(), strChildID) != m_vecChildID.end())
	{
		return true;
	}
	return false;
}
