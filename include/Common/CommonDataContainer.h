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
BASEGRAPH_COMMON_API void __cdecl SYHTable_RemoveAll(phtable pHT);

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

//array相关，随后要干掉
// 元素定义
#define SYAR_ELE_LEN	sizeof(void*)		// 元素类型的长度
typedef void*			SYAR_ELEMENT;		// 定义元素的类型
typedef SYAR_ELEMENT*	SYAR_ELE_PTR;

// 返回值定义
#define SYAR_SUCCESS			0			// 成功（只会在不返回索引的接口中使用）
#define SYAR_EMPTY				-1			// 空数组
#define SYAR_FAILED				-2			// 失败
#define SYAR_NULL				-3			// 不存在的数组
#define SYAR_INVALID_PARAM		-4			// 无效参数
#define SYAR_MEMORY_ERROR		-5			// 内存错误

// 数据结构定义
typedef struct tag_SYArray {
	SYAR_ELE_PTR	m_pData;				// 元素组指针
	INT_PTR			m_nSize;				// 用户当前占用的数组大小
	INT_PTR			m_nMaxSize;				// 当前实际分配的数组大小
	INT_PTR			m_nGrowBy;				// 分配内存时增长的元素个数
}SYArray, *LPSYARRAY;

///////////////////////////////////////////////////////////////////////////////
// 描述：	创建一个空数组
//
// 返回值：	NULL		创建失败（内存不足）
//			pA			创建的空数组的指针
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LPSYARRAY __cdecl SYAR_Create();

///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化一个空数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_Init(LPSYARRAY pA);			// 数组结构指针

															///////////////////////////////////////////////////////////////////////////////
															// 描述：	删除一个数组
															//
															// 返回值：	SYAR_SUCCESS		成功
															//			SYAR_NULL			空指针
															///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_Release(LPSYARRAY pA);			// 数组结构指针

																//////////////////////////////////////////////////////////////////////////
																//清除数组内容，保留内存大小
BASEGRAPH_COMMON_API int __cdecl SYAR_Clear(LPSYARRAY par);

