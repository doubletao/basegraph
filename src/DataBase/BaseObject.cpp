#include "stdafx.h"
#include "BaseObject.h"
#include "IntIDGenerator.h"
#include "ObjectFactory.h"

BaseGraph_DataBase::BaseObject::BaseObject()
	: m_pParent(nullptr)
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
		//先己后子
		int nTmp = (int)m_mapParam.size();
		archive << nTmp;
		for (std::map<ParamValueDef, Value>::iterator it = m_mapParam.begin(); it != m_mapParam.end(); it++)
		{
			nTmp = it->first;
			archive << nTmp;
			it->second.Serialize(archive);
		}
		nTmp = (int)m_vecChildObj.size();
		archive << nTmp;
		for (size_t i = 0; i < m_vecChildObj.size(); i++)
		{
			BaseObject * pObj = m_vecChildObj[i];
			nTmp = pObj->GetObjectType();
			archive << nTmp;
			pObj->Serialize(archive);
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
		archive >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			int nTmp = 0;
			archive >> nTmp;
			BaseGraph_DataBase::ObjectType emObjType = (BaseGraph_DataBase::ObjectType)nTmp;
			BaseObject * pObject = ObjectFactory::CreateObject(emObjType);
			pObject->Serialize(archive);
			pObject->m_pParent = this;
			m_vecChildObj.push_back(pObject);
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
