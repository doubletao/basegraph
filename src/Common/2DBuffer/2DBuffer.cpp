#include "stdafx.h"
#include "..\BufferRipRender.h"
#include "Common\2DBuffer.h"
#include "Common\2DBufferAPI.h"
#include "Common\ColorDef.h"
#include "Common\PlaneDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////
void CommonBufferDef32_Init(CommonBufferDef32* pBuffer32)
{
	memset(pBuffer32,0,sizeof(CommonBufferDef32));
}

void CommonBufferDef32_Release(CommonBufferDef32* pBuffer32)
{
	if (pBuffer32->m_dwBuffer)
		delete[] pBuffer32->m_dwBuffer;
	memset(pBuffer32,0,sizeof(CommonBufferDef32));
}

void CommonBufferDef32_Reset(CommonBufferDef32* pBuffer32, int nWidth, int nHeight)
{
	if (pBuffer32->m_nWidth != nWidth
		|| pBuffer32->m_nHeight != nHeight)
	{
		if(pBuffer32->m_dwBuffer)
		{
			delete[] pBuffer32->m_dwBuffer;
			pBuffer32->m_dwBuffer = NULL;
		}

		int nNewWxH = nWidth * nHeight;
		pBuffer32->m_nWidth  = nWidth;
		pBuffer32->m_nHeight = nHeight;
		pBuffer32->m_nPitch = (nWidth << 2);
		SetRect(&pBuffer32->m_rtBound,0,0,nWidth,nHeight);
		pBuffer32->m_nOriginX = 0;
		pBuffer32->m_nOriginY = 0;
		pBuffer32->m_nMaxSize = nNewWxH;

		if (nNewWxH > 0)
			pBuffer32->m_dwBuffer = new DWORD[nNewWxH];
	}
}

void CommonBufferDef32_CopyParam(CommonBufferDef32* pDes, CommonBufferDef32* pSrc)
{
	pDes->m_nWidth = pSrc->m_nWidth;
	pDes->m_nHeight = pSrc->m_nHeight;
	pDes->m_nMaxSize = pSrc->m_nMaxSize;
	pDes->m_rtBound = pSrc->m_rtBound;
	pDes->m_nOriginX = pSrc->m_nOriginX;
	pDes->m_nOriginY = pSrc->m_nOriginY;
	pDes->m_nPitch = pSrc->m_nPitch;
	pDes->m_fScale = pSrc->m_fScale;
}

BASEGRAPH_COMMON_API BOOL CommonBufferDef32_Compare(CommonBufferDef32* pBuff1, CommonBufferDef32* pBuff2)
{
	if (pBuff1 == NULL || pBuff2 == NULL)
	{
		return FALSE;
	}
	if (pBuff1->m_nWidth != pBuff2->m_nWidth ||
		pBuff1->m_nHeight != pBuff2->m_nHeight ||
		pBuff1->m_nMaxSize != pBuff2->m_nMaxSize ||
		!EqualRect(&pBuff1->m_rtBound, &pBuff2->m_rtBound) ||
		pBuff1->m_nOriginX != pBuff2->m_nOriginX ||
		pBuff1->m_nOriginY != pBuff2->m_nOriginY ||
		pBuff1->m_nPitch != pBuff2->m_nPitch ||
		pBuff1->m_fScale != pBuff2->m_fScale)
	{
		return FALSE;
	}
	if (memcmp(pBuff1->m_dwBuffer, pBuff2->m_dwBuffer, pBuff1->m_nWidth * pBuff1->m_nHeight) != 0)
	{
		return FALSE;
	}
	return TRUE;
}

