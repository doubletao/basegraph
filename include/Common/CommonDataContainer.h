#pragma once
/////////////////////////////hash////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void*		BaseHash_key;			// Hash 表入口点 key 的类型
typedef void*		BaseHash_value;		// Hash 表绑定 key 的值的类型
typedef void*		BaseHash_pos;			// Hash 表遍历时的位置（LPDSENTRY）

typedef struct tag_entry				// 存储数据的 Hash 表入口点
{
	BaseHash_key			pKey;			// key
	BaseHash_value		pValue;			// 绑定 key 的值
	UINT64				unHashValue;	// Hash 值
	int					needrelease;
	struct tag_entry*	pNextEntry;		// 下一个入口点
}entry, *pentry;

typedef pentry*	pentrytable;		// 定义桶表的类型

typedef struct tag_hash				// 拉链法 Hash 表结构
{
	unsigned int	tablelen;								// 表长
	pentrytable		table;									// 表头
	unsigned int	count;									// 入口点总数
	unsigned int	limit;									// 负载上限
	unsigned int	lower;									// 负载过低下限
	unsigned int	primeindex;							// 素数序号
	unsigned char*  pool;
	unsigned char*  poolend;
	int				dupkey;
	UINT64(*hashfn)(BaseHash_key k);				// Hash 函数指针
	BOOL(*eqfn)(BaseHash_key k1, BaseHash_key k2);	// key 比较函数指针
	BaseHash_key(*dupfn)(BaseHash_key k);					// 拷贝一份 key
	void(*freekeyfn)(BaseHash_key k);				// 释放 key
	void(*freevaluefn)(BaseHash_value v);			// 释放 value
}BaseHash, *PBaseHash, *LPBaseHash;
BASEGRAPH_COMMON_API BOOL __cdecl BaseHash_Init_Int(BaseHash* ph, unsigned int minsize);
///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化 Hash 表（函数指针为 NULL 时表示使用系统缺省函数）
//
// 返回值:	成功为TRUE，失败为FALSE
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseHash_Init(BaseHash* ph,
	unsigned int minsize,					// 表初始大小
	UINT64(*BaseHash_make)(BaseHash_key),		// Hash值计算函函数
	BOOL(*key_compare)(BaseHash_key, BaseHash_key),	// key 比较函数
	BaseHash_key(*key_dup)(BaseHash_key),		// key 拷贝函数
	void(*key_free)(BaseHash_key),			// key 释放函数
	void(*value_free)(BaseHash_value));		// value 释放函数


BASEGRAPH_COMMON_API BOOL __cdecl BaseHash_SetPool(BaseHash* ph, char*  pool, char* poolend, BOOL dupKey);

///////////////////////////////////////////////////////////////////////////////
// 描述：	创建新 Hash 表（函数指针为 NULL 时表示使用系统缺省函数）
//
// 返回值:	PBaseHash			新表指针
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API PBaseHash __cdecl BaseHash_Create(unsigned int unMinSize,					// 表初始大小
	UINT64(*hashF)(BaseHash_key),		// Hash 值计算函函数
	BOOL(*keyEqF)(BaseHash_key, BaseHash_key),	// key 比较函数
	BaseHash_key(*keyDupF)(BaseHash_key),		// key 拷贝函数
	void(*keyFreeF)(BaseHash_key),			// key 释放函数
	void(*valueFreeF)(BaseHash_value));		// value 释放函数


BASEGRAPH_COMMON_API BOOL __cdecl BaseHash_SetPool(BaseHash* ph, unsigned char*  pool, unsigned char* poolend, BOOL bdupKey);
///////////////////////////////////////////////////////////////////////////////
// 描述：	Hash 值求解
//
// 返回值:	unsigned int		key 对应的 Hash 值
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API UINT64 __cdecl BaseHash_Hash(const BaseHash* pH,			// Hash 表指针
	BaseHash_key pK);		// key

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	缺省的 Hash 值求解函数（字符串）
						//
						// 返回值:	unsigned int		key 对应的 Hash 值
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API UINT64 __cdecl BaseHash_HashStr(BaseHash_key pK);			// key

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	缺省的 key 比较函数（字符串）
																		//
																		// 返回值:	TRUE		相等
																		//			FALSE		不等
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseHash_EqualStr(BaseHash_key pK1,				// key
	BaseHash_key pK2);				// key

									///////////////////////////////////////////////////////////////////////////////
									// 描述：	缺省的 key 拷贝函数（字符串）
									//
									// 返回值:	BaseHash_key
									///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_key __cdecl BaseHash_DuplicateStr(BaseHash_key pK);		// key

																			///////////////////////////////////////////////////////////////////////////////
																			// 描述：	缺省的 key 释放函数（字符串）
																			//
																			// 返回值:	void
																			///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseHash_FreeKeyStr(BaseHash_key pK);			// key

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	缺省的 value 释放函数（只能释放由 malloc 申请的内存空间）
																		//
																		// 返回值:	void
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseHash_FreeValueStr(BaseHash_value pV);		// value

