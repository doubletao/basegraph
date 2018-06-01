#pragma once

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
	int nSize = param.GetSerializeSize();	\
	param.GetSerializeBuf(pBuf + size, nSize);	\
	size += nSize;	\
}

#define SERIALIZE_BUF_TO_PARAM_STU(pBuf, size, param) \
{	\
	int nSize = param.GetSerializeSize();	\
	param.RecoverFromBuf(pBuf + size, nSize);	\
	size += nSize;	\
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
		int nSize = param[i].GetSerializeSize();	\
		param[i].GetSerializeBuf(pBuf + size, nSize);	\
		size += nSize;	\
	}	\
}

#define SERIALIZE_BUF_TO_PARAM_STU_ARR(pBuf, size, param, count) \
{	\
	assert(count > 0);	\
	for (int i = 0; i < count; i++)	\
	{	\
		int nSize = param[i].GetSerializeSize();	\
		param[i].RecoverFromBuf(pBuf + size, nSize);	\
		size += nSize;	\
	}	\
}

namespace BaseGraph_Common
{
	struct ValueAbleInterface
	{
		virtual int GetSerializeSize() const = 0;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const = 0;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize) = 0;
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

	class BASEGRAPH_COMMON_API Value
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
		Value();
		Value(int nValue);
		Value(DWORD dwValue);
		Value(int nValue, ValueType emType);//对于枚举，角度，BOOL，时间等类型，内部其实都是int，但需要指定
		Value(float fValue);
		Value(CString strValue);
		Value(const ValueAbleInterface & stuValue);
		Value(const Value & other);
		Value & operator = (const Value & other);
		~Value();
		void Reset();
		CString ToDebugString();

		bool SetValue(int nValue);
		bool SetValue(DWORD dwValue);
		bool SetValue(int nValue, ValueType emType);//对于枚举，角度，BOOL，时间等类型，内部其实都是int，但需要指定
		bool SetValue(float fValue);
		bool SetValue(CString strValue);
		bool SetValue(const ValueAbleInterface & stuValue);

		int ToInt();
		DWORD ToDWORD();
		BOOL ToBOOL();
		float ToFloat();
		CString ToCString();
		bool ToStruct(ValueAbleInterface & stuValue);

		bool Serialize(CArchive & archive);
	};
};
