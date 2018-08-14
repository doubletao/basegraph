#pragma once
#include "Common\BufferCommonDef.h"
//////////////////////////////////////////////////////////////////////////
struct CommonBufferDef32
{
	DWORD*	m_dwBuffer;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nPitch;
	RECT	m_rtBound;
	int		m_nOriginX;
	int		m_nOriginY;
	int		m_nMaxSize;
	float	m_fScale;
	CommonBufferDef32()
	{
		memset(this, 0, sizeof(CommonBufferDef32));
	}
};
struct CommonBufferDef8
{
	BYTE*	m_pMaskBuffer; 
	int		m_nWidth;
	int		m_nHeight;
	int		m_nPitch;
	RECT	m_rtBound;
	int		m_nOriginX;
	int		m_nOriginY;
	float	m_fScale;
	CommonBufferDef8()
	{
		memset(this, 0, sizeof(CommonBufferDef8));
	}
};
BASEGRAPH_COMMON_API void CommonBufferDef32_Init(CommonBufferDef32* pBuffer32);
BASEGRAPH_COMMON_API void CommonBufferDef32_Release(CommonBufferDef32* pBuffer32);
BASEGRAPH_COMMON_API void CommonBufferDef32_Reset(CommonBufferDef32* pBuffer32, int nWidth, int nHeight);
BASEGRAPH_COMMON_API void CommonBufferDef32_CopyParam(CommonBufferDef32* pDes, CommonBufferDef32* pSrc);
BASEGRAPH_COMMON_API BOOL CommonBufferDef32_Compare(CommonBufferDef32* pBuff1, CommonBufferDef32* pBuff2);

class  BASEGRAPH_COMMON_API CCommonBuffer:public CommonBufferDef32
{
public:
	CCommonBuffer();
	CCommonBuffer(int nWidth, int nHeight,LPDWORD lpData);
	CCommonBuffer(int nWidth, int nHeight);
	CCommonBuffer(CommonBufferDef32 *pBuffer);

	~CCommonBuffer();
protected:
	CRITICAL_SECTION m_Lock;
	//是否已经被使用
	BOOL	m_bUsed;
	BOOL	m_bAutoDelete;
	//是否需要把内容渲染到屏幕上
	BOOL	m_bNeedRender;
	BOOL	m_bClearBuffer;
	//需要更新的脏矩形
	RECT	m_rtDirty;
	RECT	m_rtOldDirty;
public:
	BOOL	Create(int nWidth,int nHeight);
	BOOL	Create(int nSize);
	BOOL	Release();
	void	Resize(int nWidth,int nHeight);
	void	Clear( DWORD color,RECT* pRect);
	//Set operation buffer
	BOOL	Attach(CommonBufferDef32 *pBuffer);
	void	SetAutoDelete(BOOL bDelete){m_bAutoDelete = bDelete;};
	BOOL	IsNeedRender(){return m_bNeedRender;};
	void	SetNeedRender(BOOL bNeedRender){m_bNeedRender = bNeedRender;};
	BOOL	IsClearBuffer(){return m_bClearBuffer;};
	void	SetNeedClear(BOOL bClear){m_bClearBuffer = bClear;};
	void	SetOffset(int nOffsetX,int nOffsetY){m_nOriginX = nOffsetX;m_nOriginY = nOffsetY;};
	void	GetOffset(int& nOffsetX,int& nOffsetY){nOffsetX = m_nOriginX; nOffsetY = m_nOriginY;};

	void	SetScale(float fScale){m_fScale = fScale;};
	float	GetScale(){return m_fScale;};

	BOOL	Query(BOOL bNeedRender = TRUE);
	void	Lock();
	void	UnLock();
	void	UnQuery();

	BOOL	IsUsed(){return m_bUsed;};	
	void	SetUsed();
	void	UnsetUsed();

	void	SetDirtyAll();
	void	ClearDirty(){m_rtOldDirty = m_rtDirty;memset(&m_rtDirty,0,sizeof(RECT));}
	void	SetDirty(RECT rtDirty);
	RECT	GetDirty(){return m_rtDirty;}
	RECT	GetOldDirty(){return m_rtOldDirty;}

public:
	BOOL	BitBlt(	long desx,long desy,long deswidth,long desheight,
		CommonBufferDef32* pBuffer,long srcx, long srcy, long srcwidth , 
		long srcheight,  IN BUFFER_MODE eMode,BUFFER_PARITY_STYLE fieldFlag,RECT* lpDesCrop);

	//Copy pBuffer to self Buffer, fieldFlag is the field of self target Buffer
	BOOL	BitBlt(	RECT*	rtDes,CommonBufferDef32*	pBuffer,RECT* rtSrc,
		IN BUFFER_MODE eMode,BUFFER_PARITY_STYLE fieldFlag,RECT* lpDesCrop);

	// Copy pBuffer to DC
	BOOL	BitBltToDC(HDC hDC,LPCRECT lprtDes = NULL,LPCRECT lprtSrc = NULL,DWORD dwFlags = SRCCOPY);
	//Stretch to copy pBuffer to
	BOOL	StretchBltToDC(HDC hDC,LPCRECT lprtDes = NULL,LPCRECT lprtSrc = NULL,DWORD dwFlags = SRCCOPY);
	//Mix one color value to self Buffer, fieldFlag is the field of self target Buffer
	void	MixColor(DWORD color = 0xff000000,RECT* lprtMixRect = NULL,DWORD dwFlags = BUFFER_COPY,BUFFER_PARITY_STYLE fieldFlag = BUFFER_PARITY_BOTH);
	// Reverse Buffer Key 
	void	ReverseKey();
	//If same color value, clear all self Buffer, fieldFlag is the field of self target Buffer
	void    ClearAll(DWORD color = 0,RECT* rtRect = NULL,BUFFER_PARITY_STYLE fieldFlag = BUFFER_PARITY_BOTH);
	//Write colorbar to self Buffer，fieldFlag is the field of self target Buffer
	void	ColorBar(RECT* lprtScreen = NULL);
	//
	void	Empty();

	UINT GetSerializeSize();
	char * GetSerializeBuffer();
	int RestoreFromSerializeBuffer(char * pBuffer);
};
