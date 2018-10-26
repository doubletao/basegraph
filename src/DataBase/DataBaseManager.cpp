#include "stdafx.h"
#include "DataBaseManager.h"
#include "ObjectFactory.h"
#include "IntIDGenerator.h"
#include <assert.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
DataBaseManager::DataBaseManager()
	: m_nSceneWidth(1920)
	, m_nSceneHeight(1080)
{
	InitializeCriticalSection(&m_lockData);
	//m_nSceneWidth = 720;
	//m_nSceneHeight = 576;
}


DataBaseManager::~DataBaseManager()
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		BaseObject *& pObj = m_vecObject[i];
		delete pObj;
		pObj = nullptr;
	}
	m_vecObject.clear();
	DeleteCriticalSection(&m_lockData);
}

DataBaseManager & DataBaseManager::Instance()
{
	static DataBaseManager inst;
	return inst;
}

//#define DEBUG_LOCK
void DataBaseManager::Lock()
{
#ifdef DEBUG_LOCK
	if (true)
	{
		CString strDebug;
		strDebug.Format(_T("DataBaseManager::Lock() ask\n"));
		OutputDebugString(strDebug);
	}
#endif // DEBUG
	EnterCriticalSection(&m_lockData);
#ifdef DEBUG_LOCK
	if (true)
	{
		CString strDebug;
		strDebug.Format(_T("DataBaseManager::Lock() locked\n"));
		OutputDebugString(strDebug);
	}
#endif // DEBUG
}

void DataBaseManager::Unlock()
{
	LeaveCriticalSection(&m_lockData);
#ifdef DEBUG_LOCK
	if (true)
	{
		OutputDebugString(_T("DataBaseManager::Unlock()\n"));
	}
#endif // DEBUG
}

bool DataBaseManager::CheckItemID(CString strID)
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i] && m_vecObject[i]->GetItemID() == strID)
		{
			return false;
		}
	}
	return true;
}

bool DataBaseManager::GetAllItemID(OUT std::vector<CString>& vecID)
{
	vecID.clear();
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i])
		{
			vecID.push_back(m_vecObject[i]->GetItemID());
		}
	}
	return true;
}

bool DataBaseManager::GetAllItemIDExceptSubItem(OUT std::vector<CString>& vecID)
{
	vecID.clear();
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i]
			&& !HasSubItem(m_vecObject[i]->GetItemID()))
		{
			vecID.push_back(m_vecObject[i]->GetItemID());
		}
	}
	return true;
}

bool DataBaseManager::GetItemType(IN CString strID, OUT BaseGraph_DataBase::ObjectType & emType)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		emType = pObj->GetItemType();
		return true;
	}
	return false;
}

bool DataBaseManager::GetAllItem(OUT vector<BaseObject *>& vecObject)
{
	return false;
}

bool DataBaseManager::GetItem(IN CString strID, OUT BaseObject * & pObject)
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i] && m_vecObject[i]->GetItemID() == strID)
		{
			pObject = m_vecObject[i];
			return true;
		}
	}
	return false;
}

bool DataBaseManager::CreateItem(IN ObjectType emType, IN const map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	//物件厂造一个指定物件并赋值
	BaseObject * pObj = ObjectFactory::CreateObject(emType);
	if (pObj)
	{
		pObj->SetProperty(mapParam);
		if (AddItem(pObj))
		{
			return true;
		}
		delete pObj;
		pObj = nullptr;
		return false;
	}
	return false;
}

bool DataBaseManager::CreateItem(IN ObjectType emType, IN const map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, OUT CString & strID)
{
	//物件厂造一个指定物件并赋值
	BaseObject * pObj = ObjectFactory::CreateObject(emType);
	if (pObj)
	{
		pObj->SetProperty(mapParam);
		strID = pObj->GetItemID();
		if (AddItem(pObj))
		{
			return true;
		}
		delete pObj;
		pObj = nullptr;
		return false;
	}
	return false;
}

bool DataBaseManager::AddItem(BaseObject * pObj)
{
	CString strID = pObj->GetItemID();
	if (CheckItemID(strID))
	{
		m_vecObject.push_back(pObj);
		return true;
	}
	else
	{
		return false;
	}
}

