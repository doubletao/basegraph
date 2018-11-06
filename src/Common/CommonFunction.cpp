#include "..\..\include\Common\CommonFunction.h"
#include "..\..\include\Common\CommonFunction.h"
#include "stdafx.h"
#include "Common\CommonFunction.h"
#include <comdef.h>
#include <vector>

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

CString CommonFunction::GetFileNameExt(const CString strSrcFileName)
{
	int nPos = strSrcFileName.ReverseFind(_T('.'));
	if (nPos < 0)
	{
		return _T("");
	}
	else
	{
		CString strExt = strSrcFileName.Right(strSrcFileName.GetLength() - nPos);
		return strExt;
	}
}

BOOL CommonFunction::MakeSureDirectoryExists(CString strFilePath, CString & strErrInfo)
{
	BOOL bRet = FALSE;
	CString strPath;
	CString strName;
	GetPathAndName(strFilePath, strPath, strName);

	if (strPath.IsEmpty())
	{
		//不允许出现无路径的情况，当前目录也需要有./
		return FALSE;
	}

	if (strPath == _T("."))
	{
		//如果路径是.号，认定是当前路径，可以直接返回成功
		return TRUE;
	}

	//如果文件夹不存在，则创建
	//如果是".\"打头，则截取掉".\"替换为当前路径
	if (strPath.Left(2) == _T(".\\"))
	{
		strPath = strPath.Mid(2);
		CString strCurName;
		GetModuleFileName(NULL, strCurName.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		strCurName.ReleaseBuffer();
		CString strCurPath;
		CString strCurPurName;
		GetPathAndName(strCurName, strCurPath, strCurPurName);
		strCurPurName = strPath;
		strPath.Format(_T("%s\\%s"), strCurPath, strCurPurName);
	}
	//确保以'\'结尾以创建最后一个目录
	if (strPath.GetAt(strPath.GetLength() - 1) != _T('\\'))
	{
		strPath.AppendChar(_T('\\'));
	}
	std::vector<CString> vPath;//存放每一层目录字符串
	CString strTemp;//一个临时变量,存放目录字符串
					//遍历要创建的字符串
	for (int i = 0; i < strPath.GetLength(); ++i)
	{
		if (strPath.GetAt(i) != _T('\\'))
		{
			//如果当前字符不是'\\'
			strTemp.AppendChar(strPath.GetAt(i));
		}
		else
		{
			//如果当前字符是'\\'
			vPath.push_back(strTemp);//将当前层的字符串添加到数组中
			strTemp.AppendChar(_T('\\'));
		}
	}

	//遍历存放目录的数组,创建每层目录
	std::vector<CString>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
	{
		//如果CreateDirectory执行成功,返回true,否则返回false
		bRet = CreateDirectory(*vIter, NULL) ? true : false;
		if (!bRet)
		{
			int nErr = GetLastError();
			if (nErr == 183)
			{
				//错误号183说明路径已经存在无需创建，这种情况不视为错误
				bRet = TRUE;
				continue;
			}
			else if (nErr == 5)
			{
				//错误号5说明拒绝访问，有可能是试图创建根目录导致的，不视为错误
				if (vIter == vPath.begin())
				{
					bRet = TRUE;
					continue;
				}
			}
			else
			{
				LPVOID lpMsgBuf;
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR)&lpMsgBuf,
					0,
					NULL
				);

				// Display the string.
				strErrInfo = (LPTSTR)lpMsgBuf;

				// Free the buffer.
				LocalFree(lpMsgBuf);
				break;
			}
		}
	}

	return bRet;
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

std::string CommonFunction::ConvertCStringToStdString(CString cstrValue)
{
#if 0
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
#else
	std::string strRet = (const char*)_bstr_t(cstrValue);
	return strRet;
#endif
}

std::string CommonFunction::ConvertCStringToStdString(CString cstrValue, DWORD dwFormatW)
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
#if 0
	CString cstrRet;
	cstrRet.Empty();
	int nLength = ::MultiByteToWideChar(CP_UTF8, 0, strValue.c_str(), -1, NULL, 0);
	TCHAR * szText1 = new TCHAR[nLength + 1];
	::MultiByteToWideChar(CP_UTF8, 0, strValue.c_str(), -1, szText1, nLength + 1);
	cstrRet = szText1;
	delete[] szText1;
	szText1 = NULL;
	return cstrRet;
#else
	CString strRet(strValue.c_str());
	return strRet;
#endif
}
