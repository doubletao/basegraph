#include "stdafx.h"
#include "CalculatorTitleWord.h"
#include "CalculatorManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CalculatorTitleword::CalculatorTitleword(CString strObjID)
	: ObjectCalculator(strObjID)
	, m_nFontWidth(0)
	, m_nFontHeight(0)
{
	memset(&m_2DTextRenderUnitArray, 0, sizeof(BaseARray));
	memset(&m_arWholeRenderTextGraph, 0, sizeof(BaseARray));

	//基本属性
	m_strText = _T("");
	m_strFontName = _T("");
	m_bUnderline = FALSE;
	m_nFontDistance = 0;
	m_bHorizontal = FALSE;
	m_bLineWrap = FALSE;
	m_TextAlignH = TEXT_ALIGN_H_LEFT;
	m_TextAlignV = TEXT_ALIGN_V_TOP;
	m_bLeftInput = TRUE;
	m_bVLeftTypeset = FALSE;
	m_bFixedFrame = FALSE;
}

CalculatorTitleword::~CalculatorTitleword()
{
	SYText_Release(&m_text);
}

bool CalculatorTitleword::PrepareObjBuffer(int nWidth, int nHeight)
{
	if (nWidth > 0
		&& nHeight > 0)
	{
		DataBufferManager::Instance().UnQuery2DBuffer(m_nObjBufferID);
		m_nObjBufferID = DataBufferManager::Instance().Query2DBuffer(nWidth, nHeight);
		if (m_nObjBufferID > 0)
		{
			return true;
		}
	}
	return false;
}

bool CalculatorTitleword::NeedUpdateBuffer()
{
	if (!IsItemNeedRender())
	{
		return false;
	}
	if (GetChgLevel() > ChgLevel_Matrix)
	{
		return true;
	}
	if (m_2DTextRenderUnitArray.m_nSize < 1)
	{
		SetChgLevel(ChgLevel_Create);
		return true;
	}
	return false;
}

bool CalculatorTitleword::UpdateRenderBuffer()
{
	if (NeedUpdateBuffer())
	{
		//取得基本属性
		bool bContinue = GetCurrentParam();
		if (bContinue)
		{
			//更新2D Uint
			Update2DUnit();
			//更新画面到显存
			UpdateToHalBuf();
		}
	}
	return ObjectCalculator::UpdateRenderBuffer();
}

bool CalculatorTitleword::GetCurrentParam()
{
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_Text_Str, stuValue))
	{
		m_strText = stuValue.ToCString();
		if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_FontName_Str, stuValue))
		{
			m_strFontName = stuValue.ToCString();
			if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_UnderLine_Bool, stuValue))
			{
				m_bUnderline = stuValue.ToBOOL();
				if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_FontDistance_Int, stuValue))
				{
					m_nFontDistance = stuValue.ToInt();
					if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_Horizontal_Bool, stuValue))
					{
						m_bHorizontal = stuValue.ToBOOL();
						if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_LineWrap_Bool, stuValue))
						{
							m_bLineWrap = stuValue.ToBOOL();
							if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_FontWidth_Int, stuValue))
							{
								m_nFontWidth = stuValue.ToInt();
								if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Titleword_FontHeight_Int, stuValue))
								{
									m_nFontHeight = stuValue.ToInt();
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool CalculatorTitleword::Update2DUnit()
{
	//造一个Text并由此对象生成一个排版器并排版
	SYText_Release(&m_text);
	m_text.Reset();
	SYText_Init(&m_text);

	//字体
	SYFont newFont;
	newFont.lfWidth = m_nFontWidth;
	newFont.lfHeight = m_nFontHeight;
	newFont.lfDistance = m_nFontDistance;
	_tcscpy_s(newFont.lfFaceName, LF_FACESIZE, m_strFontName.GetBuffer());
	m_strFontName.ReleaseBuffer();

	//颜色赋值
	if (GetChgLevel() >= ChgLevel_Buffer)
	{
		BaseGraph_Common::Value stuValue;
		if (DataRender_GetItemProperty(m_strObjID, BaseGraph_DataBase::ColorParam_Face_Stu, stuValue))
		{
			stuValue.ToStruct(m_TextStaitcEffect.face);
		}
	}

	SYText_Create(&m_text,
		m_strText,
		rectf(),
		&newFont,
		&m_TextStaitcEffect,
		nullptr,//3D颜色先不使用
		&m_TextUnderLine,
		m_TextAlignH,
		m_TextAlignV,
		m_bHorizontal,
		m_bLeftInput,
		m_bVLeftTypeset,
		m_bLineWrap,
		m_bFixedFrame
	);

	m_TypeSet.Attach(&m_text, this);
	m_TypeSet.ReplaceString(m_strText, FALSE);

	//重要函数
	RefreshRenderUnit(0, FALSE);

	return true;
}

bool CalculatorTitleword::UpdateToHalBuf()
{
	//计算出bound
	CalUnitBound();

	//拿到宽高
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	if (nWidth > 0 && nHeight > 0)
	{
		int nDevice = DX3DEngine_GetMainDevice();
		if (PrepareItemHalBufer(nDevice))
		{
			DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex, 0);
			RenderUnitBufferForGpu();
			UpdateItemGpuBuffer(TRUE);
			return true;
		}
	}
	return false;
}

