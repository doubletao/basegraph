#pragma once

namespace BaseGraph_Common
{
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
	};

	class BASEGRAPH_COMMON_API Value
	{
	private:
		ValueType m_emType;
		size_t m_nSize;
		byte * m_pBuf;
	public:
		Value();
		~Value();
		void Reset();
		CString ToString();

		bool SetValue(int nValue);
		bool SetValue(DWORD dwValue);
		bool SetValue(int nValue, ValueType emType);//对于枚举，角度，BOOL，时间等类型，内部其实都是int，但需要指定
		bool SetValue(float fValue);
		bool SetValue(CString strValue);

		int ToInt();
		DWORD ToDWORD();
		BOOL ToBOOL();
		float ToFloat();
		CString ToCString();
	};
};