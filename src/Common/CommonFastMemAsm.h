#ifndef	_INC_SYPLGINMGRASM_WIN64
#define	_INC_SYPLGINMGRASM_WIN64
#ifdef __cplusplus
extern "C" 
{
#endif
void		asmX64_dmemset( void *buffer, int value, int num );
void		asmX64_dmemcpy(LPDWORD lpdwDes,LPDWORD lpdwSrc,int nSize);
void		asmX64_SSE2_FastMemset(LPDWORD lpdwDes,DWORD	dwColor,int nSize);

void		asmX64_MMX_LightDeal(/*[in]*/	LPDWORD		lpdwDes,
								/*[in]*/	LPDWORD		lpdwSrc,
								/*[in]*/	LPBYTE		lpLightMask,
								/*[in]*/	LPDWORD		lpLightTrans,	// 定长 256
								/*[in]*/	int			nWidth);

DWORD	asmX64_GetTrans(DWORD	dwFactor,DWORD	dwTrans);
extern	LPDWORD	lpdwColorMixCalTable;
DWORD	asmX64_MMX_FastColorMix( IN	DWORD clB,IN	DWORD clW);
#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////
inline  void dmemcpy(/* [in] */	LPDWORD	lpdwDes,
					/* [in] */	LPDWORD	lpdwSrc,
					/* [in] */	int		nSize)
{
	asmX64_dmemcpy(lpdwDes,lpdwSrc,nSize);
}

//MMX 快速 DSet (只对PCI板卡有加速作用)
inline  void	MMX_FastMemset(IN	LPDWORD	lpdwDes,// 目标内存指针
						IN	DWORD	dwColor,	// 欲设色值
						IN	int		nSize)		// 点个数(以 DWORD 为单位)
{
	asmX64_SSE2_FastMemset(lpdwDes,dwColor,nSize);
}

//SSE2_BEGIN
inline void	SSE2_FastMemset(/* [in ] */	LPDWORD	lpdwDes,// 目标内存指针
						/* [in ] */	DWORD	dwColor,	// 欲设色值
						/* [in ] */	int		nSize)		// 点个数(以 DWORD 为单位)
{
	asmX64_SSE2_FastMemset(lpdwDes,dwColor,nSize);
}

inline void	MMX_LightDeal(/*[in]*/	LPDWORD		lpdwDes,
						/*[in]*/	LPDWORD		lpdwSrc,
						/*[in]*/	LPBYTE		lpLightMask,
						/*[in]*/	LPDWORD		lpLightTrans,	// 定长 256
						/*[in]*/	int			nWidth)
{
	asmX64_MMX_LightDeal(lpdwDes,lpdwSrc,lpLightMask,lpLightTrans,nWidth);
}

inline DWORD	GetTrans(DWORD	dwFactor,DWORD	dwTrans)
{
	asmX64_GetTrans(dwFactor,dwTrans);
	return dwTrans;
}

inline void	dmemset( void *buffer, int value, int num )
{
	return asmX64_dmemset(buffer,value,num);
}

/* Color Mix 精确计算
A = AB+AW-AB*AW
C = CB+(AW/A)*(CW-CB)
*/
// MMX 颜色混叠(需要查表)	// 返回混叠后的颜色
inline  DWORD MMX_FastColorMix(IN	DWORD clB,	// 背景色 
	IN	DWORD clW)	// 前景色 
{
	return asmX64_MMX_FastColorMix(clB,clW);
}
#endif //_INC_SYPLGINMGRASM_WIN64