rectf CalculatorTitleword::CalUnitBound()
{
	m_rtUnitBound.left = 65535.0f;
	m_rtUnitBound.top = 65535.0f;
	m_rtUnitBound.right = -65535.0f;
	m_rtUnitBound.bottom = -65535.0f;

	BaseARray & arRenderUnitArray = m_2DTextRenderUnitArray;
	int nRenderCount = (int)BaseAR_GetSize(&arRenderUnitArray);
	RECT rtTemp = { 0 };
	rectf fbound = { 0 };
	vector3f v3d[4] = { 0 };
	for (int i = 0; i<nRenderCount; i++)
	{
		SYItemRenderUnit * pRenderUnit = (SYItemRenderUnit *)BaseAR_GetAt(&arRenderUnitArray, i);
		
		//得到每一个Unit的边界(包括面边体影信息)在这个层面图形特技已经完成，RenderUnit单元中已经包含了图形特技的因素
		GetRenderUnitBound(i, fbound);
		rip_extend_fbound_long(&fbound, &rtTemp.left, &rtTemp.top, &rtTemp.right, &rtTemp.bottom);
#if 0
		//djt 特技size计算暂时不考虑
		//vector3f v = { 0,0,0 };
		//v *= pRenderUnit->MatVToItem;
		//POINT ptPos;
		//pEnv->ItemToScene(v);
		//ptPos.x = (LONG)v.x;
		//ptPos.y = (LONG)v.y;


		BOOL bRet = pEnv->CalcEffectSize(pEnv->m_dCurFrame, i, nRenderCount, 0.0, &rtTemp, EFFECT_CALC_CONDITIONS_PER, rtScene, ptPos);

		if (bRet)//如果有逐字特技需要计算
		{
			fbound.left = (float)rtTemp.left;
			fbound.top = (float)rtTemp.top;
			fbound.right = (float)rtTemp.right;
			fbound.bottom = (float)rtTemp.bottom;
		}
#endif

		//计算每一个Unit的矩形
		matrix4f matBuffer;
		SceneToBufferMat(&matBuffer, 0, 0);
		matBuffer = pRenderUnit->m_VSelfMatrix * matBuffer;
		CalcBufferProjectSize(pRenderUnit->m_frtBound,
			v3d,
			fbound,
			&matBuffer);

		//计算总体的矩形
		m_rtUnitBound.left = min(m_rtUnitBound.left, pRenderUnit->m_frtBound.left);
		m_rtUnitBound.right = max(m_rtUnitBound.right, pRenderUnit->m_frtBound.right);
		m_rtUnitBound.top = min(m_rtUnitBound.top, pRenderUnit->m_frtBound.top);
		m_rtUnitBound.bottom = max(m_rtUnitBound.bottom, pRenderUnit->m_frtBound.bottom);
	}
	return m_rtUnitBound;
}

bool CalculatorTitleword::RefreshRenderUnit(int nFrame, BOOL bInherit)
{
	if (GetChgLevel() == ChgLevel_NULL && nFrame == m_nLastFrame)//没有变化
	{
		return FALSE;
	}
#if 0
	// 计算骨骼的变换矩阵
	CalcBone(nFrame, pEnv);
#endif
	BOOL bChanged = FALSE;
	// 计算每个单元的转换矩阵，把结果保存在RenderUnit中
	int nRenderUnitCount = 0;
	BaseARray * pRenderArray = &m_2DTextRenderUnitArray;
	nRenderUnitCount = (int)BaseAR_GetSize(pRenderArray);

	if (GetChgLevel() >= ChgLevel_Figure)
	{
		//如果不相等，需要释放并且重新计算
		int nUnitCount = Get2DTextUnitCount();
		if (nRenderUnitCount != nUnitCount)
		{
			for (int i = 0; i < nRenderUnitCount; i++)
			{
				SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)BaseAR_GetAt(pRenderArray, i);
				ReleaseRenderUnit(pRenderUnit, FALSE, DX3DEngine_GetMainDevice(), this);
				delete pRenderUnit;
			}
			BaseAR_Clear(pRenderArray);

#if 0
			//djt 3D暂时不考虑
			BOOL b3DUnit = (dwUnitType == SYITEM_UNITTYPE_3DTEXT || dwUnitType == SYITEM_UNITTYPE_3DSHAPE) ? TRUE : FALSE;
#endif
			BOOL b3DUnit = FALSE;
			for (int i = 0; i< nUnitCount; i++)
			{
				SYItemRenderUnit* pRenderUnit = CreateRenderUnit(b3DUnit);
				BaseAR_Add(pRenderArray, pRenderUnit);
			}
			SetChgLevel(ChgLevel_Figure);
			bChanged = TRUE;
		}
	}
	//计算Unit参数
	BOOL bUnitTrans = FALSE;
	int nUnitCount = (int)BaseAR_GetSize(pRenderArray);
	if (!nUnitCount)
		return bChanged;
	CCommonBuffer* pBuffer = Data_QueryCalcBuffer(nUnitCount);
	//初始化列表
#if 0
	//djt perunit暂时不考虑
	CBaseAction* pAction = pEnv->GetCurAction();
	BOOL bPerUnit = IsPerUnit(pEnv, dwUnitType);
	BOOL bPerUnitReverseOrder = bPerUnit && pAction->GetPerUnitInfo()->nOrder == Common_PER_UNIT_REVERSE_ORDER;
	for (int i = 0; i < nUnitCount; i++)
		pBuffer->m_dwBuffer[i] = bPerUnitReverseOrder ? nUnitCount - i - 1 : i;

	if (bPerUnit && pAction->GetPerUnitInfo()->nOrder == Common_PER_UNIT_RAMDOM_ORDER)
		SYShuffle((int*)pBuffer->m_dwBuffer, nUnitCount);
