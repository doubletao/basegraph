#include "stdafx.h"
#include "Common\BufferFunction.h"
#include "BufferMemSSE.h"
#include "CommonFastMemAsm.h"

const float PI = 3.1415926;
LPDWORD	lpdwColorMixCalTable = NULL;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
BufferFunction::BufferFunction()
{
}


BufferFunction::~BufferFunction()
{
}

BOOL BufferFunction::BufferBitblt(DWORD * pdwDesBuffer, int nDesX, int nDesY, int nDesWidth, int nDesHeight, int nDesBufferWidth, int nDesBufferHeight, DWORD * pdwSrcBuffer, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, int nSrcBufferWidth, int nSrcBufferHeight, BUFFER_MODE eMode, BUFFER_PARITY_STYLE eParity, RECT * prcDesClip, int n, int k)
{
	if (nSrcWidth == nDesWidth && nSrcHeight == nDesHeight)		// 固定大小
	{
		return BufferMemSSE::Buffer_BGRA_Fix_SSE(pdwDesBuffer,
			nDesX,
			nDesY,
			nDesWidth,
			nDesHeight,
			nDesBufferWidth,
			nDesBufferHeight,
			pdwSrcBuffer,
			nSrcX,
			nSrcY,
			nSrcWidth,
			nSrcHeight,
			nSrcBufferWidth,
			nSrcBufferHeight,
			eMode,
			eParity,
			prcDesClip, n, k);
	}
	else		// 缩放
	{
		return BufferMemSSE::Buffer_BGRA_BLStretch_SSE(pdwDesBuffer,
			nDesX,
			nDesY,
			nDesWidth,
			nDesHeight,
			nDesBufferWidth,
			nDesBufferHeight,
			pdwSrcBuffer,
			nSrcX,
			nSrcY,
			nSrcWidth,
			nSrcHeight,
			nSrcBufferWidth,
			nSrcBufferHeight,
			eMode,
			eParity,
			prcDesClip);
	}
	return FALSE;
}

