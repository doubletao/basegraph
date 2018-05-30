#include "stdafx.h"
#include "ObjectCalculator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#include "CalculatorManager.h"

void ObjectCalculator::SetRenderMatV(matrix4f* pMat)
{
	m_VGlobalMatrix = *pMat;
}

const matrix4f* ObjectCalculator::GetRenderMatV() const
{
	return &m_VGlobalMatrix;
}

ObjectCalculator::ObjectCalculator(CString strObjID)
	: m_strObjID(strObjID)
	, m_nObjBufferID(0)
	, m_emChgLevel(ChgLevel_Create)
	, m_bSelected(false)
	, m_nLastFrame(-1)
	, m_nRotateOrder(ROTATE_ORDER_YXZ)
{
	InitializeCriticalSection(&m_Lock);
	matrix4f_identity(&m_VSelfMatrix);
	matrix4f_identity(&m_VGlobalMatrix);
	matrix4f_identity(&m_ScrollMatrix);
}


ObjectCalculator::~ObjectCalculator()
{
	DeleteCriticalSection(&m_Lock);
	Release();
}

void ObjectCalculator::Release()
{
	ClearRenderUnitView();
	matrix4f_identity(&m_VSelfMatrix);
	matrix4f_identity(&m_VGlobalMatrix);
	matrix4f_identity(&m_ScrollMatrix);
}

int ObjectCalculator::GetWidth()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Obj_Width_Int, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

int ObjectCalculator::GetHeight()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Obj_Height_Int, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

ObjectBuffer * ObjectCalculator::GetObjectBuffer()
{
	int nBufID = GetObjBufferID();
	ObjectBuffer * pObjBuf = DataBufferManager::Instance().Get2DBuf(nBufID);
	return pObjBuf;
}

int ObjectCalculator::GetObjBufferID()
{
	return m_nObjBufferID;
}

bool ObjectCalculator::GetObjSimpBuffer(StuSimpleBuffer & pBuf)
{
	ObjectBuffer * pObjBuf = GetObjectBuffer();
	if (pObjBuf)
	{
		pBuf.pBuffer = pObjBuf->GetBuffer();
		pBuf.nWidth = pObjBuf->GetWidth();
		pBuf.nHeight = pObjBuf->GetHeight();
		return true;
	}
	return false;
}

bool ObjectCalculator::SetSelected(bool bSel)
{
	bool bSelOri = m_bSelected;
	m_bSelected = bSel;
	return bSelOri;
}

bool ObjectCalculator::IsSelected()
{
	return m_bSelected;
}

DXHalDef32 * ObjectCalculator::GetHalDes()
{
	return &m_HalDes;
}

CString ObjectCalculator::GetObjectID()
{
	return m_strObjID;
}

BaseGraph_DataBase::ITEM_FORMTYPE ObjectCalculator::Get3DType() const
{
	return DataBase_Get3DType(m_strObjID);
}

bool ObjectCalculator::GetOriBound(OUT rectf & rtBoundf)
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	if (nWidth > 0 && nHeight > 0)
	{
		rtBoundf.left = -nWidth * 1.0f / 2;
		rtBoundf.right = nWidth * 1.0f / 2;
		rtBoundf.top = -nHeight * 1.0f / 2;
		rtBoundf.bottom = nHeight * 1.0f / 2;
		return true;
	}
	return false;
}

bool ObjectCalculator::GetSceneBound(OUT rectf & rtBoundf)
{
	if (GetOriBound(rtBoundf))
	{
		//取得物件旋转缩放位移属性，进而得出物件的scene bound
		point2f ptLeftTop = { rtBoundf.left, rtBoundf.top };
		point2f ptRightBottom = { rtBoundf.right, rtBoundf.bottom };
		ptLeftTop *= m_VSelfMatrix;
		ptRightBottom *= m_VSelfMatrix;
		
		rtBoundf.left = min(rtBoundf.left, ptLeftTop.x);
		rtBoundf.top = min(rtBoundf.top, -ptLeftTop.y);
		rtBoundf.right = max(rtBoundf.right, ptLeftTop.x);
		rtBoundf.bottom = max(rtBoundf.bottom, -ptLeftTop.y);

		rtBoundf.left = min(rtBoundf.left, ptRightBottom.x);
		rtBoundf.top = min(rtBoundf.top, -ptRightBottom.y);
		rtBoundf.right = max(rtBoundf.right, ptRightBottom.x);
		rtBoundf.bottom = max(rtBoundf.bottom, -ptRightBottom.y);
		return true;
	}
	return false;
}