UINT64 __cdecl BaseHash_HashInt(BaseHash_key pk);
BOOL __cdecl BaseHash_EqualInt(BaseHash_key pK1, BaseHash_key pK2);
BaseHash_key __cdecl BaseHash_DuplicateInt(BaseHash_key pk);
void __cdecl BaseHash_FreeKeyInt(BaseHash_key pk);
void __cdecl BaseHash_FreeValueInt(BaseHash_value pV);
///////////////////////////////////////////////////////////////////////////////
// 描述：	获取入口点总数
//
// 返回值:	unsigned int
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API unsigned int __cdecl BaseHash_GetCount(PBaseHash pH);		// Hash 表指针

																		//////////////////////////////////////////////////////////////////////////
																		// 描述：	获取第一个入口点的位置
																		//
																		// 返回值：	BaseHash_pos
																		//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_pos __cdecl BaseHash_GetHeadPosition(PBaseHash ph);		// Hash 表指针

																			//////////////////////////////////////////////////////////////////////////
																			// 描述：	获取第一个入口点的数据
																			//
																			// 返回值：	BaseHash_key / BaseHash_value
																			//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_key __cdecl BaseHash_GetHeadKey(PBaseHash ph);		// Hash 表指针
BASEGRAPH_COMMON_API BaseHash_value __cdecl BaseHash_GetHeadValue(PBaseHash ph);	// Hash 表指针

																		//////////////////////////////////////////////////////////////////////////
																		// 描述：	获取下个位置或当前位置数据，获取数据时不会自动更新位置。
																		//
																		// 返回值：	BaseHash_pos / BaseHash_key / BaseHash_value
																		//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_value __cdecl BaseHash_GetNextPosition(PBaseHash ph,		// Hash 表指针
	BaseHash_pos* ppos);	// 当前位置
BASEGRAPH_COMMON_API BaseHash_key __cdecl BaseHash_GetPosKey(PBaseHash ph,			// Hash 表指针
	BaseHash_pos pos);		// 当前位置
BASEGRAPH_COMMON_API BaseHash_value __cdecl BaseHash_GetPosValue(PBaseHash ph,		// Hash 表指针
	BaseHash_pos pos);	// 当前位置

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	插入一个数据，允许重复插入同一个 key
						//
						// 返回值:	TRUE			插入成功
						//			FALSE			插入失败
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseHash_Insert(PBaseHash ph,			// Hash 表指针
	BaseHash_key pk,		// key
	BaseHash_value pv);	// 绑定 key 的值

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	通过 key 查找数据
						//
						// 返回值:	BaseHash_value
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_value __cdecl BaseHash_Search(const BaseHash* ph,			// Hash 表指针
	BaseHash_key pk);		// key

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	删除为 key 的入口点并返回相应的数据，如有必要则压缩 Hash 表的容量
						//			允许释放数据，但使用缺省函数时要小心，释放后返回 NULL
						//
						// 返回值:	BaseHash_value
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_value __cdecl BaseHash_RemoveAt(PBaseHash ph,			// Hash 表指针
	BaseHash_key pk,			// key
	BOOL bFreeValue);		// 释放元素值内存的标志

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	删除所有的入口点，允许释放数据，但使用缺省函数时要小心
							//
							// 返回值:	无
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseHash_RemoveAll(PBaseHash ph,			// Hash 表指针
	BOOL bFreeValue);		// 释放元素值内存的标志

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	销除 Hash 表，允许释放数据，但使用缺省函数时要小心
							//
							// 返回值:	无
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseHash_Release(PBaseHash ph,				// Hash 表指针
	BOOL bFreeValue);			// 释放元素值内存的标志

BASEGRAPH_COMMON_API void __cdecl BaseHash_Delete(PBaseHash ph,				// Hash 表指针
	BOOL bFreeValue);			// 释放元素值内存的标志
								///////////////////////////////////////////////////////////////////////////////
								// 描述：	更改绑定 key 的值并返回原值
								//			允许释放数据，但使用缺省函数时要小心，释放后返回 NULL
								//
								// 返回值:	BaseHash_value
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseHash_value __cdecl BaseHash_ChangeValue(PBaseHash ph,		// Hash 表指针
	BaseHash_key pk,		// key
	BaseHash_value pnewv,	// 欲更改的值
	BOOL bFreeValue);	// 释放元素值内存的标志


