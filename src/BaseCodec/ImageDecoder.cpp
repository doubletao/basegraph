#include "stdafx.h"
#include "ImageDecoder.h"

std::vector<std::pair<CString, CString>> ImageDecoder::m_vecSupportExt;

ImageDecoder::ImageDecoder()
	: bOpen(false)
{
}

ImageDecoder::~ImageDecoder()
{
	bOpen = false;
}

bool ImageDecoder::InitialSupportExtList()
{
	if (m_vecSupportExt.size() > 0)
	{
		return true;
	}
	//暂时支持几个简单的其他的再说
	m_vecSupportExt.push_back(std::make_pair(_T("PNG"), _T("*.png")));
	m_vecSupportExt.push_back(std::make_pair(_T("JPG"), _T("*.jpg")));
	m_vecSupportExt.push_back(std::make_pair(_T("BMP"), _T("*.bmp")));
	//m_vecSupportExt.push_back(std::make_pair(_T("TGA"), _T("*.tga")));
	//m_vecSupportExt.push_back(std::make_pair(_T("PCX"), _T("*.pcx")));
	//m_vecSupportExt.push_back(std::make_pair(_T("TNG"), _T("*.tng")));
	//m_vecSupportExt.push_back(std::make_pair(_T("WMF"), _T("*.wmf")));
	//m_vecSupportExt.push_back(std::make_pair(_T("EMF"), _T("*.wmf")));
	return true;
}

bool ImageDecoder::QueryImageSupport(IN CString strFile, IN bool bDecode)
{
	InitialSupportExtList();
	CString strExt = strFile;
	int nDotPos = strFile.ReverseFind(_T('.'));
	if (nDotPos > -1)
	{
		strExt = strFile.Mid(nDotPos + 1);
	}
	for (size_t i = 0; i < m_vecSupportExt.size(); i++)
	{
		CString strExtTmp = m_vecSupportExt[i].first;
		if (strExtTmp.CompareNoCase(strExt) == 0)
		{
			return true;
		}
	}
	return false;
}

LPCTSTR ImageDecoder::EnumSuportImageType(bool bAddAllFile, bool bTerminated)
{
	InitialSupportExtList();
	static const size_t NAME_LENGTH = 1024;
	static TCHAR szSuportFile[NAME_LENGTH];
	szSuportFile[0] = NULL;

	CString strFilter;
	strFilter.Empty();

	strFilter.Append(_T("All Image Types("));
	for (int i = 0; i < m_vecSupportExt.size(); i++)
	{
		strFilter.Append(m_vecSupportExt[i].second);
		strFilter.Append(_T(";"));
	}
	strFilter.Append(_T(")|"));
	for (int i = 0; i < m_vecSupportExt.size(); i++)
	{
		strFilter.Append(m_vecSupportExt[i].second);
		strFilter.Append(_T(";"));
	}
	strFilter.Append(_T("|"));

	for (int i = 0; i < m_vecSupportExt.size(); i++)
	{
		strFilter.AppendFormat(_T("%s(%s)|%s|"), m_vecSupportExt[i].first, m_vecSupportExt[i].second, m_vecSupportExt[i].second);
	}

	_tcscpy_s(szSuportFile, NAME_LENGTH, strFilter.GetBuffer());
	strFilter.ReleaseBuffer();

	if (bAddAllFile)
		_tcscat_s(szSuportFile, NAME_LENGTH, _T("All File(*.*)|*.*|"));

	if (bTerminated)
		_tcscat_s(szSuportFile, NAME_LENGTH, _T("|"));

	return szSuportFile;
}

bool ImageDecoder::Open(LPCTSTR szFileName)
{
	m_image.Destroy();
	if (m_image.Load(szFileName) == E_FAIL)
		return false;
	bOpen = true;
	return true;
}

bool ImageDecoder::GetImgInfo(OUT int & xStart, OUT int & yStart, OUT int & nWidth, OUT int & nHeight, int nLayer)
{
	if (!bOpen)
	{
		return false;
	}
	xStart = 0;
	yStart = 0;
	nWidth = m_image.GetWidth();
	nHeight = m_image.GetHeight();
	return true;
}

bool ImageDecoder::ReadImage(IN int nWidth, IN int nHeight, OUT byte * pBuffer, IN int nPixSize)
{
	if (!bOpen)
	{
		return false;
	}

	int nBufSize = nWidth * nHeight * nPixSize;

	//图像每一行的字节数  
	int nRowBytes = m_image.GetPitch();
	int nImageWidth = m_image.GetWidth();
	int nImageHeight = m_image.GetHeight();


	//每个像素所占的字节数  
	int nClrCount = m_image.GetBPP() / 8;
	LPBYTE pBits = nullptr;
	for (int index = 0; index < nClrCount; index++)
	{
		pBits = (LPBYTE)m_image.GetBits();
		for (int i = 0; i < nImageHeight; i++)
		{
			for (int j = 0; j < nImageWidth; j++)
			{
				if (j * nClrCount + index < nBufSize)
				{
					pBuffer[j * nClrCount + index] = pBits[j * nClrCount + index];
				}
				else
				{
					return false;
				}
			}

			//如果nRowBytes>0  则从开始到结尾  
			//如果nRowBytes<0, 则从结尾到开始  
			pBits += nRowBytes;
		}
	}

	return true;
}
