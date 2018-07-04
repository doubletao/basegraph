#include "stdafx.h"
#include "DataBufferManager.h"
#include <assert.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

class AutoLockTool
{
private:
	CRITICAL_SECTION & m_lock;
public:
	AutoLockTool(CRITICAL_SECTION & lock) : m_lock(lock)
	{
		EnterCriticalSection(&m_lock);
	}
	~AutoLockTool()
	{
		LeaveCriticalSection(&m_lock);
	}
};

DataBufferManager::DataBufferManager()
	: m_nWidthSeed(120)
	, m_nHeightSeed(70)
{
	InitializeCriticalSection(&m_Lock);
}


DataBufferManager::~DataBufferManager()
{
	Lock();
	for (std::map<int, ObjectBuffer *>::iterator it = m_map2DBuffer.begin(); it != m_map2DBuffer.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
	UnLock();
	DeleteCriticalSection(&m_Lock);
}

int DataBufferManager::GetNextID()
{
	AutoLockTool lock(m_Lock);
	if (m_map2DBuffer.size() > 0)
	{
		return m_map2DBuffer.rbegin()->first + 1;
	}
	return 1;
}

int DataBufferManager::CreateBuffer(int nWidth, int nHeight)
{
	AutoLockTool lock(m_Lock);
	auto GetAdptValue = [](int nReq, int nSeed)
	{
		bool bDiv = nReq % nSeed > 0 ? false : true;
		int nReserve = nReq / nSeed;
		if (bDiv)
		{
			nReserve = nReserve * nSeed;
		}
		else
		{
			nReserve = (nReserve + 1) * nSeed;
		}
		return nReserve;
	};

	int nReserveWidth = GetAdptValue(nWidth, m_nWidthSeed);
	int nReserveHeight = GetAdptValue(nHeight, m_nHeightSeed);
	ObjectBuffer * pBuffer = new ObjectBuffer;
	if (pBuffer && pBuffer->InitializeBuffer(nReserveWidth, nReserveHeight))
	{
		if (pBuffer->FitIn(nWidth, nHeight))
		{
			pBuffer->SetUsed(nWidth, nHeight);
			int nID = GetNextID();
			m_map2DBuffer[nID] = pBuffer;
			return nID;
		}
		else
		{
			assert(false);
			delete pBuffer;
		}
	}
	
	return 0;
}

DataBufferManager & DataBufferManager::Instance()
{
	static DataBufferManager inst;
	return inst;
}

int DataBufferManager::GetWidthGrowSeed()
{
	return m_nWidthSeed;
}

int DataBufferManager::GetHeightGrowSeed()
{
	return m_nHeightSeed;
}

void DataBufferManager::InitialSeed(int nWidthSeed, int nHeightSeed)
{
	m_nWidthSeed = nWidthSeed;
	m_nHeightSeed = nHeightSeed;
}

int DataBufferManager::Query2DBuffer(int nWidth, int nHeight)
{
	AutoLockTool lock(m_Lock);
	//先遍历已有buf库，查找合适的buf
	for (std::map<int, ObjectBuffer *>::iterator it = m_map2DBuffer.begin(); it != m_map2DBuffer.end(); it++)
	{
		if (it->second && it->second->FitIn(nWidth, nHeight))
		{
			it->second->SetUsed(nWidth, nHeight);
			return it->first;
		}
	}
	//没找到，创建一个
	return CreateBuffer(nWidth, nHeight);
}

ObjectBuffer * DataBufferManager::Query2DBuffer(int & nBufferID, int nWidth, int nHeight)
{
	AutoLockTool lock(m_Lock);
	UnQuery2DBuffer(nBufferID);
	nBufferID = Query2DBuffer(nWidth, nHeight);
	return Get2DBuf(nBufferID);
}

bool DataBufferManager::UnQuery2DBuffer(int & nBufferID)
{
	AutoLockTool lock(m_Lock);
	if (m_map2DBuffer.count(nBufferID) > 0)
	{
		if (m_map2DBuffer[nBufferID])
		{
			m_map2DBuffer[nBufferID]->ReleaseUse();
			nBufferID = 0;
			return true;
		}
	}
	return false;
}

bool DataBufferManager::Compress2DBuffer()
{
	AutoLockTool lock(m_Lock);
	for (std::map<int, ObjectBuffer *>::iterator it = m_map2DBuffer.begin(); it != m_map2DBuffer.end();)
	{
		if (it->second)
		{
			if (!it->second->IsUsed())
			{
				delete it->second;
				it->second = nullptr;
				it = m_map2DBuffer.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it = m_map2DBuffer.erase(it);
		}
	}
	return true;
}

ObjectBuffer * DataBufferManager::Get2DBuf(int nBufferID)
{
	AutoLockTool lock(m_Lock);
	if (m_map2DBuffer.count(nBufferID) > 0)
	{
		return m_map2DBuffer[nBufferID];
	}
	return nullptr;
}

bool DataBufferManager::Clear(int nBufferID, COLORREF color)
{
	AutoLockTool lock(m_Lock);
	if (m_map2DBuffer.count(nBufferID) > 0)
	{
		if (m_map2DBuffer[nBufferID])
		{
			m_map2DBuffer[nBufferID]->Clear(color);
			return true;
		}
	}
	return false;
}

void DataBufferManager::Lock()
{
	EnterCriticalSection(&m_Lock);
}

void DataBufferManager::UnLock()
{
	LeaveCriticalSection(&m_Lock);
}
