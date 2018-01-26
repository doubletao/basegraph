#include "stdafx.h"
#include "BaseObject.h"
#include "IntIDGenerator.h"
#include "ObjectFactory.h"

BaseGraph_DataBase::BaseObject::BaseObject()
	: m_pParent(nullptr)
{
}


BaseGraph_DataBase::BaseObject::~BaseObject()
{
}

void BaseGraph_DataBase::BaseObject::InitialObjectParam()
{
	//如果有值则不覆盖，如果没值，则给值
	if (m_mapParam.count(ObjID) < 1)
	{
		m_mapParam[ObjID].SetValue(IntIDGenerator::Instance().GetNewID());
	}
}

bool BaseGraph_DataBase::BaseObject::Serilize(CArchive & archive)
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
			it->second.Serilize(archive);
		}
		nTmp = (int)m_vecChildObj.size();
		archive << nTmp;
		for (size_t i = 0; i < m_vecChildObj.size(); i++)
		{
			BaseObject * pObj = m_vecChildObj[i];
			nTmp = pObj->GetObjectType();
			archive << nTmp;
			pObj->Serilize(archive);
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
			valueTmp.Serilize(archive);
			m_mapParam.insert(std::make_pair(emParamValueDef, valueTmp));
		}
		archive >> nSize;
		for (int i = 0; i < nSize; i++)
		{
			int nTmp = 0;
			archive >> nTmp;
			BaseGraph_DataBase::ObjectType emObjType = (BaseGraph_DataBase::ObjectType)nTmp;
			BaseObject * pObject = ObjectFactory::CreateObject(emObjType);
			pObject->Serilize(archive);
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
