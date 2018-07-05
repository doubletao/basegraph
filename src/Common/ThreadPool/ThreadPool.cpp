#include "stdAfx.h"
#include "Common\ThreadPool.h"
#include <assert.h>
#include <list>
#include "Common\CommonFunction.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//#define DEBUG_HANDLE
//#ifdef DEBUG_HANDLE
//std::set<DWORD> g_setHandleValue;
//#endif // DEBUG_HANDLE


///////////////ThreadPool_Task/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ThreadPool_Task::ThreadPool_Task(ThreadTaskFunc pFunc, LPVOID pParam)
	: m_pParam(pParam)
	, m_pFunc(pFunc)
	, m_hFinish(nullptr)
{
	m_hFinish = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	::ResetEvent(m_hFinish);
}

ThreadPool_Task::~ThreadPool_Task()
{
	if (m_hFinish)
		CloseHandle(m_hFinish);
	//TRACE(_T("Task closed = %p\n"), m_hFinish);
}

void ThreadPool_Task::Run(HANDLE hEnd)
{
	//TRACE(_T("Task begin = %p\n"), m_hFinish);
	if (m_pFunc)
	{
		if (WaitForSingleObject(hEnd, 0) != WAIT_OBJECT_0)
		{
			m_pFunc(m_pParam, hEnd);
		}
	}
	//注意，检测到退出指令导致实际上未执行的情况也被算做完成
	::SetEvent(m_hFinish);
	//TRACE(_T("Task finish = %p\n"), m_hFinish);
}

HANDLE ThreadPool_Task::GetFinishHandle()
{
	return m_hFinish;
}

void ThreadPool_Task::WaitTaskFinish(int nWaitTime)
{
	WaitForSingleObject(m_hFinish, nWaitTime);
}

/////////////////ThreadPool_Worker///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ThreadPool_Worker::ThreadPool_Worker(ThreadPool * pPool)
	: m_pPool(pPool)
	, m_hEnd(nullptr)
	, m_hRun(nullptr)
	, m_hThread(nullptr)
{
	m_hEnd = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	::ResetEvent(m_hEnd);
	m_hRun = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	::ResetEvent(m_hRun);
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
}

ThreadPool_Worker::~ThreadPool_Worker()
{
	StopWork();
	if (m_hEnd)
	{ 
		CloseHandle(m_hEnd);
		m_hEnd = nullptr;
	}
	if (m_hRun)
	{
		CloseHandle(m_hRun);
		m_hRun = nullptr;
	}
}

unsigned int ThreadPool_Worker::WorkThread(LPVOID pParam)
{
	ThreadPool_Worker * pWorker = (ThreadPool_Worker *)pParam;
	if (pWorker)
	{
		HANDLE h[2] = { pWorker->m_hEnd, pWorker->m_hRun };
		while (true)
		{
			DWORD dwResult = WaitForMultipleObjects(2, h, FALSE, INFINITE);
			if (dwResult == WAIT_OBJECT_0)
			{
				break;
			}
			else
			{
				ThreadPool_Task * pTask = pWorker->m_pTask;
				if (pTask)
				{
					pTask->Run(pWorker->m_hEnd);
				}
			}
			::ResetEvent(pWorker->m_hRun);
			//一个任务结束，通知池子
			assert(pWorker->m_pPool);
			if (pWorker->m_pPool)
			{
				pWorker->m_pPool->OnTaskFinish(pWorker);
			}
		}
	}
	return 0;
}

void ThreadPool_Worker::StopWork()
{
	::SetEvent(m_hEnd);
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = nullptr;
	}
}

void ThreadPool_Worker::SetTask(ThreadPool_Task * pTask)
{
	assert(m_pTask == nullptr);
	m_pTask = pTask;
	::SetEvent(m_hRun);
}

bool ThreadPool_Worker::IsRunning()
{
	if (WaitForSingleObject(m_hRun, 0) == WAIT_OBJECT_0)
	{
		return true;
	}
	return false;
}

ThreadPool_Task *& ThreadPool_Worker::GetTask()
{
	return m_pTask;
}

