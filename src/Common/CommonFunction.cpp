#include "..\..\include\Common\CommonFunction.h"
#include "stdafx.h"
#include "Common\CommonFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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

CString CommonFunction::GetFilePath(LPCTSTR szFullPathFileName)
{
	CString strPath(szFullPathFileName);
	int nPos = strPath.ReverseFind(_T('\\'));
	if (nPos<0)
		nPos = strPath.ReverseFind(_T('/'));

	if (nPos >= 0)
		strPath = strPath.Left(nPos + 1);
	else
		strPath.Empty();

	return strPath;
}

CString CommonFunction::ConvertNumToCString(DWORD dwValue)
{
	CString strRet;
	strRet.Format(_T("%d"), dwValue);
	return strRet;
}

CString CommonFunction::ConvertNumToCString(int nValue)
{
	CString strRet;
	strRet.Format(_T("%d"), nValue);
	return strRet;
}

CString CommonFunction::ConvertNumToCString(float fValue, int nPrecision/* = 2*/)
{
	CString strRet;
	switch (nPrecision)
	{
	case 0:
		strRet.Format(_T("%d"), (int)fValue);
		break;
	case 1:
		strRet.Format(_T("%.1f"), fValue);
		break;
	case 2:
		strRet.Format(_T("%.2f"), fValue);
		break;
	case 3:
		strRet.Format(_T("%.3f"), fValue);
		break;
	case 4:
		strRet.Format(_T("%.4f"), fValue);
		break;
	case 5:
		strRet.Format(_T("%.5f"), fValue);
		break;
	case 6:
		strRet.Format(_T("%.6f"), fValue);
		break;
	default:
		strRet.Format(_T("%.6f"), fValue);
		break;
	}
	return strRet;
}

CString CommonFunction::ConvertNumToCString(double dValue, int nPrecision/* = 2*/)
{
	CString strRet;
	switch (nPrecision)
	{
	case 0:
		strRet.Format(_T("%d"), (int)dValue);
		break;
	case 1:
		strRet.Format(_T("%.1f"), dValue);
		break;
	case 2:
		strRet.Format(_T("%.2f"), dValue);
		break;
	case 3:
		strRet.Format(_T("%.3f"), dValue);
		break;
	case 4:
		strRet.Format(_T("%.4f"), dValue);
		break;
	case 5:
		strRet.Format(_T("%.5f"), dValue);
		break;
	case 6:
		strRet.Format(_T("%.6f"), dValue);
		break;
	default:
		strRet.Format(_T("%.6f"), dValue);
		break;
	}
	return strRet;
}

int CommonFunction::ConvertCStringToInt(CString strValue)
{
	int nRet = _ttoi(strValue);
	return nRet;
}

float CommonFunction::ConvertCStringToFloat(CString strValue)
{
	float fRet = (float)_ttof(strValue);
	return fRet;
}

double CommonFunction::ConvertCStringToDouble(CString strValue)
{
	double dRet = _ttof(strValue);
	return dRet;
}

std::string CommonFunction::ConverCStringToStdString(CString & cstrValue)
{
	std::string strRet;
	strRet.clear();
	int nSize = ::WideCharToMultiByte(CP_UTF8, 0, cstrValue.GetBuffer(), -1, NULL, 0, NULL, NULL);
	char * szcSrc = new char[nSize];
	memset(szcSrc, 0, sizeof(char) * nSize);
	::WideCharToMultiByte(CP_UTF8, 0, cstrValue.GetBuffer(), -1, szcSrc, nSize, NULL, NULL);
	strRet = szcSrc;
	delete[] szcSrc;
	szcSrc = NULL;
	return strRet;

}

std::string CommonFunction::ConverCStringToStdString(CString & cstrValue, DWORD dwFormatW)
{
	std::string strRet;
	strRet.clear();
	int nSize = ::WideCharToMultiByte(dwFormatW, 0, cstrValue.GetBuffer(), -1, NULL, 0, NULL, NULL);
	char * szcSrc = new char[nSize];
	memset(szcSrc, 0, sizeof(char) * nSize);
	::WideCharToMultiByte(dwFormatW, 0, cstrValue.GetBuffer(), -1, szcSrc, nSize, NULL, NULL);
	strRet = szcSrc;
	delete[] szcSrc;
	szcSrc = NULL;
	return strRet;
}

CString CommonFunction::ConvertStdStringToCString(const std::string & strValue)
{
	CString cstrRet;
	cstrRet.Empty();
	int nLength = ::MultiByteToWideChar(CP_UTF8, 0, strValue.c_str(), -1, NULL, 0);
	TCHAR * szText1 = new TCHAR[nLength + 1];
	::MultiByteToWideChar(CP_UTF8, 0, strValue.c_str(), -1, szText1, nLength + 1);
	cstrRet = szText1;
	delete[] szText1;
	szText1 = NULL;
	return cstrRet;
}