///操作buffer
CCommonBuffer::CCommonBuffer()
{
	m_bUsed = FALSE;
	m_dwBuffer = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nOriginX = 0;
	m_nOriginY = 0;
	m_nMaxSize = 0;
	m_nPitch = m_nWidth * sizeof(DWORD);
	m_fScale = 1;
	m_rtBound.left = 0;
	m_rtBound.top = 0;
	m_rtBound.right = 0;
	m_rtBound.bottom = 0;
	m_bNeedRender = FALSE;
	m_bClearBuffer = TRUE;
	m_bAutoDelete = FALSE;
	memset(&m_rtOldDirty,0,sizeof(RECT));
	memset(&m_rtDirty,0,sizeof(RECT));
	InitializeCriticalSection(&m_Lock);

}
CCommonBuffer::CCommonBuffer(int nWidth, int nHeight,LPDWORD lpData)
{
	m_dwBuffer = lpData;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nPitch = m_nWidth* sizeof(DWORD);
	m_fScale = 1;
	m_rtBound.left = 0;
	m_rtBound.top = 0;
	m_rtBound.right = m_nWidth;
	m_rtBound.bottom = m_nHeight;
	m_nOriginX = 0;
	m_nOriginY = 0;
	m_nMaxSize = m_nWidth * m_nHeight;

	m_bUsed = FALSE;

	m_bNeedRender = FALSE;
	m_bClearBuffer = TRUE;
	m_bAutoDelete = FALSE;
	memset(&m_rtOldDirty,0,sizeof(RECT));
	memset(&m_rtDirty,0,sizeof(RECT));
	InitializeCriticalSection(&m_Lock);

}
CCommonBuffer::CCommonBuffer(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nPitch = m_nWidth << 2;
	m_fScale = 1;
	m_rtBound.left = 0;
	m_rtBound.top = 0;
	m_rtBound.right = m_nWidth;
	m_rtBound.bottom = m_nHeight;
	m_nOriginX = 0;
	m_nOriginY = 0;
	m_nMaxSize = m_nWidth * m_nHeight;
	m_dwBuffer = new DWORD[m_nMaxSize];
	memset(m_dwBuffer,0,m_nMaxSize * sizeof(DWORD));

	m_bUsed = FALSE;

	m_bNeedRender = FALSE;
	m_bClearBuffer = TRUE;
	m_bAutoDelete = TRUE;
	memset(&m_rtOldDirty,0,sizeof(RECT));
	memset(&m_rtDirty,0,sizeof(RECT));
	InitializeCriticalSection(&m_Lock);
}

void CCommonBuffer::Resize(int nWidth,int nHeight)
{
	CommonBufferDef32_Reset(this,nWidth,nHeight);
}

CCommonBuffer::CCommonBuffer(CommonBufferDef32 *pBuffer)
{
	m_nWidth			= pBuffer->m_nWidth;
	m_nHeight			= pBuffer->m_nHeight;
	m_nPitch			= pBuffer->m_nPitch;
	m_fScale			= pBuffer->m_fScale;
	m_rtBound			= pBuffer->m_rtBound;
	m_nOriginX			= pBuffer->m_nOriginX;
	m_nOriginY			= pBuffer->m_nOriginY;
	m_nMaxSize			= pBuffer->m_nMaxSize;
	m_dwBuffer			= pBuffer->m_dwBuffer;

	m_bUsed = FALSE;

	m_bNeedRender = FALSE;
	m_bClearBuffer = TRUE;
	m_bAutoDelete = FALSE;
	memset(&m_rtOldDirty,0,sizeof(RECT));
	memset(&m_rtDirty,0,sizeof(RECT));
	InitializeCriticalSection(&m_Lock);
}

CCommonBuffer::~CCommonBuffer()
{
	Release();
	DeleteCriticalSection(&m_Lock);
}
void	CCommonBuffer::Lock()
{
	EnterCriticalSection(&m_Lock);
}
void	CCommonBuffer::UnLock()
{
	LeaveCriticalSection(&m_Lock);
}
BOOL	CCommonBuffer::Create(int nSize)
{
	if(m_dwBuffer)
		Release();
	Lock();
	m_nWidth = nSize;
	m_nHeight = 1;
	m_nMaxSize = nSize;
	m_dwBuffer = new DWORD[nSize];
	m_bAutoDelete = TRUE;
	if(!m_dwBuffer)
	{
		m_nWidth = 0;
		m_nHeight = 0;
		UnLock();
		return FALSE;
	}
	UnLock();
	return TRUE;
}
BOOL	CCommonBuffer::Create(int nWidth,int nHeight)
{
	int nOldSize = m_nWidth * m_nHeight;
	int nSize = nWidth * nHeight;

	if(m_dwBuffer && (nSize != nOldSize))
	{
		Release();
	}

	Lock();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nMaxSize = m_nWidth*m_nHeight;
	m_nPitch = (m_nWidth << 2);

	m_rtBound.left = 0;
	m_rtBound.top = 0;
	m_rtBound.right = m_nWidth;
	m_rtBound.bottom = m_nHeight;

	if(!m_dwBuffer && m_nMaxSize>0)
		m_dwBuffer = new DWORD[m_nMaxSize];
	
	if(nSize > 1327000)
		int a = 0;

	m_bAutoDelete = TRUE;
	if(!m_dwBuffer)
	{
		m_nWidth = 0;
		m_nHeight = 0;
		UnLock();
		return FALSE;
	}
	UnLock();
	return TRUE;
}
BOOL	CCommonBuffer::Release()
{
	Lock();
	if(m_bAutoDelete&&m_dwBuffer)
	{
		delete[] m_dwBuffer;
		m_dwBuffer = NULL;
	}
	m_nWidth = 0;
	m_nHeight = 0;
	UnLock();
	return TRUE;
}

