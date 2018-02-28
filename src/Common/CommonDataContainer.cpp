#include "stdafx.h"
#include "Common/CommonDataContainer.h"

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
