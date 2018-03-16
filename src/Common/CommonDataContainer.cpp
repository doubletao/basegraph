#include "stdafx.h"
#include "Common/CommonDataContainer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//hashtable
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT64 __cdecl SYHash_HashStr(SYHash_key pk);			// key
														// 素数表
static const unsigned int PRIMES[] =
{
	53, 97, 193, 389,
	769, 1543, 3079, 6151,
	12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869,
	3145739, 6291469, 12582917, 25165843,
	50331653, 100663319, 201326611, 402653189,
	805306457, 1610612741
};
// 素数表大小
const unsigned int PRIME_TABLE_LENGTH = (int)(sizeof(PRIMES) / sizeof(PRIMES[0]));
const float MAX_LOAD_FACTOR = 0.65f;			// 最大负载因子
const float MIN_LOAD_FACTOR = 0.15f;			// 最小负载因子
BOOL SYHash_Init_Int(SYHash* ph, unsigned int minsize)
{
	return SYHash_Init(ph, minsize, SYHash_HashInt, SYHash_EqualInt, SYHash_DuplicateInt, SYHash_FreeKeyInt, SYHash_FreeValueInt);
}
BOOL SYHash_Init(SYHash* ph,
	unsigned int minsize,					// 表初始大小
	UINT64(*SYHash_make)(SYHash_key),		// Hash值计算函函数
	BOOL(*key_compare)(SYHash_key, SYHash_key),	// key 比较函数
	SYHash_key(*key_dup)(SYHash_key),		// key 拷贝函数
	void(*key_free)(SYHash_key),			// key 释放函数
	void(*value_free)(SYHash_value))		// value 释放函数
{
	// 确保表不要过大
	if (minsize > (1U << 30))
		return FALSE;

	unsigned int unPIndex(0), unSize = PRIMES[unPIndex];

	// 强制表的大小为素数
	for (; unPIndex<PRIME_TABLE_LENGTH; unPIndex++)
	{
		if (PRIMES[unPIndex] >= minsize)
		{
			unSize = PRIMES[unPIndex];
			break;
		}
	}

	if (NULL == ph)
		return NULL;

	ph->pool = NULL;
	ph->poolend = NULL;
	ph->dupkey = TRUE;
	// 创建表并初始化
	ph->table = (pentrytable)malloc(sizeof(pentry) * unSize);
	if (NULL == ph->table)
	{
		return FALSE;
	}
	memset(ph->table, 0, unSize * sizeof(pentry));
	ph->tablelen = unSize;
	ph->primeindex = unPIndex;
	ph->count = 0;
	ph->limit = (unsigned int)ceilf(unSize * MAX_LOAD_FACTOR);
	ph->lower = (unsigned int)ceilf(unSize * MIN_LOAD_FACTOR);
	// 初始化接口指针
	ph->hashfn = (SYHash_make == NULL) ? SYHash_HashStr : SYHash_make;
	ph->eqfn = (key_compare == NULL) ? SYHash_EqualStr : key_compare;
	ph->dupfn = (key_dup == NULL) ? SYHash_DuplicateStr : key_dup;
	ph->freekeyfn = (key_free == NULL) ? SYHash_FreeKeyStr : key_free;
	ph->freevaluefn = (value_free == NULL) ? SYHash_FreeValueStr : value_free;

	return TRUE;
}