BASEGRAPH_COMMON_API void* __cdecl pool_malloc(unsigned char** pool, int size, BOOL bfromsystem);

/////////////////////////////htable////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BseeHTable_null		0						// 空句柄
typedef void*				BseeHTable_element;		// 元素类型
typedef BseeHTable_element*		BseeHTable_ele_ptr;		// 元素指针


typedef struct tag_htable {
	UINT				maxsize;			// 数组空间大小
	UINT				count;			// 元素个数
	BseeHTable_ele_ptr		eles;		// 元素数组指针
}htable, *phtable, *LPBseeHTable;

///////////////////////////////////////////////////////////////////////////////
// 描述：	创建新句柄管理表
//
// 返回值:	phtable			新表指针
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API phtable __cdecl BseeHTable_Create(UINT nMaxSize);			// 数组空间大小

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	释放句柄管理表
																		//
																		// 返回值:	无
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BseeHTable_Delete(phtable pHT);			// 表指针
BASEGRAPH_COMMON_API void __cdecl BseeHTable_Release(phtable pHT);			// 表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	初始化句柄管理表
																	//
																	// 返回值:	无
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BseeHTable_Init(phtable pHT);			// 表指针

BASEGRAPH_COMMON_API void __cdecl BseeHTable_SetSize(phtable pHT,		// 表指针
	UINT nMaxSize);	// 数组大小

					///////////////////////////////////////////////////////////////////////////////
					// 描述：	表是否已满
					//
					// 返回值:	TRUE			是
					//			FALSE			否
					///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BseeHTable_IsFull(phtable pHT);			// 表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	是否空表
																	//
																	// 返回值:	TRUE			是
																	//			FALSE			否
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BseeHTable_IsEmpty(phtable pHT);			// 表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	获取句柄总数
																	//
																	// 返回值:	UINT
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API UINT __cdecl BseeHTable_GetCount(phtable pHT);			// 表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	就近插入数据并返回句柄
																	//
																	// 返回值:	UINT
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API UINT __cdecl BseeHTable_Insert(phtable pHT,				// 表指针
	BseeHTable_element e);		// 待插入的数据

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	获取某个句柄的数据
								//
								// 返回值:	BseeHTable_element
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BseeHTable_element __cdecl BseeHTable_GetAt(phtable pHT,			// 表指针
	UINT nIndex);		// 句柄

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	删除某个句柄并返回数据
						//
						// 返回值:	BseeHTable_element
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BseeHTable_element __cdecl BseeHTable_RemoveAt(phtable pHT,		// 表指针
	UINT nIndex);		// 句柄

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	删除所有的句柄
						//
						// 返回值:	无
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BseeHTable_RemoveAll(phtable pHT);			// 表指针

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	设置某个句柄的数据值并返回原值
																		//
																		// 返回值:	BseeHTable_element
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BseeHTable_element __cdecl BseeHTable_SetAt(phtable pHT,			// 表指针
	UINT nIndex,			// 句柄
	BseeHTable_element e);	// 数据

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	哈希表中的有效句柄遍历
							//
							// 返回值:	UINT / BseeHTable_element
							///////////////////////////////////////////////////////////////////////////////
							// 得到第一个有效的句柄
BASEGRAPH_COMMON_API UINT __cdecl BseeHTable_GetFirstValid(phtable pHT);		// 表指针
																		// 遍历下一个有效的句柄
BASEGRAPH_COMMON_API BseeHTable_element __cdecl BseeHTable_GetNextValid(phtable pHT,		// 表指针
	UINT *pnIndex);	// 句柄


/////////////////////////////array////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//array相关，随后要干掉
// 元素定义
#define BaseAR_ELE_LEN	sizeof(void*)		// 元素类型的长度
typedef void*			BaseAR_ELEMENT;		// 定义元素的类型
typedef BaseAR_ELEMENT*	BaseAR_ELE_PTR;

// 返回值定义
#define BaseAR_SUCCESS			0			// 成功（只会在不返回索引的接口中使用）
#define BaseAR_EMPTY				-1			// 空数组
#define BaseAR_FAILED				-2			// 失败
#define BaseAR_NULL				-3			// 不存在的数组
#define BaseAR_INVALID_PARAM		-4			// 无效参数
#define BaseAR_MEMORY_ERROR		-5			// 内存错误