//////////////////ThreadPool//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ThreadPool::ThreadPool()
	: m_nMaxThreadCount(16)//假设8核每个核两个线程
	, m_hEnd(nullptr)
	, m_hNewTask(nullptr)
	, m_hTaskFinish(nullptr)
	, m_hThreadTaskDispatch(nullptr)
{
	m_hEnd = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	::ResetEvent(m_hEnd);
	m_hNewTask = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	::ResetEvent(m_hNewTask);
	m_hTaskFinish = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	::ResetEvent(m_hTaskFinish);
	m_hFinishAll = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	::SetEvent(m_hFinishAll);//池子刚创建时，全部完成的信号量是有信号
	::ResetEvent(m_hTaskFinish);
	InitializeCriticalSection(&m_LockTask);
	InitializeCriticalSection(&m_LockWorker);

	m_hThreadTaskDispatch = (HANDLE)::_beginthreadex(NULL, 0, ThreadTaskDispatch, this, 0, NULL);
}

ThreadPool::~ThreadPool()
{
	Release();
	if (m_hEnd)
	{
		CloseHandle(m_hEnd);
		m_hEnd = nullptr;
	}
	if (m_hNewTask)
	{
		CloseHandle(m_hNewTask);
		m_hNewTask = nullptr;
	}
	if (m_hTaskFinish)
	{
		CloseHandle(m_hTaskFinish);
		m_hTaskFinish = nullptr;
	}
	if (m_hFinishAll)
	{
		CloseHandle(m_hFinishAll);
		m_hFinishAll = nullptr;
	}
	DeleteCriticalSection(&m_LockTask);
	DeleteCriticalSection(&m_LockWorker);
}

unsigned int ThreadPool::ThreadTaskDispatch(LPVOID pParam)
{
	ThreadPool * pPool = (ThreadPool *)pParam;
	if (pPool)
	{
		HANDLE h[3] = { pPool->m_hEnd, pPool->m_hNewTask, pPool->m_hTaskFinish };
		while (true)
		{
			DWORD dwResult = WaitForMultipleObjects(3, h, FALSE, INFINITE);
			if (dwResult == WAIT_OBJECT_0)
			{
				break;
			}
			else if (dwResult == WAIT_OBJECT_0 + 1)
			{
				//先拿到新任务
				while (pPool->HasTask())
				{
					//检查有没有空位
					ThreadPool_Worker * pWorker = pPool->GetIdleWorker();
					if (pWorker)
					{
						ThreadPool_Task * pTask = pPool->PopFrontTask();
						if (pTask)
						{
							//如果有空位，直接生成任务
							pWorker->SetTask(pTask);
						}
						else
						{
							//明明有任务，此时又没有了，这不正常
							assert(pTask);
							pPool->LockWorker();
							pPool->m_setBusiWorker.erase(pWorker);
							pPool->m_setIdleWorker.insert(pWorker);
							pPool->UnLockWorker();
						}
					}
					else
					{
						//如果没有空位，算了，等现有任务完成的通知来处理
						break;
					}
				}
			}
			else
			{
				//有任务完成
				//检查任务列表，如果有新任务，则触发任务信号，否则就不管了
				if (pPool->HasTask())
				{
					::SetEvent(pPool->m_hNewTask);
				}
				else
				{
					//每当有任务完成，就判定一下还有没有待分配的任务和正在执行的任务，如果没有，则重置工作信号
					pPool->LockWorker();
					if (pPool->m_setBusiWorker.size() > 0)
					{
						pPool->UnLockWorker();
						continue;
					}
					pPool->UnLockWorker();

					pPool->LockTask();
					if (pPool->m_lstTask.size() > 0)
					{
						pPool->UnLockTask();
						continue;
					}
					pPool->UnLockTask();
					::SetEvent(pPool->m_hFinishAll);
				}
			}
		}
	}
	return 0;
}

void ThreadPool::LockTask()
{
	EnterCriticalSection(&m_LockTask);
}

void ThreadPool::UnLockTask()
{
	LeaveCriticalSection(&m_LockTask);
}

void ThreadPool::LockWorker()
{
	EnterCriticalSection(&m_LockWorker);
}

