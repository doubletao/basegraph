#pragma once
#include <assert.h>

namespace BaseGraph_Common
{
	struct ValueAbleInterface
	{
		virtual int GetSerializeSize() const = 0;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const = 0;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize) = 0;
		virtual CString GetValueStr() = 0;
		virtual bool RecoverFromCString(CString strType, CString strValue) = 0;
	};

	enum ValueType
	{
		ValueType_ERR = 0,
		ValueType_INT,
		ValueType_DWORD,
		ValueType_ENUM,
		ValueType_BOOL,
		ValueType_FLOAT,
		ValueType_ANGLE,
		ValueType_TIME,
		ValueType_TEXT,
		ValueType_STRUCT,
	};

	extern inline CString GetValueTypeStr(ValueType emValuleType);

	class Value
	{
	private:
		ValueType m_emType;
		size_t m_nSize;
#ifdef DEBUG
		union
		{
			int * pnValue;
			DWORD * pdwValue;
			TCHAR * pszValue;
			float * pfValue;
			byte * m_pBuf;
		};
#else
		byte * m_pBuf;
#endif // DEBUG
	public:
		inline Value();
		inline Value(int nValue);
		inline Value(DWORD dwValue);
		inline Value(int nValue, ValueType emType);//对于枚举，角度，BOOL，时间等类型，内部其实都是int，但需要指定
		inline Value(float fValue);
		inline Value(CString strValue);
		inline Value(const ValueAbleInterface & stuValue);
		inline Value(const Value & other);
		inline Value & operator = (const Value & other);
		inline ~Value();
		inline void Reset();
		inline CString ToDebugString();

		inline bool SetValue(int nValue);
		inline bool SetValue(DWORD dwValue);
		inline bool SetValue(int nValue, ValueType emType);//对于枚举，角度，BOOL，时间等类型，内部其实都是int，但需要指定
		inline bool SetValue(float fValue);
		inline bool SetValue(CString strValue);
		inline bool SetValue(const ValueAbleInterface & stuValue);

		inline int ToInt() const;
		inline DWORD ToDWORD() const;
		inline BOOL ToBOOL() const;
		inline float ToFloat() const;
		inline CString ToCString() const;
		inline bool ToStruct(ValueAbleInterface & stuValue) const;

		inline bool Serialize(CArchive & archive);
		
		inline ValueType GetValueType();
		//对于普通类型，ValueAbleInterface给空就行，对于stu类型，ValueAbleInterface应当给出原类型结构体指针，以调用到正确的恢复还原接口
		inline CString GetValueStr(ValueAbleInterface * pStu = nullptr) const;//给一个可以当作值填入xml的字符串
		inline bool RecoverFromCString(CString strType, CString strValue, ValueAbleInterface * pStu = nullptr);//从值类型和值内容将value还原出来
	};
};

BaseGraph_Common::Value::Value()
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
}

BaseGraph_Common::Value::Value(int nValue)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	SetValue(nValue);
}

BaseGraph_Common::Value::Value(DWORD dwValue)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	SetValue(dwValue);
}

BaseGraph_Common::Value::Value(int nValue, ValueType emType)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	SetValue(nValue, emType);
}

BaseGraph_Common::Value::Value(float fValue)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	SetValue(fValue);
}

BaseGraph_Common::Value::Value(CString strValue)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	SetValue(strValue);
}

BaseGraph_Common::Value::Value(const ValueAbleInterface & stuValue)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	SetValue(stuValue);
}

BaseGraph_Common::Value::Value(const Value & other)
	: m_emType(ValueType_ERR)
	, m_nSize(0)
	, m_pBuf(nullptr)
{
	m_emType = other.m_emType;
	m_nSize = other.m_nSize;
	if (m_nSize > 0)
	{
		m_pBuf = new byte[m_nSize];
		memcpy(m_pBuf, other.m_pBuf, m_nSize);
	}
}

BaseGraph_Common::Value & BaseGraph_Common::Value::operator=(const Value & other)
{
	if (m_nSize != other.m_nSize)
	{
		m_nSize = other.m_nSize;
		delete m_pBuf;
		m_pBuf = nullptr;
		if (m_nSize > 0)
		{
			m_pBuf = new byte[m_nSize];
		}
	}
	m_emType = other.m_emType;
	m_nSize = other.m_nSize;
	if (other.m_nSize > 0)
	{
		memcpy(m_pBuf, other.m_pBuf, m_nSize);
	}
	return *this;
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

CString BaseGraph_Common::Value::ToDebugString()
{
	CString strRet = _T("");
	strRet.Format(_T("%s:%s"), GetValueTypeStr(m_emType), GetValueStr());
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

bool BaseGraph_Common::Value::SetValue(const ValueAbleInterface & stuValue)
{
	Reset();
	m_emType = ValueType_STRUCT;
	m_nSize = stuValue.GetSerializeSize();
	m_pBuf = new byte[m_nSize];
	stuValue.GetSerializeBuf(m_pBuf, (int)m_nSize);
	return true;
}

int BaseGraph_Common::Value::ToInt() const
{
	int nTmp = -1;
	if (m_emType == ValueType_INT
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
		return false;
	}
	return nTmp;
}

DWORD BaseGraph_Common::Value::ToDWORD() const
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
		return false;
	}
	return dwTmp;
}

