#include "stdafx.h"
#include "Common\TinyXMLHelper.h"
#include "Common\CommonFunction.h"
#include "Common\ColorDef.h"

TinyXMLHelper::TinyXMLHelper()
{
}


TinyXMLHelper::~TinyXMLHelper()
{
}

bool TinyXMLHelper::SetAttribute(TiXmlElement *& pElement, std::string cstrName, CString strValue)
{
	pElement->SetAttribute(cstrName, CommonFunction::ConvertCStringToStdString(strValue));
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
	int nA = COLORARGB_A(dwValue);
	int nR = COLORARGB_R(dwValue);
	int nG = COLORARGB_G(dwValue);
	int nB = COLORARGB_B(dwValue);
	char pszHexNum[16] = {0};
	sprintf_s(pszHexNum, 16, "0x%02X%02X%02X%02X", nA, nR, nG, nB);
	pElement->SetAttribute(cstrName, pszHexNum);
	return true;
}

bool TinyXMLHelper::Attribute(TiXmlElement *& pElement, std::string cstrName, DWORD & dwValue)
{
	CString strValue;
	const char * pcstr = pElement->Attribute(cstrName.c_str());
	if (pcstr)
	{
		int nA = 0;
		int nR = 0;
		int nG = 0;
		int nB = 0;
		sscanf_s(pcstr, "0x%02X%02X%02X%02X", &nA, &nR, &nG, &nB);
		dwValue = ARGBColor(nA, nR, nG, nB);
		return true;
	}
	else
	{
		return false;
	}
}