///////////////////////////////////////////////////////////////////////////////
// 描述：	实现数组大小的设定
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//			SYAR_INVALID_PARAM	无效参数
//			SYAR_MEMORY_ERROR	内存错误
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_SetSize(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nNewSize,		// 新的数组大小
	INT_PTR nGrowBy);		// 需要增大数组时增加的元素个数

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	设置最大数组大小，不会添加数组元素
							//
							// 返回值：	SYAR_SUCCESS		成功
							//			SYAR_NULL			空指针
							//			SYAR_INVALID_PARAM	无效参数
							//			SYAR_MEMORY_ERROR	内存错误
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_SetMaxSize(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nNewSize		// 新的数组大小
);

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取数组大小
//
// 返回值：	m_nSize			数组大小
//			SYAR_NULL		pA == NULL
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR	__cdecl SYAR_GetSize(const SYArray* pA);		// 数组结构指针
BASEGRAPH_COMMON_API int	__cdecl SYAR_GetCount(const SYArray* pA);	// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	获取数组的上界
																//
																// 返回值：	m_nSize-1		数组上界
																//			SYAR_NULL		pA == NULL
																//			SYAR_EMPTY		空数组（-1）
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR	__cdecl SYAR_GetUpperBound(LPSYARRAY pA);	// 数组结构指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	判断是否为空数组
																	//
																	// 返回值：	TRUE		为空
																	//			FALSE		不为空
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYAR_IsEmpty(LPSYARRAY pA);			// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	释放超越上界的元素的内存
																//
																// 返回值：	SYAR_SUCCESS		成功
																//			SYAR_NULL			空指针
																//			SYAR_MEMORY_ERROR	内存错误
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_FreeExtra(LPSYARRAY pA);		// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	释放所有元素的内存
																//
																// 返回值：	SYAR_SUCCESS		成功
																//			SYAR_NULL			空指针
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_RemoveAll(LPSYARRAY pA);		// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	获取数据指针
																//
																// 返回值：	NULL			pA == NULL
																//			SYAR_ELE_PTR	当前数据指针
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API SYAR_ELE_PTR __cdecl SYAR_GetData(LPSYARRAY pA);		// 数组结构指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	获取某一元素
																	//
																	// 返回值：	SYAR_ELEMENT		索引元素数据
																	//			NULL				pA == NULL || nIndex 无效
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API SYAR_ELEMENT __cdecl SYAR_GetAt(const SYArray* pA,		// 数组结构指针
	INT_PTR nIndex);	// 元素索引号

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	设置某一非越界元素的值
						//
						// 返回值：	NULL				失败
						//			SYAR_ELEMENT		旧元素数据
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API SYAR_ELEMENT __cdecl SYAR_SetAt(LPSYARRAY pA,				// 数组结构指针
	INT_PTR nIndex,				// 元素索引号
	SYAR_ELEMENT newData);		// 预设元素值

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	在数组尾扩充添加一个元素
								//
								// 返回值：	索引				添加成功
								//			SYAR_NULL			空指针
								//			SYAR_FAILED			失败
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR __cdecl SYAR_Add(LPSYARRAY pA,				// 数组结构指针
	SYAR_ELEMENT newData);		// 预设元素值

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	在数组尾扩充添加另一个数组
								//
								// 返回值：	SYAR_SUCCESS		成功
								//			SYAR_NULL			空指针
								//			SYAR_FAILED			失败
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_Append(LPSYARRAY pA,			// 数组结构指针
	LPSYARRAY pSrc);		// 预添加的数组

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	复制另一个数组的内容
							//
							// 返回值：	SYAR_SUCCESS		成功
							//			SYAR_NULL			空指针
							//			SYAR_FAILED			失败
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_Copy(LPSYARRAY pA,			// 数组结构指针
	LPSYARRAY pSrc);		// 预复制的数组

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	在某一非越界元素前或者最后一个元素后插入几个新元素
							//
							// 返回值：	索引				最后一个插入的元素
							//			SYAR_NULL			空指针
							//			SYAR_FAILED			失败
							//			SYAR_INVALID_PARAM	无效参数
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR __cdecl SYAR_InsertAt(LPSYARRAY pA,				// 数组结构指针
	INT_PTR nIndex,			// 元素索引号
	SYAR_ELEMENT newData,		// 预插入元素值
	INT_PTR nCount);			// 预插入的元素个数

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	在某一非越界元素前或者最后一个元素后插入一个数组
								//
								// 返回值：	SYAR_SUCCESS		成功
								//			SYAR_NULL			空指针
								//			SYAR_FAILED			失败
								//			SYAR_INVALID_PARAM	无效参数
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_InsertArrayAt(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nIndex,			// 元素索引号
	LPSYARRAY pSrc);			// 预插入数组

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	从某一非越界元素开始删除几个元素
								//
								// 返回值：	SYAR_SUCCESS		成功
								//			SYAR_NULL			空指针
								//			SYAR_FAILED			失败
								//			SYAR_INVALID_PARAM	无效参数
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API SYAR_ELEMENT __cdecl SYAR_RemoveAt(LPSYARRAY pA,			// 数组结构指针
	INT_PTR nIndex,		// 元素索引号
	INT_PTR nCount);		// 预删除的元素个数

							//////////////////////////////////////////////////////////////////////////
							// 描述：	交换两个非越界元素的值
							//
							// 返回值：	SYAR_SUCCESS		成功
							//			SYAR_NULL			空指针
							//			SYAR_INVALID_PARAM	无效参数
							//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_Swap(LPSYARRAY pA,			// 数组结构指针
	INT_PTR i,			// 元素索引号
	INT_PTR j);