#endif

	for (int i = 0; i<nUnitCount; i++)
	{
		SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)BaseAR_GetAt(pRenderArray, i);
		if (pRenderUnit->m_bChanged || GetChgLevel() >= ChgLevel_Buffer || !pRenderUnit->m_pUnit) //材质发生改变时，也需要重新拷贝
		{
			SYUnit* pUnit = GetUnit(i);
			if (pUnit)
			{
				if (!pRenderUnit->m_pUnit)
				{

					pRenderUnit->m_pUnit = new SYUnit;
					sy_init_unit(pRenderUnit->m_pUnit, pUnit->type);
					pRenderUnit->m_bChanged = TRUE;
				}
				sy_copy_unit(pRenderUnit->m_pUnit, pUnit);
			}
			else
			{
				//异常数据，直接返回
				return FALSE;
			}
		}

		SYUnit* punitin = pRenderUnit->m_pUnit;
		if (!punitin)
			continue;

		if (pRenderUnit->m_bChanged || GetChgLevel() > ChgLevel_Matrix)
		{
			if (!pRenderUnit->graph)
			{
				pRenderUnit->graph = new vectorgraph;
				memset(pRenderUnit->graph, 0, sizeof(vectorgraph));
				pRenderUnit->m_bChanged = TRUE;
				bChanged = TRUE;
			}

			if (pRenderUnit->m_pUnit && pRenderUnit->m_pUnit->u.u2d.staticeffect.renderline)
			{
				//////////////////////////////////////////////////////////////////////////
				//标注：这里还需要修改。Unit和RenderUnit的graph不同步，会造出操作与显示不一致。
				//////////////////////////////////////////////////////////////////////////
				float fwidth = pRenderUnit->m_pUnit->u.u2d.staticeffect.fwidth;
				vectorgraph	desfigure1, desfigure2;
				memset(&desfigure1, 0, sizeof(vectorgraph));
				memset(&desfigure2, 0, sizeof(vectorgraph));

				figure_extend(&desfigure1, &punitin->u.u2d.graph, NULL, fwidth*0.5f, -1);
				figure_extend(&desfigure2, &punitin->u.u2d.graph, NULL, fwidth*0.5f, 1);
				ReverseVectorGraph(&desfigure2);
				MergeVectorGraph(pRenderUnit->graph, &desfigure1, &desfigure2);
				sy_release_vectorgraph(&desfigure2);
				sy_release_vectorgraph(&desfigure1);
				bChanged = TRUE;

			}
			else if (pRenderUnit->m_bChanged || GetChgLevel() >= ChgLevel_Figure)
			{
				CopyVectorGraph(pRenderUnit->graph, &punitin->u.u2d.graph);
				bChanged = TRUE;
			}
			//整体渲染

		}
#if 0
		//djt 3D
		else if ((dwUnitType == SYITEM_UNITTYPE_3DTEXT || dwUnitType == SYITEM_UNITTYPE_3DSHAPE)
			&& (pRenderUnit->m_bChanged || nChgLevel > CBaseCGEnv::Envchg_UnitTrans /*|| pEnv->m_nRenderChangedLevel > CBaseCGEnv::Envchg_RotateZoom*/))
		{
			FillRenderUnit(pEnv, pRenderUnit, punitin);
			bChanged = TRUE;
		}
#endif
		bUnitTrans = _trans_calcunit(nFrame, i, pBuffer->m_dwBuffer[i], bInherit);
		bChanged = bUnitTrans || bChanged;
	}
	//djt 整体渲染暂时不考虑
	if (/*IsWholeRender() && */(bChanged || GetChgLevel() >= ChgLevel_Figure))
	{
		bChanged |= UpdateWholeRenderGraph(pRenderArray);
	}
	if (bChanged)
	{
		SetChgLevel(ChgLevel_Figure);//todo:由于changedlevel改变，可能要设置为Envchg_UnitChange,现在还不确定
	}
	if (pBuffer)
		Data_ReleaseCalcBuffer(pBuffer);
	return bChanged;
}

BOOL CalculatorTitleword::_trans_calcunit(int nFrame, int nUnitIndex, int nOrderIndex, BOOL bInherit)
{
	SYItemRenderUnit* prenderunit = ObjectCalculator::GetRenderUnit(nUnitIndex, m_2DTextRenderUnitArray);
	SYUnit* punitin = prenderunit->m_pUnit;//GetUnit(dwUnitType,nUnitIndex,lpEnv);
	if (!punitin)
	{
		//prenderunit->m_pUnit = punitin;//定义	
		return FALSE;
	}
	//判断颜色是否需要动画
	if (nFrame != m_nLastFrame)
	{
		if (punitin->u.u2d.staticeffect.face.color.gradual.animation != GRAD_ANI_NONE
			|| punitin->u.u2d.staticeffect.border.color.gradual.animation != GRAD_ANI_NONE
			|| punitin->u.u2d.staticeffect.shadow.color.gradual.animation != GRAD_ANI_NONE)
		{
			prenderunit->m_bChanged = TRUE;
			//				 (pEnv,nIndex,TRUE);
		}
	}
#if 0
	//djt 活动纹理先后置
	//计算2D活动纹理
	if (punitin->u.u2d.staticeffect.dyntex.disable == FALSE)
	{
		if (punitin->u.u2d.staticeffect.dyntex.szPath[0])
		{
			dynamictexture* pDy = &punitin->u.u2d.staticeffect.dyntex;
			float2	Offset = { 0,0 }, Scale = { 1,1 }, Center = { 0,0 };
			float	Rotate = 0;
			double dOffset = nFrame / 2 * pDy->speed * 0.01;
			Offset.x = pDy->offsetx * 0.01 + cos(pDy->vRunDirx) * (float)dOffset;
			Offset.y = pDy->offsety * 0.01 + sin(pDy->vRunDirx) * (float)dOffset;
			prenderunit->unit2D.effect.fAlpha = pDy->alpha;
			matrix4f_from_pos(&prenderunit->unit2D.effect.DynamicMat, Offset.x, Offset.y, 0);
			matrix4f CenterMat, CenterMatInv, ScaleMat, RotateMat, TexMat;
			matrix4f_from_scale(&ScaleMat, pDy->scalex, pDy->scaley, 1);
			matrix4f_from_pos(&CenterMat, pDy->centerx * 0.01, pDy->centery * 0.01, 0);
			matrix4f_from_pos(&CenterMatInv, -pDy->centerx * 0.01, -pDy->centery * 0.01, 0);
			matrix4f_fromeuler_lyxz(&RotateMat, 0, 0, pDy->rotate);
			prenderunit->unit2D.effect.DynamicMat = CenterMat * ScaleMat * RotateMat * CenterMatInv *prenderunit->unit2D.effect.DynamicMat;
			prenderunit->unit2D.effect.bRepeat = pDy->bRepeat;
			prenderunit->m_bChanged = TRUE;
		}
	}
#endif
	prenderunit->m_dwMatrixType = enum_Matrix_Identity;
	int nRet = _trans_calcunit_matrix(&prenderunit->m_VSelfMatrix, &prenderunit->m_dwMatrixType, nFrame, nUnitIndex, nOrderIndex);

	if (prenderunit->graph == 0)
		prenderunit->m_bChanged = TRUE;
	return (nRet || prenderunit->m_bChanged);
}