bool ObjectCalculator::GetCtrlPoint(OUT std::vector<pointf>& vecCtrlPoint)
{
	rectf rtBoundf;
	if (GetOriBound(rtBoundf))
	{
		float2 pt[4] = {
			{ rtBoundf.left, -rtBoundf.top},
			{ rtBoundf.left, -rtBoundf.bottom},
			{ rtBoundf.right, -rtBoundf.bottom},
			{ rtBoundf.right, -rtBoundf.top},
		};

		matrix4f vSelfMatrix = GetSelfMatrix();
		for (int i = 0; i < 4; i++)
		{
			pt[i] *= vSelfMatrix;
			vecCtrlPoint.push_back(pt[i]);
		}
		return true;
	}
	return false;
}

void ObjectCalculator::Lock()
{
	EnterCriticalSection(&m_Lock);
}

void ObjectCalculator::Unlock()
{
	LeaveCriticalSection(&m_Lock);
}

//判定是否点在物件内
bool ObjectCalculator::HitTest(IN pointf ptHit)
{
	//不需要渲染的物件，不执行hittest
	if (IsItemNeedRender())
	{
		//先拿到控制点矩形
		std::vector<pointf> vecCtrlPoint;
		if (GetCtrlPoint(vecCtrlPoint))
		{
			return Common_IsPointInPolygon(ptHit, vecCtrlPoint);
		}
	}
	return false;
}

//框选，条件是碰着就算
bool ObjectCalculator::BoxHitTest(IN rectf rtBox)
{
	//不需要渲染的物件，不执行hittest
	if (!IsItemNeedRender())
	{
		return false;
	}
	auto PushOneLine = [](std::vector<std::pair<pointf, pointf>> & vecBoxLine, float fX1, float fY1, float fX2, float fY2)
	{
		pointf ptStart = { fX1, fY1 };
		pointf ptEnd = { fX2, fY2 };
		vecBoxLine.push_back(std::make_pair(ptStart, ptEnd));
	};
	auto IsCrossLine = [](std::pair<pointf, pointf> line1, std::pair<pointf, pointf> line2) -> bool
	{
		return Common_IsCrossLine(line1.first, line1.second, line2.first, line2.second);
	};
	//先拿到控制点矩形
	bool bRet = false;
	std::vector<pointf> vecCtrlPoint;
	if (GetCtrlPoint(vecCtrlPoint))
	{
		//先判定控制点
		for (size_t i = 0; i < vecCtrlPoint.size(); i++)
		{
			if (Common_IsPointInPolygon(vecCtrlPoint[i], rtBox))
			{
				//只要有一个控制点在里边就视为选中
				bRet = true;
				break;
			}
		}
		//如果选中框中任何一个点被包含在控制点内也视为选中
		if (!bRet)
		{
			if (Common_IsPointInPolygon(rtBox.left, rtBox.top, vecCtrlPoint))
			{
				//只要有一个在里边就视为选中
				bRet = true;
			}
			else if (Common_IsPointInPolygon(rtBox.right, rtBox.top, vecCtrlPoint))
			{
				//只要有一个在里边就视为选中
				bRet = true;
			}
			else if (Common_IsPointInPolygon(rtBox.right, rtBox.bottom, vecCtrlPoint))
			{
				//只要有一个在里边就视为选中
				bRet = true;
			}
			else if (Common_IsPointInPolygon(rtBox.left, rtBox.bottom, vecCtrlPoint))
			{
				//只要有一个在里边就视为选中
				bRet = true;
			}
		}
		//没有被包含则判定边相交情况
		if (!bRet)
		{
			//box的四条边和控制点构成的边进行相交判定
			std::vector<std::pair<pointf, pointf>> vecBoxLine;
			PushOneLine(vecBoxLine, rtBox.left, rtBox.top, rtBox.right, rtBox.top);
			PushOneLine(vecBoxLine, rtBox.right, rtBox.top, rtBox.right, rtBox.bottom);
			PushOneLine(vecBoxLine, rtBox.right, rtBox.bottom, rtBox.left, rtBox.bottom);
			PushOneLine(vecBoxLine, rtBox.left, rtBox.bottom, rtBox.left, rtBox.top);

			std::vector<std::pair<pointf, pointf>> vecCtrlLine;
			for (int i = 0; i < vecCtrlPoint.size() - 1; i++)
			{
				vecCtrlLine.push_back(std::make_pair(vecCtrlPoint[i], vecCtrlPoint[i + 1]));
			}
			vecCtrlLine.push_back(std::make_pair(vecCtrlPoint[0], *vecCtrlPoint.rbegin()));

			for (int i = 0; i < vecBoxLine.size(); i++)
			{
				for (int j = 0; j < vecCtrlLine.size(); j++)
				{
					if (IsCrossLine(vecBoxLine[i], vecCtrlLine[j]))
					{
						bRet = true;
						break;
					}
				}
			}
		}
	}
	return bRet;
}

