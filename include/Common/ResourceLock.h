// ResourceLock.h: interface for the ResourceLock class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**	执行自动锁定资源，自动并在析构函数中自动解除锁定
*/
class BASEGRAPH_COMMON_API ResourceLock
{
public:
	ResourceLock(LPCTSTR szModuleName);
	ResourceLock(HINSTANCE hInst);
	virtual ~ResourceLock();

	void Unlock();

protected:
	HINSTANCE m_hResourceHandle;	
};