BOOL __cdecl SYHash_SetPool(SYHash* ph, unsigned char*  pool, unsigned char* poolend, BOOL dupKey)
{
	ph->pool = pool;
	ph->poolend = poolend;
	ph->dupkey = dupKey;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
// 描述：	创建新 Hash 表
//
// 返回值:	PSYHash			新表指针
///////////////////////////////////////////////////////////////////////////////
PSYHash __cdecl SYHash_Create(unsigned int minsize,					// 表初始大小
	UINT64(*SYHash_make)(SYHash_key),		// Hash值计算函函数
	BOOL(*key_compare)(SYHash_key, SYHash_key),	// key 比较函数
	SYHash_key(*key_dup)(SYHash_key),		// key 拷贝函数
	void(*key_free)(SYHash_key),			// key 释放函数
	void(*value_free)(SYHash_value))		// value 释放函数
{
	// 确保表不要过大
	if (minsize > (1U << 30))
		return NULL;

	unsigned int unPIndex(0), unSize = PRIMES[unPIndex];

	// 强制表的大小为素数
	for (; unPIndex<PRIME_TABLE_LENGTH; unPIndex++)
	{
		if (PRIMES[unPIndex] >= minsize)
		{
			unSize = PRIMES[unPIndex];
			break;
		}
	}
	// 创建表结构
	PSYHash ph = (PSYHash)malloc(sizeof(SYHash));

	if (NULL == ph)
		return NULL;

	// 创建表并初始化
	ph->table = (pentrytable)malloc(sizeof(pentry) * unSize);
	if (NULL == ph->table)
	{
		free(ph);
		return NULL;
	}
	memset(ph->table, 0, unSize * sizeof(pentry));
	ph->tablelen = unSize;
	ph->primeindex = unPIndex;
	ph->count = 0;
	ph->limit = (unsigned int)ceilf(unSize * MAX_LOAD_FACTOR);
	ph->lower = (unsigned int)ceilf(unSize * MIN_LOAD_FACTOR);
	ph->pool = NULL;
	ph->poolend = NULL;
	ph->dupkey = TRUE;
	// 初始化接口指针
	ph->hashfn = (SYHash_make == NULL) ? SYHash_HashStr : SYHash_make;
	ph->eqfn = (key_compare == NULL) ? SYHash_EqualStr : key_compare;
	ph->dupfn = (key_dup == NULL) ? SYHash_DuplicateStr : key_dup;
	ph->freekeyfn = (key_free == NULL) ? SYHash_FreeKeyStr : key_free;
	ph->freevaluefn = (value_free == NULL) ? SYHash_FreeValueStr : value_free;

	return ph;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	Hash 值求解
//
// 返回值:	unsigned int		key 对应的 Hash 值
///////////////////////////////////////////////////////////////////////////////
UINT64 __cdecl SYHash_Hash(const SYHash* ph,			// Hash 表指针
	SYHash_key pk)			// key
{


	UINT64 i = ph->hashfn(pk);
	// 防止用户使用过于简单的 Hash 函数，算法来源于 java 1.4
	i += ~(i << 9);
	i ^= ((i >> 14) | (i << 18));
	i += (i << 4);
	i ^= ((i >> 10) | (i << 22));

	return i;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缺省的 Hash 值求解函数（字符串）
//
// 返回值:	unsigned int		key 对应的 Hash 值
///////////////////////////////////////////////////////////////////////////////
UINT64 __cdecl SYHash_HashStr(SYHash_key pk)			// key
{
	unsigned int h(0);
	LPTSTR p = (LPTSTR)pk;

	for (; *p; p++)
	{
		h = 31 * h + (unsigned int)*p;
	}
	return h;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缺省的 key 比较函数（字符串）
//
// 返回值:	TRUE		相等
//			FALSE		不等
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYHash_EqualStr(SYHash_key pK1,				// key
	SYHash_key pK2)				// key
{
	return (pK1 == pK2 || _tcscmp((LPTSTR)pK1, (LPTSTR)pK2) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缺省的 key 拷贝函数（字符串）
//
// 返回值:	SYHash_key
///////////////////////////////////////////////////////////////////////////////
SYHash_key __cdecl SYHash_DuplicateStr(SYHash_key pk)		// key
{
	LPTSTR szKey = (LPTSTR)pk;

	LPTSTR szNewKey = (LPTSTR)malloc(sizeof(TCHAR) * (_tcslen(szKey) + 1));

	_tcscpy_s(szNewKey, _tcslen(szKey) + 1, szKey);

	return (SYHash_key)szNewKey;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缺省的 key 释放函数（字符串）
//
// 返回值:	void
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHash_FreeKeyStr(SYHash_key pk)			// key
{
	free(pk);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缺省的 value 释放函数（只能释放由 malloc 申请的内存空间）
//
// 返回值:	void
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHash_FreeValueStr(SYHash_value pV)		// key
{
	free(pV);
}
UINT64 __cdecl SYHash_HashInt(SYHash_key pk)			// key
{
	return (UINT64)pk;
}

BOOL __cdecl SYHash_EqualInt(SYHash_key pK1,				// key
	SYHash_key pK2)				// key
{
	return pK1 == pK2;
}

SYHash_key __cdecl SYHash_DuplicateInt(SYHash_key pk)		// key
{
	SYHash_key NewKey = pk;
	return NewKey;
}

void __cdecl SYHash_FreeKeyInt(SYHash_key pk)			// key
{
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缺省的 value 释放函数（只能释放由 malloc 申请的内存空间）
//
// 返回值:	void
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHash_FreeValueInt(SYHash_value pV)		// key
{
	free(pV);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取入口点总数
//
// 返回值:	unsigned int
///////////////////////////////////////////////////////////////////////////////
unsigned int __cdecl SYHash_GetCount(PSYHash ph)		// Hash 表指针
{


	return ph->count;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	获取第一个入口点的位置
//
// 返回值：	SYHash_pos
//////////////////////////////////////////////////////////////////////////
SYHash_pos __cdecl SYHash_GetHeadPosition(PSYHash ph)		// Hash 表指针
{


	if (ph->count > 0)		// 至少有一个入口点
	{
		// 按桶从头开始搜索第一个入口点
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			if (ph->table[i] != NULL)
			{
				return ph->table[i];
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	获取第一个入口点的数据
//
// 返回值：	SYHash_key / SYHash_value
//////////////////////////////////////////////////////////////////////////
SYHash_key __cdecl SYHash_GetHeadKey(PSYHash ph)		// Hash 表指针
{


	if (ph->count > 0)		// 至少有一个入口点
	{
		// 按桶从头开始搜索第一个入口点
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			if (ph->table[i] != NULL)
			{
				return ph->table[i]->pKey;
			}
		}
	}
	return NULL;
}
SYHash_value __cdecl SYHash_GetHeadValue(PSYHash ph)	// Hash 表指针
{


	if (ph->count > 0)		// 至少有一个入口点
	{
		// 按桶从头开始搜索第一个入口点
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			if (ph->table[i] != NULL)
			{
				return ph->table[i]->pValue;
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	获取下个位置或当前位置数据，获取数据时会自动更新位置。
//
// 返回值：	SYHash_pos / SYHash_key / SYHash_value
//////////////////////////////////////////////////////////////////////////
SYHash_value __cdecl SYHash_GetNextPosition(PSYHash ph,			// Hash 表指针
	SYHash_pos* pPos)		// 当前位置
{


	if (ph == NULL || pPos == NULL || *pPos == NULL)
		return NULL;

	pentry pEntry = (pentry)(*pPos);
	pentry pNext = pEntry->pNextEntry;

	if (pNext == NULL)		// 如果当前桶已到最后一个元素，则搜索后面的桶。
	{
		unsigned int unIndex = (pEntry->unHashValue % ph->tablelen) + 1;

		for (; unIndex<ph->tablelen; unIndex++)
		{
			pNext = ph->table[unIndex];
			if (pNext != NULL)	// 如果桶不为空，则取第一个元素。
				break;
			// 否则直接搜索下一桶
		}
	}

	*pPos = pNext;
	return pEntry->pValue;
}
SYHash_key __cdecl SYHash_GetPosKey(PSYHash ph,			// Hash 表指针
	SYHash_pos pos)		// 当前位置
{


	if (ph == NULL || pos == NULL)
		return NULL;

	pentry pEntry = (pentry)pos;

	return pEntry->pKey;
}
SYHash_value __cdecl SYHash_GetPosValue(PSYHash ph,			// Hash 表指针
	SYHash_pos pos)		// 当前位置
{


	if (ph == NULL || pos == NULL)
		return NULL;

	pentry pEntry = (pentry)pos;

	return pEntry->pValue;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩充 Hash 表容量为原来的两倍大小（实际上为最接近两倍大小的素数）
//
// 返回值:	TRUE			扩充成功
//			FALSE			扩充失败
///////////////////////////////////////////////////////////////////////////////
static BOOL SYHash_expand(PSYHash ph)			// Hash 表指针
{


	pentrytable pNewTable = NULL;
	pentry pE = NULL, pTmpE = NULL;
	unsigned int unNewSize(0), i(0), unIndex(0);

	// 确保未使用最大容量
	if (ph->primeindex == (PRIME_TABLE_LENGTH - 1))
		return FALSE;

	unNewSize = PRIMES[++(ph->primeindex)];
	pNewTable = (pentrytable)malloc(sizeof(pentry) * unNewSize);
	if (NULL != pNewTable)
	{
		memset(pNewTable, 0, unNewSize * sizeof(pentry));
		// 按桶一步一步移动入口点
		for (i = 0; i<ph->tablelen; i++)
		{
			// 移动同一桶内的入口点并翻转了原来的顺序
			while (NULL != (pE = ph->table[i]))
			{
				ph->table[i] = pE->pNextEntry;
				unIndex = pE->unHashValue % unNewSize;
				pE->pNextEntry = pNewTable[unIndex];
				pNewTable[unIndex] = pE;
			}
		}
		free(ph->table);
		ph->table = pNewTable;
	}
	else		// 如果内存不足则采用 realloc
	{
		pNewTable = (pentrytable)realloc(ph->table,
			unNewSize * sizeof(pentry));
		if (NULL == pNewTable)
		{
			(ph->primeindex)--;
			return FALSE;
		}
		ph->table = pNewTable;
		memset(pNewTable[ph->tablelen], 0, unNewSize - ph->tablelen);
		// 重新分配入口点到桶
		for (i = 0; i<ph->tablelen; i++)
		{
			for (pE = pNewTable[i]; pE != NULL; pE = pTmpE)
			{
				unIndex = pE->unHashValue % unNewSize;
				if (unIndex == i)
				{
					pTmpE = pE->pNextEntry;
				}
				else
				{
					pTmpE = pE->pNextEntry;
					pE->pNextEntry = pNewTable[unIndex];
					pNewTable[unIndex] = pE;
				}
			}
		}
	}
	ph->tablelen = unNewSize;
	ph->limit = (unsigned int)ceilf(unNewSize * MAX_LOAD_FACTOR);
	ph->lower = (unsigned int)ceilf(unNewSize * MIN_LOAD_FACTOR);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩 Hash 表容量为原来的一半大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
static void SYHash_compact(PSYHash ph)			// Hash 表指针
{


	pentrytable pNewTable = NULL;
	pentry pE = NULL, pTmpE = NULL;
	unsigned int unNewSize(0), i(0), unIndex(0);

	// 确保未使用最小容量
	if (ph->primeindex == 0)
		return;

	unNewSize = PRIMES[--(ph->primeindex)];
	pNewTable = (pentrytable)malloc(sizeof(pentry) * unNewSize);
	// 如果内存申请失败则不压缩
	if (NULL == pNewTable)
	{
		ph->primeindex++;
		return;
	}
	memset(pNewTable, 0, unNewSize * sizeof(pentry));
	// 按桶一步一步移动入口点
	for (; i<ph->tablelen; i++)
	{
		// 移动同一桶内的入口点并翻转了原来的顺序
		while (NULL != (pE = ph->table[i]))
		{
			ph->table[i] = pE->pNextEntry;
			unIndex = pE->unHashValue % unNewSize;
			pE->pNextEntry = pNewTable[unIndex];
			pNewTable[unIndex] = pE;
		}
	}
	free(ph->table);
	ph->table = pNewTable;
	ph->tablelen = unNewSize;
	ph->limit = (unsigned int)ceilf(unNewSize * MAX_LOAD_FACTOR);
	ph->lower = (unsigned int)ceilf(unNewSize * MIN_LOAD_FACTOR);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	插入一个数据，允许重复插入同一个 key
//
// 返回值:	TRUE			插入成功
//			FALSE			插入失败
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYHash_Insert(PSYHash ph,			// Hash 表指针
	SYHash_key pk,		// key
	SYHash_value pV)		// 绑定 key 的值
{


	unsigned int unIndex(0);
	pentry pE = NULL;

	// 当超过负载上限时扩充 Hash 表的容量
	if ((++(ph->count)) > ph->limit)
	{
		// 如果扩充失败则下次插入时会重新扩充
		SYHash_expand(ph);
	}

	int nsize = sizeof(entry);
	BOOL bRelease = TRUE;
	if (ph->pool)
		bRelease = (ph->pool + nsize >= ph->poolend);

	pE = (entry*)pool_malloc(&ph->pool, nsize, bRelease);
	pE->needrelease = bRelease;

	//pE = (pentry)malloc(sizeof(entry));
	if (NULL == pE)
	{
		--(ph->count);
		return FALSE;
	}
	// 计算 Hash 值及桶入口点
	pE->unHashValue = SYHash_Hash(ph, pk);
	unIndex = pE->unHashValue % ph->tablelen;
	// 拷贝一份 key 进行保存，由外部释放
	if (ph->dupkey)
		pE->pKey = ph->dupfn(pk);
	else
		pE->pKey = pk;
	pE->pValue = pV;
	// 将该节点放在桶入口点的前面
	pE->pNextEntry = ph->table[unIndex];
	ph->table[unIndex] = pE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	通过 key 查找数据
//
// 返回值:	SYHash_value
///////////////////////////////////////////////////////////////////////////////
SYHash_value SYHash_Search(const SYHash* ph,			// Hash 表指针
	SYHash_key pk)		// key
{
	UINT64 unHashValue = SYHash_Hash(ph, pk);
	pentry pE = ph->table[unHashValue % ph->tablelen];

	while (NULL != pE)
	{
		// 从桶头开始比较，增加 Hash 值的比较以减少比较的消耗
		// 如果使用同样的 key 保存不同的数据，将会得到离桶头最近的数据
		if ((unHashValue == pE->unHashValue) && (ph->eqfn(pk, pE->pKey)))
		{
			return pE->pValue;
		}
		pE = pE->pNextEntry;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除为 key 的入口点并返回相应的数据，如有必要则压缩 Hash 表的容量
//			允许释放数据，但使用缺省函数时要小心，释放后返回 NULL
//
// 返回值:	SYHash_value
///////////////////////////////////////////////////////////////////////////////
SYHash_value __cdecl SYHash_RemoveAt(PSYHash ph,			// Hash 表指针
	SYHash_key pk,			// key
	BOOL bFreeValue)		// 释放元素值内存的标志
{


	UINT64 unHashValue = SYHash_Hash(ph, pk);
	pentry* pPE = ph->table + unHashValue % ph->tablelen;	// 记录可能发生匹配的入口点指针的地址
	pentry pE = *pPE;
	SYHash_value pV = NULL;

	while (NULL != pE)
	{
		// 通过比较 Hash 值来减少比较次数
		// 如果使用同样的 key 保存不同的数据，将会删除离桶头最近的节点
		if ((unHashValue == pE->unHashValue) && (ph->eqfn(pk, pE->pKey)))
		{
			// 修改链条，并释放 key 与节点
			*pPE = pE->pNextEntry;
			ph->count--;
			pV = pE->pValue;
			if (bFreeValue)
			{
				ph->freevaluefn(pV);
				pV = NULL;
			}
			if (ph->dupkey)
				ph->freekeyfn(pE->pKey);
			if (pE->needrelease)
				free(pE);
			if (ph->count < ph->lower)
			{
				SYHash_compact(ph);
			}
			return pV;
		}
		pPE = &(pE->pNextEntry);
		pE = *pPE;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除所有的入口点，允许释放数据，但使用缺省函数时要小心
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHash_RemoveAll(PSYHash ph,			// Hash 表指针
	BOOL bFreeValue)		// 释放元素值内存的标志
{
	if (ph == NULL)
		return;

	pentry pE = NULL, pF = NULL;

	// 分开写循环减少了判断次数
	if (bFreeValue)
	{
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			pE = ph->table[i];
			while (NULL != pE)
			{
				pF = pE;
				pE = pE->pNextEntry;
				if (ph->dupkey)
					ph->freekeyfn(pF->pKey);
				ph->freevaluefn(pF->pValue);

				if (pF->needrelease)
					free(pF);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			pE = ph->table[i];
			while (NULL != pE)
			{
				pF = pE;
				pE = pE->pNextEntry;
				if (ph->dupkey)
					ph->freekeyfn(pF->pKey);
				if (pF->needrelease)
					free(pF);
			}
		}
	}
	memset(ph->table, 0, ph->tablelen * sizeof(pentry));
	ph->count = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	销除 Hash 表，允许释放数据，但使用缺省函数时要小心
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHash_Release(PSYHash ph,				// Hash 表指针
	BOOL bFreeValue)			// 释放元素值内存的标志
{
	if (ph == NULL)
		return;

	pentry pE = NULL, pF = NULL;

	// 分开写循环减少了判断次数
	if (bFreeValue)
	{
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			pE = ph->table[i];
			while (NULL != pE)
			{
				pF = pE;
				pE = pE->pNextEntry;
				ph->freevaluefn(pF->pValue);
				if (ph->dupkey)
					ph->freekeyfn(pF->pKey);
				if (pF->needrelease)
					free(pF);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i<ph->tablelen; i++)
		{
			pE = ph->table[i];
			while (NULL != pE)
			{
				pF = pE;
				pE = pE->pNextEntry;
				if (ph->dupkey)
					ph->freekeyfn(pF->pKey);
				if (pF->needrelease)
					free(pF);
			}
		}
	}
	free(ph->table);
	ph->count = 0;
	//free(ph);
}

void __cdecl SYHash_Delete(PSYHash ph,				// Hash 表指针
	BOOL bFreeValue)			// 释放元素值内存的标志
{
	if (!ph)
		return;
	SYHash_Release(ph, bFreeValue);
	free(ph);
}
///////////////////////////////////////////////////////////////////////////////
// 描述：	更改绑定 key 的值并返回原值
//			允许释放数据，但使用缺省函数时要小心，释放后返回 NULL
//
// 返回值:	SYHash_value
///////////////////////////////////////////////////////////////////////////////
SYHash_value __cdecl SYHash_ChangeValue(PSYHash ph,		// Hash 表指针
	SYHash_key pk,		// key
	SYHash_value pV,	// 欲更改的值
	BOOL bFreeValue)	// 释放元素值内存的标志
{


	UINT64 unHashValue = SYHash_Hash(ph, pk);
	pentry pE = ph->table[unHashValue % ph->tablelen];
	SYHash_value pTmpV = NULL;

	while (NULL != pE)
	{
		// 通过比较 Hash 值来减少比较次数
		// 如果使用同样的 key 保存不同的数据，将会更改离桶头最近的节点
		if ((unHashValue == pE->unHashValue) && (ph->eqfn(pk, pE->pKey)))
		{
			if (bFreeValue)
			{
				ph->freevaluefn(pE->pValue);
			}
			else
			{
				pTmpV = pE->pValue;
			}
			pE->pValue = pV;

			return pTmpV;
		}
		pE = pE->pNextEntry;
	}

	return pTmpV;
}
void* __cdecl pool_malloc(unsigned char** pool, int size, BOOL bfromsystem)
{
	if (bfromsystem)
		return new char[size];
	unsigned char* p = *pool;
	*pool += size;
	return p;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//htable
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	创建新句柄管理表
//
// 返回值:	phtable			新表指针
///////////////////////////////////////////////////////////////////////////////
phtable __cdecl SYHTable_Create(UINT maxsize)		// 数组空间大小
{
	phtable p = (phtable)malloc(sizeof(htable));

	if (p != NULL)
	{
		memset(p, 0, sizeof(htable));
		if (maxsize > 0)
		{
			p->eles = (SYHTable_ele_ptr)malloc(sizeof(SYHTable_element) * maxsize);
			if (NULL == p->eles)
			{
				free(p);
				return NULL;
			}
			memset(p->eles, 0, sizeof(SYHTable_element) * maxsize);
			p->maxsize = maxsize;
		}
	}
	return p;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	释放句柄管理表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHTable_Delete(phtable pHT)
{
	if (pHT->eles != NULL)
	{
		free(pHT->eles);
	}
	free(pHT);
}
void __cdecl SYHTable_Release(phtable pHT)			// 表指针
{
	if (pHT->eles != NULL)
	{
		free(pHT->eles);
	}
	memset(pHT, 0, sizeof(htable));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化句柄管理表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHTable_Init(phtable p)			// 表指针
{
	memset(p, 0, sizeof(htable));
}

void __cdecl SYHTable_SetSize(phtable p,		// 表指针
	UINT nMaxSize)		// 数组大小
{
	if (p->maxsize == 0)	// 原来没有元素
	{
		p->eles = (SYHTable_ele_ptr)malloc(sizeof(SYHTable_element) * nMaxSize);
		if (NULL == p->eles)
			return;

		memset(p->eles, 0, sizeof(SYHTable_element)*nMaxSize);
		p->maxsize = nMaxSize;
	}
	else	// 扩充原来的数组
	{
		SYHTable_ele_ptr pTmpAr = (SYHTable_ele_ptr)malloc(sizeof(SYHTable_element) * nMaxSize);

		if (NULL == pTmpAr)
			return;

		// 移动数据
		memcpy(pTmpAr, p->eles, sizeof(SYHTable_element) * p->count);
		memset((pTmpAr + p->count), 0, sizeof(SYHTable_element) * (nMaxSize - p->count));
		free(p->eles);
		p->eles = pTmpAr;
		p->maxsize = nMaxSize;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	表是否已满
//
// 返回值:	TRUE			是
//			FALSE			否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYHTable_IsFull(phtable p)			// 表指针
{
	return (p->count == p->maxsize);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	是否空表
//
// 返回值:	TRUE			是
//			FALSE			否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYHTable_IsEmpty(phtable p)			// 表指针
{
	return (p->count == 0);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取句柄总数
//
// 返回值:	UINT
///////////////////////////////////////////////////////////////////////////////
UINT __cdecl SYHTable_GetCount(phtable p)			// 表指针
{
	return p->count;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	就近插入数据并返回句柄
//
// 返回值:	UINT
///////////////////////////////////////////////////////////////////////////////
UINT __cdecl SYHTable_Insert(phtable p,				// 表指针
	SYHTable_element e)			// 待插入的数据
{
	if (p->count < p->maxsize)
	{
		for (UINT i = 0; i<p->maxsize; i++)
		{
			if (p->eles[i] == NULL)
			{
				p->eles[i] = e;
				p->count++;

				return (i + 1);			// 句柄从 1 开始，0 表示空句柄
			}
		}
	}
	else		// Expand the table size automatically.
	{
		UINT newMaxSize = p->maxsize > 0 ? (p->maxsize << 1) : 1;
		SYHTable_SetSize(p, newMaxSize);
		p->eles[p->count] = e;
		p->count++;

		return p->count;
	}
	return SYHTable_null;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取某个句柄的数据
//
// 返回值:	SYHTable_element
///////////////////////////////////////////////////////////////////////////////
SYHTable_element __cdecl SYHTable_GetAt(phtable p,			// 表指针
	UINT nIndex)			// 句柄
{
	if ((p == NULL) || (p->eles == NULL) || (nIndex <= 0) || (nIndex > p->maxsize))
		return NULL;

	return p->eles[nIndex - 1];
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除某个句柄并返回数据
//
// 返回值:	SYHTable_element
///////////////////////////////////////////////////////////////////////////////
SYHTable_element __cdecl SYHTable_RemoveAt(phtable p,		// 表指针
	UINT nIndex)		// 位置
{
	SYHTable_element e = p->eles[nIndex - 1];

	p->eles[nIndex - 1] = NULL;
	p->count--;

	return e;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除所有的句柄
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYHTable_RemoveAll(phtable p)			// 表指针
{
	memset(p->eles, 0, sizeof(SYHTable_element)*p->maxsize);
	p->count = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置某个句柄的数据值并返回原值
//
// 返回值:	SYHTable_element
///////////////////////////////////////////////////////////////////////////////
SYHTable_element __cdecl SYHTable_SetAt(phtable p,			// 表指针
	UINT nIndex,			// 句柄
	SYHTable_element e)		// 数据
{
	SYHTable_element tmpE = p->eles[nIndex - 1];
	p->eles[nIndex - 1] = e;
	return tmpE;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	哈希表中的有效句柄遍历
//
// 返回值:	UINT / SYHTable_element
///////////////////////////////////////////////////////////////////////////////
// 得到第一个有效的句柄
UINT __cdecl SYHTable_GetFirstValid(phtable p)		// 表指针
{
	if (p->count > 0)
	{
		for (UINT i = 0; i<p->maxsize; i++)
		{
			if (p->eles[i] != NULL)
			{
				return (i + 1);			// 句柄从 1 开始，0 表示空句柄
			}
		}
	}
	return SYHTable_null;
}
// 遍历下一个有效的句柄
SYHTable_element __cdecl SYHTable_GetNextValid(phtable p,		// 表指针
	UINT *pnIndex)	// 句柄
{
	UINT i = *pnIndex;

	*pnIndex = SYHTable_null;
	if (p->count > 0)
	{
		for (; i<p->maxsize; i++)
		{
			if (p->eles[i] != NULL)
			{
				*pnIndex = (i + 1);		// 句柄从 1 开始，0 表示空句柄

				return p->eles[i];
			}
		}
	}
	return NULL;
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

// 调试宏定义
#ifdef _DEBUG
#define SET_NODE_LIST_VALUE(pNode, pList)		(pNode)->parent = (pList)
#define IS_NODE_IN_LIST(pNode, pList)			((pNode)->parent == (pList))
#else
#define SET_NODE_LIST_VALUE(pNode, pList)
#define IS_NOED_IN_LIST(pNode, pList)			TRUE
#endif
///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化链表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYList_Init(pSYList pList)			// 链表指针
{


	if (pList != NULL)
	{
		pList->count = 0;
		pList->head = NULL;
		pList->tail = NULL;
		pList->idelfirst = NULL;
		pList->ideltail = NULL;
		pList->idelpool = NULL;
	}
}
void __cdecl SYList_SetIdelSize(pSYList pList, int nSize)			// 链表指针
{
	pList->idelpool = (listnode*)malloc(sizeof(listnode) * nSize);
	pList->idelfirst = pList->idelpool;
	pList->ideltail = &pList->idelpool[nSize - 1];
	memset(pList->idelpool, 0, sizeof(listnode) * nSize);
	for (int i = 0; i< nSize - 1; i++)
	{
		pList->idelpool[i].next = &pList->idelpool[i + 1];
		pList->idelpool[i + 1].prev = &pList->idelpool[i];
		pList->idelpool[i].idelpool = TRUE;
		pList->idelpool[i + 1].idelpool = TRUE;
	}
}

listnode* __cdecl _SYList_QueryIdelNode(pSYList pList)			// 链表指针
{
	if (!pList->idelfirst)
		return NULL;

	listnode* pnode = pList->idelfirst;
	if (pList->idelfirst == pList->ideltail)
	{
		pList->idelfirst = NULL;
		pList->ideltail = NULL;
		pnode->next = NULL;
		pnode->prev = NULL;
	}
	else
	{
		pnode->next->prev = NULL;
		pList->idelfirst = pnode->next;
	}
	return pnode;
}

BOOL __cdecl _SYList_FreeIdelNode(pSYList pList, listnode* pnode)			// 链表指针
{
	if (!pnode->idelpool)
		return FALSE;

	if (!pList->idelfirst)
	{
		pnode->next = NULL;
		pnode->prev = NULL;
		pList->idelfirst = pnode;
		pList->ideltail = pnode;
	}
	else
	{
		pList->ideltail->next = pnode;
		pnode->prev = pList->ideltail;
		pnode->next = NULL;
		pList->ideltail = pnode;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取头节点位置
//
// 返回值:	LISTPOSITION
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_GetHeadPosition(SYList* pList)	// 链表指针
{
	if (pList == NULL)
		return NULL;

	return pList->head;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取尾节点位置
//
// 返回值:	LISTPOSITION
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_GetTailPosition(SYList* pList)	// 链表指针
{
	if (pList == NULL)
		return NULL;

	return pList->tail;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取链表的节点个数
//
// 返回值:	INT_PTR			当前链表结点个数
///////////////////////////////////////////////////////////////////////////////
INT_PTR	__cdecl SYList_GetCount(SYList* pList)		// 链表指针
{


	if (pList == NULL)
		return DSLT_NULL;

	return pList->count;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断是否为空链表
//
// 返回值:	TRUE		为空
//			FALSE		不为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYList_IsEmpty(pSYList pList)			// 链表指针
{


	if (pList == NULL)
		return FALSE;

	return (pList->count <= 0);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取后一节点位置并返回原节点的数据
//
// 返回值:	LT_DATA_PTR
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_GetNext(const SYList* pList,	// 链表指针
	LISTPOSITION* pPos)	// 当前节点的位置的指针
{


	if (pList == NULL || pPos == NULL || *pPos == NULL)
		return NULL;

	plistnode pNode = (plistnode)(*pPos);

	*pPos = pNode->next;

	return pNode->data;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取前一节点位置并返回原节点的数据
//
// 返回值:	LT_DATA_PTR
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_GetPrev(pSYList pList,		// 链表指针
	LISTPOSITION* pPos)	// 当前节点的位置的指针
{


	if (pList == NULL || pPos == NULL || *pPos == NULL)
		return NULL;

	plistnode pNode = (plistnode)(*pPos);

	*pPos = pNode->prev;

	return pNode->data;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取当前节点的数据
//
// 返回值:	LT_DATA_PTR
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_GetAt(pSYList pList,		// 链表指针
	LISTPOSITION pos)		// 当前节点的位置
{
	if (pList == NULL || pos == NULL)
		return NULL;

	return ((plistnode)pos)->data;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取头节点的数据
//
// 返回值:	LT_DATA_PTR
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_GetHead(pSYList pList)		// 链表指针
{


	if (pList == NULL || pList->head == NULL)
		return NULL;

	return pList->head->data;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取尾节点的数据
//
// 返回值:	LT_DATA_PTR
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_GetTail(pSYList pList)		// 链表指针
{


	if (pList == NULL || pList->tail == NULL)
		return NULL;

	return pList->tail->data;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置某一节点的数据
//
// 返回值:	LT_DATA_PTR	原数据指针
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_SetAt(pSYList pList,			// 链表指针
	LISTPOSITION pos,			// 设置数据的节点位置
	LT_DATA_PTR pData)		// 数据指针
{
	if (pList == NULL || pos == NULL)
		return NULL;

	plistnode pNode = (plistnode)pos;
	LT_DATA_PTR pTempData = pNode->data;

	pNode->data = pData;

	return pTempData;
}
///////////////////////////////////////////////////////////////////////////////
// 描述：	在头节点前增加一个新的节点
//
// 返回值:	LISTPOSITION		新节点的位置
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_AddHead(pSYList pList,			// 链表指针
	LT_DATA_PTR pData,
	unsigned char** pool,
	unsigned char* poolend)		// 新节点的数据
{


	if (pList == NULL)
		return NULL;

	int nsize = sizeof(listnode);
	plistnode pNode = NULL;
	BOOL bRelease = FALSE;
	if (pool)
		bRelease = (*pool + nsize >= poolend);
	else
		bRelease = TRUE;

	if (!bRelease)
		pNode = (plistnode)pool_malloc(pool, nsize, bRelease);
	else
	{
		pNode = _SYList_QueryIdelNode(pList);			// 链表指针
		if (pNode)
		{
			bRelease = FALSE;
		}
		else
			pNode = (plistnode)malloc(sizeof(listnode));
	}

	pNode->needrelease = bRelease;
	pNode->data = pData;
	pNode->prev = NULL;
	pNode->next = pList->head;
	pNode->idelpool = (!bRelease && !pool);
	SET_NODE_LIST_VALUE(pNode, pList);
	// 存在头节点则必存在尾节点
	if (pList->head != NULL)
	{
		pList->head->prev = pNode;
	}
	else
	{
		pList->tail = pNode;
	}
	pList->head = pNode;
	pList->count++;

	return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在尾节点后增加一个新的节点
//
// 返回值:	LISTPOSITION		新节点的位置
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_AddTail(pSYList pList,			// 链表指针
	LT_DATA_PTR pData,
	unsigned char** pool,
	unsigned char* poolend)		// 新节点的数据
{


	if (pList == NULL)
		return NULL;

	int nsize = sizeof(listnode);
	plistnode pNode = NULL;
	BOOL bRelease = FALSE;
	if (pool)
		bRelease = (*pool + nsize >= poolend);
	else
		bRelease = TRUE;

	if (!bRelease)
		pNode = (plistnode)pool_malloc(pool, nsize, bRelease);
	else
	{
		pNode = _SYList_QueryIdelNode(pList);			// 链表指针
		if (!pNode)
			pNode = (plistnode)malloc(sizeof(listnode));
		else
			bRelease = FALSE;
	}

	pNode->needrelease = bRelease;
	pNode->data = pData;
	pNode->next = NULL;
	pNode->prev = pList->tail;
	pNode->idelpool = (!bRelease && !pool);
	SET_NODE_LIST_VALUE(pNode, pList);
	// 存在尾节点则必存在头节点
	if (pList->tail != NULL)
	{
		pList->tail->next = pNode;
	}
	else
	{
		pList->head = pNode;
	}
	pList->tail = pNode;
	pList->count++;

	return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在头节点前增加一个新的链表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYList_AddListHead(pSYList pList,			// 链表指针
	pSYList pNewList)		// 新链表指针
{
	if (pList != NULL && pNewList != NULL && pList != pNewList)
	{
		LISTPOSITION pos = pNewList->tail;

		// 从新链表的尾节点开始向前一个一个加入
		while (pos != NULL)
		{
			SYList_AddHead(pList, SYList_GetPrev(pNewList, &pos), NULL, NULL);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在尾节点后增加一个新的链表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYList_AddListTail(pSYList pList,			// 链表指针
	pSYList pNewList)		// 新链表指针
{
	if (pList != NULL && pNewList != NULL && pList != pNewList)
	{
		LISTPOSITION pos = pNewList->head;

		// 从新链表的头节点开始向后一个一个加入
		while (pos != NULL)
		{
			SYList_AddTail(pList, SYList_GetNext(pNewList, &pos), NULL, NULL);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在某一节点前插一个新的节点
//
// 返回值:	LISTPOSITION		新节点的位置
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_InsertBefore(pSYList pList,		// 链表指针
	LISTPOSITION pos,		// 被插节点的位置
	LT_DATA_PTR pData)	// 新节点数据
{

	if (pList == NULL)
		return NULL;

	plistnode pPosNode = (plistnode)pos;
	plistnode pNode = (plistnode)malloc(sizeof(listnode));

	pNode->needrelease = TRUE;
	pNode->data = pData;
	pNode->next = pPosNode;
	pNode->prev = NULL;
	pNode->idelpool = 0;
	SET_NODE_LIST_VALUE(pNode, pList);
	if (pos == NULL)
	{
		// 当插入位置不存在时判断头节点是否存在
		if (pList->head == NULL)
		{
			pList->head = pNode;
			pList->tail = pNode;
		}
		else
		{
			// 当头节点存在时在头节点前插入新节点
			pList->head->prev = pNode;
			pNode->next = pList->head;
			pList->head = pNode;
		}
	}
	else
	{
		// 当插入位置存在时判断其前一节点是否存在
		if (pPosNode->prev != NULL)
		{
			pPosNode->prev->next = pNode;
			pNode->prev = pPosNode->prev;
			pPosNode->prev = pNode;
		}
		else
		{
			// 如果前一节点不存在则要注意该位置是否就是头节点
			pList->head = pNode;
			pPosNode->prev = pNode;
		}
	}
	pList->count++;

	return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	SYList_InsertAfter() 在某一节点后插一个新的节点
//
// 返回值:	LISTPOSITION		新节点的位置
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_InsertAfter(pSYList pList,		// 链表指针
	LISTPOSITION pos,		// 被插节点的位置
	LT_DATA_PTR pData)	// 新节点数据
{

	if (pList == NULL)
		return NULL;

	plistnode pPosNode = (plistnode)pos;
	plistnode pNode = (plistnode)malloc(sizeof(listnode));

	pNode->needrelease = TRUE;
	pNode->data = pData;
	pNode->next = NULL;
	pNode->prev = pPosNode;
	pNode->idelpool = 0;
	SET_NODE_LIST_VALUE(pNode, pList);
	if (pos == NULL)
	{
		// 当插入位置不存在时判断尾节点是否存在
		if (pList->head == NULL)
		{
			pList->head = pNode;
			pList->tail = pNode;
		}
		else
		{
			// 当头节点存在时在尾节点后插入新节点
			pList->tail->next = pNode;
			pNode->prev = pList->tail;
			pList->tail = pNode;
		}
	}
	else
	{
		// 当插入位置存在时判断其后一节点是否存在
		if (pPosNode->next != NULL)
		{
			pNode->next = pPosNode->next;
			pPosNode->next->prev = pNode;
			pPosNode->next = pNode;
		}
		else
		{
			// 如果后一节点不存在则要注意该位置是否就是尾节点
			pPosNode->next = pNode;
			pList->tail = pNode;
		}
	}
	pList->count++;

	return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除某一节点并返回该节点的数据
//
// 返回值:	LT_DATA_PTR
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_RemoveAt(pSYList pList,		// 链表指针
	LISTPOSITION pos)		// 预删除节点的位置
{
	if (pList == NULL || pos == NULL)
		return NULL;

	plistnode pNode = (plistnode)pos;
	LT_DATA_PTR pData = pNode->data;

	// 判断其前后节点是否存在
	if (pNode->prev != NULL)
	{
		pNode->prev->next = pNode->next;
	}
	else
	{
		pList->head = pNode->next;
	}
	if (pNode->next != NULL)
	{
		pNode->next->prev = pNode->prev;
	}
	else
	{
		pList->tail = pNode->prev;
	}
	pList->count--;
	if (!_SYList_FreeIdelNode(pList, pNode))
	{
		if (pNode->needrelease)
			free(pos);
	}
	//delete pList->idelpool;
	//pList->idelpool = NULL;
	//pList->idelfirst = NULL;
	//pList->ideltail = NULL;
	return pData;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除头节点
//
// 返回值:	LT_DATA_PTR	原尾节点数据
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_RemoveHead(pSYList pList)		// 链表指针
{


	if (pList == NULL || pList->head == NULL)
		return NULL;

	plistnode pNode = pList->head;
	LT_DATA_PTR pData = pNode->data;

	// 判断是否只有一个节点
	if (pNode->next == NULL)
	{
		pList->head = pList->tail = NULL;
	}
	else
	{
		pList->head = pNode->next;
		pNode->next->prev = NULL;
	}
	if (!_SYList_FreeIdelNode(pList, pNode))
	{
		if (pNode->needrelease)
			free(pNode);
	}
	pList->count--;

	return pData;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除尾节点
//
// 返回值:	LT_DATA_PTR	原尾节点数据
///////////////////////////////////////////////////////////////////////////////
LT_DATA_PTR __cdecl SYList_RemoveTail(pSYList pList)		// 链表指针
{


	if (pList == NULL || pList->tail == NULL)
		return NULL;

	plistnode pNode = pList->tail;
	LT_DATA_PTR pData = pNode->data;

	// 判断是否只有一个节点
	if (pNode->prev == NULL)
	{
		pList->head = pList->tail = NULL;
	}
	else
	{
		pList->tail = pNode->prev;
		pNode->prev->next = NULL;
	}
	if (!_SYList_FreeIdelNode(pList, pNode))
	{
		if (pNode->needrelease)
			free(pNode);
	}
	pList->count--;

	return pData;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	删除所有节点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYList_RemoveAll(pSYList pList)		// 链表指针
{
	if (pList == NULL)
		return;

	plistnode pNode = pList->head, pCurNode = NULL;

	while (pNode != NULL)
	{
		pCurNode = pNode;
		pNode = pNode->next;
		if (!_SYList_FreeIdelNode(pList, pCurNode))
		{
			if (pCurNode->needrelease)
				free(pCurNode);
		}
	}
	pList->count = 0;
	pList->head = NULL;
	pList->tail = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	从某一节点位置开始根据数据查找节点的位置
//
// 返回值:	LISTPOSITION
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_FindNode(pSYList pList,			// 链表指针
	LT_DATA_PTR pData,		// 查找的数据
	LISTPOSITION posStart)		// 查找起始位置
{

	if (pList == NULL)
		return NULL;

	// 如果未指定查找起始位置则从头位置开始
	if (posStart == NULL)
	{
		posStart = pList->head;
	}
	while (posStart != NULL)
	{
		LISTPOSITION posCur = posStart;

		if (SYList_GetNext(pList, &posStart) == pData)
			return posCur;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	根据序号查找节点的位置
//
// 返回值:	LISTPOSITION
///////////////////////////////////////////////////////////////////////////////
LISTPOSITION __cdecl SYList_FindByIndex(pSYList pList,		// 链表指针
	INT_PTR nIndex)		// 查找序号
{

	plistnode pNode = NULL;

	if (pList != NULL && nIndex >= 0 && nIndex < pList->count)
	{
		pNode = pList->head;
		while (nIndex--)
		{
			pNode = pNode->next;
		}
	}
	return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	翻转链表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYList_Reverse(pSYList pList)			// 链表指针
{


	// 当节点数小于等于 1 时不需要翻转
	if (pList == NULL || pList->count <= 1)
		return;

	plistnode pCurHeadNode = pList->head;
	plistnode pMoveNode = pList->head->next;

	// 从原头节点位置向后开始，将其后节点逐个移至前面
	while (NULL != pMoveNode)
	{
		pList->head->next = pMoveNode->next;
		pMoveNode->next = pCurHeadNode;
		pCurHeadNode->prev = pMoveNode;
		pCurHeadNode = pMoveNode;
		pMoveNode = pList->head->next;
	}
	pList->tail = pList->head;
	pCurHeadNode->prev = NULL;
	pList->head = pCurHeadNode;
}

void __cdecl SYList_Release(pSYList pList)
{
	SYList_RemoveAll(pList);
	if (pList->idelpool)
		free(pList->idelpool);
	pList->idelpool = NULL;
	pList->idelfirst = NULL;
	pList->ideltail = NULL;
}