void ThreadPool::UnLockWorker()
{
	LeaveCriticalSection(&m_LockWorker);
}

void ThreadPool::ResizePool(int nMaxThreadCount)
{
	m_nMaxThreadCount = nMaxThreadCount;
}

void ThreadPool::WaitAll()
{
	WaitForSingleObject(m_hFinishAll, INFINITE);
}

ThreadPool_Task * ThreadPool::StartNewTask(ThreadTaskFunc pFunc, LPVOID pParam)
{
	//只要有任务加进来，完成的信号量就是reset状态
	::ResetEvent(m_hFinishAll);
	ThreadPool_Task * pTask = new ThreadPool_Task(pFunc, pParam);
	LockTask();
	m_lstTask.push_back(pTask);
	UnLockTask();
	::SetEvent(m_hNewTask);
	return pTask;
}

void ThreadPool::OnTaskFinish(ThreadPool_Worker * pWorker)
{
	//一个worker的任务完成，将其从busi列表中迁移回idle
	LockWorker();
	assert(m_setBusiWorker.count(pWorker) > 0);
	ThreadPool_Task *& pTask = pWorker->GetTask();
	delete pTask;
	pTask = nullptr;
	m_setBusiWorker.erase(pWorker);
	m_setIdleWorker.insert(pWorker);
	UnLockWorker();
	::SetEvent(m_hTaskFinish);
}

ThreadPool_Worker * ThreadPool::GetIdleWorker()
{
	ThreadPool_Worker * pWorker = nullptr;
	LockWorker();
	//有空闲的直接用
	if (m_setIdleWorker.size() > 0)
	{
		pWorker = *m_setIdleWorker.begin();
		m_setIdleWorker.erase(pWorker);
		m_setBusiWorker.insert(pWorker);
	}
	//没有空闲，再查一下总数是不是已经到了池子的上限，如果没到则创建个新的
	else if (m_setBusiWorker.size() < m_nMaxThreadCount)
	{
		pWorker = new ThreadPool_Worker(this);
		m_setBusiWorker.insert(pWorker);
	}
	UnLockWorker();
	return pWorker;
}

bool ThreadPool::HasTask()
{
	bool bRet = false;
	LockTask();
	if (m_lstTask.size() > 0)
	{
		bRet = true;
	}
	UnLockTask();
	return bRet;
}

ThreadPool_Task * ThreadPool::PopFrontTask()
{
	ThreadPool_Task * pTask = nullptr;
	LockTask();
	if (m_lstTask.size() > 0)
	{
		pTask = m_lstTask.front();
		m_lstTask.pop_front();
	}
	UnLockTask();
	return pTask;
}

void ThreadPool::Release()
{
	//清空剩余任务
	LockTask();
	for (std::list<ThreadPool_Task *>::iterator it = m_lstTask.begin(); it != m_lstTask.end(); it++)
	{
		delete *it;
		*it = nullptr;
	}
	m_lstTask.clear();
	UnLockTask();
	//给正在运行的任务发退出指令
	LockWorker();
	for (std::set<ThreadPool_Worker *>::iterator it = m_setBusiWorker.begin(); it != m_setBusiWorker.end(); it++)
	{
		ThreadPool_Worker * pWorker = *it;
		if (pWorker)
		{
			pWorker->StopWork();
		}
	}
	UnLockWorker();
	//等待完全结束
	WaitAll();
	//停止任务分配线程
	::SetEvent(m_hEnd);
	//释放线程
	if (m_hThreadTaskDispatch)
	{
		WaitForSingleObject(m_hThreadTaskDispatch, INFINITE);
		m_hThreadTaskDispatch = nullptr;
	}
	//释放所有工作线程
	assert(m_lstTask.size() == 0);
	assert(m_setBusiWorker.size() == 0);
	for (std::set<ThreadPool_Worker *>::iterator it = m_setIdleWorker.begin(); it != m_setIdleWorker.end(); it++)
	{
		ThreadPool_Worker * pWorker = *it;
		delete pWorker;
		pWorker = nullptr;
	}
	m_setIdleWorker.clear();
}