//////////////////////////////////////////////////////////////////////////
// 描述：	翻转数组
//
// 返回值：	SYAR_SUCCESS		成功
//			SYAR_NULL			空指针
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl SYAR_Reverse(LPSYARRAY pA);		// 数组结构指针

															//////////////////////////////////////////////////////////////////////////
															// 描述：	移动数组内元素的位置：把a[i]移动到a[j]之前紧靠a[j]的位置 
															// 注意：	i和j都是在移动动作发生之前的数组中的位置，移动完毕后保证除了a[i]
															//			之外的元素的相对顺序不变，并且原来<j的元素都在移动后的a[i]之前，
															//			原来>=j的元素都在移动后的a[i]之后 (靠前表示位置索引较小)
															// 示例：	对于数组[1,2,3,4,5]，如果i=3,j=1则得到：[1,4,2,3,5]
															//			如果i=1,j=3,则得到：[1,3,2,4,5]，注意结果中2的位置并不是(j==3)
															//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYAR_MoveElement(LPSYARRAY pA,		// 数组结构指针
	size_t i,			// 要移动的元素的索引，应该属于[0,size)
	size_t j);		// 要移动到的位置，应该属于[0,size]

typedef int(*SYCompareFun)(const void*, const void*);
BASEGRAPH_COMMON_API void __cdecl SYAR_Sort(LPSYARRAY pA, SYCompareFun fun);
///////////////////////////////////////////////////////////////////////////////
// 类封装
#ifdef __cplusplus

template<class TYPE, class ARG_TYPE = const TYPE&>
class CSYArray
{
public:
	CSYArray();
	~CSYArray();

	INT_PTR	GetSize() const;
	int		GetCount() const;
	BOOL	IsEmpty();
	INT_PTR	GetUpperBound() const;
	void	SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

	void	FreeExtra();
	void	RemoveAll();

	// 对数据的常量引用
	const	TYPE& GetAt(INT_PTR nIndex) const;
	TYPE&	GetAt(INT_PTR nIndex);
	// 对数据的直接引用
	const TYPE& ElementAt(INT_PTR nIndex) const;
	TYPE&	ElementAt(INT_PTR nIndex);
	TYPE	SetAt(INT_PTR nIndex, ARG_TYPE newElement);

	const	TYPE* GetData() const;
	TYPE*	GetData();

	INT_PTR	Add(ARG_TYPE newElement);
	INT_PTR Append(const CSYArray& src);
	INT_PTR Append(const SYArray& src);
	void	Copy(const CSYArray& src);

	const	TYPE& operator[](INT_PTR nIndex) const;
	TYPE&	operator[](INT_PTR nIndex);

	INT_PTR	InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount = 1);
	void	RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void	InsertAt(INT_PTR nStartIndex, CSYArray* pNewArray);

	void	Swap(INT_PTR i, INT_PTR j);
	void	Reverse();
	void	MoveElement(INT_PTR i, INT_PTR j); //说明参见SYAR_MoveElement

											   // Purpose:
											   // 使用数组之前，预设数组大小后， 设定m_nSize = 0;
	void SetCurSize(const INT_PTR nSize) { m_nSize = nSize; }


protected:
	TYPE*			m_pData;				// 元素组指针
	INT_PTR			m_nSize;				// 用户当前定义的数组大小
	INT_PTR			m_nMaxSize;				// 当前实际分配的数组大小
	INT_PTR			m_nGrowBy;				// 分配内存时增长的元素个数
};

// inline implementions.

template<class TYPE, class ARG_TYPE>
inline INT_PTR CSYArray<TYPE, ARG_TYPE>::GetSize() const
{
	return m_nSize;
}

template<class TYPE, class ARG_TYPE>
inline int CSYArray<TYPE, ARG_TYPE>::GetCount() const
{
	return (int)m_nSize;
}

template<class TYPE, class ARG_TYPE>
inline BOOL CSYArray<TYPE, ARG_TYPE>::IsEmpty()
{
	return (m_nSize == 0);
}

template<class TYPE, class ARG_TYPE>
inline INT_PTR CSYArray<TYPE, ARG_TYPE>::GetUpperBound() const
{
	return (m_nSize - 1);
}

template<class TYPE, class ARG_TYPE>
inline void CSYArray<TYPE, ARG_TYPE>::RemoveAll()
{
	SetSize(0, -1);
}

