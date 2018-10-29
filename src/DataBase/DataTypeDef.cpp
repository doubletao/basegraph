#include "stdafx.h"
#include "DataBase\DataTypeDefDetail.h"
#include "DataBase\DataTypeDef.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
using namespace BaseGraph_DataBase;

BaseGraph_DataBase::artinfo::artinfo()
{
	Reset();
}

void BaseGraph_DataBase::artinfo::Reset()
{
	type = ARTNULL;
	cv_height = 0;
	cu_angle = 0;
	soft = 0;
	color = 0;
	metal.Reset();
}

int BaseGraph_DataBase::artinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, type);		//效果类型
	SERIALIZE_PARAM_GET_SIZE(nSize, cv_height);   //凹凸高度       
	SERIALIZE_PARAM_GET_SIZE(nSize, cu_angle);	//凹凸角度
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);		//虚边宽
	SERIALIZE_PARAM_GET_SIZE(nSize, color);		//凹凸单色,不透明
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, metal);		//立体边
	return nSize;
}

bool BaseGraph_DataBase::artinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, type);		//效果类型
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, cv_height);   //凹凸高度       
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, cu_angle);	//凹凸角度
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);		//虚边宽
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, color);		//凹凸单色,不透明
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, metal);		//立体边
	return true;
}