void CCommonBuffer::Clear( DWORD color ,RECT* pRect)
{
	Lock();
	if(!pRect)
	{
		Common_DMemSet(m_dwBuffer,color,m_nWidth * m_nHeight);

//		m_rtOldDirty = m_rtDirty;
		m_rtDirty.left = 0;
		m_rtDirty.top = 0;
		m_rtDirty.right = m_nWidth;
		m_rtDirty.bottom = m_nHeight;
	}
	else
	{
		BGRA32_FillRect(m_dwBuffer,
			pRect->left,
			pRect->top,
			pRect->right - pRect->left,
			pRect->bottom - pRect->top,
			m_nWidth,
			m_nHeight,
			NULL,color);
//		m_rtOldDirty = m_rtDirty;
		m_rtDirty = *pRect;
	}
	UnLock();
}

void CCommonBuffer::SetUsed()
{
	Lock();
	m_bUsed = TRUE;
	UnLock();
}
void CCommonBuffer::UnsetUsed()
{
	Lock();
	m_bUsed = FALSE;
	UnLock();
}
void CCommonBuffer::SetDirty(RECT rtDirty)
{
	//UnionRect(&m_rtDirty, &rtDirty, &m_rtDirty);
//	m_rtOldDirty = m_rtDirty;
	m_rtDirty = rtDirty;
}

void CCommonBuffer::SetDirtyAll()
{
	Lock();

//	m_rtOldDirty = m_rtDirty;
	m_rtDirty.left = 0;
	m_rtDirty.top = 0;
	m_rtDirty.right = m_nWidth;
	m_rtDirty.bottom = m_nHeight;
	
	UnLock();
}

BOOL	CCommonBuffer::Query(BOOL bNeedRender)
{
	Lock();
	if(!m_bUsed)
	{
		//		memset(m_dwBuffer,0,sizeof(DWORD)*m_nWidth*m_nHeight);
		m_bNeedRender = bNeedRender;
		m_bUsed = TRUE;
		UnLock();
		return TRUE;
	}
	UnLock();
	return FALSE;
}

void	CCommonBuffer::UnQuery()
{
	Lock();
	m_bNeedRender = FALSE;
	m_bUsed = FALSE;
	UnLock();
	if(m_bAutoDelete)
		delete this;
}


BOOL	CCommonBuffer::BitBlt(	
						  long desx,
						  long desy,
						  long deswidth,
						  long desheight,
						  CommonBufferDef32* pBuffer,
						  long srcx, 
						  long srcy, 
						  long srcwidth , 
						  long srcheight,  
						  IN BUFFER_MODE eMode,
						  BUFFER_PARITY_STYLE fieldFlag,
						  RECT* lpDesCrop)
{
	
	Common_BitBlt(m_dwBuffer,
		desx,
		desy,
		deswidth,
		desheight,
		m_nWidth,
		m_nHeight,
		pBuffer->m_dwBuffer,
		srcx, 
		srcy, 
		srcwidth, 
		srcheight,  
		pBuffer->m_nWidth,
		pBuffer->m_nHeight,
		eMode,
		fieldFlag,
		lpDesCrop);
	return 1;
}

//Copy pBuffer to self Buffer, fieldFlag is the field of self target Buffer
BOOL	CCommonBuffer::BitBlt(	
						  RECT*	rtDes,
						  CommonBufferDef32*	pBuffer,
						  RECT* rtSrc,
						  IN BUFFER_MODE eMode,
						  BUFFER_PARITY_STYLE fieldFlag,
						  RECT* lpDesCrop)
{
	return BitBlt(
		rtDes->left,
		rtDes->top,
		rtDes->right - rtDes->left,
		rtDes->bottom - rtDes->top,
		pBuffer,
		rtSrc->left,
		rtSrc->top,
		rtSrc->right - rtSrc->left,
		rtSrc->bottom - rtSrc->top,
		eMode,
		fieldFlag,
		lpDesCrop
		);
}
/*********************************	inline function ***********************************/


// 彩条数目
#define	RB_ColorBarColorNumber	8