bool DataBaseManager::AddSubItemTo(IN CString strParentID, IN CString strSubID)
{
	return false;
}

bool DataBaseManager::DeleteItem(IN CString strID)
{
	for (vector<BaseObject *>::iterator it = m_vecObject.begin(); it != m_vecObject.end(); it++)
	{
		BaseObject * pObj = *it;
		if (pObj)
		{
			if (pObj->GetItemID() == strID)
			{
				delete pObj;
				pObj = nullptr;
				m_vecObject.erase(it);
				break;
			}
		}
	}
	return false;
}

bool DataBaseManager::MoveItemToTop(IN CString strID)
{
	return false;
}

bool DataBaseManager::MoveItemToBottom(IN CString strID)
{
	return false;
}

bool DataBaseManager::MoveItemBefore(IN CString strSrcID, IN CString strDesID)
{
	return false;
}

bool DataBaseManager::MoveItemAfter(IN CString strSrcID, IN CString strDesID)
{
	return false;
}

bool DataBaseManager::HasSubItem(IN CString strParentID)
{
	return false;
}

bool DataBaseManager::GetSubItem(IN CString strParentID, OUT std::vector<CString>& vecSubID)
{
	return false;
}

bool DataBaseManager::HasParentItem(IN CString strSubID)
{
	return false;
}

bool DataBaseManager::GetParentItem(IN CString strSubID, OUT CString & strParentID)
{
	return false;
}

bool DataBaseManager::GetItemProperty(IN CString strID, IN ParamValueDef emParam, OUT Value & stuParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		stuParam = pObj->GetValue(emParam);
		return true;
	}
	return false;
}

bool DataBaseManager::GetItemProperty(IN CString strID, OUT map<ParamValueDef, Value>& mapParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		mapParam = pObj->GetProperty();
		return true;
	}
	return false;
}

float3 DataBaseManager::GetItemPos(IN CString strID)
{
	float3 f3Pos = {0};
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Pos = pObj->GetItemPos();
	}
	return f3Pos;
}

float3 DataBaseManager::GetItemCenter(IN CString strID)
{
	float3 f3Center = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Center = pObj->GetItemCenter();
	}
	return f3Center;
}

float3 DataBaseManager::GetItemAngle(IN CString strID)
{
	float3 f3Angle = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Angle = pObj->GetItemRotate();
	}
	return f3Angle;
}

float3 DataBaseManager::GetItemAngleRadian(IN CString strID)
{
	float3 f3Angle = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Angle = pObj->GetItemRotateRadian();
	}
	return f3Angle;
}

float3 DataBaseManager::GetItemScale(IN CString strID)
{
	float3 f3Scale = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Scale = pObj->GetItemScale();
	}
	return f3Scale;
}

BaseGraph_DataBase::ITEM_FORMTYPE DataBaseManager::Get3DType(IN CString strID)
{
	BaseGraph_DataBase::ITEM_FORMTYPE emType = BaseGraph_DataBase::ITEM_FORMTYPE_2D;
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		emType = pObj->Get3DType();
	}
	return emType;
}

bool DataBaseManager::SetItemProperty(IN CString strID, IN ParamValueDef emParam, IN const Value & stuParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		pObj->SetValue(emParam, stuParam);
		return true;
	}
	return false;
}

bool DataBaseManager::SetItemProperty(IN CString strID, IN const map<ParamValueDef, Value>& mapParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		for (map<ParamValueDef, Value>::const_iterator it = mapParam.begin(); it != mapParam.end(); it++)
		{
			pObj->SetValue(it->first, it->second);
		}
		return true;
	}
	return false;
}

CString DataBaseManager::GetItemTypePrefix(BaseGraph_DataBase::ObjectType emObjType)
{
	switch (emObjType)
	{
	case BaseGraph_DataBase::ObjectType_Err:
		return _T("未知类型");
	case BaseGraph_DataBase::ObjectType_2DLayer:
		return _T("2D层");
	case BaseGraph_DataBase::ObjectType_Titleword:
		return _T("标题字");
	case BaseGraph_DataBase::ObjectType_Scroll:
		return _T("滚屏");
	case BaseGraph_DataBase::ObjectType_Image:
		return _T("图片");
	default:
		break;
	}
	return _T("");
}

