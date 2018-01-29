#include "stdafx.h"
#include "Common/CommonFunction.h"
#pragma comment(lib, "Rpcrt4.lib")

CommonFunction::CommonFunction()
{
}


CommonFunction::~CommonFunction()
{
}

CString CommonFunction::CreateGUID()
{
	GUID guid = GUID_NULL;
	::CoCreateGuid(&guid);
	CString strGUID;
	GuidToCString(guid, strGUID);
	strGUID.MakeUpper();
	return strGUID;
}

HRESULT CommonFunction::GuidToCString(const GUID & guid, CString & str)
{
	GUID guidNo = guid;
#ifdef UNICODE
	RPC_WSTR pUuidString;
#else
	BYTE* pUuidString;
#endif
	UuidToString(&guidNo, &pUuidString);
	str = (LPCTSTR)pUuidString;
	str.MakeUpper();

	if (RPC_S_OK == RpcStringFree(&pUuidString))
		return S_OK;

	return S_FALSE;
}