// 彩条色值
static	DWORD	RB_dwColorBarColor[] =
{
	ARGB_BLACK+255*256*256+255*256+255,
	ARGB_BLACK+256*256*191+256*191,
	ARGB_BLACK+256*191+191,
	ARGB_BLACK+256*191,
	ARGB_BLACK+256*256*191+191,
	ARGB_BLACK+256*256*191,
	ARGB_BLACK+191,
	ARGB_BLACK
};

// Copy pBuffer to DC
// BITMAPINFO 结构常用值
#define	RB_Use_BmpInfo(Width, Height)	\
	BITMAPINFO  BmpInfo;									\
	BmpInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);	\
	BmpInfo.bmiHeader.biWidth  = Width;						\
	BmpInfo.bmiHeader.biHeight = Height;					\
	BmpInfo.bmiHeader.biPlanes = 1;							\
	BmpInfo.bmiHeader.biBitCount = 32;						\
	BmpInfo.bmiHeader.biCompression = BI_RGB;				\
	BmpInfo.bmiHeader.biSizeImage = 0;						\
	BmpInfo.bmiHeader.biXPelsPerMeter = 0;					\
	BmpInfo.bmiHeader.biYPelsPerMeter = 0;					\
	BmpInfo.bmiHeader.biClrUsed = 0;						\
	BmpInfo.bmiHeader.biClrImportant = 0					\


//空行必须留
//////////////////////////////////////////////////////////////////////////
BOOL	CCommonBuffer::BitBltToDC(
							  HDC	hDC,
							  LPCRECT lprtDes,
							  LPCRECT lprtSrc,
							  DWORD dwFlags)
{
	RECT rtSrc = {0,0,m_nWidth, m_nHeight};
	RECT rtDest = {0, 0 , m_nWidth, m_nHeight };

	if( lprtSrc )
	{
		::IntersectRect( &rtSrc, &rtSrc, lprtSrc );
	}

	if( lprtDes )
		rtDest = *lprtDes;

	RB_Use_BmpInfo(m_nPitch/4, -m_nHeight);

	const DWORD* pbuf = m_dwBuffer+rtSrc.top*m_nWidth;

	::SetDIBitsToDevice(
		hDC,				// handle of device context 

		rtDest.left,			// x-coordinate of upper-left corner of dest. rect. 
		rtDest.top,			// y-coordinate of upper-left corner of dest. rect. 

		rtSrc.right - rtSrc.left,
		rtSrc.bottom - rtSrc.top,
		rtSrc.left,
		rtSrc.top,

		rtSrc.top,			// first scan line in array 
		rtSrc.bottom - rtSrc.top,		// number of scan lines 
		pbuf,				// address of array with DIB bits 
		&BmpInfo,			// address of structure with bitmap info. 
		DIB_RGB_COLORS 		// RGB or palette indices 
		);

	return 1;
}

BOOL CCommonBuffer::StretchBltToDC(
	HDC	hDC,
	LPCRECT lprtDes /*= NULL*/,
	LPCRECT lprtSrc /*= NULL*/,
	DWORD	dwFlags )
{
	RECT rtDes;
	if(lprtDes)
	{
		rtDes = *lprtDes;
	}
	else
	{
		rtDes.left = 0;
		rtDes.top = 0;
		rtDes.right = GetDeviceCaps(hDC, HORZRES);
		rtDes.bottom = GetDeviceCaps(hDC, VERTRES);
	}

	RECT rtSrc;
	if(lprtSrc)
	{
		rtSrc = *lprtSrc;
	}
	else
	{
		rtSrc.left = 0;
		rtSrc.top = 0;
		rtSrc.right = m_nWidth;
		rtSrc.bottom = m_nHeight;
	}

	if(rtSrc.right - rtSrc.left == rtDes.right - rtDes.left &&
		rtSrc.bottom - rtSrc.top == rtDes.bottom - rtDes.top)
		return	BitBltToDC(hDC, &rtDes, &rtSrc);

	RECT rtSrcTemp = {0, 0 , m_nWidth, m_nHeight };
	::IntersectRect( &rtSrc, &rtSrc, &rtSrcTemp );

	RB_Use_BmpInfo(m_nPitch/4, m_nHeight);


	int iStretchModeOld = SetStretchBltMode(hDC, COLORONCOLOR);
	::StretchDIBits(
		hDC,				// handle of device context 

		rtDes.left,			// x-coordinate of upper-left corner of dest. rect. 
		rtDes.top,			// y-coordinate of upper-left corner of dest. rect. 
		rtDes.right - rtDes.left,		// width of destination rectangle 
		rtDes.bottom - rtDes.top,		// height of destination rectangle 

		rtSrc.left,			// x-coordinate of lower-left corner of source rect. 
		rtSrc.bottom,		// y-coordinate of lower-left corner of source rect. 
		rtSrc.right - rtSrc.left,	// source rectangle width 
		rtSrc.top - rtSrc.bottom,	// source rectangle height 

		m_dwBuffer,			// address of array with DIB bits 
		&BmpInfo,			// address of structure with bitmap info. 
		DIB_RGB_COLORS,		// RGB or palette indices 

		dwFlags				// raster operation code 
		);
	::SetStretchBltMode(hDC, iStretchModeOld);
	return	TRUE;
}