template<class TYPE, class ARG_TYPE>
inline const TYPE& CSYArray<TYPE, ARG_TYPE>::ElementAt(INT_PTR nIndex) const
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));

	if (nIndex < 0)
	{
		nIndex = 0;
	}
	else if (nIndex >= m_nSize)
	{
		nIndex = m_nSize - 1;
	}
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
inline TYPE& CSYArray<TYPE, ARG_TYPE>::ElementAt(INT_PTR nIndex)
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));

	if (nIndex < 0)
	{
		nIndex = 0;
	}
	else if (nIndex >= m_nSize)
	{
		nIndex = m_nSize - 1;
	}
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
inline const TYPE& CSYArray<TYPE, ARG_TYPE>::GetAt(INT_PTR nIndex) const
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));

	if (nIndex < 0 || nIndex >= m_nSize)
	{
		nIndex = 0;
	}
	else if (nIndex >= m_nSize)
	{
		nIndex = m_nSize - 1;
	}
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
inline TYPE& CSYArray<TYPE, ARG_TYPE>::GetAt(INT_PTR nIndex)
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));

	if (nIndex < 0 || nIndex >= m_nSize)
	{
		nIndex = 0;
	}
	else if (nIndex >= m_nSize)
	{
		nIndex = m_nSize - 1;
	}
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
inline TYPE CSYArray<TYPE, ARG_TYPE>::SetAt(INT_PTR nIndex, ARG_TYPE newElement)
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));

	TYPE tmpData = 0;

	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		tmpData = m_pData[nIndex];
		m_pData[nIndex] = newElement;
	}
	return tmpData;
}

template<class TYPE, class ARG_TYPE>
inline const TYPE* CSYArray<TYPE, ARG_TYPE>::GetData() const
{
	return (const TYPE*)m_pData;
}

template<class TYPE, class ARG_TYPE>
inline TYPE* CSYArray<TYPE, ARG_TYPE>::GetData()
{
	return (TYPE*)m_pData;
}

template<class TYPE, class ARG_TYPE>
inline INT_PTR CSYArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
{
	INT_PTR nIndex = m_nSize;

	SetSize(nIndex + 1, -1);
	SetAt(nIndex, newElement);

	return nIndex;
}

template<class TYPE, class ARG_TYPE>
inline const TYPE& CSYArray<TYPE, ARG_TYPE>::operator[](INT_PTR nIndex) const
{
	return GetAt(nIndex);
}

template<class TYPE, class ARG_TYPE>
inline TYPE& CSYArray<TYPE, ARG_TYPE>::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

// out-of-inline implementions.

template<class TYPE, class ARG_TYPE>
CSYArray<TYPE, ARG_TYPE>::CSYArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
CSYArray<TYPE, ARG_TYPE>::~CSYArray()
{
	if (m_pData != NULL)
	{
		delete[](BYTE*)m_pData;
	}
}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	//assert(nNewSize >= 0);

	if (nNewSize < 0)
		return;

	if (nGrowBy >= 0)
	{
		m_nGrowBy = nGrowBy;
	}
	// 设定新的数组大小
	if (nNewSize == 0)
	{
		// 释放数组所占内存并在指定的情况下处理元素所占内存
		if (m_pData != NULL)
		{
			for (int i = 0; i < m_nSize; i++)
				(m_pData + i)->~TYPE();
			delete[](BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// 申请足够的空间来满足nNewSize或m_nGrowBy的要求
		size_t nAllocSize = (nNewSize > m_nGrowBy) ? nNewSize : m_nGrowBy;

		m_pData = (TYPE*) new BYTE[(size_t)nAllocSize * sizeof(TYPE)];
		memset((void*)m_pData, 0, (size_t)nAllocSize * sizeof(TYPE));

		//当m_pData含有__vfptr时，memset使之为0，需要new
		for (int i = 0; i < nNewSize; i++)
#pragma push_macro("new")
#undef new
			::new((void*)(m_pData + i)) TYPE;
#pragma pop_macro("new")

		m_nSize = nNewSize;
		m_nMaxSize = nAllocSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// 无需增大数组的大小
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			memset((void*)(m_pData + m_nSize), 0, (size_t)(nNewSize - m_nSize) * sizeof(TYPE));
			for (int i = 0; i < nNewSize - m_nSize; i++)
#pragma push_macro("new")
#undef new
				::new((void*)(m_pData + m_nSize + i)) TYPE;
#pragma pop_macro("new")
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			for (int i = 0; i < m_nSize - nNewSize; i++)
				(m_pData + nNewSize + i)->~TYPE();
		}
		m_nSize = nNewSize;
	}
	else
	{
		// 需要增大数组
		nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		INT_PTR nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around

										//if(nNewMax  < m_nMaxSize)
										//	AfxThrowInvalidArgException();

#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX / sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = (TYPE*) new BYTE[(size_t)nNewMax * sizeof(TYPE)];

		// copy new data from old
		memcpy_s(pNewData, (size_t)nNewMax * sizeof(TYPE), m_pData, (size_t)m_nSize * sizeof(TYPE));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);
		memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize - m_nSize) * sizeof(TYPE));
		for (int i = 0; i < nNewSize - m_nSize; i++)