bool BaseGraph_DataBase::artinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, type);		//效果类型
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, cv_height);   //凹凸高度       
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, cu_angle);	//凹凸角度
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);		//虚边宽
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, color);		//凹凸单色,不透明
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, metal);		//立体边
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::artinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleArtinfo = new TiXmlElement("artinfo");
		if (eleArtinfo)
		{
			xmlDoc->LinkEndChild(eleArtinfo);
			eleArtinfo->SetAttribute("type", type);
			TinyXMLHelper::SetAttribute(eleArtinfo, "cv_height", cv_height);
			TinyXMLHelper::SetAttribute(eleArtinfo, "cu_angle", cu_angle);
			eleArtinfo->SetAttribute("soft", soft);
			eleArtinfo->SetAttribute("color", color);
			eleArtinfo->SetAttribute("metal", CommonFunction::ConvertCStringToStdString(metal.GetValueStr()).c_str());
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::artinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					TiXmlElement * eleArtinfo = xmlDoc->FirstChildElement("artinfo");
					if (eleArtinfo)
					{
						eleArtinfo->Attribute("type", &nIntValue);
						type = (GRAPH_ART_PROC_TYPE)nIntValue;
						TinyXMLHelper::Attribute(eleArtinfo, "cv_height", cv_height);
						TinyXMLHelper::Attribute(eleArtinfo, "cu_angle", cu_angle);
						eleArtinfo->Attribute("soft", &soft);
						TinyXMLHelper::Attribute(eleArtinfo, "color", color);
						CString strValue;
						TinyXMLHelper::Attribute(eleArtinfo, "metal", strValue);
						if (!strValue.IsEmpty())
						{
							metal.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::faceinfo::faceinfo()
{
	Reset();
}

void BaseGraph_DataBase::faceinfo::Reset()
{
	disable = 0;
	soft = 0;
	effect_type = 0;
	effect_width = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::faceinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_width);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::faceinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::faceinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::faceinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleFaceinfo = new TiXmlElement("faceinfo");
		if (eleFaceinfo)
		{
			xmlDoc->LinkEndChild(eleFaceinfo);
			eleFaceinfo->SetAttribute("disable", disable);
			eleFaceinfo->SetAttribute("soft", soft);
			eleFaceinfo->SetAttribute("effect_type", effect_type);
			eleFaceinfo->SetAttribute("effect_width", effect_width);
			eleFaceinfo->SetAttribute("avcolor", avcolor);
			eleFaceinfo->SetAttribute("art", CommonFunction::ConvertCStringToStdString(art.GetValueStr()).c_str());
			eleFaceinfo->SetAttribute("color", CommonFunction::ConvertCStringToStdString(color.GetValueStr()).c_str());
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::faceinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					TiXmlElement * eleFaceinfo = xmlDoc->FirstChildElement("faceinfo");
					if (eleFaceinfo)
					{
						eleFaceinfo->Attribute("disable", &disable);
						eleFaceinfo->Attribute("soft", &soft);
						eleFaceinfo->Attribute("effect_type", &effect_type);
						eleFaceinfo->Attribute("effect_width", &effect_width);
						eleFaceinfo->Attribute("avcolor", &nIntValue);
						avcolor = nIntValue;
						cstrValue = eleFaceinfo->Attribute("art");
						if (cstrValue)
						{
							art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrValue));
						}
						cstrValue = eleFaceinfo->Attribute("color");
						if (cstrValue)
						{
							color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrValue));
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::borderinfo::borderinfo()
{
	Reset();
}

void BaseGraph_DataBase::borderinfo::Reset()
{
	disable = 1;
	soft = 0;
	linestyle = 0;
	width = 0;
	interval = 0;//点
	effect_type = 0;
	effect_width = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::borderinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);
	SERIALIZE_PARAM_GET_SIZE(nSize, linestyle);
	SERIALIZE_PARAM_GET_SIZE(nSize, width);
	SERIALIZE_PARAM_GET_SIZE(nSize, interval);//点线的间距
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_width);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::borderinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, linestyle);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, interval);//点线的间距
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::borderinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, linestyle);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, interval);//点线的间距
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::borderinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleBorderinfo = new TiXmlElement("borderinfo");
		if (eleBorderinfo)
		{
			xmlDoc->LinkEndChild(eleBorderinfo);
			eleBorderinfo->SetAttribute("disable", disable);
			eleBorderinfo->SetAttribute("soft", soft);
			eleBorderinfo->SetAttribute("linestyle", linestyle);
			TinyXMLHelper::SetAttribute(eleBorderinfo, "width", width);
			TinyXMLHelper::SetAttribute(eleBorderinfo, "interval", interval);
			eleBorderinfo->SetAttribute("effect_type", effect_type);
			eleBorderinfo->SetAttribute("effect_width", effect_width);
			eleBorderinfo->SetAttribute("avcolor", avcolor);
			eleBorderinfo->SetAttribute("art", CommonFunction::ConvertCStringToStdString(art.GetValueStr()).c_str());
			eleBorderinfo->SetAttribute("color", CommonFunction::ConvertCStringToStdString(color.GetValueStr()).c_str());
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::borderinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					TiXmlElement * eleBorderinfo = xmlDoc->FirstChildElement("borderinfo");
					if (eleBorderinfo)
					{
						eleBorderinfo->Attribute("disable", &disable);
						eleBorderinfo->Attribute("soft", &soft);
						eleBorderinfo->Attribute("linestyle", &linestyle);
						TinyXMLHelper::Attribute(eleBorderinfo, "width", width);
						TinyXMLHelper::Attribute(eleBorderinfo, "interval", interval);
						TinyXMLHelper::Attribute(eleBorderinfo, "effect_type", effect_type);
						eleBorderinfo->Attribute("effect_width", &effect_width);
						TinyXMLHelper::Attribute(eleBorderinfo, "avcolor", avcolor);
						CString strValue;
						TinyXMLHelper::Attribute(eleBorderinfo, "art", strValue);
						if (!strValue.IsEmpty())
						{
							art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
						TinyXMLHelper::Attribute(eleBorderinfo, "color", strValue);
						if (!strValue.IsEmpty())
						{
							color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::shadowinfo::shadowinfo()
{
	Reset();
}

void BaseGraph_DataBase::shadowinfo::Reset()
{
	disable = 1;
	soft = 0;
	offsetx = 0;
	offsety = 0;
	effect_type = 0;
	effect_fxtitle = 0;
	effect_fyscale = 0;
	effect_fadeout = 0;
	effect_style = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::shadowinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, soft);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsetx);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsety);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_fxtitle);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_fyscale);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_fadeout);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_style);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::shadowinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, soft);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsetx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsety);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_fxtitle);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_fyscale);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_fadeout);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_style);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::shadowinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, soft);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsetx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsety);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_fxtitle);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_fyscale);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_fadeout);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_style);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::shadowinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleShadowinfo = new TiXmlElement("shadowinfo");
		if (eleShadowinfo)
		{
			xmlDoc->LinkEndChild(eleShadowinfo);
			eleShadowinfo->SetAttribute("disable", disable);
			eleShadowinfo->SetAttribute("soft", soft);
			eleShadowinfo->SetAttribute("offsetx", offsetx);
			eleShadowinfo->SetAttribute("offsety", offsety);
			eleShadowinfo->SetAttribute("effect_type", effect_type);
			TinyXMLHelper::SetAttribute(eleShadowinfo,"effect_fxtitle", effect_fxtitle);
			TinyXMLHelper::SetAttribute(eleShadowinfo,"effect_fyscale", effect_fyscale);
			TinyXMLHelper::SetAttribute(eleShadowinfo,"effect_fadeout", effect_fadeout);
			TinyXMLHelper::SetAttribute(eleShadowinfo,"effect_style", effect_style);
			TinyXMLHelper::SetAttribute(eleShadowinfo,"avcolor", avcolor);
			TinyXMLHelper::SetAttribute(eleShadowinfo, "art", art.GetValueStr());
			TinyXMLHelper::SetAttribute(eleShadowinfo, "color", color.GetValueStr());
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::shadowinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					TiXmlElement * eleShadowinfo = xmlDoc->FirstChildElement("shadowinfo");
					if (eleShadowinfo)
					{
						eleShadowinfo->Attribute("disable", &disable);
						eleShadowinfo->Attribute("soft", &soft);
						eleShadowinfo->Attribute("offsetx", &offsetx);
						eleShadowinfo->Attribute("offsety", &offsety);
						eleShadowinfo->Attribute("effect_type", &effect_type);
						TinyXMLHelper::Attribute(eleShadowinfo, "effect_fxtitle", effect_fxtitle);
						TinyXMLHelper::Attribute(eleShadowinfo, "effect_fyscale", effect_fyscale);
						TinyXMLHelper::Attribute(eleShadowinfo, "effect_fadeout", effect_fadeout);
						TinyXMLHelper::Attribute(eleShadowinfo, "effect_style", effect_style);
						TinyXMLHelper::Attribute(eleShadowinfo, "avcolor", avcolor);
						CString strValue;
						TinyXMLHelper::Attribute(eleShadowinfo, "art", strValue);
						if (!strValue.IsEmpty())
						{
							art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
						TinyXMLHelper::Attribute(eleShadowinfo, "color", strValue);
						if (!strValue.IsEmpty())
						{
							color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::cubeinfo::cubeinfo()
{
	Reset();
}

void BaseGraph_DataBase::cubeinfo::Reset()
{
	disable = 1;
	width = 0;
	angle = 0;
	effect_type = 0;
	effect_width = 0;
	avcolor = 0;
	art.Reset();
	color.Reset();
}

int BaseGraph_DataBase::cubeinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, width);
	SERIALIZE_PARAM_GET_SIZE(nSize, angle);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_type);
	SERIALIZE_PARAM_GET_SIZE(nSize, effect_width);
	SERIALIZE_PARAM_GET_SIZE(nSize, avcolor);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, art);
	SERIALIZE_PARAM_GET_SIZE_STU(nSize, color);
	return nSize;
}

