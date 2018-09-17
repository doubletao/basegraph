#include "stdafx.h"
#include "Common\TinyXMLHelper.h"
#include "Common\CommonFunction.h"

TinyXMLHelper::TinyXMLHelper()
{
}


TinyXMLHelper::~TinyXMLHelper()
{
}

bool TinyXMLHelper::SetAttribute(TiXmlElement *& pElement, std::string cstrName, CString strValue)
{
	pElement->SetAttribute(cstrName, CommonFunction::ConverCStringToStdString(strValue));
	return true;
}

bool TinyXMLHelper::Attribute(TiXmlElement *& pElement, std::string cstrName, CString & strValue)
{
	const char * pcstr = pElement->Attribute(cstrName.c_str());
	if (pcstr)
	{
		strValue = CommonFunction::ConvertStdStringToCString(pcstr);
		return true;
	} 
	else
	{
		return false;
	}
}

bool TinyXMLHelper::SetAttribute(TiXmlElement *& pElement, std::string cstrName, float fValue)
{
	double dValue = fValue;
	pElement->SetDoubleAttribute(cstrName, dValue);
	return true;
}

bool TinyXMLHelper::Attribute(TiXmlElement *& pElement, std::string cstrName, float & fValue)
{
	double dValue = 0.0f;
	pElement->Attribute(cstrName, &dValue);
	fValue = static_cast<float>(dValue);
	return true;
}

bool TinyXMLHelper::SetAttribute(TiXmlElement *& pElement, std::string cstrName, DWORD dwValue)
{
	int nValue = static_cast<int>(dwValue);
	pElement->SetAttribute(cstrName, nValue);
	return true;
}

bool TinyXMLHelper::Attribute(TiXmlElement *& pElement, std::string cstrName, DWORD & dwValue)
{
	int nValue = 0;
	pElement->Attribute(cstrName, &nValue);
	return true;
}
