#pragma once
//////////////////////////////////////////////////////////////////////////
template < typename T1, typename T2> 
class  ResourceUnit
{
public:
	ResourceUnit();
	virtual ~ResourceUnit();
	virtual BOOL Create(LPCTSTR strResName,T2 createparam) = 0;
	virtual void Release();
	LONG Ref();
	LONG Del();
	inline T1*		GetResourcePtr();
	inline LPCTSTR	GetResourceName() const;
protected:			  
	LONG	m_ref;
	TCHAR	m_resource_name[MAX_PATH];
	T1*		m_resource_ptr;
};

template < typename T1, typename T2>
ResourceUnit<T1,T2>::ResourceUnit():m_ref(0),m_resource_ptr(0)
{
}

template <  typename T1, typename T2>
ResourceUnit<T1,T2>::~ResourceUnit()
{
	if(m_resource_ptr)
		m_resource_ptr->Release();
	m_resource_ptr = 0;
}

template <  typename T1, typename T2>
void ResourceUnit<T1,T2>::Release()
{
	if(m_resource_ptr)
		m_resource_ptr->Release();
	m_resource_ptr = 0;
}

template <  typename T1, typename T2 >
LONG ResourceUnit<T1,T2>::Ref()
{
	InterlockedIncrement(&m_ref);
	return m_ref;
}

template <typename T1, typename T2 >
LONG ResourceUnit<T1,T2>::Del()
{
	InterlockedDecrement(&m_ref);
	if(m_ref <= 0)
	{
		if(m_resource_ptr)
			m_resource_ptr->Release();
		m_resource_ptr = 0;
	}
	return m_ref;
}

template <typename T1, typename T2 >
T1*	ResourceUnit<T1,T2>::GetResourcePtr()
{
	return m_resource_ptr;
}

template <typename T1, typename T2 >
LPCTSTR	ResourceUnit<T1,T2>::GetResourceName() const
{
	return m_resource_name;
}
/////////////////////////////////////////////////////
class Autolock
{
public:
	Autolock(LPCRITICAL_SECTION plock)
	{
		m_pLock = plock;
		EnterCriticalSection(m_pLock); 
	}

	~Autolock()
	{
		LeaveCriticalSection(m_pLock); 
	}
private:
	LPCRITICAL_SECTION m_pLock;
};

template <typename T1,typename T2 > class ResourcePool
{
public:
	ResourcePool();
	~ResourcePool();

	T1*	CreateResource(LPCTSTR	strResName ,T2 createparam);	
	void	ReleaseResource(LPCTSTR	strResName);
	void	ReleaseAll();
	T1*	GetResource(LPCTSTR strResName);
protected:	
	CBaseARray<T1*,T1*> m_arResPool;
	int  search_resource(LPCTSTR strResName);
	int  insert_resource(T1* pResUnit);
	CRITICAL_SECTION m_csLock;
};

template <typename T1,typename T2> 
ResourcePool<T1,T2>::ResourcePool()
{
	InitializeCriticalSection(&m_csLock);
}

template <typename T1,typename T2> 
ResourcePool<T1,T2>::~ResourcePool()
{
	ReleaseAll();
	DeleteCriticalSection(&m_csLock); 
}

template <typename T1,typename T2> 
T1*	ResourcePool<T1,T2>::CreateResource(LPCTSTR strResName,T2 createparam)
{
	Autolock lock(&m_csLock);
	int i = search_resource(strResName);
	try
	{
		if(i<0)
		{	
			T1* pRes = new T1() ;
			if(!pRes->Create(strResName,createparam))
			{
				delete pRes;
				return NULL;
			}
			i = insert_resource(pRes);
			m_arResPool[i]->Ref();	
			return m_arResPool[i];
		}
		else
		{
			m_arResPool[i]->Ref();
			return m_arResPool[i];
		}
	}
	catch (...)
	{
		return NULL;
	}
}

template <typename T1,typename T2> 
void	ResourcePool<T1,T2>::ReleaseResource(LPCTSTR strResName)
{
	Autolock lock(&m_csLock);

	int i = search_resource(strResName);
	if(i>=0)
	{
		int ref = m_arResPool[i]->Del();
		if(ref <= 0)
		{
			SAFE_DELETE(m_arResPool[i]);
			m_arResPool.RemoveAt(i);
		}
	}
}

template <typename T1,typename T2> 
void	ResourcePool<T1,T2>::ReleaseAll()
{
	Autolock lock(&m_csLock);

	int n = (int)m_arResPool.GetSize();

	while(--n>=0)
	{
		m_arResPool[n]->Release();
		if(m_arResPool[n])
		{
			delete m_arResPool[n];
			m_arResPool[n] = 0;
		}
	}
	m_arResPool.RemoveAll();
}