// 数据结构定义
typedef struct tag_BaseARray {
	BaseAR_ELE_PTR	m_pData;				// 元素组指针
	INT_PTR			m_nSize;				// 用户当前占用的数组大小
	INT_PTR			m_nMaxSize;				// 当前实际分配的数组大小
	INT_PTR			m_nGrowBy;				// 分配内存时增长的元素个数
}BaseARray, *LPBaseARRAY;

///////////////////////////////////////////////////////////////////////////////
// 描述：	创建一个空数组
//
// 返回值：	NULL		创建失败（内存不足）
//			pA			创建的空数组的指针
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LPBaseARRAY __cdecl BaseAR_Create();

///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化一个空数组
//
// 返回值：	BaseAR_SUCCESS		成功
//			BaseAR_NULL			空指针
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_Init(LPBaseARRAY pA);			// 数组结构指针

															///////////////////////////////////////////////////////////////////////////////
															// 描述：	删除一个数组
															//
															// 返回值：	BaseAR_SUCCESS		成功
															//			BaseAR_NULL			空指针
															///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_Release(LPBaseARRAY pA);			// 数组结构指针

																//////////////////////////////////////////////////////////////////////////
																//清除数组内容，保留内存大小
BASEGRAPH_COMMON_API int __cdecl BaseAR_Clear(LPBaseARRAY par);

///////////////////////////////////////////////////////////////////////////////
// 描述：	实现数组大小的设定
//
// 返回值：	BaseAR_SUCCESS		成功
//			BaseAR_NULL			空指针
//			BaseAR_INVALID_PARAM	无效参数
//			BaseAR_MEMORY_ERROR	内存错误
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_SetSize(LPBaseARRAY pA,			// 数组结构指针
	INT_PTR nNewSize,		// 新的数组大小
	INT_PTR nGrowBy);		// 需要增大数组时增加的元素个数

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	设置最大数组大小，不会添加数组元素
							//
							// 返回值：	BaseAR_SUCCESS		成功
							//			BaseAR_NULL			空指针
							//			BaseAR_INVALID_PARAM	无效参数
							//			BaseAR_MEMORY_ERROR	内存错误
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_SetMaxSize(LPBaseARRAY pA,			// 数组结构指针
	INT_PTR nNewSize		// 新的数组大小
);

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取数组大小
//
// 返回值：	m_nSize			数组大小
//			BaseAR_NULL		pA == NULL
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR	__cdecl BaseAR_GetSize(const BaseARray* pA);		// 数组结构指针
BASEGRAPH_COMMON_API int	__cdecl BaseAR_GetCount(const BaseARray* pA);	// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	获取数组的上界
																//
																// 返回值：	m_nSize-1		数组上界
																//			BaseAR_NULL		pA == NULL
																//			BaseAR_EMPTY		空数组（-1）
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR	__cdecl BaseAR_GetUpperBound(LPBaseARRAY pA);	// 数组结构指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	判断是否为空数组
																	//
																	// 返回值：	TRUE		为空
																	//			FALSE		不为空
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseAR_IsEmpty(LPBaseARRAY pA);			// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	释放超越上界的元素的内存
																//
																// 返回值：	BaseAR_SUCCESS		成功
																//			BaseAR_NULL			空指针
																//			BaseAR_MEMORY_ERROR	内存错误
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_FreeExtra(LPBaseARRAY pA);		// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	释放所有元素的内存
																//
																// 返回值：	BaseAR_SUCCESS		成功
																//			BaseAR_NULL			空指针
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_RemoveAll(LPBaseARRAY pA);		// 数组结构指针

																///////////////////////////////////////////////////////////////////////////////
																// 描述：	获取数据指针
																//
																// 返回值：	NULL			pA == NULL
																//			BaseAR_ELE_PTR	当前数据指针
																///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseAR_ELE_PTR __cdecl BaseAR_GetData(LPBaseARRAY pA);		// 数组结构指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	获取某一元素
																	//
																	// 返回值：	BaseAR_ELEMENT		索引元素数据
																	//			NULL				pA == NULL || nIndex 无效
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseAR_ELEMENT __cdecl BaseAR_GetAt(const BaseARray* pA,		// 数组结构指针
	INT_PTR nIndex);	// 元素索引号

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	设置某一非越界元素的值
						//
						// 返回值：	NULL				失败
						//			BaseAR_ELEMENT		旧元素数据
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseAR_ELEMENT __cdecl BaseAR_SetAt(LPBaseARRAY pA,				// 数组结构指针
	INT_PTR nIndex,				// 元素索引号
	BaseAR_ELEMENT newData);		// 预设元素值

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	在数组尾扩充添加一个元素
								//
								// 返回值：	索引				添加成功
								//			BaseAR_NULL			空指针
								//			BaseAR_FAILED			失败
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR __cdecl BaseAR_Add(LPBaseARRAY pA,				// 数组结构指针
	BaseAR_ELEMENT newData);		// 预设元素值

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	在数组尾扩充添加另一个数组
								//
								// 返回值：	BaseAR_SUCCESS		成功
								//			BaseAR_NULL			空指针
								//			BaseAR_FAILED			失败
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_Append(LPBaseARRAY pA,			// 数组结构指针
	LPBaseARRAY pSrc);		// 预添加的数组

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	复制另一个数组的内容
							//
							// 返回值：	BaseAR_SUCCESS		成功
							//			BaseAR_NULL			空指针
							//			BaseAR_FAILED			失败
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_Copy(LPBaseARRAY pA,			// 数组结构指针
	LPBaseARRAY pSrc);		// 预复制的数组

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	在某一非越界元素前或者最后一个元素后插入几个新元素
							//
							// 返回值：	索引				最后一个插入的元素
							//			BaseAR_NULL			空指针
							//			BaseAR_FAILED			失败
							//			BaseAR_INVALID_PARAM	无效参数
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR __cdecl BaseAR_InsertAt(LPBaseARRAY pA,				// 数组结构指针
	INT_PTR nIndex,			// 元素索引号
	BaseAR_ELEMENT newData,		// 预插入元素值
	INT_PTR nCount);			// 预插入的元素个数

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	在某一非越界元素前或者最后一个元素后插入一个数组
								//
								// 返回值：	BaseAR_SUCCESS		成功
								//			BaseAR_NULL			空指针
								//			BaseAR_FAILED			失败
								//			BaseAR_INVALID_PARAM	无效参数
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_InsertArrayAt(LPBaseARRAY pA,			// 数组结构指针
	INT_PTR nIndex,			// 元素索引号
	LPBaseARRAY pSrc);			// 预插入数组

								///////////////////////////////////////////////////////////////////////////////
								// 描述：	从某一非越界元素开始删除几个元素
								//
								// 返回值：	BaseAR_SUCCESS		成功
								//			BaseAR_NULL			空指针
								//			BaseAR_FAILED			失败
								//			BaseAR_INVALID_PARAM	无效参数
								///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BaseAR_ELEMENT __cdecl BaseAR_RemoveAt(LPBaseARRAY pA,			// 数组结构指针
	INT_PTR nIndex,		// 元素索引号
	INT_PTR nCount);		// 预删除的元素个数

							//////////////////////////////////////////////////////////////////////////
							// 描述：	交换两个非越界元素的值
							//
							// 返回值：	BaseAR_SUCCESS		成功
							//			BaseAR_NULL			空指针
							//			BaseAR_INVALID_PARAM	无效参数
							//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_Swap(LPBaseARRAY pA,			// 数组结构指针
	INT_PTR i,			// 元素索引号
	INT_PTR j);