#pragma push_macro("new")
#undef new
			::new((void*)(pNewData + m_nSize + i)) TYPE;
#pragma pop_macro("new")

		// get rid of old stuff (note: no destructors called)
		delete[](BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::FreeExtra()
{
	if (m_nSize != m_nMaxSize)
	{
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			// 重新申请内存并拷贝数据
			pNewData = (TYPE*) new BYTE[(size_t)m_nSize * sizeof(TYPE)];
			memcpy((void*)pNewData,
				(void*)m_pData,
				(size_t)m_nSize * sizeof(TYPE));
		}
		delete[](BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE, class ARG_TYPE>
INT_PTR CSYArray<TYPE, ARG_TYPE>::Append(const CSYArray& src)
{
	//assert(this != &src);   // cannot append to itself

	if (this == &src)
		return SYAR_INVALID_PARAM;

	INT_PTR nOldSize = m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy((void*)(m_pData + nOldSize),
		(void*)src.m_pData,
		(size_t)src.m_nSize * sizeof(TYPE));

	return nOldSize;
}


template<class TYPE, class ARG_TYPE>
INT_PTR CSYArray<TYPE, ARG_TYPE>::Append(const SYArray& src)
{
	//assert(this != &src);   // cannot append to itself

	INT_PTR nOldSize = m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy((void*)(m_pData + nOldSize),
		(void*)src.m_pData,
		(size_t)src.m_nSize * sizeof(TYPE));

	return nOldSize;
}


template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::Copy(const CSYArray& src)
{
	//assert(this != &src);   // cannot copy itself

	if (this != &src)
	{
		SetSize(src.m_nSize);
		memcpy((void*)m_pData,
			(void*)src.m_pData,
			(size_t)src.m_nSize * sizeof(TYPE));
	}
}

template<class TYPE, class ARG_TYPE>
INT_PTR CSYArray<TYPE, ARG_TYPE>::InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount /*=1*/)
{
	//assert(nIndex >= 0);    // will expand to meet need
	//assert(nCount > 0);     // zero or negative size not allowed

	if ((nIndex < 0) || (nCount <= 0))
		return SYAR_INVALID_PARAM;

	if (nIndex < m_nSize)		// inserting in the middle of the array
	{
		INT_PTR nOldSize = m_nSize;

		SetSize(m_nSize + nCount, -1);  // grow it to new size
										// shift old data up to fill gap
		memmove((void*)(m_pData + nIndex + nCount),
			(void*)(m_pData + nIndex),
			(size_t)(nOldSize - nIndex) * sizeof(TYPE));
		// re-init slots we copied from
		memset((void*)(m_pData + nIndex), 0, (size_t)nCount * sizeof(TYPE));
		//assert((nIndex + nCount) <= m_nSize);
		while (nCount--)
		{
			m_pData[nIndex++] = newElement;
		}
		return (nIndex - 1);
	}
	// otherwise add to the last of the array
	nIndex = Add(newElement);
	if (nIndex < 0)
		return nIndex;

	nCount--;
	if (nCount > 0)		// if greater than 1, insert again to deduce the times of calling setsize
		return InsertAt(nIndex, newElement, nCount);

	return nIndex;
}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));
	//assert((nCount > 0) && ((nIndex + nCount) <= m_nSize));

	if ((nIndex < 0) || (nIndex >= m_nSize) || (nCount <= 0) || ((nIndex + nCount > m_nSize)))
		return;

	// just remove a range
	INT_PTR nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)		// 如果需要移动的数量为 0 说明需要删除最后几个元素
	{
		memmove((void*)(m_pData + nIndex),
			(void*)(m_pData + nIndex + nCount),
			(size_t)nMoveCount * sizeof(TYPE));
	}
	m_nSize -= nCount;
}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::InsertAt(INT_PTR nStartIndex, CSYArray* pNewArray)
{
	//assert(pNewArray != NULL);
	//assert((nStartIndex >= 0) && (nStartIndex <= m_nSize));

	if ((pNewArray == NULL) || (nStartIndex < 0) || (nStartIndex > m_nSize))
		return;

	INT_PTR nNewSize = pNewArray->GetSize();

	if (nNewSize == 0)
		return;

	if (nStartIndex < m_nSize)		// 在之前添加
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), nNewSize);
		for (INT_PTR i = 0; i<nNewSize; i++)
		{
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
	else		// 在数组最后添加
	{
		Append(*pNewArray);
	}

}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::Swap(INT_PTR i, INT_PTR j)
{
	//assert((i >= 0) && (j >= 0) && (i < m_nSize) && (j < m_nSize));

	if ((i < 0) || (j < 0) || (i >= m_nSize) || (j >= m_nSize))
		return;

	TYPE tmpData = m_pData[j];

	m_pData[j] = m_pData[i];
	m_pData[i] = tmpData;
}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::Reverse()
{
	if (m_nSize <= 1)
		return;

	TYPE tmpData;
	INT_PTR nHalfSize = (m_nSize >> 1);
	INT_PTR i(0), j = (m_nSize - i - 1);

	for (; i<nHalfSize; i++, j--)
	{
		tmpData = m_pData[j];
		m_pData[j] = m_pData[i];
		m_pData[i] = tmpData;
	}
}