std::set<CString> DataBaseManager::GetAllItemName()
{
	std::set<CString> setItemNames;
	std::vector<CString> vecItemID;
	GetAllItemID(vecItemID);
	for (int i = 0; i < vecItemID.size(); i++)
	{
		CString strID = vecItemID[i];
		BaseGraph_Common::Value valueItemName;
		if (GetItemProperty(strID, BaseGraph_DataBase::Obj_Name_Str, valueItemName))
		{
			CString strName = valueItemName.ToCString();
			setItemNames.insert(strName);
		}
	}
	return setItemNames;
}

bool DataBaseManager::IsItemNameExist(CString strItemName)
{
	std::vector<CString> vecItemID;
	GetAllItemID(vecItemID);
	for (int i = 0; i < vecItemID.size(); i++)
	{
		CString strID = vecItemID[i];
		BaseGraph_Common::Value valueItemName;
		if (GetItemProperty(strID, BaseGraph_DataBase::Obj_Name_Str, valueItemName))
		{
			CString strName = valueItemName.ToCString();
			if (strName == strItemName)
			{
				return true;
			}
		}
	}
	return false;
}

CString DataBaseManager::GetNextItemName(BaseGraph_DataBase::ObjectType emObjType)
{
	//取名的方式是在类型后边加数字，判断是否重复，重复则数字++并继续，不重复则使用
	CString strPrefix = GetItemTypePrefix(emObjType);
	int nSuffix = 0;
	std::set<CString> setItemNames = GetAllItemName();
	CString strNextName = strPrefix;
	do 
	{
		if (nSuffix > 0)
		{
			strNextName.Format(_T("%s%d"), strPrefix, nSuffix);
		}
		nSuffix++;
	} while (setItemNames.count(strNextName) > 0);
	return strNextName;
}

bool DataBaseManager::GetItemLength(OUT int & nLength)
{
	std::vector<CString> vecItemID;
	GetAllItemID(vecItemID);
	return GetItemLength(vecItemID, nLength);
}

bool DataBaseManager::GetItemLength(IN const std::vector<CString>& vecID, OUT int & nLength)
{
	bool bRet = false;
	nLength = 0;
	for (size_t i = 0; i < vecID.size(); i++)
	{
		CString strID = vecID[i];
		BaseGraph_Common::Value valueEndFrame;
		if (GetItemProperty(strID, BaseGraph_DataBase::Obj_ActionEnd_Int, valueEndFrame))
		{
			int nEndFrame = valueEndFrame.ToInt();
			if (nLength < nEndFrame)
			{
				nLength = nEndFrame;
			}
			bRet = true;
		}
	}
	return bRet;
}

int DataBaseManager::GetSceneWidth()
{
	return m_nSceneWidth;
}

int DataBaseManager::GetSceneHeight()
{
	return m_nSceneHeight;
}

bool DataBaseManager::SaveToFile(CString strFile)
{
	CFile cf;
	CFileException e;
	BOOL bOpenSuc = FALSE;
	bOpenSuc = cf.Open(strFile, CFile::modeCreate | CFile::modeWrite, &e);

	if (bOpenSuc)
	{
		CArchive ar(&cf, CArchive::store);
		//先序列化物件
		bool bRet = SerializeObjects(ar, m_vecObject);
		//序列化其他
		ar.Close();
		cf.Close();
		return bRet;
	}
	return false;
}

bool DataBaseManager::LoadFromFile(CString strFile)
{
	CFile cf;
	CFileException e;
	BOOL bOpenSuc = FALSE;
	if (_taccess(strFile, 0) != -1)
	{
		bOpenSuc = cf.Open(strFile, CFile::modeRead, &e);
	}

	if (bOpenSuc)
	{
		CArchive ar(&cf, CArchive::load);
		bool bRet = SerializeObjects(ar, m_vecObject);
		ar.Close();
		cf.Close();
		return bRet;
	}
	return false;
}

bool DataBaseManager::SerializeObjects(CArchive & archive, vector<BaseObject*>& vecObject)
{
	if (archive.IsStoring())
	{
	}
	else
	{
	}
	return false;
}