//////////////////////////////////////////////////////////////////////////
// 描述：	翻转数组
//
// 返回值：	BaseAR_SUCCESS		成功
//			BaseAR_NULL			空指针
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API int __cdecl BaseAR_Reverse(LPBaseARRAY pA);		// 数组结构指针

															//////////////////////////////////////////////////////////////////////////
															// 描述：	移动数组内元素的位置：把a[i]移动到a[j]之前紧靠a[j]的位置 
															// 注意：	i和j都是在移动动作发生之前的数组中的位置，移动完毕后保证除了a[i]
															//			之外的元素的相对顺序不变，并且原来<j的元素都在移动后的a[i]之前，
															//			原来>=j的元素都在移动后的a[i]之后 (靠前表示位置索引较小)
															// 示例：	对于数组[1,2,3,4,5]，如果i=3,j=1则得到：[1,4,2,3,5]
															//			如果i=1,j=3,则得到：[1,3,2,4,5]，注意结果中2的位置并不是(j==3)
															//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseAR_MoveElement(LPBaseARRAY pA,		// 数组结构指针
	size_t i,			// 要移动的元素的索引，应该属于[0,size)
	size_t j);		// 要移动到的位置，应该属于[0,size]

typedef int(*SYCompareFun)(const void*, const void*);
BASEGRAPH_COMMON_API void __cdecl BaseAR_Sort(LPBaseARRAY pA, SYCompareFun fun);
///////////////////////////////////////////////////////////////////////////////
// 类封装
#ifdef __cplusplus

template<class TYPE, class ARG_TYPE = const TYPE&>
class CBaseARray
{
public:
	CBaseARray();
	~CBaseARray();

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
	INT_PTR Append(const CBaseARray& src);
	INT_PTR Append(const BaseARray& src);
	void	Copy(const CBaseARray& src);