bool BaseGraph_DataBase::cubeinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, angle);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_type);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, effect_width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, avcolor);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, art);
	SERIALIZE_PARAM_TO_BUF_STU(pBuf, nSize, color);
	return true;
}

bool BaseGraph_DataBase::cubeinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, angle);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_type);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, effect_width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, avcolor);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, art);
	SERIALIZE_BUF_TO_PARAM_STU(pBuf, nSize, color);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::cubeinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleCubeinfo = new TiXmlElement("cubeinfo");
		if (eleCubeinfo)
		{
			xmlDoc->LinkEndChild(eleCubeinfo);
			eleCubeinfo->SetAttribute("disable", disable);
			TinyXMLHelper::SetAttribute(eleCubeinfo, "width", width);
			TinyXMLHelper::SetAttribute(eleCubeinfo, "angle", angle);
			eleCubeinfo->SetAttribute("effect_type", effect_type);
			eleCubeinfo->SetAttribute("effect_width", effect_width);
			TinyXMLHelper::SetAttribute(eleCubeinfo, "avcolor", avcolor);
			TinyXMLHelper::SetAttribute(eleCubeinfo, "art", art.GetValueStr());
			TinyXMLHelper::SetAttribute(eleCubeinfo, "color", color.GetValueStr());
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::cubeinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					TiXmlElement * eleCubeinfo = xmlDoc->FirstChildElement("cubeinfo");
					if (eleCubeinfo)
					{
						eleCubeinfo->Attribute("disable", &disable);
						TinyXMLHelper::Attribute(eleCubeinfo, "width", width);
						TinyXMLHelper::Attribute(eleCubeinfo, "angle", angle);
						eleCubeinfo->Attribute("effect_type", &effect_type);
						eleCubeinfo->Attribute("effect_width", &effect_width);
						TinyXMLHelper::Attribute(eleCubeinfo, "avcolor", avcolor);
						CString strValue;
						TinyXMLHelper::Attribute(eleCubeinfo, "art", strValue);
						if (!strValue.IsEmpty())
						{
							art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
						TinyXMLHelper::Attribute(eleCubeinfo, "color", strValue);
						if (!strValue.IsEmpty())
						{
							color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::lightinfo::lightinfo()
{
	Reset();
}

void BaseGraph_DataBase::lightinfo::Reset()
{
	ambience = 0;
	ambience_degree = 0;
	reflectcolor = 0;
	dwReverse1 = 0;
	dwReverse2 = 0;
	memset(szPath, 0, sizeof(szPath));
}

int BaseGraph_DataBase::lightinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, ambience);	//环镜单色,不透明
	SERIALIZE_PARAM_GET_SIZE(nSize, ambience_degree);  //明亮程度1-100
	SERIALIZE_PARAM_GET_SIZE(nSize, reflectcolor);		//反射单色,不透明
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse1);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse2);
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	return nSize;
}