BOOL CalculatorTitleword::_trans_calcunit_matrix(matrix4f * pVMatrix, int * pdwMatrixType, double dFrame, int nUnitIndex, int nOrderIndex)
{
	BOOL bChanged = FALSE;
#if 0
	//djt
	//得到当前的动作
	CBaseAction* pAction = pEnv->GetCurAction();
	if (!pAction)
		return FALSE;
#endif
	//得到单元
	SYUnit* punitin = GetRenderUnit(nUnitIndex)->m_pUnit;//  GetUnit(dwUnitType,nUnitIndex,lpPlayEnv);
																		   //位置
	vector3f v = { punitin->pos.x, punitin->pos.y, punitin->pos.z };
	/////////////////计算unit自身的形变/////////////////
	////////////////////////2.计算形变/////////////////
	vector3f vPosition = { 0.0f,0.0f,0.0f }, vScale = { 1.0f,1.0f,1.0f }, VCenter = { 0.0f,0.0f,0.0f }, vScale2 = { 1.0f,1.0f,1.0f };
	SYQuatn Qrotate = { 0.0f,0.0f,0.0f,1.0f };
	vector3f vAngle = { 0,0,0 };
	matrix4f tempVMatrix;

	//TODO:消除Scale
#if 0
	//djt
	calc_trans_group(&tempVMatrix, &vPosition, &vScale, &vAngle, &VCenter, dFrame, pAction->GetUnitTrans(nUnitIndex), NULL, m_nRotateOrder, NULL, &vScale2);
#endif
	calc_trans_group(&tempVMatrix, &vPosition, &vScale, &vAngle, &VCenter, dFrame, NULL, m_nRotateOrder, NULL, &vScale2);

	////////////////////////4.变化方式标识设置/////////////////
	if (pdwMatrixType && (vPosition.x != 0.0f || vPosition.y != 0.0f || vPosition.z != 0.0f))
		*pdwMatrixType |= enum_Matrix_Offset;
	if (pdwMatrixType && (vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f))
		*pdwMatrixType |= enum_Matrix_Zoom;
	if (pdwMatrixType && (vAngle.x != 0.0f || vAngle.y != 0.0f || vAngle.z != 0.0f))
		*pdwMatrixType |= enum_Matrix_Rotate;

#if 0
	//djt 先不考虑骨骼及3D的问题
	////////////////////////5.骨骼叠加/////////////////
	//5.1：如果有骨骼，乘上父骨骼的矩阵
	if (punitin->parentbone > 0)
	{
		BoneEnvNode* pBoneEnv = (BoneEnvNode*)BaseAR_GetAt(&pEnv->m_pBoneEnvData->BoneEnvArray, punitin->parentbone);
		SYBoneDef* pBonedef = (SYBoneDef*)GetBone(punitin->parentbone);
		vector3f vBone = { pBonedef->pivot.x, pBonedef->pivot.y, pBonedef->pivot.z };
		matrix4f matpivotE;
		float3ToTransMatQ(&matpivotE, &vBone);
		tempVMatrix *= matpivotE;
		tempVMatrix *= pBoneEnv->vMatrix;
	}
	else//5.2：如果没有骨骼，乘上到父物件的转换矩阵
	{
		//转换到Item空间
		if (dwUnitType == SYITEM_UNITTYPE_3DSHAPE || dwUnitType == SYITEM_UNITTYPE_3DTEXT)
			tempVMatrix *= punitin->MatVToItem;
	}
#endif
#if 0
	//djt 先不考虑逐字形变
	//添加逐字形变
	if (IsPerUnit(pEnv, dwUnitType))
	{
		double dPerUnitFrame = sy_get_real_frame(pAction->GetPerUnitInfo(), pAction->GetLength(), dFrame, nOrderIndex, GetUnitCount(dwUnitType, pEnv));
		////////////////////////1.计算路径/////////////////
		//
		matrix4f matpivotR;//因路径引起的旋转
		matrix4f_identity(&matpivotR);
		_calc_path(pEnv, pAction->GetPathParam(), dPerUnitFrame, &v, &matpivotR);

		float3 pos = { 0,0,0 }, scale = { 1,1,1 }, angle = { 0,0,0 }, center = { 0,0,0 };

		calc_trans_group_tovector(&pos, &scale, NULL, &angle, &center, dPerUnitFrame, pAction->GetTrans());
		DWORD dwMatrixType = 0;
		if (pos.x != 0.0f || pos.y != 0.0f || pos.z != 0.0f)
			dwMatrixType |= enum_Matrix_Offset;
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
			dwMatrixType |= enum_Matrix_Zoom;
		if (angle.x != 0.0f || angle.y != 0.0f || angle.z != 0.0f)
			dwMatrixType |= enum_Matrix_Rotate;
		if (*pdwMatrixType)
			*pdwMatrixType |= dwMatrixType;
		matrix4f mat;
		calc_trans_vector_tomatrix(&mat, &pos, &scale, &angle, &center, NULL, NULL, m_nRotateOrder);
		////////////////////////3.叠加路径变化/////////////////
		tempVMatrix *= mat * matpivotR;
	}
#endif
	matrix4f matpivotE;
	float3ToTransMatQ(&matpivotE, &v);
	matrix4f_mul(&tempVMatrix, &tempVMatrix, &matpivotE);

	////////////////////////5.如果有变化，输出，并且设置改变标志/////////////////
	if (*pVMatrix != tempVMatrix)
	{
		*pVMatrix = tempVMatrix;
		bChanged = TRUE;
	}
	return bChanged;
}

void CalculatorTitleword::GetRenderUnitOrder(CBaseIntArray & arOrder)
{
	int nUnitCnt = BaseAR_GetCount(&m_text.ar2DUnit);
	//特殊效果
	if (m_text.nSpecialUnitCnt > 0)
	{
		arOrder.Add(nUnitCnt - m_text.nSpecialUnitCnt);
		arOrder.Add(nUnitCnt);
	}
	//下划线
	if (m_text.nUnderlineUnitCnt > 0)
	{
		arOrder.Add(m_text.nTextUnitCnt);
		arOrder.Add(m_text.nTextUnitCnt + m_text.nUnderlineUnitCnt);
	}
	//文字
	arOrder.Add(0);
	arOrder.Add(m_text.nTextUnitCnt);
}

