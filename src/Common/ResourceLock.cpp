// ResourceLock.cpp: implementation of the ResourceLock class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Common\ResourceLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResourceLock::ResourceLock(LPCTSTR szModuleName)
{
	m_hResourceHandle= AfxGetResourceHandle();
	
	HINSTANCE hNewModule= ::GetModuleHandle(szModuleName);
	if (hNewModule){
		AfxSetResourceHandle(hNewModule);	
	}
}

ResourceLock::ResourceLock(HINSTANCE hInst)
{
	m_hResourceHandle= AfxGetResourceHandle();
	if (hInst){
		AfxSetResourceHandle(hInst);	
	}
}

void ResourceLock::Unlock()
{
	if (m_hResourceHandle){
		AfxSetResourceHandle(m_hResourceHandle);
	};
	m_hResourceHandle= NULL;
}

ResourceLock::~ResourceLock()
{
	if (m_hResourceHandle){
		AfxSetResourceHandle(m_hResourceHandle);
	};
}