bool BaseGraph_DataBase::lightinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, ambience);	//环镜单色,不透明
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, ambience_degree);  //明亮程度1-100
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, reflectcolor);		//反射单色,不透明
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse1);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse2);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	return true;
}

bool BaseGraph_DataBase::lightinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, ambience);	//环镜单色,不透明
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, ambience_degree);  //明亮程度1-100
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, reflectcolor);		//反射单色,不透明
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse1);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse2);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::lightinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleLightinfo = new TiXmlElement("lightinfo");
		if (eleLightinfo)
		{
			xmlDoc->LinkEndChild(eleLightinfo);
			eleLightinfo->SetAttribute("ambience", ambience);
			eleLightinfo->SetAttribute("ambience_degree", ambience_degree);
			eleLightinfo->SetAttribute("reflectcolor", reflectcolor);
			eleLightinfo->SetAttribute("dwReverse1", dwReverse1);
			eleLightinfo->SetAttribute("dwReverse2", dwReverse2);
			eleLightinfo->SetAttribute("szPath", CommonFunction::ConvertCStringToStdString(szPath).c_str());
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::lightinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleLightinfo = xmlDoc->FirstChildElement("lightinfo");
					if (eleLightinfo)
					{
						eleLightinfo->Attribute("ambience", &nIntValue);
						ambience = nIntValue;
						eleLightinfo->Attribute("ambience_degree", &nIntValue);
						ambience_degree = nIntValue;
						eleLightinfo->Attribute("reflectcolor", &nIntValue);
						reflectcolor = nIntValue;
						eleLightinfo->Attribute("dwReverse1", &nIntValue);
						dwReverse1 = nIntValue;
						eleLightinfo->Attribute("dwReverse2", &nIntValue);
						dwReverse2 = nIntValue;
						CString strValue;
						TinyXMLHelper::Attribute(eleLightinfo, "szPath", strValue);
						if (!strValue.IsEmpty())
						{
							memcpy(szName, strValue.GetBuffer(), strValue.GetLength() * sizeof(TCHAR));
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::channelinfo::channelinfo()
{
	Reset();
}

void BaseGraph_DataBase::channelinfo::Reset()
{
	memset(szPath, 0, sizeof(szPath));
	weight = 0;
	channelid = 0;
	dwReverse1 = 0;
	dwReverse2 = 0;
}

int BaseGraph_DataBase::channelinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	SERIALIZE_PARAM_GET_SIZE(nSize, weight);
	SERIALIZE_PARAM_GET_SIZE(nSize, channelid);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse1);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse2);
	return nSize;
}

bool BaseGraph_DataBase::channelinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, weight);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, channelid);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse1);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse2);
	return true;
}