//框选，条件是框住了才算
bool ObjectCalculator::BoxInterTest(IN rectf rtBox)
{
	//不需要渲染的物件，不执行hittest
	if (!IsItemNeedRender())
	{
		return false;
	}
	//先拿到控制点矩形
	std::vector<pointf> vecCtrlPoint;
	if (GetCtrlPoint(vecCtrlPoint))
	{
		for (size_t i = 0; i < vecCtrlPoint.size(); i++)
		{
			//如果控制点都在矩形内，则就是框住了
			if (!Common_IsPointInPolygon(vecCtrlPoint[i], rtBox))
			{
				//有一个不在里边就算是外边
				return false;
			}
		}
		return true;
	}
	return false;
}

bool ObjectCalculator::NeedUpdateBuffer()
{
	if (!IsItemNeedRender())
	{
		return false;
	}
	//如果连bufer都还没有呢，必然需要render
	ObjectBuffer * pBufer = DataBufferManager::Instance().Get2DBuf(GetObjBufferID());
	if (!pBufer)
	{
		return true;
	}
	//如果level大于等于buf，需要render
	if (m_emChgLevel >= ChgLevel_Buffer)
	{
		return true;
	}
	return false;
}

SYRenderUnitView * ObjectCalculator::GetRenderUnitView(CString strUnitID)
{
	SYRenderUnitView * pRenderUnitView = nullptr;
	if (m_mapRenderUnitView.count(strUnitID) > 0)
	{
		pRenderUnitView = m_mapRenderUnitView[strUnitID];
	}
	return pRenderUnitView;
}

void ObjectCalculator::AddRenderUnitView(CString strUnitID, SYRenderUnitView * pRenderUnitView)
{
	if (m_mapRenderUnitView.count(strUnitID) > 0)
	{
		SAFE_DELETE(m_mapRenderUnitView[strUnitID]);
	}
	m_mapRenderUnitView.insert(std::make_pair(strUnitID, pRenderUnitView));
}

void ObjectCalculator::ClearRenderUnitView()
{
	for (std::map<CString, SYRenderUnitView *>::iterator it = m_mapRenderUnitView.begin(); it != m_mapRenderUnitView.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_mapRenderUnitView.clear();
}

bool ObjectCalculator::UpdateRenderBuffer()
{
	SetChgLevel(ChgLevel_NULL);
	return true;
}

bool ObjectCalculator::PrepareItemHalBufer(int nDevice)
{
	int nTexWidth = 0;
	int nTexHeight = 0;
	int nWidth = GetWidth();
	int nHeight = GetHeight();
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
	m_HalDes.m_nOriginX = nWidth / 2;
	m_HalDes.m_nOriginY = nHeight / 2;
	m_HalDes.m_rtBound.left = -nWidth / 2;
	m_HalDes.m_rtBound.right = nWidth / 2;
	m_HalDes.m_rtBound.top = -nHeight / 2;
	m_HalDes.m_rtBound.bottom = nHeight / 2;
	DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex);
	return true;
}

bool ObjectCalculator::IsItemNeedRender()
{
	//这里边不只是对frame的判定，应该还会有物件可视性灯其他决定因素
	if (!ValidAction())
	{
		return false;
	}
	return true;
}