BOOL BaseGraph_Common::Value::ToBOOL() const
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
		return false;
	}
	return bTmp;
}

float BaseGraph_Common::Value::ToFloat() const
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
		return false;
	}
	return fTmp;
}

CString BaseGraph_Common::Value::ToCString() const
{
	CString strTmp = _T("");
	if (m_emType == ValueType_TEXT)
	{
		TCHAR * szBuf = new TCHAR[m_nSize / sizeof(TCHAR)];
		memcpy(szBuf, m_pBuf, m_nSize);
		strTmp = szBuf;
		delete[] szBuf;
		szBuf = nullptr;
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
		return false;
	}
	return strTmp;
}

bool BaseGraph_Common::Value::ToStruct(ValueAbleInterface & stuValue) const
{
	if (m_emType == ValueType_STRUCT)
	{
		stuValue.RecoverFromBuf(m_pBuf, (int)m_nSize);
	}
	else
	{
		//不匹配的类型转换，可能出问题
		assert(false);
		return false;
	}
	return true;
}

bool BaseGraph_Common::Value::Serialize(CArchive & archive)
{
	if (archive.IsStoring())
	{
		int nTmp = m_emType;
		archive << nTmp;
		nTmp = (int)m_nSize;
		archive << nTmp;
		archive.Write(m_pBuf, (UINT)m_nSize);
	}
	else
	{
		int nTmp = 0;
		archive >> nTmp;
		m_emType = (ValueType)nTmp;
		archive >> nTmp;
		m_nSize = nTmp;
		m_pBuf = new byte[m_nSize];
		archive.Read(m_pBuf, (UINT)m_nSize);
	}
	return true;
}

inline BaseGraph_Common::ValueType BaseGraph_Common::Value::GetValueType()
{
	return m_emType;
}

inline CString BaseGraph_Common::Value::GetValueStr(ValueAbleInterface * pStu/* = nullptr*/) const
{
	CString strRet = _T("error");
	switch (m_emType)
	{
	case BaseGraph_Common::ValueType_ERR:
		break;
	case BaseGraph_Common::ValueType_INT:
		strRet.Format(_T("%d"), ToInt());
		break;
	case BaseGraph_Common::ValueType_DWORD:
		strRet.Format(_T("%d"), ToDWORD());
		break;
	case BaseGraph_Common::ValueType_ENUM:
		strRet.Format(_T("%d"), ToInt());
		break;
	case BaseGraph_Common::ValueType_BOOL:
		strRet.Format(_T("%d"), ToBOOL());
		break;
	case BaseGraph_Common::ValueType_FLOAT:
		strRet.Format(_T("%f"), ToFloat());
		break;
	case BaseGraph_Common::ValueType_ANGLE:
		strRet.Format(_T("%d"), ToInt());
		break;
	case BaseGraph_Common::ValueType_TIME:
		strRet.Format(_T("%d"), ToInt());
		break;
	case BaseGraph_Common::ValueType_TEXT:
		strRet.Format(_T("%s"), ToCString());
		break;
	case BaseGraph_Common::ValueType_STRUCT:
		if (pStu)
		{
			ToStruct(*pStu);
			strRet = pStu->GetValueStr();
		}
		else
		{
			assert(false);
		}
		break;
	default:
		break;
	}
	return strRet;
}

