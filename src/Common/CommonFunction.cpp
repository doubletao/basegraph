#include "..\..\include\Common\CommonFunction.h"
#include "stdafx.h"
#include "Common\CommonFunction.h"
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

CString CommonFunction::GetBinPath()
{
	CString strCurName;
	GetModuleFileName(NULL, strCurName.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strCurName.ReleaseBuffer();
	CString strCurPath;
	CString strCurPurName;
	GetPathAndName(strCurName, strCurPath, strCurPurName);
	return strCurPath;
}

void CommonFunction::GetPathAndName(const CString & strSrcFilePath, CString & strPath, CString & strName)
{
	strPath.Empty();
	strName.Empty();

	int nPos = strSrcFilePath.ReverseFind(_T('\\'));
	if (nPos < 0)
	{
		//未发现反斜杠时，可能只有文件名而无路径，可以继续获取文件名
		strPath.Empty();
	}
	else
	{
		strPath = strSrcFilePath.Left(nPos);
		strName = strSrcFilePath.Right(strSrcFilePath.GetLength() - nPos - 1);
	}

	nPos = strName.ReverseFind(_T('.'));
	if (nPos < 0)
	{
		//未发现点号(.)，视为无后缀的文件名，不处理
	}
	else
	{
		strName = strName.Left(nPos);
	}
}

CString CommonFunction::ConverFrameToText(int nFrame, float fFPS/* = 25.0*/)
{
	int nFps = (int)fFPS;
	int nWholeSec = nFrame / nFps;
	int nFrameTail = nFrame % nFps;
	int nHour = nWholeSec / 3600;
	int nMin = nWholeSec % 3600 / 60;
	int nSec = nWholeSec % 3600 % 60;
	CString strDisp;
	strDisp.Format(_T("%02d:%02d:%02d:%02d"), nHour, nMin, nSec, nFrameTail);
	return strDisp;
}

BOOL CommonFunction::IsFileExists(const CString & strSrcFilePath)
{
	if (strSrcFilePath.IsEmpty())
		return FALSE;

	return  (_taccess(strSrcFilePath, 00) == 0);
}