BOOL BufferFunction::WriteBitmap(LPCTSTR ptszFileName, int iWidth, int iHeight, int iDepth, BYTE * pData)
{
	BITMAPFILEHEADER stFileHdr;
	stFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	stFileHdr.bfSize = sizeof(BITMAPFILEHEADER);
	stFileHdr.bfType = 0x4d42;
	BITMAPINFOHEADER stInfoHdr;
	memset(&stInfoHdr, 0, sizeof(BITMAPINFOHEADER));
	stInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	stInfoHdr.biPlanes = 1;
	stInfoHdr.biWidth = iWidth;
	stInfoHdr.biHeight = iHeight;
	stInfoHdr.biBitCount = iDepth;
	stInfoHdr.biSizeImage = stInfoHdr.biWidth * stInfoHdr.biHeight * stInfoHdr.biBitCount / 8;
	FILE* fp = NULL;
	_tfopen_s(&fp, ptszFileName, _T("wb"));
	if (!fp)
		return FALSE;
	fwrite(&stFileHdr, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite(&stInfoHdr, 1, sizeof(BITMAPINFOHEADER), fp);
	size_t nSize = fwrite(pData, 1, stInfoHdr.biSizeImage, fp);
	fclose(fp);
	return TRUE;
}

BOOL BufferFunction::WriteBitmap(LPCTSTR ptszFileName, int nDesX, int nDesY, int nDesWidth, int nDesHeight, int nSrcWidth, int nSrcHeight, DWORD * pData)
{
	BITMAPFILEHEADER stFileHdr;
	stFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	stFileHdr.bfSize = sizeof(BITMAPFILEHEADER);
	stFileHdr.bfType = 0x4d42;
	BITMAPINFOHEADER stInfoHdr;
	memset(&stInfoHdr, 0, sizeof(BITMAPINFOHEADER));
	stInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	stInfoHdr.biPlanes = 1;
	stInfoHdr.biWidth = nSrcWidth;
	stInfoHdr.biHeight = nSrcHeight;
	stInfoHdr.biBitCount = 4;
	stInfoHdr.biSizeImage = stInfoHdr.biWidth * stInfoHdr.biHeight * stInfoHdr.biBitCount / 8;
	FILE* fp = NULL;
	_tfopen_s(&fp, ptszFileName, _T("wb"));
	if (!fp)
		return FALSE;
	fwrite(&stFileHdr, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite(&stInfoHdr, 1, sizeof(BITMAPINFOHEADER), fp);
	size_t nSize = fwrite(pData, 1, stInfoHdr.biSizeImage, fp);
	fclose(fp);
	return TRUE;
}

void BufferFunction::DrawImageWithAplha(CDC * pDC, HBITMAP hBitmap, const CRect & rc, const CRect & rcPaint, const CRect & rcBmpPart, const CRect & rcCorners, BOOL alphaChannel, BYTE uFade, BOOL hole)
{
	if (hBitmap == NULL) return;

	CBitmap Bitmap;
	Bitmap.Attach(hBitmap);

	if (false)
	{
		CImage img;
		img.Attach(hBitmap);
		img.Save(_T("e:\\test.bmp"));
		img.Detach();
	}

	CDC CloneDC;
	CloneDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = CloneDC.SelectObject(&Bitmap);
	pDC->SetStretchBltMode(COLORONCOLOR);

	CRect rcTemp(0, 0, 0, 0);
	CRect rcDest(0, 0, 0, 0);
	if (alphaChannel || uFade < 255)
	{
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
		// middle
		if (!hole) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
					rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
					rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}

		// left-top
		if (rcCorners.left > 0 && rcCorners.top > 0) {
			rcDest.left = rc.left;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
			}
		}
		// top
		if (rcCorners.top > 0) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
					rcCorners.left - rcCorners.right, rcCorners.top, bf);
			}
		}
		// right-top
		if (rcCorners.right > 0 && rcCorners.top > 0) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
			}
		}
		// left
		if (rcCorners.left > 0) {
			rcDest.left = rc.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
					rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// right
		if (rcCorners.right > 0) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
					rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// left-bottom
		if (rcCorners.left > 0 && rcCorners.bottom > 0) {
			rcDest.left = rc.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
			}
		}
		// bottom
		if (rcCorners.bottom > 0) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
					rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
			}
		}
		// right-bottom
		if (rcCorners.right > 0 && rcCorners.bottom > 0) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				AlphaBitBlt(pDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, &CloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
					rcCorners.bottom, bf);
			}
		}
	}
	CloneDC.SelectObject(pOldBitmap);
	CloneDC.DeleteDC();
	Bitmap.Detach();
}

void BufferFunction::DrawImageWithAplha(CDC * pDC, CImage * pImg, const CRect & rc, const CRect & rcPaint, const CRect & rcBmpPart, const CRect & rcCorners, BOOL alphaChannel, BYTE uFade, BOOL hole)
{
	HBITMAP hBmp = pImg->Detach();
	DrawImageWithAplha(pDC, hBmp, rc, rcPaint, rcBmpPart, rcCorners, alphaChannel, uFade, hole);
	pImg->Attach(hBmp);
}

