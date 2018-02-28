#pragma once
#include <map>
#include <set>
//本文件将C风格一些存储容器如array及hash转为stl的array及hash，仅供过渡期使用，后期会逐渐丢弃

//用map模拟hash表
typedef struct tag_htable 
{
	std::map<int, void *> m_mapContainer;
}htable, *phtable, *LPSYHTABLE;
BASEGRAPH_COMMON_API void __cdecl SYHTable_Init(phtable pHT);
BASEGRAPH_COMMON_API void * __cdecl SYHTable_GetAt(phtable pHT, UINT nIndex);
BASEGRAPH_COMMON_API void * __cdecl SYHTable_RemoveAt(phtable pHT, UINT nIndex);
BASEGRAPH_COMMON_API UINT __cdecl SYHTable_Insert(phtable pHT, void * e);
BASEGRAPH_COMMON_API UINT __cdecl SYHTable_GetFirstValid(phtable pHT);
BASEGRAPH_COMMON_API void * __cdecl SYHTable_GetNextValid(phtable pHT, UINT *pnIndex);
BASEGRAPH_COMMON_API UINT __cdecl SYHTable_GetCount(phtable pHT);
BASEGRAPH_COMMON_API void __cdecl SYHTable_Delete(phtable pHT);			// 表指针
BASEGRAPH_COMMON_API void __cdecl SYHTable_Release(phtable pHT);			// 表指针

//用map模拟hash表
typedef struct tag_hash
{
	std::map<void *, void *> m_mapContainer;
	bool bKeyRelease;
	bool bValueRelease;
	void(*freekeyfn)(void* k);				// 释放 key
	void(*freevaluefn)(void* v);			// 释放 value
}SYHash, *PSYHash, *LPSYHASH;

BASEGRAPH_COMMON_API void __cdecl SYHash_FreeKeyStr(void * pK);
BASEGRAPH_COMMON_API void __cdecl SYHash_FreeValueStr(void * pV);

BASEGRAPH_COMMON_API PSYHash __cdecl SYHash_Create(unsigned int minsize,					// 表初始大小
	UINT64(*hashF)(void *),		// Hash 值计算函函数
	BOOL(*keyEqF)(void *, void *),	// key 比较函数
	void *(*keyDupF)(void *),		// key 拷贝函数
	void(*keyFreeF)(void *),			// key 释放函数
	void(*valueFreeF)(void *));		// value 释放函数
BASEGRAPH_COMMON_API void __cdecl SYHash_Release(PSYHash ph, BOOL bFreeValue);
BASEGRAPH_COMMON_API void __cdecl SYHash_Delete(PSYHash ph, BOOL bFreeValue);
BASEGRAPH_COMMON_API void * __cdecl SYHash_Search(const SYHash* ph, void * pk);
BASEGRAPH_COMMON_API void * __cdecl SYHash_RemoveAt(PSYHash ph, void * pk, BOOL bFreeValue);
BASEGRAPH_COMMON_API BOOL __cdecl SYHash_Insert(PSYHash ph, void * pk, void * pv);