template<class TYPE, class ARG_TYPE>
void CSYArray<TYPE, ARG_TYPE>::MoveElement(INT_PTR i, INT_PTR j)
{
	//如果索引不合法，则不需要处理
	if (i < 0 || i >= m_nSize || j < 0 || j > m_nSize) //j允许定于m_nSize，此时表示移动到末尾
		return;

	// i < j 时暂存a[i],把i+1到就j-1的所有元素向前移，把j-1处元素设定为a[i]
	if (i < j - 1)
	{
		TYPE element_i = m_pData[i];
		memmove(&(m_pData[i]), &(m_pData[i + 1]), sizeof(TYPE*)*(j - i - 1));
		m_pData[j - 1] = element_i;
	}
	else if (i > j) // i > j 时暂存a[i],把j到就i-1的所有元素向后移，把j处元素设定为a[i]
	{
		TYPE element_i = m_pData[i];
		memmove(&(m_pData[j + 1]), &(m_pData[j]), sizeof(TYPE*)*(i - j));
		m_pData[j] = element_i;
	}
	else // i == j || i == j - 1 时不需要处理
	{
		;
	}
}

typedef CSYArray<int, int>				CSYIntArray;
typedef CSYArray<float, float>			CSYFloatArray;
typedef CSYArray<double, double>		CSYDoubleArray;
typedef CSYArray<BYTE, BYTE>			CSYBYTEArray;
typedef CSYArray<DWORD, DWORD>			CSYDWORDArray;
typedef CSYArray<void*, void*>			CSYPtrArray;
typedef CSYArray<int*, int*>			CSYIntPtrArray;
typedef CSYArray<float*, float*>		CSYFloatPtrArray;
typedef CSYArray<double*, double*>		CSYDoublePtrArray;
typedef CSYArray<BYTE*, BYTE*>			CSYBYTEPtrArray;
typedef CSYArray<DWORD*, DWORD*>		CSYDWORDPtrArray;
typedef CSYArray<LPTSTR, LPTSTR>		CSYStrArray;
typedef CSYArray<LPCTSTR, LPCTSTR>		CSYConstStrArray;

BASEGRAPH_COMMON_API void __cdecl SYStrAR_Release(CSYStrArray& ary);
#endif