inline bool ObjectCalculator::ValidAction()
{
	bool bRet = true;
	//如果当前帧号在action起始点之外，则无效
	BaseGraph_Common::Value valueBeginFrame;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Obj_ActionBegin_Int, valueBeginFrame))
	{
		BaseGraph_Common::Value valueEndFrame;
		if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Obj_ActionEnd_Int, valueEndFrame))
		{
			int nCurFrame = CalculatorManager::Instance().GetFrame();
			int nBeginFrame = valueBeginFrame.ToInt();
			int nEndFrame = valueEndFrame.ToInt();
			if (nCurFrame < nBeginFrame || nCurFrame > nEndFrame)
			{
				bRet = false;
			}
		}
	}
	return bRet;
}

SYItemRenderUnit * ObjectCalculator::GetRenderUnit(int nIndex, const SYArray & pUnitArray)
{
	return (SYItemRenderUnit*)SYAR_GetAt(&pUnitArray, nIndex);
}

void ObjectCalculator::GetSceneCenter(float3& pt)
{
	pt.x = DataBase_GetSceneWidth() / 2.0f;
	pt.y = DataBase_GetSceneHeight() / 2.0f;
	pt.z = 0;
}

void ObjectCalculator::Update2DRenderMatrix(IN float fScale)
{
	if (GetChgLevel() >= ChgLevel_Matrix)
	{
		m_VSelfMatrix = GetSelfMatrix();
		m_VSelfMatrix._41 *= fScale;
		m_VSelfMatrix._42 *= fScale;
		m_VSelfMatrix._43 *= fScale;
	}
}

matrix4f ObjectCalculator::Get2DRenderMatrix()
{
	return m_VSelfMatrix;
}

matrix4f ObjectCalculator::GetScrollMatrix()
{
	return m_ScrollMatrix;
}

matrix4f ObjectCalculator::GetSelfMatrix()
{
	matrix4f vSelfMatrix;
	matrix4f_identity(&vSelfMatrix);
	calc_trans_vector_tomatrix(vSelfMatrix, DataBase_GetItemCenter(m_strObjID), DataBase_GetItemScale(m_strObjID), DataBase_GetItemAngleRadian(m_strObjID), DataBase_GetItemPos(m_strObjID), ROTATE_ORDER_YXZ);
	return vSelfMatrix;
}

void ObjectCalculator::SetChgLevel(EmObjectChgLevel emChgLevel)
{
	m_emChgLevel = emChgLevel;
}

void ObjectCalculator::AddChgLevel(EmObjectChgLevel emChgLevel)
{
	if (m_emChgLevel < emChgLevel)
	{
		m_emChgLevel = emChgLevel;
	}
}

EmObjectChgLevel ObjectCalculator::GetChgLevel()
{
	return m_emChgLevel;
}

////////////////////////SYRenderUnitView//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SYRenderUnitView::SYRenderUnitView()
{
	bChanged = FALSE;
	pUnitBuffer = NULL;
	pUnitHalBuf = NULL;
	pUnitDymHalBuffer = NULL;
	bUseColorAlpha = FALSE;
}

SYRenderUnitView::~SYRenderUnitView()
{
	bChanged = FALSE;
	if (pUnitBuffer)
	{
		SYBufferDef32_Release(pUnitBuffer);
	}
	pUnitBuffer = nullptr;
	int nDevice = DX3DEngine_GetMainDevice();
	if (pUnitHalBuf && pUnitHalBuf->m_bRelease)
	{
		DX3DEngine_ReleaseTexture(nDevice, pUnitHalBuf->m_hTex);
		memset(pUnitHalBuf, 0, sizeof(DXHalDef32));
	}
	pUnitHalBuf = nullptr;
	if (pUnitDymHalBuffer && pUnitDymHalBuffer->m_bRelease)
	{
		DX3DEngine_ReleaseTexture(nDevice, pUnitDymHalBuffer->m_hTex);
		memset(pUnitDymHalBuffer, 0, sizeof(DXHalDef32));
	}
	pUnitDymHalBuffer = nullptr;
	bUseColorAlpha = FALSE;
}