bool BaseGraph_DataBase::channelinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, weight);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, channelid);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse1);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse2);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::channelinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleChannelinfo = new TiXmlElement("channelinfo");
		if (eleChannelinfo)
		{
			xmlDoc->LinkEndChild(eleChannelinfo);
			eleChannelinfo->SetAttribute("szPath", CommonFunction::ConvertCStringToStdString(szPath).c_str());
			eleChannelinfo->SetAttribute("weight", weight);
			eleChannelinfo->SetAttribute("channelid", channelid);
			eleChannelinfo->SetAttribute("dwReverse1", dwReverse1);
			eleChannelinfo->SetAttribute("dwReverse2", dwReverse2);
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::channelinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleChannelinfo = xmlDoc->FirstChildElement("channelinfo");
					if (eleChannelinfo)
					{
						cstrValue = eleChannelinfo->Attribute("szPath");
						if (nullptr != cstrValue)
						{
							CString strValue = CommonFunction::ConvertStdStringToCString(cstrValue);
							memcpy(szName, strValue.GetBuffer(), strValue.GetLength() * sizeof(TCHAR));
						}
						eleChannelinfo->Attribute("weight", &weight);
						eleChannelinfo->Attribute("channelid", &channelid);
						eleChannelinfo->Attribute("dwReverse1", &nIntValue);
						dwReverse1 = nIntValue;
						eleChannelinfo->Attribute("dwReverse2", &nIntValue);
						dwReverse2 = nIntValue;
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::materialinfo::materialinfo()
{
	Reset();
}

void BaseGraph_DataBase::materialinfo::Reset()
{
	memset(szPath, 0, sizeof(szPath));
	dwReverse1 = 0;
	dwReverse2 = 0;
}

int BaseGraph_DataBase::materialinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse1);
	SERIALIZE_PARAM_GET_SIZE(nSize, dwReverse2);
	return nSize;
}

bool BaseGraph_DataBase::materialinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse1);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, dwReverse2);
	return true;
}

bool BaseGraph_DataBase::materialinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse1);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, dwReverse2);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::materialinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleChannelinfo = new TiXmlElement("channelinfo");
		if (eleChannelinfo)
		{
			xmlDoc->LinkEndChild(eleChannelinfo);
			eleChannelinfo->SetAttribute("szPath", CommonFunction::ConvertCStringToStdString(szPath).c_str());
			eleChannelinfo->SetAttribute("dwReverse1", dwReverse1);
			eleChannelinfo->SetAttribute("dwReverse2", dwReverse2);
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::materialinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleChannelinfo = xmlDoc->FirstChildElement("channelinfo");
					if (eleChannelinfo)
					{
						cstrValue = eleChannelinfo->Attribute("szPath");
						if (nullptr != cstrValue)
						{
							CString strValue = CommonFunction::ConvertStdStringToCString(cstrValue);
							memcpy(szName, strValue.GetBuffer(), strValue.GetLength() * sizeof(TCHAR));
						}
						eleChannelinfo->Attribute("dwReverse1", &nIntValue);
						dwReverse1 = nIntValue;
						eleChannelinfo->Attribute("dwReverse2", &nIntValue);
						dwReverse2 = nIntValue;
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::dynamictexture::dynamictexture()
{
	Reset();
}

void BaseGraph_DataBase::dynamictexture::Reset()
{
	memset(szPath, 0, sizeof(szPath));
	disable = 1;
	speed = 0;
	offsetx = 0;
	offsety = 0;
	vRunDirx = 0;
	vRunDiry = 0;
	scalex = 0;
	scaley = 0;
	centerx = 0;
	centery = 0;
	rotate = 0;
	alpha = 0;
	bRepeat = 0;
	bUseAlpha = 0;
}

int BaseGraph_DataBase::dynamictexture::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, szPath);
	SERIALIZE_PARAM_GET_SIZE(nSize, disable);
	SERIALIZE_PARAM_GET_SIZE(nSize, speed);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsetx);
	SERIALIZE_PARAM_GET_SIZE(nSize, offsety);
	SERIALIZE_PARAM_GET_SIZE(nSize, vRunDirx);
	SERIALIZE_PARAM_GET_SIZE(nSize, vRunDiry);
	SERIALIZE_PARAM_GET_SIZE(nSize, scalex);
	SERIALIZE_PARAM_GET_SIZE(nSize, scaley);
	SERIALIZE_PARAM_GET_SIZE(nSize, centerx);
	SERIALIZE_PARAM_GET_SIZE(nSize, centery);
	SERIALIZE_PARAM_GET_SIZE(nSize, rotate);
	SERIALIZE_PARAM_GET_SIZE(nSize, alpha);
	SERIALIZE_PARAM_GET_SIZE(nSize, bRepeat);
	SERIALIZE_PARAM_GET_SIZE(nSize, bUseAlpha);
	return nSize;
}