void CalculatorTitleword::DeleteWholeRenderGraph()
{
	BaseARray* parGraph = &m_arWholeRenderTextGraph;

	if (parGraph != NULL)
	{
		int nGraphCnt = BaseAR_GetCount(parGraph);
		for (int i = 0; i < nGraphCnt; ++i)
		{
			vectorgraph* pSingleGraph = (vectorgraph*)BaseAR_GetAt(parGraph, i);
			SYVectorGraph_Release(pSingleGraph);
			delete pSingleGraph;
		}
		BaseAR_RemoveAll(parGraph);
	}
}

BOOL CalculatorTitleword::UpdateWholeRenderGraph(BaseARray * pRenderUnitArray)
{
	//本函数假设前提：已经准备好所有零散Unit和Graph。
	INT_PTR nCount = BaseAR_GetSize(pRenderUnitArray);
	if (nCount <= 0)
	{
		DeleteWholeRenderGraph();
		return TRUE;
	}

	DeleteWholeRenderGraph();
	return TRUE;
#if 0
	//整体渲染暂时不考虑
	if (!IsWholeRender())
	{
		return TRUE;
	}
#endif

	BaseARray* parSingleGraph = &m_arWholeRenderTextGraph;

	CBaseIntArray arOrder;
	GetRenderUnitOrder(arOrder);

	int nOrderCnt = arOrder.GetCount();
	int nRenderUnitCnt = BaseAR_GetCount(pRenderUnitArray);
	int nOrderIdx = 0;
	do
	{
		//左闭右开区间
		int nOrderStart, nOrderEnd;
		if (nOrderCnt > 1)
		{
			nOrderStart = arOrder.GetAt(nOrderIdx++);
			nOrderEnd = arOrder.GetAt(nOrderIdx++);
		}
		else
		{
			nOrderStart = 0;
			nOrderEnd = nRenderUnitCnt;
		}

		vectorgraph* pSingleGraph = new vectorgraph;
		memset(pSingleGraph, 0, sizeof(vectorgraph));
		BaseAR_Add(parSingleGraph, pSingleGraph);

		//合并Graph
		vectorgraph* pGraphTemp;
		int nCellCnt = 0;

		int nStep = nOrderStart <= nOrderEnd ? 1 : -1;
		int i = nOrderStart;
		while (i != nOrderEnd)
		{
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)BaseAR_GetAt(pRenderUnitArray, i);
			if (pRenderUnit)
				nCellCnt += pRenderUnit->graph->ncell;
			i += nStep;
		}

		if (nCellCnt > 0)
		{
			vectorgraph vgTemp;
			memset(&vgTemp, 0, sizeof(vectorgraph));

			i = nOrderStart;
			vector3f pt1stUnit;
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)BaseAR_GetAt(pRenderUnitArray, i);
			pt1stUnit = pRenderUnit->m_pUnit->pos;

			pSingleGraph->ncell = nCellCnt;
			pSingleGraph->cell = new vectorcell[nCellCnt];
			int nCellIdx = 0;
			BaseRectf_SetRectEmpty(&pSingleGraph->bound);
			while (i != nOrderEnd)
			{
				SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)BaseAR_GetAt(pRenderUnitArray, i);
				if (pRenderUnit)
				{
					pGraphTemp = pRenderUnit->graph;
					CopyVectorGraph(&vgTemp, pGraphTemp);
					SYVectorGraph_Offset(&vgTemp, pRenderUnit->m_pUnit->pos.x - pt1stUnit.x, -(pRenderUnit->m_pUnit->pos.y - pt1stUnit.y));
					for (int nC = 0; nC<pGraphTemp->ncell; ++nC)
					{
						CopyCell(&(pSingleGraph->cell[nCellIdx]), &(vgTemp.cell[nC]));
						++nCellIdx;
					}

					if (i != nOrderStart)
					{
						BaseRectf_UnionRect(&pSingleGraph->bound, &pSingleGraph->bound, &vgTemp.bound);
					}
					else
					{
						pSingleGraph->bound = vgTemp.bound;
					}

					graph_release(&vgTemp);
				}
				i += nStep;
			}
		}
	} while (nOrderIdx < nOrderCnt);

	return TRUE;
}

void CalculatorTitleword::UpdateItemGpuBuffer(BOOL bLock)
{
	int nDevice = DX3DEngine_GetMainDevice();
	DXHalDef32* pHalBuffer = &m_HalDes;

	BaseARray* pRenderUnitArray = &m_2DTextRenderUnitArray;
	int nUnitCnt = BaseAR_GetCount(pRenderUnitArray);
	//修订基于以下原因：
	//1、某些物件需要多个Shape Unit；
	//2、实际上，目前Shape和Text Unit仅支持其中之一；
	//3、Text的buf会覆盖Shape的buf；
	if (nUnitCnt <= 0)
		return;

	float fScale = CalculatorManager::Instance().GetScale();

	int oldRT, oldDepth;
	RECT oldView, oldScissor;

	if (bLock) DX3DEngine_Lock(nDevice);
	DX3DEngine_GetRenderTarget(nDevice, &oldRT, &oldDepth);
	DX3DEngine_GetViewPort(nDevice, &oldView);
	DX3DEngine_GetScissor(nDevice, &oldScissor);

	DX3DEngine_SetRenderTarget(nDevice, pHalBuffer->m_hTex, 0);
	DX3DEngine_SetRenderState(nDevice, DXDepthState, DXDepthDisable);
	RECT newView = pHalBuffer->m_rtBound;
	OffsetRect(&newView, pHalBuffer->m_nOriginX, pHalBuffer->m_nOriginY);
	DX3DEngine_SetViewPort(nDevice, &newView);
	DX3DEngine_SetScissor(nDevice, &newView);
	CBaseIntArray arOrder;
	GetRenderUnitOrder(arOrder);
	int nOrderCnt = arOrder.GetCount();

	int nOrderIdx = 0;
	do
	{
		//左闭右开区间
		int nOrderStart, nOrderEnd;
		if (nOrderCnt > 1)
		{
			nOrderStart = arOrder.GetAt(nOrderIdx++);
			nOrderEnd = arOrder.GetAt(nOrderIdx++);
		}
		else
		{
			nOrderStart = 0;
			nOrderEnd = nUnitCnt;
		}

		//一个RenderUnit段
		int nStep = nOrderStart <= nOrderEnd ? 1 : -1;
		int i = nOrderStart;
		while (i != nOrderEnd)
		{
			SYItemRenderUnit* pRenderUnit = (SYItemRenderUnit*)BaseAR_GetAt(pRenderUnitArray, i);;
			if (!pRenderUnit)
			{
				i += nStep;
				continue;
			}
			//计算原始buffer
			DWORD *pBuffer = NULL;
			int facew = 0, faceh = 0, pitch = 0;
			SYRenderUnitView * pUnitView = GetRenderUnitView(pRenderUnit->szID);
			if (!pUnitView)
			{
				i += nStep;
				continue;
			}

			matrix4f matTrans;
			matTrans = pRenderUnit->m_VSelfMatrix;
			matTrans._41 *= fScale;
			matTrans._42 *= fScale;
			matTrans._43 *= fScale;

			DX3DEngine_RenderTexture(nDevice, pUnitView->pUnitHalBuf, &matTrans, &pHalBuffer->m_rtBound, FALSE);
			if (0)
			{
				DX3DEngine_DumpTexture(nDevice, pUnitView->pUnitHalBuf->m_hTex, _T("E:\\unit.bmp"));
				DX3DEngine_DumpTexture(nDevice, pHalBuffer->m_hTex, _T("E:\\whole.bmp"));
			}

			if (pUnitView)
				pUnitView->bChanged = FALSE;
			i += nStep;
		}
	} while (nOrderIdx < nOrderCnt);

	if (0)
		DX3DEngine_DumpTexture(nDevice, pHalBuffer->m_hTex, _T("E:\\123.bmp"));
	DX3DEngine_SetRenderTarget(nDevice, oldRT, oldDepth);
	DX3DEngine_SetViewPort(nDevice, &oldView);
	DX3DEngine_SetScissor(nDevice, &oldScissor);

	if (bLock)
		DX3DEngine_UnLock(nDevice);
}