	const	TYPE& operator[](INT_PTR nIndex) const;
	TYPE&	operator[](INT_PTR nIndex);

	INT_PTR	InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount = 1);
	void	RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);
	void	InsertAt(INT_PTR nStartIndex, CBaseARray* pNewArray);

	void	Swap(INT_PTR i, INT_PTR j);
	void	Reverse();
	void	MoveElement(INT_PTR i, INT_PTR j); //说明参见BaseAR_MoveElement

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
inline INT_PTR CBaseARray<TYPE, ARG_TYPE>::GetSize() const
{
	return m_nSize;
}

template<class TYPE, class ARG_TYPE>
inline int CBaseARray<TYPE, ARG_TYPE>::GetCount() const
{
	return (int)m_nSize;
}

template<class TYPE, class ARG_TYPE>
inline BOOL CBaseARray<TYPE, ARG_TYPE>::IsEmpty()
{
	return (m_nSize == 0);
}

template<class TYPE, class ARG_TYPE>
inline INT_PTR CBaseARray<TYPE, ARG_TYPE>::GetUpperBound() const
{
	return (m_nSize - 1);
}

template<class TYPE, class ARG_TYPE>
inline void CBaseARray<TYPE, ARG_TYPE>::RemoveAll()
{
	SetSize(0, -1);
}

template<class TYPE, class ARG_TYPE>
inline const TYPE& CBaseARray<TYPE, ARG_TYPE>::ElementAt(INT_PTR nIndex) const
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
inline TYPE& CBaseARray<TYPE, ARG_TYPE>::ElementAt(INT_PTR nIndex)
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
inline const TYPE& CBaseARray<TYPE, ARG_TYPE>::GetAt(INT_PTR nIndex) const
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
inline TYPE& CBaseARray<TYPE, ARG_TYPE>::GetAt(INT_PTR nIndex)
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
inline TYPE CBaseARray<TYPE, ARG_TYPE>::SetAt(INT_PTR nIndex, ARG_TYPE newElement)
{
	//assert((nIndex >= 0) && (nIndex < m_nSize));

	TYPE tmpData;
	memset(&tmpData, 0, sizeof(TYPE));

	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		tmpData = m_pData[nIndex];
		m_pData[nIndex] = newElement;
	}
	return tmpData;
}

template<class TYPE, class ARG_TYPE>
inline const TYPE* CBaseARray<TYPE, ARG_TYPE>::GetData() const
{
	return (const TYPE*)m_pData;
}

template<class TYPE, class ARG_TYPE>
inline TYPE* CBaseARray<TYPE, ARG_TYPE>::GetData()
{
	return (TYPE*)m_pData;
}

template<class TYPE, class ARG_TYPE>
inline INT_PTR CBaseARray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
{
	INT_PTR nIndex = m_nSize;

	SetSize(nIndex + 1, -1);
	SetAt(nIndex, newElement);

	return nIndex;
}

template<class TYPE, class ARG_TYPE>
inline const TYPE& CBaseARray<TYPE, ARG_TYPE>::operator[](INT_PTR nIndex) const
{
	return GetAt(nIndex);
}

template<class TYPE, class ARG_TYPE>
inline TYPE& CBaseARray<TYPE, ARG_TYPE>::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

// out-of-inline implementions.

template<class TYPE, class ARG_TYPE>
CBaseARray<TYPE, ARG_TYPE>::CBaseARray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
CBaseARray<TYPE, ARG_TYPE>::~CBaseARray()
{
	if (m_pData != NULL)
	{
		delete[](BYTE*)m_pData;
	}
}

template<class TYPE, class ARG_TYPE>
void CBaseARray<TYPE, ARG_TYPE>::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
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
void CBaseARray<TYPE, ARG_TYPE>::FreeExtra()
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
INT_PTR CBaseARray<TYPE, ARG_TYPE>::Append(const CBaseARray& src)
{
	//assert(this != &src);   // cannot append to itself

	if (this == &src)
		return BaseAR_INVALID_PARAM;

	INT_PTR nOldSize = m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy((void*)(m_pData + nOldSize),
		(void*)src.m_pData,
		(size_t)src.m_nSize * sizeof(TYPE));

	return nOldSize;
}