inline bool BaseGraph_Common::Value::RecoverFromCString(CString strType, CString strValue, ValueAbleInterface * pStu/* = nullptr*/)
{
	bool bRet = false;
	if (GetValueTypeStr(ValueType_INT) == strType)
	{
		int nValue = _ttoi(strValue);
		SetValue(nValue, BaseGraph_Common::ValueType_INT);
	}
	else if (GetValueTypeStr(ValueType_DWORD) == strType)
	{
		DWORD dwValue = _ttoi(strValue);
		SetValue(dwValue, BaseGraph_Common::ValueType_DWORD);
	}
	else if (GetValueTypeStr(ValueType_ENUM) == strType)
	{
		int nValue = _ttoi(strValue);
		SetValue(nValue, BaseGraph_Common::ValueType_ENUM);
	}
	else if (GetValueTypeStr(ValueType_BOOL) == strType)
	{
		int nValue = _ttoi(strValue);
		SetValue(nValue, BaseGraph_Common::ValueType_BOOL);
	}
	else if (GetValueTypeStr(ValueType_FLOAT) == strType)
	{
		float fValue = (float)_ttof(strValue);
		SetValue(fValue);
	}
	else if (GetValueTypeStr(ValueType_ANGLE) == strType)
	{
		int nValue = _ttoi(strValue);
		SetValue(nValue, BaseGraph_Common::ValueType_INT);
	}
	else if (GetValueTypeStr(ValueType_TIME) == strType)
	{
		int nValue = _ttoi(strValue);
		SetValue(nValue, BaseGraph_Common::ValueType_TIME);
	}
	else if (GetValueTypeStr(ValueType_TEXT) == strType)
	{
		int nValue = _ttoi(strValue);
		SetValue(nValue, BaseGraph_Common::ValueType_TIME);
	}
	else if (GetValueTypeStr(ValueType_STRUCT) == strType)
	{
		if (pStu)
		{
			pStu->RecoverFromCString(strType, strValue);
			SetValue(*pStu);
		}
		else
		{
			assert(false);
		}
	}
	return bRet;
}

inline CString BaseGraph_Common::GetValueTypeStr(BaseGraph_Common::ValueType emValuleType)
{
	switch (emValuleType)
	{
	case BaseGraph_Common::ValueType_ERR:
		break;
	case BaseGraph_Common::ValueType_INT:
		return _T("int");
		break;
	case BaseGraph_Common::ValueType_DWORD:
		return _T("dword");
		break;
	case BaseGraph_Common::ValueType_ENUM:
		return _T("enum");
		break;
	case BaseGraph_Common::ValueType_BOOL:
		return _T("bool");
		break;
	case BaseGraph_Common::ValueType_FLOAT:
		return _T("float");
		break;
	case BaseGraph_Common::ValueType_ANGLE:
		return _T("angle");
		break;
	case BaseGraph_Common::ValueType_TIME:
		return _T("frame");
		break;
	case BaseGraph_Common::ValueType_TEXT:
		return _T("text");
		break;
	case BaseGraph_Common::ValueType_STRUCT:
		return _T("struct");
		break;
	default:
		break;
	}
	return _T("ErrorType");
}

//宏函数，方便串入串出
#define SERIALIZE_PARAM_GET_SIZE(size, param) \
{	\
	size += sizeof(param);	\
}

#define SERIALIZE_PARAM_TO_BUF(pBuf, size, param) \
{	\
	memcpy(pBuf + size, &param, sizeof(param));	\
	size += sizeof(param);	\
}

#define SERIALIZE_BUF_TO_PARAM(pBuf, size, param) \
{	\
	memcpy(&param, pBuf + size, sizeof(param));	\
	size += sizeof(param);	\
}

#define SERIALIZE_PARAM_GET_SIZE_STU(size, param) \
{	\
	size += param.GetSerializeSize();	\
}

#define SERIALIZE_PARAM_TO_BUF_STU(pBuf, size, param) \
{	\
	int nParSize = param.GetSerializeSize();	\
	param.GetSerializeBuf(pBuf + size, nParSize);	\
	size += nParSize;	\
}

#define SERIALIZE_BUF_TO_PARAM_STU(pBuf, size, param) \
{	\
	int nParSize = param.GetSerializeSize();	\
	param.RecoverFromBuf(pBuf + size, nParSize);	\
	size += nParSize;	\
}

#define SERIALIZE_PARAM_GET_SIZE_STU_ARR(size, param, count) \
{	\
	assert(count > 0);	\
	size += param[0].GetSerializeSize() * count;	\
}

#define SERIALIZE_PARAM_TO_BUF_STU_ARR(pBuf, size, param, count) \
{	\
	assert(count > 0);	\
	for (int i = 0; i < count; i++)	\
	{	\
		int nParSize = param[i].GetSerializeSize();	\
		param[i].GetSerializeBuf(pBuf + size, nParSize);	\
		size += nParSize;	\
	}	\
}

#define SERIALIZE_BUF_TO_PARAM_STU_ARR(pBuf, size, param, count) \
{	\
	assert(count > 0);	\
	for (int i = 0; i < count; i++)	\
	{	\
		int nParSize = param[i].GetSerializeSize();	\
		param[i].RecoverFromBuf(pBuf + size, nParSize);	\
		size += nParSize;	\
	}	\
}