BOOL BufferFunction::AlphaBitBlt(CDC * pDC, int nDestX, int nDestY, int dwWidth, int dwHeight, CDC * SrcDC, int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
	CDC	TempDC;
	TempDC.CreateCompatibleDC(pDC);
	//Creates Source DIB
	LPBITMAPINFO lpbiSrc = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
		return FALSE;
	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = dwWidth;
	lpbiSrc->bmiHeader.biHeight = dwHeight;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biClrUsed = 0;
	lpbiSrc->bmiHeader.biClrImportant = 0;

	COLORREF* pSrcBits = NULL;
	HBITMAP hSrcDib = CreateDIBSection(
		SrcDC->m_hDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
		NULL, NULL);

	if ((NULL == hSrcDib) || (NULL == pSrcBits))
	{
		delete[] lpbiSrc;
		return FALSE;
	}

	CBitmap *pBitmap = NULL;
	pBitmap = CBitmap::FromHandle(hSrcDib);
	CBitmap* pOldTempBmp = TempDC.SelectObject(pBitmap);
	TempDC.StretchBlt(0, 0, dwWidth, dwHeight, SrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
	TempDC.SelectObject(pOldTempBmp);


	//Creates Destination DIB
	LPBITMAPINFO lpbiDest = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiDest == NULL)
	{
		delete[] lpbiSrc;
		return FALSE;
	}

	lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiDest->bmiHeader.biWidth = dwWidth;
	lpbiDest->bmiHeader.biHeight = dwHeight;
	lpbiDest->bmiHeader.biPlanes = 1;
	lpbiDest->bmiHeader.biBitCount = 32;
	lpbiDest->bmiHeader.biCompression = BI_RGB;
	lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiDest->bmiHeader.biXPelsPerMeter = 0;
	lpbiDest->bmiHeader.biYPelsPerMeter = 0;
	lpbiDest->bmiHeader.biClrUsed = 0;
	lpbiDest->bmiHeader.biClrImportant = 0;

	COLORREF* pDestBits = NULL;
	HBITMAP hDestDib = CreateDIBSection(
		pDC->m_hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
		NULL, NULL);

	if ((NULL == hDestDib) || (NULL == pDestBits))
	{
		delete[] lpbiSrc;
		delete[] lpbiDest;
		return FALSE;
	}

	CBitmap* pBitmapD = NULL;
	pBitmapD = CBitmap::FromHandle(hDestDib);
	TempDC.SelectObject(pBitmapD);
	TempDC.BitBlt(0, 0, dwWidth, dwHeight, pDC, nDestX, nDestY, SRCCOPY);
	TempDC.SelectObject(pOldTempBmp);

	double src_darken;
	BYTE nAlpha;

	for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
	{
		nAlpha = LOBYTE(*pSrcBits >> 24);
		src_darken = (double)(nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
		if (src_darken < 0.0) src_darken = 0.0;
		*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
	}
	TempDC.SelectObject(hDestDib);

	pDC->BitBlt(nDestX, nDestY, dwWidth, dwHeight, &TempDC, 0, 0, SRCCOPY);

	if (false)
	{
		CImage img;
		img.Attach(hSrcDib);
		img.Save(_T("e:\\hSrcDib.bmp"));
		img.Detach();

		img.Attach(hDestDib);
		img.Save(_T("e:\\hDesDib.bmp"));
		img.Detach();
	}

	::DeleteObject(hDestDib);
	delete[] lpbiDest;
	::DeleteObject(hSrcDib);
	delete[] lpbiSrc;
	TempDC.SelectObject(pOldTempBmp);
	TempDC.DeleteDC();
	pBitmap->DeleteObject();
	pBitmapD->DeleteObject();
	return TRUE;
}

COLORREF BufferFunction::PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB(GetRValue(clrSrc) * src_darken + GetRValue(clrDest) * dest_darken,
		GetGValue(clrSrc) * src_darken + GetGValue(clrDest) * dest_darken,
		GetBValue(clrSrc) * src_darken + GetBValue(clrDest) * dest_darken);
}