SYItemRenderUnit * CalculatorTitleword::GetRenderUnit(int nIndex)
{
	BaseARray* pRenderArray = &m_2DTextRenderUnitArray;
	return (SYItemRenderUnit*)BaseAR_GetAt(pRenderArray, nIndex);
}

int CalculatorTitleword::Get2DTextUnitCount()
{
	return (int)BaseAR_GetSize(&m_text.ar2DUnit);
}

void CalculatorTitleword::GetRenderUnitBound(int nIndex, rectf & rtBoundf)
{
	BaseARray* pRenderUnitArray = &m_2DTextRenderUnitArray;
	SYItemRenderUnit * pRenderUnit = (SYItemRenderUnit *)BaseAR_GetAt(pRenderUnitArray, nIndex);
	if (!pRenderUnit->graph || !pRenderUnit->m_pUnit)
	{
		memset(&rtBoundf, 0, sizeof(rtBoundf));
		return;
	}
	rectf rtface = pRenderUnit->graph->bound;
	rtBoundf = rtface;
	float fcubedeltX = 0;
	float fcubedeltY = 0;
	if (!pRenderUnit->m_pUnit->u.u2d.staticeffect.cube.disable && pRenderUnit->m_pUnit->u.u2d.staticeffect.cube.width)
	{
		fcubedeltX = pRenderUnit->m_pUnit->u.u2d.staticeffect.cube.width * cos(pRenderUnit->m_pUnit->u.u2d.staticeffect.cube.angle);
		fcubedeltY = pRenderUnit->m_pUnit->u.u2d.staticeffect.cube.width * sin(pRenderUnit->m_pUnit->u.u2d.staticeffect.cube.angle);

		rectf rtcubef = rtBoundf;
		//BaseRectf_OffsetRect1(&rtcubef,fcubedeltX,fcubedeltY);
		BaseRectf_InflateRect1(&rtcubef, abs(fcubedeltX), abs(fcubedeltY));
		BaseRectf_UnionRect(&rtBoundf, &rtBoundf, &rtcubef);
	}
	if (!pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.disable &&
		(pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.offsetx
			|| pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.soft
			|| pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.offsety
			|| pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.effect_fxtitle
			|| pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.effect_fyscale))
	{
		rectf rtShadow = rtface;
		BaseGraph_DataBase::shadowinfo* pinfo = &pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow;
		float fheight = rtBoundf.bottom - rtBoundf.top;
		float fwidth = rtBoundf.right - rtBoundf.left;
		float bottom = rtBoundf.bottom;
		bottom = 0;

		vector3f vVector[4];
		//BaseRectf_OffsetRect1(&rtShadow,(float)pinfo->offsetx,(float)pinfo->offsety);
		BaseRectf_InflateRect1(&rtShadow, abs(pinfo->offsetx), abs(pinfo->offsety));
		BaseRectf_InflateRect1(&rtShadow, abs(fcubedeltX), abs(fcubedeltY));	// 在立体边基础上扩大
																			//BaseRectf_OffsetRect1(&rtShadow,fcubedeltX,fcubedeltY);

																			// 此值暂时无用，之前版本错误，有些模板存有错误值，特此修正
		pinfo->effect_fxtitle = 0;

		vVector[0].x = rtShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.top - bottom) / fheight;
		vVector[0].y = bottom + (rtShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

		vVector[1].x = rtShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.top - bottom) / fheight;
		vVector[1].y = bottom + (rtShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

		vVector[2].x = rtShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.bottom - bottom) / fheight;
		vVector[2].y = bottom + (rtShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

		vVector[3].x = rtShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtShadow.bottom - bottom) / fheight;
		vVector[3].y = bottom + (rtShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

		for (int i = 0; i < 4; i++)
		{
			rtShadow.left = min(vVector[i].x, rtShadow.left);
			rtShadow.top = min(vVector[i].y, rtShadow.top);
			rtShadow.right = max(vVector[i].x, rtShadow.right);
			rtShadow.bottom = max(vVector[i].y, rtShadow.bottom);
		}
		if (pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.soft)
		{
			BaseRectf_InflateRect1(&rtShadow,
				(float)pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.soft,
				(float)pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.soft);
		}
		BaseRectf_UnionRect(&rtBoundf, &rtBoundf, &rtShadow);
	}
	if (!pRenderUnit->m_pUnit->u.u2d.staticeffect.border.disable)
	{
		int nRadWidth = Common_FToI(pRenderUnit->m_pUnit->u.u2d.staticeffect.border.width) + pRenderUnit->m_pUnit->u.u2d.staticeffect.border.soft;
		rectf rtborder = rtface;
		BaseRectf_InflateRect1(&rtborder, (float)nRadWidth, (float)nRadWidth);
		//BaseRectf_OffsetRect1(&rtborder,fcubedeltX,fcubedeltY);
		BaseRectf_UnionRect(&rtBoundf, &rtBoundf, &rtborder);

		float fdeltX = 0;
		float fdeltY = 0;
		if (!pRenderUnit->m_pUnit->u.u2d.staticeffect.bordercube.disable && pRenderUnit->m_pUnit->u.u2d.staticeffect.bordercube.width)
		{
			fdeltX = pRenderUnit->m_pUnit->u.u2d.staticeffect.bordercube.width * cos(pRenderUnit->m_pUnit->u.u2d.staticeffect.bordercube.angle);
			fdeltY = pRenderUnit->m_pUnit->u.u2d.staticeffect.bordercube.width * sin(pRenderUnit->m_pUnit->u.u2d.staticeffect.bordercube.angle);

			rectf rtbordercubef = rtborder;
			BaseRectf_InflateRect1(&rtbordercubef, abs(fdeltX), abs(fdeltY));
			BaseRectf_UnionRect(&rtBoundf, &rtBoundf, &rtbordercubef);
		}

		if (!pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow.disable &&
			(pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow.offsetx
				|| pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow.soft
				|| pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow.offsety
				|| pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow.effect_fxtitle
				|| pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow.effect_fyscale))
		{
			rectf rtBorderShadow = rtborder;
			BaseGraph_DataBase::shadowinfo* pinfo = &pRenderUnit->m_pUnit->u.u2d.staticeffect.bordershadow;
			float fheight = rtBoundf.bottom - rtBoundf.top;
			float fwidth = rtBoundf.right - rtBoundf.left;
			float bottom = rtBoundf.bottom;
			bottom = 0;

			vector3f vVector[4];
			BaseRectf_InflateRect1(&rtBorderShadow, abs(pinfo->offsetx), abs(pinfo->offsety));
			// 			BaseRectf_OffsetRect1(&rtBorderShadow,fdeltX,fdeltY);

			// 此值暂时无用，之前版本错误，有些模板存有错误值，特此修正
			pinfo->effect_fxtitle = 0;

			vVector[0].x = rtBorderShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.top - bottom) / fheight;
			vVector[0].y = bottom + (rtBorderShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

			vVector[1].x = rtBorderShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.top - bottom) / fheight;
			vVector[1].y = bottom + (rtBorderShadow.top - bottom) * (pinfo->effect_fyscale + 1.0f);

			vVector[2].x = rtBorderShadow.right + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.bottom - bottom) / fheight;
			vVector[2].y = bottom + (rtBorderShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

			vVector[3].x = rtBorderShadow.left + pinfo->effect_fxtitle * fwidth * fabs(rtBorderShadow.bottom - bottom) / fheight;
			vVector[3].y = bottom + (rtBorderShadow.bottom - bottom) * (pinfo->effect_fyscale + 1.0f);

			for (int i = 0; i < 4; i++)
			{
				rtBorderShadow.left = min(vVector[i].x, rtBorderShadow.left);
				rtBorderShadow.top = min(vVector[i].y, rtBorderShadow.top);
				rtBorderShadow.right = max(vVector[i].x, rtBorderShadow.right);
				rtBorderShadow.bottom = max(vVector[i].y, rtBorderShadow.bottom);
			}
			if (pRenderUnit->m_pUnit->u.u2d.staticeffect.shadow.soft)
			{
				BaseRectf_InflateRect1(&rtBorderShadow,
					(float)pinfo->soft,
					(float)pinfo->soft);
			}
			BaseRectf_UnionRect(&rtBoundf, &rtBoundf, &rtBorderShadow);
		}
	}
}