bool BaseGraph_DataBase::dynamictexture::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, szPath);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, disable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, speed);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsetx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, offsety);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, vRunDirx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, vRunDiry);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, scalex);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, scaley);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, centerx);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, centery);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, rotate);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, alpha);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, bRepeat);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, bUseAlpha);
	return true;
}

bool BaseGraph_DataBase::dynamictexture::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, szPath);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, disable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, speed);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsetx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, offsety);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, vRunDirx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, vRunDiry);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, scalex);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, scaley);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, centerx);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, centery);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, rotate);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, alpha);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, bRepeat);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, bUseAlpha);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::dynamictexture::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		TiXmlElement * eleDynamictexture = new TiXmlElement("dynamictexture");
		if (eleDynamictexture)
		{
			xmlDoc->LinkEndChild(eleDynamictexture);
			eleDynamictexture->SetAttribute("szPath", CommonFunction::ConvertCStringToStdString(szPath).c_str());
			eleDynamictexture->SetAttribute("disable", disable);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "speed", speed);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "offsetx", offsetx);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "offsety", offsety);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "vRunDirx", vRunDirx);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "vRunDiry", vRunDiry);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "scalex", scalex);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "scaley", scaley);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "centerx", centerx);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "centery", centery);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "rotate", rotate);
			TinyXMLHelper::SetAttribute(eleDynamictexture, "alpha", alpha);
			eleDynamictexture->SetAttribute("bRepeat", bRepeat);
			eleDynamictexture->SetAttribute("bUseAlpha", bUseAlpha);
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::dynamictexture::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			const char * cstrValue = nullptr;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleDynamictexture = xmlDoc->FirstChildElement("dynamictexture");
					if (eleDynamictexture)
					{
						cstrValue = eleDynamictexture->Attribute("szPath");
						if (nullptr != cstrValue)
						{
							CString strValue = CommonFunction::ConvertStdStringToCString(cstrValue);
							memcpy(szName, strValue.GetBuffer(), strValue.GetLength() * sizeof(TCHAR));
						}
						eleDynamictexture->Attribute("disable", &disable);
						TinyXMLHelper::Attribute(eleDynamictexture, "speed", speed);
						TinyXMLHelper::Attribute(eleDynamictexture, "offsetx", offsetx);
						TinyXMLHelper::Attribute(eleDynamictexture, "offsety", offsety);
						TinyXMLHelper::Attribute(eleDynamictexture, "vRunDirx", vRunDirx);
						TinyXMLHelper::Attribute(eleDynamictexture, "vRunDiry", vRunDiry);
						TinyXMLHelper::Attribute(eleDynamictexture, "scalex", scalex);
						TinyXMLHelper::Attribute(eleDynamictexture, "scaley", scaley);
						TinyXMLHelper::Attribute(eleDynamictexture, "centerx", centerx);
						TinyXMLHelper::Attribute(eleDynamictexture, "centery", centery);
						TinyXMLHelper::Attribute(eleDynamictexture, "rotate", rotate);
						TinyXMLHelper::Attribute(eleDynamictexture, "alpha", alpha);
						eleDynamictexture->Attribute("bRepeat", &bRepeat);
						eleDynamictexture->Attribute("bUseAlpha", &bUseAlpha);
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::metallight::metallight()
{
	Reset();
}

void BaseGraph_DataBase::metallight::Reset()
{
	enable = 0;
	memset(&pos, 0, sizeof(pos));
	memset(strength, 0, sizeof(strength));
}

int BaseGraph_DataBase::metallight::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, enable);
	SERIALIZE_PARAM_GET_SIZE(nSize, pos);
	SERIALIZE_PARAM_GET_SIZE(nSize, strength);
	return nSize;
}

bool BaseGraph_DataBase::metallight::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf装不下
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, enable);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, pos);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, strength);

	return true;
}