void BufferFunction::MakeStaticFlag(CCommonBuffer * pBuffer)
{
	/***********************************/
	/*	1.加光影效果
	/*	2.用多条sin曲线模拟旗帜形状
	/*	3.对上下边缘做反走样(加权平均)
	/***********************************/

	if (pBuffer == NULL || pBuffer->m_nWidth <= 0 || pBuffer->m_nHeight <= 0)
		return;

	int Width = pBuffer->m_nWidth;
	int Height = pBuffer->m_nHeight;
	CCommonBuffer DesBuffer(pBuffer);
	//2.用多条sin曲线模拟旗帜形状
	double dbX;
	double dbY;
	CCommonBuffer mtBuffer;
	mtBuffer.Create(Width, Height + Height / 4);
	memset(mtBuffer.m_dwBuffer, 0, sizeof(DWORD)*mtBuffer.m_nWidth*mtBuffer.m_nHeight);

	std::vector<int> arCurveHigh;
	std::vector<int> arCurveLow;
	DWORD dwAntiColor;
	int x = 0;
	for (x = 0; x < Width * 39 / 109 - 1; x++)
	{
		dbX = x*2.5 / (Width * 5 / 9);
		dbY = sin(dbX / 1.28 + PI / 3.5);
		dbY *= Height / 4;

		RECT r, r1;
		SetRect(&r, x, (long)(Height / 4 - dbY + 10), 1 + x, (Height / 4 - dbY + 10) + DesBuffer.m_nHeight);
		SetRect(&r1, x, 0, x + 1, Height);

		mtBuffer.BitBlt(&r, &DesBuffer, &r1, BUFFER_COPY, BUFFER_PARITY_BOTH, NULL);
		arCurveHigh.push_back(int(Height / 4 - dbY + 10));

		dwAntiColor = *(DesBuffer.m_dwBuffer + x);
		dwAntiColor &= 0x00ffffff;
		SetRect(&r, x, 0, x + 1, int(Height / 4 - dbY + 10) - 1);
		mtBuffer.ClearAll(dwAntiColor, &r);

		dwAntiColor = *(DesBuffer.m_dwBuffer + (Height - 1)*DesBuffer.m_nWidth + x);
		dwAntiColor &= 0x00ffffff;
		SetRect(&r, x, int((Height / 4 - dbY + 10) - 1 + DesBuffer.m_nHeight), x + 1, mtBuffer.m_nHeight);
		mtBuffer.ClearAll(dwAntiColor, &r);
		arCurveLow.push_back(int(Height / 4 - dbY + 10) - 1 + DesBuffer.m_nHeight);
	}
	for (; x < (Width * 39 / 109 - 1) * 2; x++)
	{
		dbX = x * 1 / (Width * 1 / 4.5);
		dbY = sin(dbX / 2 + PI / 1.1) + 1.345;
		dbY *= Height / 4;

		RECT r, r1;
		SetRect(&r, x, (long)(Height / 4 - dbY + 10), x + 1, (long)(Height / 4 - dbY + 10) + Height);
		SetRect(&r1, x, 0, x + 1, Height);
		mtBuffer.BitBlt(&r, &DesBuffer, &r1, BUFFER_COPY, BUFFER_PARITY_BOTH, NULL);
		arCurveHigh.push_back(int(Height / 4 - dbY + 10));

		dwAntiColor = *(DesBuffer.m_dwBuffer + x);
		dwAntiColor &= 0x00ffffff;
		SetRect(&r, x, 0, x + 1, int(Height / 4 - dbY + 10) - 1);
		mtBuffer.ClearAll(dwAntiColor, &r);

		dwAntiColor = *(DesBuffer.m_dwBuffer + (Height - 1)*DesBuffer.m_nWidth + x);
		dwAntiColor &= 0x00ffffff;
		SetRect(&r, x, int((Height / 4 - dbY + 10) - 1 + DesBuffer.m_nHeight), x + 1, mtBuffer.m_nHeight);
		mtBuffer.ClearAll(dwAntiColor, &r);
		arCurveLow.push_back(int(Height / 4 - dbY + 10) - 1 + DesBuffer.m_nHeight);
	}

	for (; x < Width; x++)
	{
		dbX = x * 1 / (Width * 1 / 4.5);
		dbY = sin(dbX / 1000) + 0.38;
		dbY *= Height / 4;
		RECT r, r1;
		SetRect(&r, x, (long)(Height / 4 - dbY + 10), x + 1, (long)(Height / 4 - dbY + 10) + Height);
		SetRect(&r1, x, 0, x + 1, Height);
		mtBuffer.BitBlt(&r, &DesBuffer, &r1, BUFFER_COPY, BUFFER_PARITY_BOTH, NULL);
		arCurveHigh.push_back(int(Height / 4 - dbY + 10));

		dwAntiColor = *(DesBuffer.m_dwBuffer + x);
		dwAntiColor &= 0x00ffffff;
		SetRect(&r, x, 0, x + 1, int(Height / 4 - dbY + 10) - 1);
		mtBuffer.ClearAll(dwAntiColor, &r);

		dwAntiColor = *(DesBuffer.m_dwBuffer + (Height - 1)*DesBuffer.m_nWidth + x);
		dwAntiColor &= 0x00ffffff;
		SetRect(&r, x, int((Height / 4 - dbY + 10) - 1 + DesBuffer.m_nHeight), x + 1, mtBuffer.m_nHeight);
		mtBuffer.ClearAll(dwAntiColor, &r);
		arCurveLow.push_back(int(Height / 4 - dbY + 10) - 1 + DesBuffer.m_nHeight);
	}
	//3.对上下边缘做反走样(加权平均)
	RECT rtSrc;
	SetRect(&rtSrc, 0, 0, mtBuffer.m_nWidth, mtBuffer.m_nHeight);
	RECT rtDes;
	SetRect(&rtDes, 0, 0, pBuffer->m_nWidth, pBuffer->m_nHeight);
	pBuffer->BitBlt(&rtDes, &mtBuffer, &rtSrc, BUFFER_COPY, BUFFER_PARITY_BOTH, NULL);
	AntiAliasingBuffer(pBuffer, arCurveHigh, arCurveLow);
}