int CalculatorTitleword::GetWidth()
{
	return BaseRectf_Width(&m_rtUnitBound);
}

int CalculatorTitleword::GetHeight()
{
	return BaseRectf_Height(&m_rtUnitBound);
}

SYText * CalculatorTitleword::GetText()
{
	return &m_text;
}

BaseARray * CalculatorTitleword::Get2DTextUnitArray()
{
	return &m_text.ar2DUnit;
}

SYUnit * CalculatorTitleword::GetUnit(int nIndex)
{
	BaseARray* pAryUnit = Get2DTextUnitArray();
	if (nIndex < 0 || nIndex >= BaseAR_GetCount(pAryUnit))
		return NULL;
	return (SYUnit*)BaseAR_GetAt(pAryUnit, nIndex);
}

void CalculatorTitleword::DeleteRenderUnitView(LPCTSTR szUnitID)
{
	for (int i = 1; i < BaseAR_GetCount(&m_text.ar2DUnit); ++i)
	{
		SYUnit* pTemplUnit = (SYUnit*)BaseAR_GetAt(&m_text.ar2DUnit, i);
		if (pTemplUnit)
		{
			assert(false);
		}
	}
}

void CalculatorTitleword::RenderUnitBufferForGpu()
{
	if (GetChgLevel() < ChgLevel_Figure)
		return;

	int nDevice = DX3DEngine_GetMainDevice();
	BaseARray* pUnitArray = &m_2DTextRenderUnitArray;
	int nUnitCnt = BaseAR_GetCount(pUnitArray);
	for (int i = 0; i < nUnitCnt; i++)
	{
		SYItemRenderUnit * pRenderUnit = (SYItemRenderUnit *)BaseAR_GetAt(pUnitArray, i);
		//计算原始buffer
		DWORD *pBuffer = NULL;
		int facew = 0, faceh = 0, pitch = 0;
		pointf ptOrg;
		RECT rtResultBound;// = pRenderUnit->unit2D.bound; 

		SYRenderUnitView* pUnitView = GetRenderUnitView(pRenderUnit->szID);
		if (!pRenderUnit || pRenderUnit->graph == 0 || pRenderUnit->graph->ncell == 0)
		{
			if (pUnitView)
				pUnitView->bChanged = FALSE;

			//清空原来的Buf
			if (pUnitView != NULL && pUnitView->pUnitHalBuf->m_hTex != NULL)
				DX3DEngine_ClearTexture(nDevice, pUnitView->pUnitHalBuf->m_hTex, 0);
			continue;
		}

		//djt change level的问题后边再逐步考虑
		//if (pUnitView != NULL && !pUnitView->bChanged)
		//	continue;

		CCommonBuffer* pBufferPool = NULL;
		//MAC_STARTTIME_TEST();
		SYRenderGraphToOpBuff(pRenderUnit->graph,
			&pRenderUnit->m_pUnit->u.u2d.staticeffect,
			pBufferPool,
			rtResultBound,
			facew,
			faceh,
			pitch,
			CalculatorManager::Instance().GetScale(),
			ptOrg,
			CalculatorManager::Instance().GetField());

		if (0)
		{
			BufferFunction::WriteBitmap(_T("e:\\8.bmp"), facew, faceh, 32, (BYTE*)pBufferPool->m_dwBuffer);
		}
		if (!pBufferPool)
		{
			//清空原来的Buf
			if (pUnitView != NULL && pUnitView->pUnitHalBuf->m_hTex != NULL)
				DX3DEngine_ClearTexture(nDevice, pUnitView->pUnitHalBuf->m_hTex, 0);
			continue;
		}

		if (!pUnitView)
		{
			pUnitView = new SYRenderUnitView;
			pUnitView->pUnitBuffer = NULL;
			pUnitView->pUnitDymHalBuffer = NULL;
			pUnitView->pUnitHalBuf = new DXHalDef32;
			pUnitView->bChanged = TRUE;
			memset(pUnitView->pUnitHalBuf, 0, sizeof(DXHalDef32));
			AddRenderUnitView(pRenderUnit->szID, pUnitView);
		}

		DXHalDef32* pUnitHalBuffer = pUnitView->pUnitHalBuf;
		if (pUnitHalBuffer)
		{
			if (pUnitHalBuffer->m_hTex)
			{
				int nTexWidth = 0, nTexHeight = 0;
				DX3DEngine_GetTextureInfo(nDevice, pUnitHalBuffer->m_hTex, &nTexWidth, &nTexHeight, NULL);
				if (nTexWidth < facew || nTexHeight < faceh)
				{
					if (pUnitHalBuffer->m_bRelease)
						DX3DEngine_ReleaseTexture(nDevice, pUnitHalBuffer->m_hTex);
					pUnitHalBuffer->m_hTex = NULL;
				}
			}

			if (!pUnitHalBuffer->m_hTex)
			{
				pUnitHalBuffer->m_hTex = DX3DEngine_CreateTexture(nDevice, facew, faceh, DXTexture_Normal2DTex);
				pUnitHalBuffer->m_bRelease = TRUE;
			}
			DX3DEngine_ClearTexture(nDevice, pUnitHalBuffer->m_hTex, 0);

			pUnitHalBuffer->m_rtBound = rtResultBound;
			pUnitHalBuffer->m_nOriginX = -rtResultBound.left;
			pUnitHalBuffer->m_nOriginY = -rtResultBound.top;

			int hTransTex = DXTextureManager_QueryTexture(DXTexture_MemTexture, facew, faceh);
			DX3DEngine_UpLoad(pUnitHalBuffer->m_hTex, nDevice, hTransTex,
				0, 0, facew, faceh, pBufferPool->m_dwBuffer, 0, 0, facew, faceh, facew, faceh);
			if (0)
				DX3DEngine_DumpTexture(nDevice, pUnitHalBuffer->m_hTex, _T("E:\\123.bmp"));
			DXTextureManager_UnQueryTexture(hTransTex);
		}
		if (pBufferPool)
			Data_ReleaseCalcBuffer(pBufferPool);
		pBufferPool = NULL;
	}
}

