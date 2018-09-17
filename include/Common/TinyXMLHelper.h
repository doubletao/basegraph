#pragma once
#include "Common\tinyxml.h"

//tinyXMLµÄ¸¨ÖúÀà
class BASEGRAPH_COMMON_API TinyXMLHelper
{
private:
	TinyXMLHelper();
	~TinyXMLHelper();
public:
	static bool SetAttribute(TiXmlElement *& pElement, std::string cstrName, CString strValue);
	static bool Attribute(TiXmlElement *& pElement, std::string cstrName, CString & strValue);
	static bool SetAttribute(TiXmlElement *& pElement, std::string cstrName, float fValue);
	static bool Attribute(TiXmlElement *& pElement, std::string cstrName, float & fValue);
	static bool SetAttribute(TiXmlElement *& pElement, std::string cstrName, DWORD dwValue);
	static bool Attribute(TiXmlElement *& pElement, std::string cstrName, DWORD & dwValue);
};

