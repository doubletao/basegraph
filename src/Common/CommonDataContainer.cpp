#include "stdafx.h"
#include "Common/CommonDataContainer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//hashtable
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void SYHTable_Init(phtable pHT)
{
	if (pHT)
	{
		pHT->m_mapContainer.clear();
	}
}

BASEGRAPH_COMMON_API void * SYHTable_GetAt(phtable pHT, UINT nIndex)
{
	if (pHT && pHT->m_mapContainer.count(nIndex) > 0)
	{
		return pHT->m_mapContainer[nIndex];
	}
	return nullptr;
}

BASEGRAPH_COMMON_API void * SYHTable_RemoveAt(phtable pHT, UINT nIndex)
{
	if (pHT && pHT->m_mapContainer.count(nIndex) > 0)
	{
		pHT->m_mapContainer.erase(nIndex);
	}
	return nullptr;
}

BASEGRAPH_COMMON_API UINT SYHTable_Insert(phtable pHT, void * e)
{
	if (pHT)
	{
		UINT nID = 1;
		if (pHT->m_mapContainer.size() > 0)
		{
			nID = pHT->m_mapContainer.rbegin()->first + 1;
		}
		pHT->m_mapContainer.insert(std::make_pair(nID, e));
		return nID;
	}
	return 0;
}

BASEGRAPH_COMMON_API UINT SYHTable_GetFirstValid(phtable pHT)
{
	if (pHT && pHT->m_mapContainer.size() > 0)
	{
		return pHT->m_mapContainer.begin()->first;
	}
	return 0;
}

BASEGRAPH_COMMON_API void * SYHTable_GetNextValid(phtable pHT, UINT * pnIndex)
{
	if (pHT && pnIndex && pHT->m_mapContainer.count(*pnIndex) > 0)
	{
		bool bFind = false;
		for (std::map<int, void *>::iterator it = pHT->m_mapContainer.begin(); it != pHT->m_mapContainer.end(); it++)
		{
			if (it->first == * pnIndex)
			{
				bFind = true;
			}
			if (bFind)
			{
				it = it++;
				if (it != pHT->m_mapContainer.end())
				{
					*pnIndex = it->first;
					return it->second;
				}
			}
		}
	}
	return nullptr;
}

BASEGRAPH_COMMON_API UINT SYHTable_GetCount(phtable pHT)
{
	if(pHT)
	{
		return (UINT)pHT->m_mapContainer.size();
	}
	return 0;
}

BASEGRAPH_COMMON_API void SYHTable_Delete(phtable pHT)
{
	if (pHT)
	{
		for (std::map<int, void *>::iterator it = pHT->m_mapContainer.begin(); it != pHT->m_mapContainer.end(); it++)
		{
			SAFE_DELETE(it->second);
		}
		delete pHT;
	}
}

BASEGRAPH_COMMON_API void SYHTable_Release(phtable pHT)
{
	if (pHT)
	{
		for (std::map<int, void *>::iterator it = pHT->m_mapContainer.begin(); it != pHT->m_mapContainer.end(); it++)
		{
			SAFE_DELETE(it->second);
		}
		pHT->m_mapContainer.clear();
	}
}

