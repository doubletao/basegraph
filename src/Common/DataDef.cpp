#include "stdafx.h"
#include "DataDef.h"
#include <assert.h>

BaseGraph_Common::Value::Value()
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
}

BaseGraph_Common::Value::~Value()
{
	Reset();
}

void BaseGraph_Common::Value::Reset()
{
	m_emType = ValueType_ERR;
	m_nSize = 0;
	if (m_pBuf)
	{
		delete m_pBuf;
		m_pBuf = nullptr;
	}
}

CString BaseGraph_Common::Value::ToString()
{
	CString strRet = _T("");
	switch (m_emType)
	{
	case BaseGraph_Common::ValueType_ERR:
	{
		strRet.Format(_T("error"));
	}
		break;
	case BaseGraph_Common::ValueType_INT:
	{
		strRet.Format(_T("int:%d"), ToInt());
	}
		break;
	case BaseGraph_Common::ValueType_DWORD:
	{
		strRet.Format(_T("dword:%d"), ToDWORD());
	}
		break;
	case BaseGraph_Common::ValueType_ENUM:
	{
		strRet.Format(_T("enum:%d"), ToInt());
	}
		break;
	case BaseGraph_Common::ValueType_BOOL:
	{
		strRet.Format(_T("BOOL:%d"), ToBOOL());
	}
		break;
	case BaseGraph_Common::ValueType_FLOAT:
	{
		strRet.Format(_T("float:%f"), ToFloat());
	}
		break;
	case BaseGraph_Common::ValueType_ANGLE:
	{
		strRet.Format(_T("angle:%d"), ToInt());
	}
		break;
	case BaseGraph_Common::ValueType_TIME:
	{
		int nTmp = ToInt();
		int nFps = 25;
		int nWholeSec = nTmp / nFps;
		int nFrame = nTmp % nFps;
		int nHour = nWholeSec / 3600;
		int nMin = nWholeSec % 3600 / 60;
		int nSec = nWholeSec % 3600 % 60;
		strRet.Format(_T("frame:%d 25fps:%02d:%02d:%02d:%02d"), nTmp, nHour, nMin, nSec, nFrame);
	}
		break;
	case BaseGraph_Common::ValueType_TEXT:
	{
		strRet.Format(_T("CString:%s"), ToCString());
	}
		break;
	default:
		break;
	}
	return strRet;
}

bool BaseGraph_Common::Value::SetValue(int nValue)
{
	Reset();
	m_emType = ValueType_INT;
	m_nSize = sizeof(int);
	m_pBuf = new byte[m_nSize];
	memcpy(m_pBuf, &nValue, m_nSize);
	return true;
}

bool BaseGraph_Common::Value::SetValue(DWORD dwValue)
{
	Reset();
	m_emType = ValueType_DWORD;
	m_nSize = sizeof(DWORD);
	m_pBuf = new byte[m_nSize];
	memcpy(m_pBuf, &dwValue, m_nSize);
	return true;
}

bool BaseGraph_Common::Value::SetValue(int nValue, ValueType emType)
{
	Reset();
	m_emType = emType;
	m_nSize = sizeof(int);
	m_pBuf = new byte[m_nSize];
	memcpy(m_pBuf, &nValue, m_nSize);
	return true;
}

bool BaseGraph_Common::Value::SetValue(float fValue)
{
	Reset();
	m_emType = ValueType_FLOAT;
	m_nSize = sizeof(float);
	m_pBuf = new byte[m_nSize];
	memcpy(m_pBuf, &fValue, m_nSize);
	return true;
}

bool BaseGraph_Common::Value::SetValue(CString strValue)
{
	Reset();
	m_emType = ValueType_TEXT;
	m_nSize = (strValue.GetLength() + 1) * sizeof(TCHAR);
	m_pBuf = new byte[m_nSize];
	memset(m_pBuf, 0, m_nSize);
	memcpy(m_pBuf, strValue.GetBuffer(), m_nSize);
	return true;
}

int BaseGraph_Common::Value::ToInt()
{
	int nTmp = -1;
	if (m_emType == ValueType_INT
		|| m_emType == ValueType_INT
		|| m_emType == ValueType_ENUM
		|| m_emType == ValueType_ANGLE
		|| m_emType == ValueType_TIME)
	{
		memcpy(&nTmp, m_pBuf, m_nSize);
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
	}
	return nTmp;
}

DWORD BaseGraph_Common::Value::ToDWORD()
{
	DWORD dwTmp = -1;
	if (m_emType == ValueType_DWORD)
	{
		memcpy(&dwTmp, m_pBuf, m_nSize);
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
	}
	return dwTmp;
}

BOOL BaseGraph_Common::Value::ToBOOL()
{
	BOOL bTmp = 0;
	if (m_emType == ValueType_BOOL)
	{
		memcpy(&bTmp, m_pBuf, m_nSize);
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
	}
	return bTmp;
}

float BaseGraph_Common::Value::ToFloat()
{
	float fTmp = 0.0f;
	if (m_emType == ValueType_FLOAT)
	{
		memcpy(&fTmp, m_pBuf, m_nSize);
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
	}
	return fTmp;
}

CString BaseGraph_Common::Value::ToCString()
{
	CString strTmp = _T("");
	if (m_emType == ValueType_TEXT)
	{
		TCHAR * szBuf = new TCHAR[m_nSize / sizeof(TCHAR)];
		memcpy(szBuf, m_pBuf, m_nSize);
		strTmp = szBuf;
		delete szBuf;
		szBuf = nullptr;
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
	}
	return strTmp;
}