template <typename T1,typename T2> 
int ResourcePool<T1,T2>::search_resource(LPCTSTR strResName)
{
	int n = m_arResPool.GetSize();
	if(n <= 0)
		return -1;
	int begin = 0, end = n ,mid;
	try
	{
		do
		{
			mid = begin + (end - begin)/2;
			if(_tcscmp(strResName,m_arResPool[mid]->GetResourceName()) == 0)
				return mid;
			else if(_tcscmp(strResName,m_arResPool[mid]->GetResourceName()) > 0)
				begin = mid + 1;
			else 
				end = mid ;
		}while(begin < end);
	}
	catch (...)
	{
		//ASSERT(0);
	}
	return -1;
}

template <typename T1,typename T2> 
int ResourcePool<T1,T2>::insert_resource(T1* pResUnit)
{
	Autolock lock(&m_csLock);
	int n = (int)m_arResPool.GetSize();
	if(n <= 0)
	{
		m_arResPool.Add(pResUnit);
		return 0;
	}
	else
	{
		int begin = 0, end = n ,mid;
		do
		{
			mid = begin + (end - begin)/2;

			if(_tcscmp(pResUnit->GetResourceName(),m_arResPool[mid]->GetResourceName()) > 0)
				begin = mid + 1;
			else
				end = mid ;

		}while(begin < end);
		m_arResPool.InsertAt(begin,pResUnit);
		return begin;
	}
}

template <typename T1,typename T2> 
T1*	ResourcePool<T1,T2>::GetResource(LPCTSTR strResName)
{
	Autolock lock(&m_csLock);
	int i = search_resource(strResName);
	try
	{
		if(i<0)
		{	
			return NULL;
		}
		else
		{
			m_arResPool[i]->Ref();
			return m_arResPool[i];
		}
	}
	catch (...)
	{
		return NULL;
	}

}
struct BaseGraphTechUnit
{
	BaseGraphTechUnit()
	{
		m_pTech = 0;
		m_pLayout = 0;
	}

	~BaseGraphTechUnit()
	{
		Release();
	}

	void Release()
	{
		if(m_pLayout)
			m_pLayout->Release();
		m_pLayout = 0;
		m_pTech = 0;
	}
	ID3D10EffectTechnique*	m_pTech;
	ID3D10InputLayout*	m_pLayout;
};

struct BaseGraphTechCreateParam
{
	BaseGraphTechCreateParam()
	{
		pInputDesc = 0;
		nInputElements = 0;
		pd3deffect = 0;
		pDevice = 0;
		pTechName = 0;	
	}
	ID3D10Effect*					pd3deffect;
	const D3D10_INPUT_ELEMENT_DESC*	pInputDesc;
	UINT							nInputElements;
	ID3D10Device*					pDevice;
	LPCTSTR							pTechName;				
};	

struct BaseGraphEffectCreateParam
{
	BaseGraphEffectCreateParam()
	{
		pTechParam	= 0;
		nTechs		= 0;
		pDefines	= 0;
		pDevice		= 0;
		pPath		= 0;
		pEffectName = 0;
	}
	
	BaseGraphTechCreateParam*	pTechParam;
	int					nTechs;
	D3D10_SHADER_MACRO*	pDefines;
	ID3D10Device*		pDevice;
	LPCTSTR				pPath;
	LPCTSTR				pEffectName;
};
//////////////////////////////////////////////////////////////////////////
class  CTechUnit:public ResourceUnit <BaseGraphTechUnit, BaseGraphTechCreateParam*>
{
public:
	CTechUnit(){};	
	virtual ~CTechUnit();
	virtual void Release();
	virtual BOOL Create(LPCTSTR	strResName,BaseGraphTechCreateParam* pCreateParam);
};

class  CEffectUnit:public ResourceUnit <ID3D10Effect,BaseGraphEffectCreateParam*>
{
public:
	CEffectUnit(){};
	virtual ~CEffectUnit();
	virtual BOOL Create(LPCTSTR strResName,BaseGraphEffectCreateParam* pCreateParam);
	virtual void Release();
	CTechUnit*	GetTechnique(LPCTSTR strTechName);
protected :
	ResourcePool<CTechUnit, BaseGraphTechCreateParam*>	m_TechPool;
};

class BaseGraphD3DEffectManager
{
private:
	BaseGraphD3DEffectManager(void);
	int	CreateQuadVB();
	ResourcePool<CEffectUnit,BaseGraphEffectCreateParam*> m_EffectPool;
	ID3D10Device*	m_d3d_device;
	TCHAR		m_fx_path[MAX_PATH] ;	
	int			m_hQuadVB;
public:
	BaseGraphD3DEffectManager(ID3D10Device* device);
	~BaseGraphD3DEffectManager(void);
	CEffectUnit*	CreateEffectUnit(BaseGraphEffectCreateParam* pCreateParam);	
	void			ReleaseEffectUnit(LPCTSTR strEffectName);
	CTechUnit*	GetTechUnit(LPCTSTR strEffectName,LPCTSTR strTechName);	
	int			GetQuadVB()
	{
		return m_hQuadVB;
	}
	void			SetQuadVB(int hQuadVB)
	{
		m_hQuadVB = hQuadVB;
	}
};