//反走样加权平均Filter
float  Filter[3][3] =
{
	{ 1, 2, 1 },
	{ 2, 4, 2 },
	{ 1, 2, 1 }
};
DWORD btEqColor(DWORD dwPoint[3][3])
{
	DWORD btRed = 0;
	DWORD btGreen = 0;
	DWORD btBlue = 0;
	DWORD btAlpha = 0;

	for (int i = 0; i<3; i++)
	{
		for (int j = 0; j<3; j++)
		{
			btRed += long(((dwPoint[i][j] & 0x00ff0000) >> 16) * Filter[i][j]);
			btGreen += long(((dwPoint[i][j] & 0x0000ff00) >> 8) * Filter[i][j]);
			btBlue += long((dwPoint[i][j] & 0x000000ff) * Filter[i][j]);
			btAlpha += long(((dwPoint[i][j] & 0xff000000) >> 24) * Filter[i][j]);
		}
	}
	btRed /= 16;
	btGreen /= 16;
	btBlue /= 16;
	btAlpha /= 16;
	return (btAlpha << 24 | (btRed << 16) | (btGreen << 8) | (btBlue));
}
void Attach(CommonBufferDef32 * DestBuffer, CommonBufferDef32 *SrcBuffer)
{
	if (SrcBuffer) {
		DestBuffer->m_nWidth = SrcBuffer->m_nWidth;
		DestBuffer->m_nHeight = SrcBuffer->m_nHeight;
		DestBuffer->m_nPitch = SrcBuffer->m_nPitch;
		DestBuffer->m_dwBuffer = SrcBuffer->m_dwBuffer;
	}
}

