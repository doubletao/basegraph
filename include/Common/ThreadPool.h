#pragma once
#include <list>
#include <set>

//一个参数，一个退出指令
typedef DWORD(*ThreadTaskFunc)(LPVOID pParam, HANDLE hEnd);

class BASEGRAPH_COMMON_API ThreadPool_Task
{
protected:
	LPVOID m_pParam;
	ThreadTaskFunc m_pFunc;
	HANDLE m_hFinish;
public:
	ThreadPool_Task(ThreadTaskFunc pFunc, LPVOID pParam);
	~ThreadPool_Task();
	void Run(HANDLE hEnd);
	HANDLE GetFinishHandle();
	void WaitTaskFinish(int nWaitTime);
};

class ThreadPool;
class BASEGRAPH_COMMON_API ThreadPool_Worker
{
protected:
	HANDLE m_hEnd;
	HANDLE m_hRun;
	HANDLE m_hThread;
	ThreadPool_Task * m_pTask;
	ThreadPool * m_pPool;
public:
	ThreadPool_Worker(ThreadPool * pPool);
	~ThreadPool_Worker();
public:
	static unsigned int __stdcall WorkThread(LPVOID pParam);//工作线程
	void StopWork();
	void SetTask(ThreadPool_Task * pTask);
	bool IsRunning();
	ThreadPool_Task *& GetTask();
};

class BASEGRAPH_COMMON_API ThreadPool
{
protected:
	CRITICAL_SECTION m_LockTask;
	CRITICAL_SECTION m_LockWorker;

	HANDLE m_hEnd;//结束
	HANDLE m_hNewTask;//新任务到达
	HANDLE m_hTaskFinish;//有任务完成
	HANDLE m_hThreadTaskDispatch;

	int m_nMaxThreadCount;//池的最大线程数（不设最大任务数）
	std::list<ThreadPool_Task *> m_lstTask;

	std::set<ThreadPool_Worker *> m_setIdleWorker;
	std::set<ThreadPool_Worker *> m_setBusiWorker;
public:
	ThreadPool();
	~ThreadPool();
protected:
	static unsigned int __stdcall ThreadTaskDispatch(LPVOID pParam);

	void LockTask();
	void UnLockTask();
	void LockWorker();
	void UnLockWorker();

	void Release();
	ThreadPool_Worker * GetIdleWorker();
	bool HasTask();
	ThreadPool_Task * PopFrontTask();
public:
	void ResizePool(int nMaxThreadCount);//如果当前任务数已经大于原有size了，则不会释放已有的，所以最好在一开始就设置好size，之后只会增长，不会减少
	void WaitAll();//注意，waitall函数仅wait调用时已有的任务，如果调用后又添加了新任务，则不wait
	ThreadPool_Task * StartNewTask(ThreadTaskFunc pFunc, LPVOID pParam);
	void OnTaskFinish(ThreadPool_Worker * pWorker);
	
};