bool DataBaseManager::SaveObjToXMLStr(IN const std::vector<CString>& vecObjID, OUT CString & strXML)
{
	bool bRet = false;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleBaseObject = new TiXmlElement("BaseObject");
		if (eleBaseObject)
		{
			xmlDoc->LinkEndChild(eleBaseObject);
			for (auto it : vecObjID)
			{
				BaseObject * pObject = nullptr;
				if (GetItem(it, pObject))
				{
					if (pObject)
					{
						TiXmlElement * eleObject = new TiXmlElement(CommonFunction::ConverCStringToStdString(it));
						if (eleObject)
						{
							eleBaseObject->LinkEndChild(eleObject);
							std::map<ParamValueDef, Value> mapProperty = pObject->GetProperty();
							TiXmlElement * eleProperty = new TiXmlElement("property");
							if (eleProperty)
							{
								eleObject->LinkEndChild(eleProperty);
								for (auto it : mapProperty)
								{
									CString strValueStr = BaseGraph_DataBase::GetDateTypeValueStr(it.first, it.second);
									TinyXMLHelper::SetAttribute(eleProperty, CommonFunction::ConverCStringToStdString(BaseGraph_DataBase::GetValueDescriptor(it.first).strDispName), strValueStr);
								}
							}
							TinyXMLHelper::SetAttribute(eleObject, "m_strParentID", pObject->GetParentID());
							std::vector<CString> vecChildID = pObject->GetChildID();
							TiXmlElement * eleChildID = new TiXmlElement("ChildID");
							if (eleChildID)
							{
								eleObject->LinkEndChild(eleChildID);
								for (auto it : vecChildID)
								{
									TiXmlElement * eleID = new TiXmlElement(CommonFunction::ConverCStringToStdString(it));
									eleChildID->LinkEndChild(eleID);
								}
							}
							bRet = true;
						}
					}
				}
			}
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strXML = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return bRet;
}

bool DataBaseManager::LoadObjFromXMLStr(IN const CString & strXML, OUT std::vector<CString>& vecObjID)
{
	bool bRet = false;
	TiXmlDocument* xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strXML).c_str()))
		{
			TiXmlHandle docHandle(xmlDoc);
			TiXmlElement * eleBaseObject = xmlDoc->FirstChildElement("BaseObject");
			if (eleBaseObject)
			{
				//先拿到物件节点，然后遍历物件节点
				TiXmlElement * pEleObj = eleBaseObject->FirstChildElement();
				while (pEleObj)
				{
					//先尝试拿物件类型
					std::string cstrObjType = CommonFunction::ConverCStringToStdString(BaseGraph_DataBase::GetValueDescriptor(BaseGraph_DataBase::Obj_Type_Enum).strDispName);
					int nType = -1;
					pEleObj->Attribute(cstrObjType, &nType);
					ObjectType emType = static_cast<ObjectType>(nType);
					if (emType != BaseGraph_DataBase::ObjectType_Err)
					{
						//拿到属性节点
						TiXmlElement * eleProperty = pEleObj->FirstChildElement("property");
						if (eleProperty)
						{
							//按照类型创建默认物件，然后填充属性
							std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam = ObjectFactory::GetDefaultParam(emType);
							//按照当前物件属性集合去xml里查找相应属性，如果找到了就赋值，找不到算了，用默认值
							for (auto it : mapParam)
							{
								//根据属性类型取到属性节点名
								std::string cstrEleName = CommonFunction::ConverCStringToStdString(BaseGraph_DataBase::GetValueDescriptor(it.first).strDispName);
								//查找xml中的对应属性
								std::string cstrProperValue = eleProperty->Attribute(cstrEleName.c_str());
								BaseGraph_DataBase::RestoreDataTypeValueFromXML(it.first, BaseGraph_Common::GetValueTypeStr(it.second.GetValueType()), CommonFunction::ConvertStdStringToCString(cstrProperValue), it.second);
							}
							CreateItem(emType, mapParam);
							//取子物件id集合
						}
					}
					pEleObj = pEleObj->NextSiblingElement();
				}
				bRet = true;
			}
		}
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return bRet;
}