bool BaseGraph_DataBase::metallight::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, enable);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, pos);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, strength);

	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::metallight::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleMetallight = new TiXmlElement("metallight");
		if (eleMetallight)
		{
			xmlDoc->LinkEndChild(eleMetallight);
			eleMetallight->SetAttribute("enable", enable);
			TinyXMLHelper::SetAttribute(eleMetallight, "pos_x", pos.x);
			TinyXMLHelper::SetAttribute(eleMetallight, "pos_y", pos.y);
			TinyXMLHelper::SetAttribute(eleMetallight, "pos_z", pos.z);
			for (int i = 0; i < 3; i++)
			{
				memset(szName, 0, MAX_PATH);
				sprintf_s(szName, 256, "strength%d", i);
				TinyXMLHelper::SetAttribute(eleMetallight, szName, strength[i]);
			}
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::metallight::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleMetallight = xmlDoc->FirstChildElement("metallight");
					if (eleMetallight)
					{
						eleMetallight->Attribute("highlight", &nIntValue);
						enable = nIntValue;
						TinyXMLHelper::Attribute(eleMetallight, "pos_x", pos.x);
						TinyXMLHelper::Attribute(eleMetallight, "pos_y", pos.y);
						TinyXMLHelper::Attribute(eleMetallight, "pos_z", pos.z);
						for (int i = 0; i < 3; i++)
						{
							memset(szName, 0, MAX_PATH);
							sprintf_s(szName, 256, "strength%d", i);
							TinyXMLHelper::Attribute(eleMetallight, szName, strength[i]);
						}
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BaseGraph_DataBase::metalinfo::metalinfo()
{
	Reset();
}

void BaseGraph_DataBase::metalinfo::Reset()
{
	highlight = 0;
	barc = 0;
	specular = 0;
	width = 0;
	height = 0;
	light[0].Reset();
	light[1].Reset();
	light[2].Reset();
	memset(&eyepos, 0, sizeof(eyepos));
}

int BaseGraph_DataBase::metalinfo::GetSerializeSize() const
{
	int nSize = 0;
	SERIALIZE_PARAM_GET_SIZE(nSize, highlight);
	SERIALIZE_PARAM_GET_SIZE(nSize, barc);
	SERIALIZE_PARAM_GET_SIZE(nSize, specular);
	SERIALIZE_PARAM_GET_SIZE(nSize, width);
	SERIALIZE_PARAM_GET_SIZE(nSize, height);
	SERIALIZE_PARAM_GET_SIZE_STU_ARR(nSize, light, 3);
	SERIALIZE_PARAM_GET_SIZE(nSize, eyepos);
	return nSize;
}

bool BaseGraph_DataBase::metalinfo::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf装不下
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, highlight);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, barc);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, specular);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, width);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, height);
	SERIALIZE_PARAM_TO_BUF_STU_ARR(pBuf, nSize, light, 3);
	SERIALIZE_PARAM_TO_BUF(pBuf, nSize, eyepos);
	return true;
}

bool BaseGraph_DataBase::metalinfo::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf装不下
		assert(false);
		return false;
	}
	nSize = 0;
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, highlight);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, barc);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, specular);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, width);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, height);
	SERIALIZE_BUF_TO_PARAM_STU_ARR(pBuf, nSize, light, 3);
	SERIALIZE_BUF_TO_PARAM(pBuf, nSize, eyepos);
	return true;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::metalinfo::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleMetalinfo = new TiXmlElement("metalinfo");
		if (eleMetalinfo)
		{
			xmlDoc->LinkEndChild(eleMetalinfo);
			TinyXMLHelper::SetAttribute(eleMetalinfo, "highlight", highlight);
			eleMetalinfo->SetAttribute("barc", barc);
			TinyXMLHelper::SetAttribute(eleMetalinfo, "specular", specular);
			TinyXMLHelper::SetAttribute(eleMetalinfo, "width", width);
			TinyXMLHelper::SetAttribute(eleMetalinfo, "height", height);
			for (int i = 0; i < 3; i++)
			{
				memset(szName, 0, MAX_PATH);
				sprintf_s(szName, 256, "light%d", i);
				TinyXMLHelper::SetAttribute(eleMetalinfo, szName, light[i].GetValueStr());
			}
			TinyXMLHelper::SetAttribute(eleMetalinfo, "eyepos_x", eyepos.x);
			TinyXMLHelper::SetAttribute(eleMetalinfo, "eyepos_y", eyepos.y);
			TinyXMLHelper::SetAttribute(eleMetalinfo, "eyepos_z", eyepos.z);
		}
		int nXMLSize = xmlDoc->SaveToString(NULL, 0);
		char * szXMLValue = new char[nXMLSize + 1];
		memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
		nXMLSize = xmlDoc->SaveToString(szXMLValue, 0);
		strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
		delete szXMLValue;
		szXMLValue = nullptr;
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::metalinfo::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	if (BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT) == strType)
	{
		if (!strValue.IsEmpty())
		{
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConvertCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleMetalinfo = xmlDoc->FirstChildElement("metalinfo");
					if (eleMetalinfo)
					{
						TinyXMLHelper::Attribute(eleMetalinfo, "highlight", highlight);
						eleMetalinfo->Attribute("barc", &barc);
						TinyXMLHelper::Attribute(eleMetalinfo, "specular", specular);
						TinyXMLHelper::Attribute(eleMetalinfo, "width", width);
						TinyXMLHelper::Attribute(eleMetalinfo, "height", height);
						for (int i = 0; i < 3; i++)
						{
							memset(szName, 0, MAX_PATH);
							sprintf_s(szName, 256, "light%d", i);
							CString strValue;
							TinyXMLHelper::Attribute(eleMetalinfo, szName, strValue);
							if (!strValue.IsEmpty())
							{
								light[i].RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), strValue);
							}
						}
						TinyXMLHelper::Attribute(eleMetalinfo, "eyepos_x", eyepos.x);
						TinyXMLHelper::Attribute(eleMetalinfo, "eyepos_y", eyepos.y);
						TinyXMLHelper::Attribute(eleMetalinfo, "eyepos_z", eyepos.z);
					}
					bRet = true;
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}