template<class TYPE, class ARG_TYPE>
INT_PTR CBaseARray<TYPE, ARG_TYPE>::Append(const BaseARray& src)
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
void CBaseARray<TYPE, ARG_TYPE>::Copy(const CBaseARray& src)
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
INT_PTR CBaseARray<TYPE, ARG_TYPE>::InsertAt(INT_PTR nIndex, ARG_TYPE newElement, INT_PTR nCount /*=1*/)
{
	//assert(nIndex >= 0);    // will expand to meet need
	//assert(nCount > 0);     // zero or negative size not allowed

	if ((nIndex < 0) || (nCount <= 0))
		return BaseAR_INVALID_PARAM;

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
void CBaseARray<TYPE, ARG_TYPE>::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
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
void CBaseARray<TYPE, ARG_TYPE>::InsertAt(INT_PTR nStartIndex, CBaseARray* pNewArray)
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
void CBaseARray<TYPE, ARG_TYPE>::Swap(INT_PTR i, INT_PTR j)
{
	//assert((i >= 0) && (j >= 0) && (i < m_nSize) && (j < m_nSize));

	if ((i < 0) || (j < 0) || (i >= m_nSize) || (j >= m_nSize))
		return;

	TYPE tmpData = m_pData[j];

	m_pData[j] = m_pData[i];
	m_pData[i] = tmpData;
}

template<class TYPE, class ARG_TYPE>
void CBaseARray<TYPE, ARG_TYPE>::Reverse()
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
void CBaseARray<TYPE, ARG_TYPE>::MoveElement(INT_PTR i, INT_PTR j)
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

typedef CBaseARray<int, int>				CSYIntArray;
typedef CBaseARray<float, float>			CSYFloatArray;
typedef CBaseARray<double, double>		CSYDoubleArray;
typedef CBaseARray<BYTE, BYTE>			CSYBYTEArray;
typedef CBaseARray<DWORD, DWORD>			CSYDWORDArray;
typedef CBaseARray<void*, void*>			CSYPtrArray;
typedef CBaseARray<int*, int*>			CSYIntPtrArray;
typedef CBaseARray<float*, float*>		CSYFloatPtrArray;
typedef CBaseARray<double*, double*>		CSYDoublePtrArray;
typedef CBaseARray<BYTE*, BYTE*>			CSYBYTEPtrArray;
typedef CBaseARray<DWORD*, DWORD*>		CSYDWORDPtrArray;
typedef CBaseARray<LPTSTR, LPTSTR>		CSYStrArray;
typedef CBaseARray<LPCTSTR, LPCTSTR>		CSYConstStrArray;

BASEGRAPH_COMMON_API void __cdecl SYStrAR_Release(CSYStrArray& ary);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DSLT_NULL		-2						// 不存在的链表
typedef void*			LT_DATA_PTR;			// 定义节点数据的类型
typedef void*			LISTPOSITION;			// 链表遍历时节点的位置（LPDSLISTNODE）

typedef struct tag_listnode
{
	LT_DATA_PTR				data;			// 数据指针
	struct tag_listnode*	prev;			// 前一节点
	struct tag_listnode*	next;			// 后一节点
	int						needrelease;	//需要删除
	BOOL					idelpool;
#ifdef _DEBUG
	void*					parent;		// 节点所在链表指针（调试用）
#endif
}listnode, *plistnode;

typedef struct tag_list
{
	INT_PTR			count;				// 节点个数
	plistnode		head;				// 头节点
	plistnode		tail;				// 尾节点

	plistnode		idelfirst;
	plistnode		ideltail;
	plistnode		idelpool;
}SYList, *pSYList;

///////////////////////////////////////////////////////////////////////////////
// 描述：	初始化链表
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYList_Init(pSYList pList);			// 链表指针
BASEGRAPH_COMMON_API void __cdecl SYList_Release(pSYList pList);			// 链表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	设置空闲节点池
																	//
																	// 返回值:	无
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYList_SetIdelSize(pSYList pList, int nSize);			// 链表指针


																					///////////////////////////////////////////////////////////////////////////////
																					// 描述：	获取头节点位置
																					//
																					// 返回值:	LISTPOSITION
																					///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_GetHeadPosition(SYList* pList);	// 链表指针

																			///////////////////////////////////////////////////////////////////////////////
																			// 描述：	获取尾节点位置
																			//
																			// 返回值:	LISTPOSITION
																			///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_GetTailPosition(pSYList pList);	// 链表指针

																			///////////////////////////////////////////////////////////////////////////////
																			// 描述：	获取链表的节点个数
																			//
																			// 返回值:	INT_PTR			当前链表结点个数
																			///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API INT_PTR	__cdecl SYList_GetCount(pSYList pList);		// 链表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	判断是否为空链表
																	//
																	// 返回值:	TRUE		为空
																	//			FALSE		不为空
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYList_IsEmpty(pSYList pList);			// 链表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	获取后一节点位置并返回原节点的数据
																	//
																	// 返回值:	LT_DATA_PTR
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_GetNext(const SYList* pList,	// 链表指针
	LISTPOSITION* pPos);	// 当前节点的位置的指针

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	获取前一节点位置并返回原节点的数据
							//
							// 返回值:	LT_DATA_PTR
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_GetPrev(pSYList pList,		// 链表指针
	LISTPOSITION* pPos);	// 当前节点的位置的指针

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	获取当前节点的数据
							//
							// 返回值:	LT_DATA_PTR
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_GetAt(pSYList pList,		// 链表指针
	LISTPOSITION pos);		// 当前节点的位置

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	获取头节点的数据
							//
							// 返回值:	LT_DATA_PTR
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_GetHead(pSYList pList);		// 链表指针

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	获取尾节点的数据
																		//
																		// 返回值:	LT_DATA_PTR
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_GetTail(pSYList pList);		// 链表指针

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	设置某一节点的数据
																		//
																		// 返回值:	LT_DATA_PTR	原数据指针
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_SetAt(pSYList pList,			// 链表指针
	LISTPOSITION pos,			// 设置数据的节点位置
	LT_DATA_PTR pData);	// 数据指针

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	在头节点前增加一个新的节点
						//
						// 返回值:	LISTPOSITION		新节点的位置
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_AddHead(pSYList pList,			// 链表指针
	LT_DATA_PTR pData, unsigned char** pool, unsigned char* poolend);		// 新节点的数据

																			///////////////////////////////////////////////////////////////////////////////
																			// 描述：	在尾节点后增加一个新的节点
																			//
																			// 返回值:	LISTPOSITION		新节点的位置
																			///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_AddTail(pSYList pList,			// 链表指针
	LT_DATA_PTR pData, unsigned char** pool, unsigned char* poolend);		// 新节点的数据

																			///////////////////////////////////////////////////////////////////////////////
																			// 描述：	在头节点前增加一个新的链表
																			//
																			// 返回值:	无
																			///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYList_AddListHead(pSYList pList,			// 链表指针
	pSYList pNewList);		// 新链表指针

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	在尾节点后增加一个新的链表
							//
							// 返回值:	无
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYList_AddListTail(pSYList pList,			// 链表指针
	pSYList pNewList);		// 新链表指针

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	在某一节点前插一个新的节点
							//
							// 返回值:	LISTPOSITION		新节点的位置
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_InsertBefore(pSYList pList,		// 链表指针
	LISTPOSITION pos,		// 被插节点的位置
	LT_DATA_PTR pData);// 新节点数据

					   ///////////////////////////////////////////////////////////////////////////////
					   // 描述：	SYList_InsertAfter() 在某一节点后插一个新的节点
					   //
					   // 返回值:	LISTPOSITION		新节点的位置
					   ///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_InsertAfter(pSYList pList,		// 链表指针
	LISTPOSITION pos,		// 被插节点的位置
	LT_DATA_PTR pData);	// 新节点数据

						///////////////////////////////////////////////////////////////////////////////
						// 描述：	删除某一节点并返回该节点的数据
						//
						// 返回值:	LT_DATA_PTR
						///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_RemoveAt(pSYList pList,		// 链表指针
	LISTPOSITION pos);		// 预删除节点的位置

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	删除头节点
							//
							// 返回值:	LT_DATA_PTR	原尾节点数据
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_RemoveHead(pSYList pList);		// 链表指针

																			///////////////////////////////////////////////////////////////////////////////
																			// 描述：	删除尾节点
																			//
																			// 返回值:	LT_DATA_PTR	原尾节点数据
																			///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LT_DATA_PTR __cdecl SYList_RemoveTail(pSYList pList);	// 链表指针

																		///////////////////////////////////////////////////////////////////////////////
																		// 描述：	删除所有节点
																		//
																		// 返回值:	无
																		///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYList_RemoveAll(pSYList pList);		// 链表指针

																	///////////////////////////////////////////////////////////////////////////////
																	// 描述：	从某一节点位置开始根据数据查找节点的位置
																	//
																	// 返回值:	LISTPOSITION
																	///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_FindNode(pSYList pList,			// 链表指针
	LT_DATA_PTR pData,		// 查找的数据
	LISTPOSITION posStart);	// 查找起始位置

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	根据序号查找节点的位置
							//
							// 返回值:	LISTPOSITION
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LISTPOSITION __cdecl SYList_FindByIndex(pSYList pList,		// 链表指针
	INT_PTR nIndex);		// 查找序号

							///////////////////////////////////////////////////////////////////////////////
							// 描述：	翻转链表
							//
							// 返回值:	无
							///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYList_Reverse(pSYList pList);			// 链表指针
#endif