BASEGRAPH_COMMON_API void SYHTable_RemoveAll(phtable pHT)
{
	if (pHT)
	{
		pHT->m_mapContainer.clear();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//hash
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

BASEGRAPH_COMMON_API void SYHash_FreeKeyStr(void * pK)
{
	delete pK;
}

BASEGRAPH_COMMON_API void SYHash_FreeValueStr(void * pV)
{
	delete pV;
}

BASEGRAPH_COMMON_API PSYHash SYHash_Create(unsigned int minsize, UINT64(*hashF)(void *), BOOL(*keyEqF)(void *, void *), void *(*keyDupF)(void *), void(*keyFreeF)(void *), void(*valueFreeF)(void *))
{
	// 创建表结构
	PSYHash ph = new SYHash;

	if (NULL == ph)
		return NULL;

	ph->bKeyRelease = TRUE;
	ph->bValueRelease = TRUE;
	// 初始化接口指针
	ph->freekeyfn = (keyFreeF == NULL) ? SYHash_FreeKeyStr : keyFreeF;
	ph->freevaluefn = (valueFreeF == NULL) ? SYHash_FreeValueStr : valueFreeF;

	return ph;
}

BASEGRAPH_COMMON_API void SYHash_Release(PSYHash ph, BOOL bFreeValue)
{
	if (ph == NULL)
		return;

	// 分开写循环减少了判断次数
	if (bFreeValue)
	{
		for (std::map<void *, void *>::iterator it = ph->m_mapContainer.begin(); it != ph->m_mapContainer.end();)
		{
			if (ph->bKeyRelease)
			{
				if (ph->freekeyfn)
				{
					ph->freekeyfn(it->first);
				}
				else
				{
					delete it->first;
				}
			}
			if (ph->bValueRelease)
			{
				if (ph->freevaluefn)
				{
					ph->freevaluefn(it->second);
				}
				else
				{
					SAFE_DELETE(it->second);
				}
			}
			it = ph->m_mapContainer.erase(it);
		}
	}
	else
	{
		for (std::map<void *, void *>::iterator it = ph->m_mapContainer.begin(); it != ph->m_mapContainer.end();)
		{
			if (ph->bKeyRelease)
			{
				if (ph->freekeyfn)
				{
					ph->freekeyfn(it->first);
				}
				else
				{
					delete it->first;
				}
			}
			it = ph->m_mapContainer.erase(it);
		}
	}
	ph->m_mapContainer.clear();
}

BASEGRAPH_COMMON_API void SYHash_Delete(PSYHash ph, BOOL bFreeValue)
{
	if (!ph)
		return;
	SYHash_Release(ph, bFreeValue);
	free(ph);
}

BASEGRAPH_COMMON_API void * SYHash_Search(const SYHash * ph, void * pk)
{
	if (ph && ph->m_mapContainer.count(pk) > 0)
	{
		return ph->m_mapContainer.at(pk);
	}
	return nullptr;
}

BASEGRAPH_COMMON_API void * SYHash_RemoveAt(PSYHash ph, void * pk, BOOL bFreeValue)
{
	if (ph == NULL)
		return nullptr;

	// 分开写循环减少了判断次数
	if (bFreeValue)
	{
		std::map<void *, void *>::iterator it = ph->m_mapContainer.find(pk);
		if (it != ph->m_mapContainer.end())
		{
			if (ph->bKeyRelease)
			{
				if (ph->freekeyfn)
				{
					ph->freekeyfn(it->first);
				}
				else
				{
					delete it->first;
				}
			}
			if (ph->bValueRelease)
			{
				if (ph->freevaluefn)
				{
					ph->freevaluefn(it->second);
				}
				else
				{
					SAFE_DELETE(it->second);
				}
			}
			ph->m_mapContainer.erase(it);
		}
	}
	else
	{
		std::map<void *, void *>::iterator it = ph->m_mapContainer.find(pk);
		if (it != ph->m_mapContainer.end())
		{
			if (ph->bKeyRelease)
			{
				if (ph->freekeyfn)
				{
					ph->freekeyfn(it->first);
				}
				else
				{
					delete it->first;
				}
			}
			ph->m_mapContainer.erase(it);
		}
	}
	return nullptr;
}

BASEGRAPH_COMMON_API BOOL SYHash_Insert(PSYHash ph, void * pk, void * pv)
{
	if (!ph)
	{
		return FALSE;
	}
	ph->m_mapContainer.insert(std::make_pair(pk, pv));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//array
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 描述：	创建一个空数组
//
// 返回值：	NULL		创建失败（内存不足）
//			pA			创建的空数组的指针
///////////////////////////////////////////////////////////////////////////////
LPSYARRAY __cdecl SYAR_Create()
{
	LPSYARRAY pA = (LPSYARRAY)malloc((size_t)sizeof(SYArray));

	if (pA != NULL)
	{
		pA->m_pData = NULL;
		pA->m_nSize = pA->m_nMaxSize = 0;
		pA->m_nGrowBy = 4;
	}
	return pA;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化一个空数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_Init(LPSYARRAY pA)			// 数组结构指针
{
	if (pA == NULL)
		return SYAR_NULL;

	memset(pA, 0, sizeof(SYArray));

	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除一个数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_Release(LPSYARRAY pA)			// 数组结构指针
{
	if (pA == NULL)
		return SYAR_NULL;

	if (pA->m_pData != NULL)
	{
		free(pA->m_pData);
	}
	free(pA);

	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	实现数组大小的设定
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_INVALID_PARAM	无效参数
//			SYAR_MEMORY_ERROR	内存错误
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_SetSize(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nNewSize,		// 新的数组大小
	INT_PTR nGrowBy)		// 需要增大数组时增加的元素个数
{
	if (pA == NULL)
		return SYAR_NULL;

	if (nNewSize < 0)
		return SYAR_INVALID_PARAM;

	size_t nAllocSize(0);
	INT_PTR nNewMax(0);
	SYAR_ELE_PTR pNewData = NULL;

	if (nGrowBy >= 0)
	{
		pA->m_nGrowBy = nGrowBy;
	}
	// 设定新的数组大小
	if (nNewSize == 0)
	{
		// 释放数组所占内存而不处理元素所占内存
		if (pA->m_pData != NULL)
		{
			free(pA->m_pData);
			pA->m_pData = NULL;
		}
		pA->m_nSize = pA->m_nMaxSize = 0;
	}
	else if (pA->m_pData == NULL)
	{
		// 申请足够的空间来满足 nNewSize 或 m_nGrowBy 的要求
		nNewMax = (nNewSize > pA->m_nGrowBy) ? nNewSize : pA->m_nGrowBy;
		nAllocSize = (size_t)nNewMax * SYAR_ELE_LEN;
		pA->m_pData = (SYAR_ELE_PTR)malloc(nAllocSize);
		if (pA->m_pData == NULL)
			return SYAR_MEMORY_ERROR;

		memset((void*)pA->m_pData, 0, nAllocSize);
		pA->m_nSize = nNewSize;
		pA->m_nMaxSize = nNewMax;
	}
	else if (nNewSize <= pA->m_nMaxSize)
	{
		// 无需增大数组的大小
		if (nNewSize > pA->m_nSize)
		{
			// 初始化要扩展的元素空间
			memset((void*)(pA->m_pData + pA->m_nSize),
				0,
				(size_t)(nNewSize - pA->m_nSize) * SYAR_ELE_LEN);
		}
		pA->m_nSize = nNewSize;
	}
	else
	{
		// 需要增大数组
		nGrowBy = pA->m_nGrowBy;
		if (nGrowBy == 0)
		{
			// 当 nGrowBy == 0 时确定增长量（防止堆碎片）
			nGrowBy = (pA->m_nSize >> 3);
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		// 防止元素最大个数溢出
		nNewMax = pA->m_nMaxSize + nGrowBy;
		nNewMax = (nNewMax > nNewSize) ? nNewMax : nNewSize;
		//////assert(nNewMax >= pA->m_nMaxSize);
		// 重新申请内存并拷贝数据
		pNewData = (SYAR_ELE_PTR)malloc((size_t)nNewMax * SYAR_ELE_LEN);
		if (pNewData == NULL)
			return SYAR_MEMORY_ERROR;

		memcpy((void*)pNewData,
			(void*)pA->m_pData,
			(size_t)pA->m_nSize * SYAR_ELE_LEN);
		memset((void*)(pNewData + pA->m_nSize),
			0,
			(size_t)(nNewSize - pA->m_nSize) * SYAR_ELE_LEN);
		free(pA->m_pData);
		pA->m_pData = pNewData;
		pA->m_nSize = nNewSize;
		pA->m_nMaxSize = nNewMax;
	}
	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取数组大小
//
// 返回值：	m_nSize			数组大小
//			SYAR_NULL		pA == NULL
///////////////////////////////////////////////////////////////////////////////
INT_PTR	__cdecl SYAR_GetSize(const SYArray* pA)		// 数组结构指针
{
	if (pA == NULL)
		return SYAR_NULL;

	return pA->m_nSize;
}

int	__cdecl SYAR_GetCount(const SYArray* pA)		// 数组结构指针
{
	if (pA == NULL)
		return SYAR_NULL;

	return (int)pA->m_nSize;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取数组的上界
//
// 返回值：	m_nSize-1		数组上界
//			SYAR_NULL		pA == NULL
//			SYAR_EMPTY		空数组（-1）
///////////////////////////////////////////////////////////////////////////////
INT_PTR	__cdecl SYAR_GetUpperBound(LPSYARRAY pA)	// 数组结构指针
{
	if (pA == NULL)
		return SYAR_NULL;

	return (pA->m_nSize - 1);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断是否为空数组
//
// 返回值：	TRUE		为空
//			FALSE		不为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYAR_IsEmpty(LPSYARRAY pA)			// 数组结构指针
{
	return (pA == NULL || pA->m_pData == NULL);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	释放超越上界的元素的内存
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_MEMORY_ERROR	内存错误
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_FreeExtra(LPSYARRAY pA)		// 数组结构指针
{
	//////assert(pA != NULL);

	if (pA == NULL)
		return SYAR_NULL;

	if (pA->m_nSize < pA->m_nMaxSize)
	{
		SYAR_ELE_PTR pNewData = NULL;

		if (pA->m_nSize != 0)
		{
			size_t nAllocSize = (size_t)pA->m_nSize * SYAR_ELE_LEN;

			// 重新申请内存并拷贝数据
			pNewData = (SYAR_ELE_PTR)malloc(nAllocSize);
			if (pNewData == NULL)
				return SYAR_MEMORY_ERROR;

			memcpy((void*)pNewData, (void*)pA->m_pData, nAllocSize);
		}
		// 删除旧内存
		free(pA->m_pData);
		pA->m_pData = pNewData;
		pA->m_nMaxSize = pA->m_nSize;
	}
	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	释放所有元素的内存
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_RemoveAll(LPSYARRAY pA)		// 数组结构指针
{
	//////assert(pA != NULL);

	if (pA == NULL)
		return SYAR_NULL;

	if (pA->m_pData != NULL)
	{
		free(pA->m_pData);
		pA->m_pData = NULL;
		pA->m_nMaxSize = pA->m_nSize = 0;
	}
	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取数据指针
//
// 返回值：	NULL			pA == NULL
//			SYAR_ELE_PTR	当前数据指针
///////////////////////////////////////////////////////////////////////////////
SYAR_ELE_PTR __cdecl SYAR_GetData(LPSYARRAY pA)		// 数组结构指针
{
	//////assert(pA != NULL);

	if (pA != NULL)
		return pA->m_pData;

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取某一元素
//
// 返回值：	SYAR_ELEMENT		索引元素数据
///////////////////////////////////////////////////////////////////////////////
SYAR_ELEMENT __cdecl SYAR_GetAt(const SYArray* pA,		// 数组结构指针
	INT_PTR nIndex)		// 元素索引号
{
	//////assert(pA != NULL);

	if ((pA != NULL) && (nIndex >= 0) && (nIndex < pA->m_nSize))
		return pA->m_pData[nIndex];

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置某一非越界元素的值
//
// 返回值：	NULL				失败
//			SYAR_ELEMENT		旧元素数据
///////////////////////////////////////////////////////////////////////////////
SYAR_ELEMENT __cdecl SYAR_SetAt(LPSYARRAY pA,				// 数组结构指针
	INT_PTR nIndex,				// 元素索引号
	SYAR_ELEMENT newData)		// 预设元素值
{
	//////assert(pA != NULL);
	//////assert(nIndex >= 0 && nIndex < pA->m_nSize);

	if (pA == NULL || nIndex < 0 || nIndex >= pA->m_nSize)
		return NULL;

	SYAR_ELEMENT oldData = pA->m_pData[nIndex];

	pA->m_pData[nIndex] = newData;

	return oldData;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在数组尾扩充添加一个元素
//
// 返回值：	索引			添加成功
//			SYAR_NULL		空指针
//			SYAR_FAILED		失败
///////////////////////////////////////////////////////////////////////////////
INT_PTR __cdecl SYAR_Add(LPSYARRAY pA,				// 数组结构指针
	SYAR_ELEMENT newData)		// 预设元素值
{
	//////assert(pA != NULL);

	INT_PTR nRet = (INT_PTR)SYAR_SetSize(pA, pA->m_nSize + 1, -1);

	if (nRet != SYAR_SUCCESS)
		return SYAR_FAILED;

	nRet = pA->m_nSize - 1;
	pA->m_pData[nRet] = newData;

	return nRet;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在数组尾扩充添加另一个数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_FAILED			失败
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_Append(LPSYARRAY pA,			// 数组结构指针
	LPSYARRAY pSrc)			// 预添加的数组
{
	//////assert(pA != NULL && pA != pSrc && pSrc != NULL);

	if (pA == NULL || pSrc == NULL)
		return SYAR_NULL;

	if (pA == pSrc)
		return SYAR_FAILED;

	INT_PTR nOldSize = pA->m_nSize;

	if (SYAR_SetSize(pA, nOldSize + pSrc->m_nSize, -1) != SYAR_SUCCESS)
		return SYAR_FAILED;

	memcpy((void*)(pA->m_pData + nOldSize),
		(void*)pSrc->m_pData,
		(size_t)pSrc->m_nSize * SYAR_ELE_LEN);

	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	复制另一个数组的内容
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_FAILED			失败
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_Copy(LPSYARRAY pA,			// 数组结构指针
	LPSYARRAY pSrc)		// 预复制的数组
{
	//////assert(pA != NULL && pA != pSrc && pSrc != NULL);

	if (pA == NULL || pSrc == NULL)
		return SYAR_NULL;

	if (pA == pSrc)
		return SYAR_FAILED;

	if (SYAR_SetSize(pA, pSrc->m_nSize, -1) != SYAR_SUCCESS)
		return FALSE;

	memcpy((void*)(pA->m_pData),
		(void*)pSrc->m_pData,
		(size_t)pSrc->m_nSize * SYAR_ELE_LEN);

	return SYAR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在某一非越界元素前或者最后一个元素后插入几个新元素
//
// 返回值：	索引				最后一个插入的元素
//			SYAR_NULL			空指针
//			SYAR_FAILED			失败
//			SYAR_INVALID_PARAM	无效参数
///////////////////////////////////////////////////////////////////////////////
INT_PTR __cdecl SYAR_InsertAt(LPSYARRAY pA,				// 数组结构指针
	INT_PTR nIndex,			// 元素索引号
	SYAR_ELEMENT newData,		// 预插入元素值
	INT_PTR nCount)			// 预插入的元素个数
{
	//////assert(pA != NULL);
	//////assert((nIndex >= 0) && (nCount > 0));

	if (pA == NULL)
		return SYAR_NULL;

	if ((nIndex < 0) || (nCount <= 0))
		return SYAR_INVALID_PARAM;

	if (nIndex < pA->m_nSize)		// 非越界元素前
	{
		INT_PTR nOldSize = pA->m_nSize;

		if (SYAR_SetSize(pA, nOldSize + nCount, -1) != SYAR_SUCCESS)
			return SYAR_FAILED;

		// 移动后面的元素
		memmove((void*)(pA->m_pData + nIndex + nCount),
			(void*)(pA->m_pData + nIndex),
			(size_t)(nOldSize - nIndex) * SYAR_ELE_LEN);
		// 循环设置新元素值
		while (nCount--)
		{
			pA->m_pData[nIndex++] = newData;
		}
		return (nIndex - 1);
	}
	// 否则在最后一个元素后
	nIndex = SYAR_Add(pA, newData);
	if (nIndex < 0)
		return SYAR_FAILED;

	nCount--;
	if (nCount > 0)		// 如果添加多个，则在添加后进行插入，减少数组的扩大次数
		return SYAR_InsertAt(pA, nIndex, newData, nCount);

	return nIndex;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在某一非越界元素前或者最后一个元素后插入一个数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_FAILED			失败
//			SYAR_INVALID_PARAM	无效参数
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_InsertArrayAt(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nIndex,			// 元素索引号
	LPSYARRAY pSrc)			// 预插入数组
{
	if (pA == NULL || pSrc == NULL)
		return SYAR_NULL;

	if (pA == pSrc || nIndex < 0)
		return SYAR_INVALID_PARAM;

	if (nIndex < pA->m_nSize)	// 非越界元素前
	{
		// 先扩充数组大小
		if (SYAR_InsertAt(pA, nIndex, pSrc->m_pData[0], pSrc->m_nSize) < 0)
			return SYAR_FAILED;

		// 逐个填充新元素
		for (INT_PTR i = 1; i<pSrc->m_nSize; i++)
		{
			pA->m_pData[++nIndex] = pSrc->m_pData[i];
		}
		return SYAR_SUCCESS;
	}
	// 否则在最后一个元素后
	return SYAR_Append(pA, pSrc);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	从某一非越界元素开始删除几个元素
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_FAILED			失败
//			SYAR_INVALID_PARAM	无效参数
///////////////////////////////////////////////////////////////////////////////
SYAR_ELEMENT __cdecl SYAR_RemoveAt(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nIndex,		// 元素索引号
	INT_PTR nCount)		// 预删除的元素个数
{
	if (pA == NULL)
		return NULL;

	if ((nIndex < 0) || (nIndex >= pA->m_nSize) || (nCount <= 0))
		return NULL;

	INT_PTR nMoveCount = pA->m_nSize - (nIndex + nCount);
	SYAR_ELEMENT e = NULL;
	if (nMoveCount > 0)		// 如果需要移动的数量为 0 说明需要删除最后几个元素
	{
		e = SYAR_GetAt(pA, nIndex);
		memmove((void*)(pA->m_pData + nIndex),
			(void*)(pA->m_pData + nIndex + nCount),
			(size_t)nMoveCount * SYAR_ELE_LEN);
		pA->m_nSize -= nCount;
	}
	else	// Remove all the data after "nIndex" (include "nIndex").
	{
		e = SYAR_GetAt(pA, nIndex);
		pA->m_nSize = nIndex;

	}
	return e;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	交换两个非越界元素的值
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_INVALID_PARAM	无效参数
//////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_Swap(LPSYARRAY pA,			// 数组结构指针
	INT_PTR i,			// 元素索引号
	INT_PTR j)
{
	if (pA == NULL)
		return SYAR_NULL;

	if (i < 0 || j < 0 || i >= pA->m_nSize || j >= pA->m_nSize)
		return SYAR_INVALID_PARAM;

	SYAR_ELEMENT tmpData = pA->m_pData[j];

	pA->m_pData[j] = pA->m_pData[i];
	pA->m_pData[i] = tmpData;

	return SYAR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	翻转数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_Reverse(LPSYARRAY pA)		// 数组结构指针
{
	if (pA != NULL)
		return SYAR_NULL;

	if (pA->m_nSize > 1)	// 多于一个元素时才需要翻转
	{
		INT_PTR nHalfSize = (pA->m_nSize >> 1);
		SYAR_ELEMENT tmpData = NULL;

		for (INT_PTR i = 0, j = (pA->m_nSize - i - 1); i<nHalfSize; i++, j--)
		{
			tmpData = pA->m_pData[j];
			pA->m_pData[j] = pA->m_pData[i];
			pA->m_pData[i] = tmpData;
		}
	}
	return SYAR_SUCCESS;
}

int __cdecl SYAR_Clear(LPSYARRAY par)
{
	par->m_nSize = 0;
	return SYAR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////
// 描述：	移动数组内元素的位置：把a[i]移动到a[j]之前紧靠a[j]的位置 
// 注意：	i和j都是在移动动作发生之前的数组中的位置，移动完毕后保证除了a[i]
//			之外的元素的相对顺序不变，并且原来<j的元素都在移动后的a[i]之前，
//			原来>=j的元素都在移动后的a[i]之后 (靠前表示位置索引较小)
// 示例：	对于数组[1,2,3,4,5]，如果i=3,j=1则得到：[1,4,2,3,5]
//			如果i=1,j=3,则得到：[1,3,2,4,5]，注意结果中2的位置并不是(j==3)
//////////////////////////////////////////////////////////////////////////
void __cdecl SYAR_MoveElement(LPSYARRAY pA,		// 数组结构指针
	size_t i,			// 要移动的元素的索引，应该属于[0,size)
	size_t j)		// 要移动到的位置，应该属于[0,size]
{
	if (i >= (size_t)pA->m_nSize || j >= (size_t)pA->m_nSize || i == j)
		return;
	void* pSrc = NULL;
	void* pDst = NULL;
	size_t nCount = 0;
	SYAR_ELEMENT temp = pA->m_pData[i];
	if (i < j)
	{
		pSrc = pA->m_pData + i + 1;
		pDst = pA->m_pData + i;
		nCount = j - i;
	}
	else
	{
		pSrc = pA->m_pData + j;
		pDst = pA->m_pData + j + 1;
		nCount = i - j;
	}
	memmove(pDst, pSrc, sizeof(SYAR_ELEMENT) * nCount);
	pA->m_pData[j] = temp;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置最大数组大小，不会添加数组元素
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_INVALID_PARAM	无效参数
//			SYAR_MEMORY_ERROR	内存错误
///////////////////////////////////////////////////////////////////////////////
int __cdecl SYAR_SetMaxSize(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nNewSize		// 新的数组大小
)
{
	if (pA == NULL)
		return SYAR_NULL;

	if (nNewSize <= 0)
		return SYAR_INVALID_PARAM;

	size_t nAllocSize(0);
	SYAR_ELE_PTR pNewData = NULL;
	if (nNewSize < pA->m_nSize)
		return SYAR_INVALID_PARAM;

	if (nNewSize == pA->m_nMaxSize)
		return SYAR_SUCCESS;
	if (pA->m_pData == NULL)
	{
		nAllocSize = (size_t)nNewSize * SYAR_ELE_LEN;
		pA->m_pData = (SYAR_ELE_PTR)malloc(nAllocSize);
		if (pA->m_pData == NULL)
			return SYAR_MEMORY_ERROR;

		memset((void*)pA->m_pData, 0, nAllocSize);
		pA->m_nSize = 0;
		pA->m_nMaxSize = nNewSize;
	}
	else
	{
		// 重新申请内存并拷贝数据
		pNewData = (SYAR_ELE_PTR)malloc((size_t)nNewSize * SYAR_ELE_LEN);
		if (pNewData == NULL)
			return SYAR_MEMORY_ERROR;

		memcpy((void*)pNewData,
			(void*)pA->m_pData,
			(size_t)pA->m_nSize * SYAR_ELE_LEN);
		memset((void*)(pNewData + pA->m_nSize),
			0,
			(size_t)(nNewSize - pA->m_nSize) * SYAR_ELE_LEN);
		free(pA->m_pData);
		pA->m_pData = pNewData;
		pA->m_nMaxSize = nNewSize;
	}
	return SYAR_SUCCESS;
}

void __cdecl SYAR_Sort(LPSYARRAY pA, SYCompareFun fun)
{
	if (pA->m_nSize <= 0)
		return;
	qsort(pA->m_pData, pA->m_nSize, sizeof(SYAR_ELEMENT), fun);
}

void __cdecl SYStrAR_Release(CSYStrArray& ary)
{
	int nCnt = ary.GetCount();
	for (int i = 0; i<nCnt; ++i)
	{
		LPTSTR szText = ary.GetAt(i);
		if (szText != NULL)
		{
			delete szText;
		}
	}
	ary.RemoveAll();
}