void BufferFunction::AntiAliasingBuffer(CCommonBuffer * pSrcBuf, std::vector<int>& arCurveHigh, std::vector<int>& arCurveLow)
{
	CCommonBuffer OKBuffer;
	Attach(&OKBuffer, pSrcBuf);
	int nSize = (int)arCurveHigh.size();
	for (int nCol = 0; nCol < nSize; nCol++)
	{
		int nHiEndRow = arCurveHigh[nCol];
		int nLowEndRow = arCurveLow[nCol];
		int nRow = 0;
		for (nRow = 0; nRow <= nHiEndRow + 10; nRow++)
		{

			DWORD dwPoint[3][3];
			DWORD dwOriginClr = *(OKBuffer.m_dwBuffer + nRow * pSrcBuf->m_nWidth + nCol);
			dmemset(dwPoint, dwOriginClr, 9);

			if (nRow >= 1) {

				if (nCol >= 1)
					dwPoint[0][0] = *(pSrcBuf->m_dwBuffer + (nRow - 1) * pSrcBuf->m_nWidth + nCol - 1);

				dwPoint[0][1] = *(pSrcBuf->m_dwBuffer + (nRow - 1) * pSrcBuf->m_nWidth + nCol);

				if (nCol <= (OKBuffer.m_nWidth - 1) - 1)
					dwPoint[0][2] = *(pSrcBuf->m_dwBuffer + (nRow - 1) * pSrcBuf->m_nWidth + nCol + 1);

			}
			if (nCol >= 1)
				dwPoint[1][0] = *(pSrcBuf->m_dwBuffer + (nRow)* pSrcBuf->m_nWidth + nCol - 1);

			dwPoint[1][1] = *(pSrcBuf->m_dwBuffer + (nRow)* pSrcBuf->m_nWidth + nCol);

			if (nCol <= (OKBuffer.m_nWidth - 1) - 1)
				dwPoint[1][2] = *(pSrcBuf->m_dwBuffer + (nRow)* pSrcBuf->m_nWidth + nCol + 1);

			if (nRow <= (pSrcBuf->m_nHeight - 1) - 1) {

				if (nCol >= 1)
					dwPoint[2][0] = *(pSrcBuf->m_dwBuffer + (nRow + 1) * pSrcBuf->m_nWidth + nCol - 1);

				dwPoint[2][1] = *(pSrcBuf->m_dwBuffer + (nRow + 1) * pSrcBuf->m_nWidth + nCol);

				if (nCol <= (OKBuffer.m_nWidth - 1) - 1)
					dwPoint[2][2] = *(pSrcBuf->m_dwBuffer + (nRow + 1) * pSrcBuf->m_nWidth + nCol + 1);
			}
			*(OKBuffer.m_dwBuffer + nRow * pSrcBuf->m_nWidth + nCol) = btEqColor(dwPoint);
		}
		for (nRow = nLowEndRow - 10; nRow< OKBuffer.m_nHeight; nRow++)
		{
			DWORD dwPoint[3][3];
			DWORD dwOriginClr = *(OKBuffer.m_dwBuffer + nRow * pSrcBuf->m_nWidth + nCol);
			dmemset(dwPoint, dwOriginClr, 9);

			if (nRow >= 1) {

				if (nCol >= 1)
					dwPoint[0][0] = *(pSrcBuf->m_dwBuffer + (nRow - 1) * pSrcBuf->m_nWidth + nCol - 1);

				dwPoint[0][1] = *(pSrcBuf->m_dwBuffer + (nRow - 1) * pSrcBuf->m_nWidth + nCol);

				if (nCol <= (OKBuffer.m_nWidth - 1) - 1)
					dwPoint[0][2] = *(pSrcBuf->m_dwBuffer + (nRow - 1) * pSrcBuf->m_nWidth + nCol + 1);

			}
			if (nCol >= 1)
				dwPoint[1][0] = *(pSrcBuf->m_dwBuffer + (nRow)* pSrcBuf->m_nWidth + nCol - 1);

			dwPoint[1][1] = *(pSrcBuf->m_dwBuffer + (nRow)* pSrcBuf->m_nWidth + nCol);

			if (nCol <= (OKBuffer.m_nWidth - 1) - 1)
				dwPoint[1][2] = *(pSrcBuf->m_dwBuffer + (nRow)* pSrcBuf->m_nWidth + nCol + 1);

			if (nRow <= (pSrcBuf->m_nHeight - 1) - 1) {

				if (nCol >= 1)
					dwPoint[2][0] = *(pSrcBuf->m_dwBuffer + (nRow + 1) * pSrcBuf->m_nWidth + nCol - 1);

				dwPoint[2][1] = *(pSrcBuf->m_dwBuffer + (nRow + 1) * pSrcBuf->m_nWidth + nCol);

				if (nCol <= (OKBuffer.m_nWidth - 1) - 1)
					dwPoint[2][2] = *(pSrcBuf->m_dwBuffer + (nRow + 1) * pSrcBuf->m_nWidth + nCol + 1);
			}

			*(OKBuffer.m_dwBuffer + nRow * pSrcBuf->m_nWidth + nCol) = btEqColor(dwPoint);
		}
	}
}