bool CalculatorTitleword::PrepareItemHalBufer(int nDevice)
{
	int nTexWidth = 0;
	int nTexHeight = 0;
	rectf rtBoundf = m_rtUnitBound;
	float fScale = CalculatorManager::Instance().GetScale();
	BaseRectf_Scale(&rtBoundf, fScale);
	int nLeft = 0;
	int nTop = 0;
	int nRight = 0;
	int nBottom = 0;
	rip_extend_fbound(&rtBoundf, &nLeft, &nTop, &nRight, &nBottom);
	int nWidth = nRight - nLeft;
	int nHeight = nBottom - nTop;
	m_HalDes.m_nOriginX = -nLeft;
	m_HalDes.m_nOriginY = -nTop;
	SetRect(&m_HalDes.m_rtBound, nLeft, nTop, nRight, nBottom);

	DXTextureType emTexType = DXTexture_Normal2DTex;
	if (m_HalDes.m_hTex)
	{
		DX3DEngine_GetTextureInfo(nDevice, m_HalDes.m_hTex, &nTexWidth, &nTexHeight, (int *)&emTexType);
		BOOL bRelease = nTexWidth < nWidth || nTexHeight < nHeight
			|| nWidth >= 8192 || nHeight >= 8192
			|| ((nTexWidth > nWidth || nTexHeight > nHeight) && (nTexHeight > 8192 || nTexWidth > 8192));
		// 8192以内，新的tex超过旧的，重新生成，新的tex超过8192，重新生成，新的tex正常旧的tex超过8192，重新生成
		if (bRelease)
		{
			if (m_HalDes.m_bRelease)
				DX3DEngine_ReleaseTexture(nDevice, m_HalDes.m_hTex);
			m_HalDes.m_hTex = 0;
		}
	}
	if (!m_HalDes.m_hTex)
	{
		m_HalDes.m_hTex = DX3DEngine_CreateTexture(nDevice, nWidth, nHeight, emTexType);
		m_HalDes.m_bRelease = TRUE;
	}
	DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex);
	return true;
}

bool CalculatorTitleword::GetOriBound(OUT rectf & rtBoundf)
{
	rtBoundf = CalUnitBound();
	return true;
}
