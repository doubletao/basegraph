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
	TiXmlDocument xmlDoc;
	TiXmlHandle xmlDocHandle(&xmlDoc);

	int nXMLSize = xmlDoc.SaveToString(NULL, 0);
	char * szXMLValue = new char[nXMLSize + 1];
	memset(szXMLValue, 0, sizeof(char) * (nXMLSize + 1));
	nXMLSize = xmlDoc.SaveToString(szXMLValue, 0);
	strRet = CommonFunction::ConvertStdStringToCString(szXMLValue);
	delete szXMLValue;
	szXMLValue = nullptr;
	return strRet;
}

bool tag_common_color::RecoverFromCString(CString strType, CString strValue)
{
	bool bRet = false;
	return bRet;
}