BASEGRAPH_DATABASE_API CString BaseGraph_DataBase::GetDateTypeValueStr(BaseGraph_DataBase::ParamValueDef emParamDef, const BaseGraph_Common::Value & value)
{
	//主要是为了处理stu，根据stu类型，转换为对应的stu对象然后调用其getvaluestr
	CString strRet = _T("");
	switch (emParamDef)
	{
	case ColorParam_Face_Stu:
	{
		BaseGraph_DataBase::faceinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Border_Stu:
	{
		BaseGraph_DataBase::borderinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Shadow_Stu:
	{
		BaseGraph_DataBase::shadowinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Cube_Stu:
	{
		BaseGraph_DataBase::cubeinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Bordercube_Stu:
	{
		BaseGraph_DataBase::cubeinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Bordershadow_Stu:
	{
		BaseGraph_DataBase::shadowinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Facelight_Stu:
	{
		BaseGraph_DataBase::lightinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Cubelight_Stu:
	{
		BaseGraph_DataBase::lightinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Channel_Stu:
	{
		BaseGraph_DataBase::channelinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Material_Stu:
	{
		BaseGraph_DataBase::materialinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Appendmaterial_Stu:
	{
		BaseGraph_DataBase::materialinfo stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	case ColorParam_Dyntex_Stu:
	{
		BaseGraph_DataBase::dynamictexture stu;
		value.ToStruct(stu);
		strRet = stu.GetValueStr();
	}
		break;
	default:
		strRet = value.GetValueStr();
		break;
	}
	return strRet;
}

BASEGRAPH_DATABASE_API bool BaseGraph_DataBase::RestoreDataTypeValueFromXML(BaseGraph_DataBase::ParamValueDef emParamDef, CString strValueType, CString strXML, BaseGraph_Common::Value & value)
{
	switch (emParamDef)
	{
	case ColorParam_Face_Stu:
	{
		BaseGraph_DataBase::faceinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Border_Stu:
	{
		BaseGraph_DataBase::borderinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Shadow_Stu:
	{
		BaseGraph_DataBase::shadowinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Cube_Stu:
	{
		BaseGraph_DataBase::cubeinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Bordercube_Stu:
	{
		BaseGraph_DataBase::cubeinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Bordershadow_Stu:
	{
		BaseGraph_DataBase::shadowinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Facelight_Stu:
	{
		BaseGraph_DataBase::lightinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Cubelight_Stu:
	{
		BaseGraph_DataBase::lightinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Channel_Stu:
	{
		BaseGraph_DataBase::channelinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Material_Stu:
	{
		BaseGraph_DataBase::materialinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Appendmaterial_Stu:
	{
		BaseGraph_DataBase::materialinfo stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	case ColorParam_Dyntex_Stu:
	{
		BaseGraph_DataBase::dynamictexture stu;
		stu.RecoverFromCString(strValueType, strXML);
		value.SetValue(stu);
	}
	break;
	default:
		value.RecoverFromCString(strValueType, strXML);
		break;
	}
	return true;
}
