#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
#include "..\..\include\DataBase\DataTypeDefDetail.h"
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleArtinfo = new TiXmlElement("artinfo");
		if (eleArtinfo)
		{
			xmlDoc->LinkEndChild(eleArtinfo);
			eleArtinfo->SetAttribute("type", type);
			eleArtinfo->SetAttribute("cv_height", cv_height);
			eleArtinfo->SetAttribute("cu_angle", cu_angle);
			eleArtinfo->SetAttribute("soft", soft);
			eleArtinfo->SetAttribute("color", color);
			eleArtinfo->SetAttribute("metal", CommonFunction::ConverCStringToStdString(metal.GetValueStr()).c_str());
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
			double dDoubleValue = 0.0;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleArtinfo = xmlDoc->FirstChildElement("artinfo");
					if (eleArtinfo)
					{
						eleArtinfo->Attribute("type", &nIntValue);
						type = (GRAPH_ART_PROC_TYPE)nIntValue;
						eleArtinfo->Attribute("cv_height", &nIntValue);
						cv_height = nIntValue;
						eleArtinfo->Attribute("cu_angle", &dDoubleValue);
						cu_angle = dDoubleValue;
						eleArtinfo->Attribute("soft", &soft);
						eleArtinfo->Attribute("color", &nIntValue);
						color = nIntValue;
						std::string cstrValue = eleArtinfo->Attribute("metal");
						metal.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrValue));
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleFaceinfo = new TiXmlElement("faceinfo");
		if (eleFaceinfo)
		{
			xmlDoc->LinkEndChild(eleFaceinfo);
			eleFaceinfo->SetAttribute("disable", disable);
			eleFaceinfo->SetAttribute("soft", soft);
			eleFaceinfo->SetAttribute("effect_type", effect_type);
			eleFaceinfo->SetAttribute("effect_width", effect_width);
			eleFaceinfo->SetAttribute("avcolor", avcolor);
			eleFaceinfo->SetAttribute("art", CommonFunction::ConverCStringToStdString(art.GetValueStr()).c_str());
			eleFaceinfo->SetAttribute("color", CommonFunction::ConverCStringToStdString(color.GetValueStr()).c_str());
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
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleFaceinfo = xmlDoc->FirstChildElement("faceinfo");
					if (eleFaceinfo)
					{
						eleFaceinfo->Attribute("disable", &disable);
						eleFaceinfo->Attribute("soft", &soft);
						eleFaceinfo->Attribute("effect_type", &effect_type);
						eleFaceinfo->Attribute("effect_width", &effect_width);
						eleFaceinfo->Attribute("avcolor", &nIntValue);
						avcolor = nIntValue;
						std::string cstrArt = eleFaceinfo->Attribute("art");
						art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrArt));
						std::string cstrColor = eleFaceinfo->Attribute("color");
						color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrColor));
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleBorderinfo = new TiXmlElement("borderinfo");
		if (eleBorderinfo)
		{
			xmlDoc->LinkEndChild(eleBorderinfo);
			eleBorderinfo->SetAttribute("disable", disable);
			eleBorderinfo->SetAttribute("soft", soft);
			eleBorderinfo->SetAttribute("linestyle", linestyle);
			eleBorderinfo->SetAttribute("width", width);
			eleBorderinfo->SetAttribute("interval", interval);
			eleBorderinfo->SetAttribute("effect_type", effect_type);
			eleBorderinfo->SetAttribute("effect_width", effect_width);
			eleBorderinfo->SetAttribute("avcolor", avcolor);
			eleBorderinfo->SetAttribute("art", CommonFunction::ConverCStringToStdString(art.GetValueStr()).c_str());
			eleBorderinfo->SetAttribute("color", CommonFunction::ConverCStringToStdString(color.GetValueStr()).c_str());
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
			int nIntValue = 0;
			double dDoubleValue = 0.0;
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleBorderinfo = xmlDoc->FirstChildElement("borderinfo");
					if (eleBorderinfo)
					{
						eleBorderinfo->Attribute("disable", &disable);
						eleBorderinfo->Attribute("soft", &soft);
						eleBorderinfo->Attribute("linestyle", &linestyle);
						eleBorderinfo->Attribute("width", &dDoubleValue);
						width = dDoubleValue;
						eleBorderinfo->Attribute("interval", &dDoubleValue);
						interval = dDoubleValue;
						eleBorderinfo->Attribute("effect_type", &nIntValue);
						effect_type = nIntValue;
						eleBorderinfo->Attribute("effect_width", &effect_width);
						eleBorderinfo->Attribute("avcolor", &nIntValue);
						avcolor = nIntValue;
						std::string cstrArt = eleBorderinfo->Attribute("art");
						art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrArt));
						std::string cstrColor = eleBorderinfo->Attribute("color");
						color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrColor));
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleShadowinfo = new TiXmlElement("shadowinfo");
		if (eleShadowinfo)
		{
			xmlDoc->LinkEndChild(eleShadowinfo);
			eleShadowinfo->SetAttribute("disable", disable);
			eleShadowinfo->SetAttribute("soft", soft);
			eleShadowinfo->SetAttribute("offsetx", offsetx);
			eleShadowinfo->SetAttribute("offsety", offsety);
			eleShadowinfo->SetAttribute("effect_type", effect_type);
			eleShadowinfo->SetAttribute("effect_fxtitle", effect_fxtitle);
			eleShadowinfo->SetAttribute("effect_fyscale", effect_fyscale);
			eleShadowinfo->SetAttribute("effect_fadeout", effect_fadeout);
			eleShadowinfo->SetAttribute("effect_style", effect_style);
			eleShadowinfo->SetAttribute("avcolor", avcolor);
			eleShadowinfo->SetAttribute("art", CommonFunction::ConverCStringToStdString(art.GetValueStr()).c_str());
			eleShadowinfo->SetAttribute("color", CommonFunction::ConverCStringToStdString(color.GetValueStr()).c_str());
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
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleShadowinfo = xmlDoc->FirstChildElement("shadowinfo");
					if (eleShadowinfo)
					{
						eleShadowinfo->Attribute("disable", &disable);
						eleShadowinfo->Attribute("soft", &soft);
						eleShadowinfo->Attribute("offsetx", &offsetx);
						eleShadowinfo->Attribute("offsety", &offsety);
						eleShadowinfo->Attribute("effect_type", &nIntValue);
						effect_type = nIntValue;
						eleShadowinfo->Attribute("effect_fxtitle", &dDoubleValue);
						effect_fxtitle = dDoubleValue;
						eleShadowinfo->Attribute("effect_fyscale", &dDoubleValue);
						effect_fyscale = dDoubleValue;
						eleShadowinfo->Attribute("effect_fadeout", &dDoubleValue);
						effect_fadeout = dDoubleValue;
						eleShadowinfo->Attribute("effect_style", &nIntValue);
						effect_style = nIntValue;
						eleShadowinfo->Attribute("avcolor", &nIntValue);
						avcolor = nIntValue;
						std::string cstrArt = eleShadowinfo->Attribute("art");
						art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrArt));
						std::string cstrColor = eleShadowinfo->Attribute("color");
						color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrColor));
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleCubeinfo = new TiXmlElement("cubeinfo");
		if (eleCubeinfo)
		{
			xmlDoc->LinkEndChild(eleCubeinfo);
			eleCubeinfo->SetAttribute("disable", disable);
			eleCubeinfo->SetAttribute("width", width);
			eleCubeinfo->SetAttribute("angle", angle);
			eleCubeinfo->SetAttribute("effect_type", effect_type);
			eleCubeinfo->SetAttribute("effect_width", effect_width);
			eleCubeinfo->SetAttribute("avcolor", avcolor);
			eleCubeinfo->SetAttribute("art", CommonFunction::ConverCStringToStdString(art.GetValueStr()).c_str());
			eleCubeinfo->SetAttribute("color", CommonFunction::ConverCStringToStdString(color.GetValueStr()).c_str());
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
			TiXmlDocument* xmlDoc = new TiXmlDocument;
			if (xmlDoc)
			{
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleCubeinfo = xmlDoc->FirstChildElement("cubeinfo");
					if (eleCubeinfo)
					{
						eleCubeinfo->Attribute("disable", &disable);
						eleCubeinfo->Attribute("width", &dDoubleValue);
						width = dDoubleValue;
						eleCubeinfo->Attribute("angle", &dDoubleValue);
						angle = dDoubleValue;
						eleCubeinfo->Attribute("effect_type", &effect_type);
						eleCubeinfo->Attribute("effect_width", &effect_width);
						eleCubeinfo->Attribute("avcolor", &nIntValue);
						avcolor = nIntValue;
						std::string cstrArt = eleCubeinfo->Attribute("art");
						art.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrArt));
						std::string cstrColor = eleCubeinfo->Attribute("color");
						color.RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrColor));
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleLightinfo = new TiXmlElement("lightinfo");
		if (eleLightinfo)
		{
			xmlDoc->LinkEndChild(eleLightinfo);
			eleLightinfo->SetAttribute("ambience", ambience);
			eleLightinfo->SetAttribute("ambience_degree", ambience_degree);
			eleLightinfo->SetAttribute("reflectcolor", reflectcolor);
			eleLightinfo->SetAttribute("dwReverse1", dwReverse1);
			eleLightinfo->SetAttribute("dwReverse2", dwReverse2);
			eleLightinfo->SetAttribute("szPath", CommonFunction::ConverCStringToStdString(szPath).c_str());
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
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
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
						cstrValue = eleLightinfo->Attribute("szPath");
						if (nullptr != cstrValue)
						{
							CString strValue = CommonFunction::ConvertStdStringToCString(cstrValue);
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleChannelinfo = new TiXmlElement("channelinfo");
		if (eleChannelinfo)
		{
			xmlDoc->LinkEndChild(eleChannelinfo);
			eleChannelinfo->SetAttribute("szPath", CommonFunction::ConverCStringToStdString(szPath).c_str());
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
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleChannelinfo = new TiXmlElement("channelinfo");
		if (eleChannelinfo)
		{
			xmlDoc->LinkEndChild(eleChannelinfo);
			eleChannelinfo->SetAttribute("szPath", CommonFunction::ConverCStringToStdString(szPath).c_str());
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
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleDynamictexture = new TiXmlElement("dynamictexture");
		if (eleDynamictexture)
		{
			xmlDoc->LinkEndChild(eleDynamictexture);
			eleDynamictexture->SetAttribute("szPath", CommonFunction::ConverCStringToStdString(szPath).c_str());
			eleDynamictexture->SetAttribute("disable", disable);
			eleDynamictexture->SetAttribute("speed", speed);
			eleDynamictexture->SetAttribute("offsetx", offsetx);
			eleDynamictexture->SetAttribute("offsety", offsety);
			eleDynamictexture->SetAttribute("vRunDirx", vRunDirx);
			eleDynamictexture->SetAttribute("vRunDiry", vRunDiry);
			eleDynamictexture->SetAttribute("scalex", scalex);
			eleDynamictexture->SetAttribute("scaley", scaley);
			eleDynamictexture->SetAttribute("centerx", centerx);
			eleDynamictexture->SetAttribute("centery", centery);
			eleDynamictexture->SetAttribute("rotate", rotate);
			eleDynamictexture->SetAttribute("alpha", alpha);
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
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
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
						eleDynamictexture->Attribute("disable", &nIntValue);
						disable = nIntValue;
						eleDynamictexture->Attribute("speed", &dDoubleValue);
						speed = dDoubleValue;
						eleDynamictexture->Attribute("offsetx", &dDoubleValue);
						offsetx = dDoubleValue;
						eleDynamictexture->Attribute("offsety", &dDoubleValue);
						offsety = dDoubleValue;
						eleDynamictexture->Attribute("vRunDirx", &dDoubleValue);
						vRunDirx = dDoubleValue;
						eleDynamictexture->Attribute("vRunDiry", &dDoubleValue);
						vRunDiry = dDoubleValue;
						eleDynamictexture->Attribute("scalex", &dDoubleValue);
						scalex = dDoubleValue;
						eleDynamictexture->Attribute("scaley", &dDoubleValue);
						scaley = dDoubleValue;
						eleDynamictexture->Attribute("centerx", &dDoubleValue);
						centerx = dDoubleValue;
						eleDynamictexture->Attribute("centery", &dDoubleValue);
						centery = dDoubleValue;
						eleDynamictexture->Attribute("rotate", &dDoubleValue);
						rotate = dDoubleValue;
						eleDynamictexture->Attribute("alpha", &dDoubleValue);
						alpha = dDoubleValue;
						eleDynamictexture->Attribute("bRepeat", &nIntValue);
						bRepeat = nIntValue;
						eleDynamictexture->Attribute("bUseAlpha", &nIntValue);
						bUseAlpha = nIntValue;
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleMetallight = new TiXmlElement("metallight");
		if (eleMetallight)
		{
			xmlDoc->LinkEndChild(eleMetallight);
			eleMetallight->SetAttribute("enable", enable);
			eleMetallight->SetAttribute("pos_x", pos.x);
			eleMetallight->SetAttribute("pos_y", pos.y);
			eleMetallight->SetAttribute("pos_z", pos.z);
			for (size_t i = 0; i < 3; i++)
			{
				memset(szName, 0, MAX_PATH);
				sprintf_s(szName, 256, "strength%d", i);
				eleMetallight->SetAttribute(szName, strength[i]);
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
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleMetallight = xmlDoc->FirstChildElement("metallight");
					if (eleMetallight)
					{
						eleMetallight->Attribute("highlight", &nIntValue);
						enable = nIntValue;
						eleMetallight->Attribute("pos_x", &dDoubleValue);
						pos.x = dDoubleValue;
						eleMetallight->Attribute("pos_y", &dDoubleValue);
						pos.y = dDoubleValue;
						eleMetallight->Attribute("pos_z", &dDoubleValue);
						pos.z = dDoubleValue;
						for (size_t i = 0; i < 3; i++)
						{
							memset(szName, 0, MAX_PATH);
							sprintf_s(szName, 256, "strength%d", i);
							eleMetallight->Attribute(szName, &dDoubleValue);
							strength[i] = dDoubleValue;
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
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		char szName[MAX_PATH];
		TiXmlElement * eleMetalinfo = new TiXmlElement("metalinfo");
		if (eleMetalinfo)
		{
			xmlDoc->LinkEndChild(eleMetalinfo);
			eleMetalinfo->SetAttribute("highlight", highlight);
			eleMetalinfo->SetAttribute("barc", barc);
			eleMetalinfo->SetAttribute("specular", specular);
			eleMetalinfo->SetAttribute("width", width);
			eleMetalinfo->SetAttribute("height", height);
			for (size_t i = 0; i < 3; i++)
			{
				memset(szName, 0, MAX_PATH);
				sprintf_s(szName, 256, "light%d", i);
				eleMetalinfo->SetAttribute(szName, CommonFunction::ConverCStringToStdString(light[i].GetValueStr()).c_str());
			}
			eleMetalinfo->SetAttribute("eyepos_x", eyepos.x);
			eleMetalinfo->SetAttribute("eyepos_y", eyepos.y);
			eleMetalinfo->SetAttribute("eyepos_z", eyepos.z);
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
				if (xmlDoc->Parse(CommonFunction::ConverCStringToStdString(strValue).c_str()))
				{
					TiXmlHandle docHandle(xmlDoc);
					char szName[MAX_PATH];
					TiXmlElement * eleMetalinfo = xmlDoc->FirstChildElement("metalinfo");
					if (eleMetalinfo)
					{
						eleMetalinfo->Attribute("highlight", &nIntValue);
						highlight = nIntValue;
						eleMetalinfo->Attribute("barc", &nIntValue);
						barc = nIntValue;
						eleMetalinfo->Attribute("specular", &dDoubleValue);
						specular = dDoubleValue;
						eleMetalinfo->Attribute("width", &nIntValue);
						width = nIntValue;
						eleMetalinfo->Attribute("height", &nIntValue);
						height = nIntValue;
						for (size_t i = 0; i < 3; i++)
						{
							memset(szName, 0, MAX_PATH);
							sprintf_s(szName, 256, "light%d", i);
							std::string cstrVlue = eleMetalinfo->Attribute(szName);
							light[i].RecoverFromCString(BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType_STRUCT), CommonFunction::ConvertStdStringToCString(cstrVlue));
						}
						eleMetalinfo->Attribute("eyepos_x", &dDoubleValue);
						eyepos.x = dDoubleValue;
						eleMetalinfo->Attribute("eyepos_y", &dDoubleValue);
						eyepos.y = dDoubleValue;
						eleMetalinfo->Attribute("eyepos_z", &dDoubleValue);
						eyepos.z = dDoubleValue;
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
