#include "stdafx.h"
#include "Common\ColorDef.h"
#include <assert.h>
#include "Common\tinyxml.h"
#include "Common\CommonFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
inline tag_common_color::tag_common_color()
{
	Reset();
}

void tag_common_color::Reset()
{
	color = 0;
	type = BaseGraphCOLOR_HOMO;
	memset(&gradual, 0, sizeof(gradual));
	memset(&imgtex, 0, sizeof(imgtex));
	memset(&subtheme, 0, sizeof(subtheme));
	memset(&theme, 0, sizeof(theme));
	memset(&type, 0, sizeof(type));
}

int tag_common_color::GetSerializeSize() const
{
	return sizeof(tag_common_color);
}

bool tag_common_color::GetSerializeBuf(byte * pBuf, int nBufSize) const
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	memcpy(pBuf, this, nSize);
	return true;
}

bool tag_common_color::RecoverFromBuf(byte * pBuf, int nBufSize)
{
	int nSize = GetSerializeSize();
	if (nSize > nBufSize)
	{
		//给的buf不对
		assert(false);
		return false;
	}
	memcpy(this, pBuf, nSize);
	return false;
}

CString tag_common_color::GetValueStr()
{
	CString strRet;
	TiXmlDocument * xmlDoc = new TiXmlDocument;
	TiXmlHandle xmlDocHandle(xmlDoc);
	if (xmlDoc)
	{
		TiXmlElement * eleCommon_color = new TiXmlElement("common_color");
		if (eleCommon_color)
		{
			xmlDoc->LinkEndChild(eleCommon_color);
			eleCommon_color->SetAttribute("type", type);
			TiXmlElement * eleStruct = new TiXmlElement("struct");
			if (eleStruct)
			{
				eleCommon_color->LinkEndChild(eleStruct);
				eleStruct->SetAttribute("color", color);
				TiXmlElement * eleImgtex = new TiXmlElement("imgtex");
				if (eleImgtex)
				{
					eleStruct->LinkEndChild(eleImgtex);
					eleImgtex->SetAttribute("name", CommonFunction::ConverCStringToStdString(imgtex.name).c_str());
					eleImgtex->SetAttribute("fill", imgtex.fill);
					eleImgtex->SetAttribute("align", imgtex.align);
				}
				TiXmlElement * eleGradual = new TiXmlElement("gradual");
				if (eleGradual)
				{
					eleStruct->LinkEndChild(eleGradual);
					char szName[MAX_PATH];
					for (size_t i = 0; i < 8; i++)
					{
						memset(szName, 0, MAX_PATH);
						sprintf_s(szName, 256, "color%d", i);
						eleGradual->SetAttribute(szName, gradual.color[i]);
					}
					eleGradual->SetAttribute("ncount", gradual.ncount);
					for (size_t i = 0; i < 8; i++)
					{
						memset(szName, 0, MAX_PATH);
						sprintf_s(szName, 256, "pos%d", i);
						eleGradual->SetAttribute(szName, gradual.pos[i]);
					}
					for (size_t i = 0; i < 7; i++)
					{
						memset(szName, 0, MAX_PATH);
						sprintf_s(szName, 256, "mid%d", i);
						eleGradual->SetAttribute(szName, gradual.mid[i]);
					}
					TiXmlElement * eleStruct = new TiXmlElement("struct");
					if (eleStruct)
					{
						eleGradual->LinkEndChild(eleStruct);
						TiXmlElement * eleLinear = new TiXmlElement("linear");
						if (eleLinear)
						{
							eleStruct->LinkEndChild(eleLinear);
							eleLinear->SetAttribute("frad", gradual.linear.frad);
							eleLinear->SetAttribute("repeat", gradual.linear.repeat);
							eleLinear->SetAttribute("style", gradual.linear.style);
						}
						TiXmlElement * eleCenter = new TiXmlElement("center");
						if (eleCenter)
						{
							eleStruct->LinkEndChild(eleCenter);
							eleCenter->SetAttribute("centerx", gradual.center.centerx);
							eleCenter->SetAttribute("centery", gradual.center.centery);
							eleCenter->SetAttribute("repeat", gradual.center.repeat);
							eleCenter->SetAttribute("style", gradual.center.style);
							eleCenter->SetAttribute("nvertex", gradual.center.nvertex);
							for (size_t i = 0; i < 8; i++)
							{
								memset(szName, 0, MAX_PATH);
								sprintf_s(szName, 256, "xpt%d", i);
								eleStruct->SetAttribute(szName, gradual.center.xpt[i]);
							}
							for (size_t i = 0; i < 8; i++)
							{
								memset(szName, 0, MAX_PATH);
								sprintf_s(szName, 256, "ypt%d", i);
								eleStruct->SetAttribute(szName, gradual.center.ypt[i]);
							}
						}
						TiXmlElement * eleOutline = new TiXmlElement("outline");
						if (eleOutline)
						{
							eleStruct->LinkEndChild(eleOutline);
							eleOutline->SetAttribute("centerx", gradual.outline.centerx);
							eleOutline->SetAttribute("centery", gradual.outline.centery);
							eleOutline->SetAttribute("repeat", gradual.outline.repeat);
							eleOutline->SetAttribute("style", gradual.outline.style);
							for (size_t i = 0; i < 8; i++)
							{
								memset(szName, 0, MAX_PATH);
								sprintf_s(szName, 256, "xpt%d", i);
								eleOutline->SetAttribute(szName, gradual.outline.xpt[i]);
							}
							for (size_t i = 0; i < 8; i++)
							{
								memset(szName, 0, MAX_PATH);
								sprintf_s(szName, 256, "ypt%d", i);
								eleOutline->SetAttribute(szName, gradual.outline.ypt[i]);
							}
						}
						TiXmlElement * eleMultipt = new TiXmlElement("multipt");
						if (eleMultipt)
						{
							eleStruct->LinkEndChild(eleMultipt);
							for (size_t i = 0; i < 8; i++)
							{
								memset(szName, 0, MAX_PATH);
								sprintf_s(szName, 256, "xpt%d", i);
								eleMultipt->SetAttribute(szName, gradual.multipt.xpt[i]);
							}
							for (size_t i = 0; i < 8; i++)
							{
								memset(szName, 0, MAX_PATH);
								sprintf_s(szName, 256, "ypt%d", i);
								eleMultipt->SetAttribute(szName, gradual.multipt.ypt[i]);
							}
						}
						TiXmlElement * eleTemplet = new TiXmlElement("templet");
						if (eleTemplet)
						{
							eleStruct->LinkEndChild(eleTemplet);
							eleTemplet->SetAttribute("name", CommonFunction::ConverCStringToStdString(gradual.templet.name).c_str());
						}
					}
					eleGradual->SetAttribute("animation", gradual.animation);
					for (size_t i = 0; i < GRAD_ANI_PARAM_COUNT; i++)
					{
						memset(szName, 0, MAX_PATH);
						sprintf_s(szName, 256, "aniparam%d", i);
						eleGradual->SetAttribute(szName, gradual.aniparam[i]);
					}
				}
			}
			eleCommon_color->SetAttribute("theme", theme);
			eleCommon_color->SetAttribute("subtheme", subtheme);
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

bool tag_common_color::RecoverFromCString(CString strType, CString strValue)
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
					TiXmlElement * eleCommon_color = docHandle.FirstChild("common_color").ToElement();
					if (eleCommon_color)
					{
						eleCommon_color->Attribute("type", &nIntValue);
						type = nIntValue;
						TiXmlElement * eleStruct = eleCommon_color->FirstChildElement("struct");
						if (eleStruct)
						{
							eleStruct->Attribute("color", &nIntValue);
							color = nIntValue;
							TiXmlElement * eleImgtex = eleStruct->FirstChildElement("imgtex");
							if (eleImgtex)
							{
								std::string cstrName = eleImgtex->Attribute("name");
								CString strName = CommonFunction::ConvertStdStringToCString(cstrName);
								memcpy(imgtex.name, strName.GetBuffer(), strName.GetLength() * sizeof(TCHAR));
								imgtex.name[strName.GetLength()] = _T('\0');
								eleImgtex->Attribute("fill", &nIntValue);
								imgtex.fill = nIntValue;
								eleImgtex->Attribute("align", &nIntValue);
								imgtex.align = nIntValue;
							}
							TiXmlElement * eleGradual = eleStruct->FirstChildElement("gradual");
							if (eleGradual)
							{
								char szName[MAX_PATH];
								for (size_t i = 0; i < 8; i++)
								{
									memset(szName, 0, MAX_PATH);
									sprintf_s(szName, 256, "color%d", i);
									eleGradual->Attribute(szName, &nIntValue);
									gradual.color[i] = nIntValue;
								}
								eleGradual->Attribute("ncount", &gradual.ncount);
								for (size_t i = 0; i < 8; i++)
								{
									memset(szName, 0, MAX_PATH);
									sprintf_s(szName, 256, "pos%d", i);
									eleGradual->Attribute(szName, &dDoubleValue);
									gradual.pos[i] = dDoubleValue;
								}
								for (size_t i = 0; i < 7; i++)
								{
									memset(szName, 0, MAX_PATH);
									sprintf_s(szName, 256, "mid%d", i);
									eleGradual->Attribute(szName, &dDoubleValue);
									gradual.mid[i] = dDoubleValue;
								}
								TiXmlElement * eleStruct = eleGradual->FirstChildElement("struct");
								if (eleStruct)
								{
									TiXmlElement * eleLinear = eleStruct->FirstChildElement("linear");
									if (eleLinear)
									{
										eleLinear->Attribute("frad", &dDoubleValue);
										gradual.linear.frad = dDoubleValue;
										eleLinear->Attribute("repeat", &nIntValue);
										gradual.linear.repeat = nIntValue;
										eleLinear->Attribute("style", &nIntValue);
										gradual.linear.style = nIntValue;
									}
									TiXmlElement * eleCenter = eleStruct->FirstChildElement("center");
									if (eleCenter)
									{
										eleCenter->Attribute("centerx", &dDoubleValue);
										gradual.center.centerx = dDoubleValue;
										eleCenter->Attribute("centery", &dDoubleValue);
										gradual.center.centery = dDoubleValue;
										eleCenter->Attribute("repeat", &nIntValue);
										gradual.center.repeat = nIntValue;
										eleCenter->Attribute("style", &nIntValue);
										gradual.center.style = nIntValue;
										eleCenter->Attribute("nvertex", &gradual.center.nvertex);
										for (size_t i = 0; i < 8; i++)
										{
											memset(szName, 0, MAX_PATH);
											sprintf_s(szName, 256, "xpt%d", i);
											eleStruct->Attribute(szName, &dDoubleValue);
											gradual.center.xpt[i] = dDoubleValue;
										}
										for (size_t i = 0; i < 8; i++)
										{
											memset(szName, 0, MAX_PATH);
											sprintf_s(szName, 256, "ypt%d", i);
											eleStruct->Attribute(szName, &dDoubleValue);
											gradual.center.ypt[i] = dDoubleValue;
										}
									}
									TiXmlElement * eleOutline = eleStruct->FirstChildElement("outline");
									if (eleOutline)
									{
										eleOutline->Attribute("centerx", &dDoubleValue);
										gradual.outline.centerx = dDoubleValue;
										eleOutline->Attribute("centery", &dDoubleValue);
										gradual.outline.centery = dDoubleValue;
										eleOutline->Attribute("repeat", &nIntValue);
										gradual.outline.repeat = nIntValue;
										eleOutline->Attribute("style", &nIntValue);
										gradual.outline.style = nIntValue;
										for (size_t i = 0; i < 8; i++)
										{
											memset(szName, 0, MAX_PATH);
											sprintf_s(szName, 256, "xpt%d", i);
											eleOutline->Attribute(szName, &dDoubleValue);
											gradual.outline.xpt[i] = dDoubleValue;
										}
										for (size_t i = 0; i < 8; i++)
										{
											memset(szName, 0, MAX_PATH);
											sprintf_s(szName, 256, "ypt%d", i);
											eleOutline->Attribute(szName, &dDoubleValue);
											gradual.outline.ypt[i] = dDoubleValue;
										}
									}
									TiXmlElement * eleMultipt = eleStruct->FirstChildElement("multipt");
									if (eleMultipt)
									{
										for (size_t i = 0; i < 8; i++)
										{
											memset(szName, 0, MAX_PATH);
											sprintf_s(szName, 256, "xpt%d", i);
											eleMultipt->Attribute(szName, &dDoubleValue);
											gradual.multipt.xpt[i] = dDoubleValue;
										}
										for (size_t i = 0; i < 8; i++)
										{
											memset(szName, 0, MAX_PATH);
											sprintf_s(szName, 256, "ypt%d", i);
											eleMultipt->Attribute(szName, &dDoubleValue);
											gradual.multipt.ypt[i] = dDoubleValue;
										}
									}
									TiXmlElement * eleTemplet = eleStruct->FirstChildElement("templet");
									if (eleTemplet)
									{
										std::string cstrName = eleTemplet->Attribute("name");
										CString strName = CommonFunction::ConvertStdStringToCString(cstrName);
										memcpy(gradual.templet.name, strName.GetBuffer(), strName.GetLength() * sizeof(TCHAR));
									}
								}
								eleGradual->Attribute("animation", &nIntValue);
								gradual.animation = nIntValue;
								for (size_t i = 0; i < GRAD_ANI_PARAM_COUNT; i++)
								{
									memset(szName, 0, MAX_PATH);
									sprintf_s(szName, 256, "aniparam%d", i);
									eleGradual->Attribute(szName, &dDoubleValue);
									gradual.aniparam[i] = dDoubleValue;
								}
							}
						}
						eleCommon_color->Attribute("theme", &nIntValue);
						theme = nIntValue;
						eleCommon_color->Attribute("subtheme", &nIntValue);
						subtheme = nIntValue;
						bRet = true;
					}
				}
				delete xmlDoc;
				xmlDoc = nullptr;
			}
		}
	}
	return bRet;
}
