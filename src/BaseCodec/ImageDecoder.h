#pragma once
#include <vector>
#include <atlimage.h>

class ImageDecoder
{
private:
	static std::vector<std::pair<CString, CString>> m_vecSupportExt;
	CImage m_image;
	bool bOpen;
public:
	ImageDecoder();
	~ImageDecoder();
public:
	static bool InitialSupportExtList();
	static bool QueryImageSupport(IN CString strFile, IN bool bDecode = true);
	static LPCTSTR EnumSuportImageType(bool bAddAllFile = true, bool bTerminated = true);
public:
	bool Open(LPCTSTR szFileName);
	bool GetImgInfo(OUT int& xStart, OUT int& yStart, OUT int& nWidth, OUT int& nHeight, int nLayer = -1);
	bool ReadImage(IN int nWidth, IN int nHeight, OUT byte * pBuffer, IN int nPixSize = 4);
};