//Mix one color value to self Buffer, fieldFlag is the field of self target Buffer
void	CCommonBuffer::MixColor(DWORD color,RECT* lprtMixRect,DWORD dwFlags,BUFFER_PARITY_STYLE fieldFlag)
{

}
//#define COLORABGR_A(pcr)  *((BYTE*)pcr + 4)
// Reverse Buffer Key 
void	CCommonBuffer::ReverseKey()
{
	if(	m_dwBuffer == NULL ) 
		return;

	LPDWORD pDes = m_dwBuffer;
	int nCount = m_nWidth * m_nHeight;
	for (int i = 0; i < nCount; i++)
	{
		COLORABGR_A(*pDes) = 0xFF - COLORABGR_A(*pDes);
		pDes ++;
	}
	return;
}

//If same color value, clear all self Buffer, fieldFlag is the field of self target Buffer
void    CCommonBuffer::ClearAll(DWORD color,RECT* rtRect,BUFFER_PARITY_STYLE fieldFlag )
{
	if(m_nWidth <= 0 || m_nHeight <=0 || m_nPitch <= 0 || m_dwBuffer == NULL)
		return;

	LPDWORD lpDes = m_dwBuffer;
	if(rtRect == NULL && fieldFlag == BUFFER_PARITY_BOTH)
	{

		for(int y = 0; y < m_nHeight; y++,lpDes += m_nWidth)
			MMX_FastMemset(lpDes, color, m_nWidth);
		return;
	}

	BaseRect rtTemp;
	if(rtRect == NULL)
		BaseRect_SetRect(&rtTemp,0,0,m_nWidth,m_nHeight);		
	else
		BaseRect_SetRect(&rtTemp,rtRect->left,rtRect->top,rtRect->right,rtRect->bottom);				

	int yStart = 0, yEnd = 0;
	int nLineStep = 1;
	BaseRect  rtSrc;
	BaseRect_SetRect(&rtSrc,0,0,m_nWidth,m_nHeight);
	BaseRect_IntersectRect(&rtSrc,&rtSrc,&rtTemp);		
	if(BaseRect_IsRectEmpty(&rtSrc)) 
		return;

	yStart = rtSrc.top;
	if(fieldFlag == BUFFER_PARITY_ODD || fieldFlag == BUFFER_PARITY_EVEN)
	{
		nLineStep = 2;
	}

	if(fieldFlag == BUFFER_PARITY_ODD)
	{
		if(yStart&0x01)
		{
		}
		else
			yStart++;
	}
	else if(fieldFlag == BUFFER_PARITY_EVEN)
	{
		if(yStart&0x01)
			yStart++;
	}

	lpDes = m_dwBuffer +  yStart* m_nWidth + rtSrc.left; //rtSrc.top
	int nClearWidth  = BaseRect_Width (&rtSrc);
	int nClearHeight = BaseRect_Height(&rtSrc);

	int nWidthStep = m_nWidth * nLineStep;

	yEnd = yStart + nClearHeight;
	if(yEnd > m_nHeight)
		yEnd = m_nHeight ;

	for(int y = yStart; y < yEnd; y += nLineStep,lpDes += nWidthStep)
		MMX_FastMemset(lpDes, color, nClearWidth);

}

