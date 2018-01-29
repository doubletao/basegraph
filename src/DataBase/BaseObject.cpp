#include "stdafx.h"
#include "BaseObject.h"
#include "IntIDGenerator.h"
#include "ObjectFactory.h"

BaseGraph_DataBase::BaseObject::BaseObject()
	: m_nParentID(0)
{
	m_mapParam[ObjID].SetValue(IntIDGenerator::Instance().GetNewID());
}


BaseGraph_DataBase::BaseObject::~BaseObject()
{
}

void BaseGraph_DataBase::BaseObject::InitialObjectParam()
{
	auto AddDefaultParam = [this](ParamValueDef emParamValueDef)
	{
		if (m_mapParam.count(emParamValueDef) < 1)
		{
			m_mapParam[emParamValueDef] = GetValueDescriptor(emParamValueDef).stuDefault;
		}
	};
	
	//空间值
	AddDefaultParam(SpacePosition_X);
	AddDefaultParam(SpacePosition_Y);
	AddDefaultParam(SpacePosition_Z);
	AddDefaultParam(SpaceRotate_X);
	AddDefaultParam(SpaceRotate_Y);
	AddDefaultParam(SpaceRotate_Z);
	AddDefaultParam(SpaceScale_X);
	AddDefaultParam(SpaceScale_Y);
	AddDefaultParam(SpaceScale_Z);
	AddDefaultParam(SpaceCenter_X);
	AddDefaultParam(SpaceCenter_Y);
	AddDefaultParam(SpaceCenter_Z);

	//颜色值
	AddDefaultParam(ColorFace_Enable);
	AddDefaultParam(ColorFace_Soft);
	AddDefaultParam(ColorFace_EffectType);
	AddDefaultParam(ColorFace_EffectWidth);
	AddDefaultParam(ColorFace_AVColor);
	AddDefaultParam(ColorFace_Art);
	AddDefaultParam(ColorFace_Color);
	AddDefaultParam(ColorBorder_Enable);
	AddDefaultParam(ColorBorder_Soft);
	AddDefaultParam(ColorBorder_LineStyle);
	AddDefaultParam(ColorBorder_Width);
	AddDefaultParam(ColorBorder_Interval);
	AddDefaultParam(ColorBorder_EffectType);
	AddDefaultParam(ColorBorder_EffectWidth);
	AddDefaultParam(ColorBorder_AVColor);
	AddDefaultParam(ColorBorder_Art);
	AddDefaultParam(ColorBorder_Color);
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
		archive << m_nParentID;
		nTmp = (int)m_vecChildID.size();
		archive << nTmp;
		for (size_t i = 0; i < m_vecChildID.size(); i++)
		{
			nTmp = m_vecChildID[i];
			archive << nTmp;
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
			m_mapParam.insert(std::make_pair(emParamValueDef, valueTmp));
		}
		archive >> m_nParentID;
		archive >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			int nTmp = 0;
			archive >> nTmp;
			m_vecChildID.push_back(nTmp);
		}
	}
	return true;
}

BaseGraph_DataBase::ObjectType BaseGraph_DataBase::BaseObject::GetObjectType()
{
	BaseGraph_DataBase::ObjectType emObjType = BaseGraph_DataBase::ObjectType_Err;
	if (m_mapParam.count(ObjType) > 0)
	{
		emObjType = (BaseGraph_DataBase::ObjectType)m_mapParam[ObjType].ToInt();
	}
	return emObjType;
}

Value BaseGraph_DataBase::BaseObject::GetValue(ParamValueDef emValueDef)
{
	if (m_mapParam.count(emValueDef) < 1)
	{
		return GetValueDescriptor(emValueDef).stuDefault;
	}
	return m_mapParam[emValueDef];
}

point3 BaseGraph_DataBase::BaseObject::GetItemPos()
{
	point3 ptRet = {0};
	ptRet.x = GetValue(SpacePosition_X).ToInt();
	ptRet.y = GetValue(SpacePosition_Y).ToInt();
	ptRet.z = GetValue(SpacePosition_Z).ToInt();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemPos(point3 ptValue)
{
	m_mapParam[SpacePosition_X].SetValue(ptValue.x);
	m_mapParam[SpacePosition_Y].SetValue(ptValue.y);
	m_mapParam[SpacePosition_Z].SetValue(ptValue.z);
}

point3 BaseGraph_DataBase::BaseObject::GetItemRotate()
{
	point3 ptRet = { 0 };
	ptRet.x = GetValue(SpaceRotate_X).ToInt();
	ptRet.y = GetValue(SpaceRotate_Y).ToInt();
	ptRet.z = GetValue(SpaceRotate_Z).ToInt();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemRotate(point3 ptValue)
{
	m_mapParam[SpaceRotate_X].SetValue(ptValue.x);
	m_mapParam[SpaceRotate_Y].SetValue(ptValue.y);
	m_mapParam[SpaceRotate_Z].SetValue(ptValue.z);
}

point3 BaseGraph_DataBase::BaseObject::GetItemScale()
{
	point3 ptRet = { 0 };
	ptRet.x = GetValue(SpaceScale_X).ToInt();
	ptRet.y = GetValue(SpaceScale_Y).ToInt();
	ptRet.z = GetValue(SpaceScale_Z).ToInt();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemScale(point3 ptValue)
{
	m_mapParam[SpaceScale_X].SetValue(ptValue.x);
	m_mapParam[SpaceScale_Y].SetValue(ptValue.y);
	m_mapParam[SpaceScale_Z].SetValue(ptValue.z);
}

point3 BaseGraph_DataBase::BaseObject::GetItemCenter()
{
	point3 ptRet = { 0 };
	ptRet.x = GetValue(SpaceCenter_X).ToInt();
	ptRet.y = GetValue(SpaceCenter_Y).ToInt();
	ptRet.z = GetValue(SpaceCenter_Z).ToInt();
	return ptRet;
}

void BaseGraph_DataBase::BaseObject::SetItemCenter(point3 ptValue)
{
	m_mapParam[SpaceCenter_X].SetValue(ptValue.x);
	m_mapParam[SpaceCenter_Y].SetValue(ptValue.y);
	m_mapParam[SpaceCenter_Z].SetValue(ptValue.z);
}