//Write colorbar to self Buffer，fieldFlag is the field of self target Buffer
void	CCommonBuffer::ColorBar(RECT* lprtScreen)
{
	if(m_nWidth <= 0 || m_nHeight <=0 || m_nPitch <= 0 || m_dwBuffer == NULL)
		return;

	int	x, y, xSize, ySize;

	if(lprtScreen)
	{
		x = lprtScreen->left;
		y = lprtScreen->top;
		xSize = abs(lprtScreen->right-lprtScreen->left);
		ySize = abs(lprtScreen->bottom-lprtScreen->top);
	}
	else
	{
		x = 0;
		y = 0;
		xSize = m_nWidth;
		ySize = m_nHeight;
	}

	int	nRightExtra = xSize%RB_ColorBarColorNumber;
	xSize = xSize/RB_ColorBarColorNumber;

	DWORD	color;
	RECT	rtColor;
	rtColor.left   = x;
	rtColor.top    = y;
	rtColor.right  = x+xSize;
	rtColor.bottom = y+ySize;

	for(int i=0; i<RB_ColorBarColorNumber; i++)
	{
		if(i == RB_ColorBarColorNumber-1)
			rtColor.right += nRightExtra;

		color = RB_dwColorBarColor[i];
		ClearAll(color,&rtColor);
		rtColor.left += xSize;		
	}
}

void	CCommonBuffer::Empty()
{

}

UINT CCommonBuffer::GetSerializeSize()
{
	UINT nSizeRet = 0;

	int nBufferSize = m_nWidth * m_nHeight * sizeof(DWORD);

	nSizeRet += sizeof(nBufferSize);
	nSizeRet += nBufferSize;
	nSizeRet += sizeof(m_nWidth);
	nSizeRet += sizeof(m_nHeight);
	nSizeRet += sizeof(m_nPitch);
	nSizeRet += sizeof(m_rtBound);
	nSizeRet += sizeof(m_nOriginX);
	nSizeRet += sizeof(m_nOriginY);
	nSizeRet += sizeof(m_nMaxSize);
	nSizeRet += sizeof(m_fScale);

	return nSizeRet;
}

char * CCommonBuffer::GetSerializeBuffer()
{
	if (m_dwBuffer == NULL)
	{
		return NULL;
	}
	UINT nWholeSize = GetSerializeSize();
	char * pBuffer = new char[nWholeSize];

	int nPos = 0;
	int nSize = 0;
	int nBufferSize = m_nWidth * m_nHeight;

	nSize = sizeof(nBufferSize);
	memcpy(pBuffer + nPos, &nBufferSize, nSize);
	nPos += nSize;
	nSize = nBufferSize * sizeof(DWORD);
	memcpy(pBuffer + nPos, m_dwBuffer, nSize);
	nPos += nSize;
	nSize = sizeof(m_nWidth);
	memcpy(pBuffer + nPos, &m_nWidth, nSize);
	nPos += nSize;
	nSize = sizeof(m_nHeight);
	memcpy(pBuffer + nPos, &m_nHeight, nSize);
	nPos += nSize;
	nSize = sizeof(m_nPitch);
	memcpy(pBuffer + nPos, &m_nPitch, nSize);
	nPos += nSize;
	nSize = sizeof(m_rtBound);
	memcpy(pBuffer + nPos, &m_rtBound, nSize);
	nPos += nSize;
	nSize = sizeof(m_nOriginX);
	memcpy(pBuffer + nPos, &m_nOriginX, nSize);
	nPos += nSize;
	nSize = sizeof(m_nOriginY);
	memcpy(pBuffer + nPos, &m_nOriginY, nSize);
	nPos += nSize;
	nSize = sizeof(m_nMaxSize);
	memcpy(pBuffer + nPos, &m_nMaxSize, nSize);
	nPos += nSize;
	nSize = sizeof(m_fScale);
	memcpy(pBuffer + nPos, &m_fScale, nSize);
	nPos += nSize;
	return pBuffer;
}

int CCommonBuffer::RestoreFromSerializeBuffer(char * pBuffer)
{
	ASSERT(m_dwBuffer == NULL);

	int nPos = 0;

	int nBufferSize = 0;
	int nSize = 0;

	nSize = sizeof(nBufferSize);
	memcpy(&nBufferSize, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = nBufferSize * sizeof(DWORD);
	m_dwBuffer = new DWORD[nBufferSize];
	memcpy(m_dwBuffer, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_nWidth);
	memcpy(&m_nWidth, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_nHeight);
	memcpy(&m_nHeight, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_nPitch);
	memcpy(&m_nPitch, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_rtBound);
	memcpy(&m_rtBound, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_nOriginX);
	memcpy(&m_nOriginX, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_nOriginY);
	memcpy(&m_nOriginY, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_nMaxSize);
	memcpy(&m_nMaxSize, pBuffer + nPos, nSize);
	nPos += nSize;
	nSize = sizeof(m_fScale);
	memcpy(&m_fScale, pBuffer + nPos, nSize);
	nPos += nSize;

	return nPos;
}
