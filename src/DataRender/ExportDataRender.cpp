#include "stdafx.h"
#include "ExportDataRender.h"
#include "CalculatorManager.h"
#include "DataBufferManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
BASEGRAPH_DATARENDER_API bool DataRender_UpdateRender(IN CString strID)
{
	return CalculatorManager::Instance().UpdateRenderBuffer(strID);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetBuffer(IN CString strID, OUT StuSimpleBuffer & stuBuf)
{
	return CalculatorManager::Instance().GetBuffer(strID, stuBuf);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetTexture(IN CString strID, OUT int & nTexture)
{
	nTexture = CalculatorManager::Instance().GetTexture(strID);
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_GetDesHalDef(IN CString strID, OUT DXHalDef32 *& pHalDef)
{
	pHalDef = CalculatorManager::Instance().GetDesHalDef(strID);
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_Get2DRenderMatrix(IN CString strID, OUT matrix4f & matWorld)
{
	matWorld = CalculatorManager::Instance().Get2DRenderMatrix(strID);
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_Query2DBuffer(IN int nWidth, IN int nHeight, OUT int & nBufID)
{
	nBufID = DataBufferManager::Instance().Query2DBuffer(nWidth, nHeight);
	if (nBufID > 0)
	{
		return true;
	}
	return false;
}

BASEGRAPH_DATARENDER_API bool DataRender_Query2DBuffer(OPTIONAL int & nBufferID, IN int nWidth, IN int nHeight, OUT StuSimpleBuffer & stuBuf)
{
	ObjectBuffer * pObjBuffer = DataBufferManager::Instance().Query2DBuffer(nBufferID, nWidth, nHeight);
	if (pObjBuffer)
	{
		stuBuf.pBuffer = pObjBuffer->GetBuffer();
		stuBuf.nWidth = pObjBuffer->GetWidth();
		stuBuf.nHeight = pObjBuffer->GetHeight();
		return true;
	}
	return false;
}

BASEGRAPH_DATARENDER_API bool DataRender_UnQuery2DBuffer(OPTIONAL int & nBufferID)
{
	return DataBufferManager::Instance().UnQuery2DBuffer(nBufferID);
}

BASEGRAPH_DATARENDER_API bool DataRender_Compress2DBuffer()
{
	return DataBufferManager::Instance().Compress2DBuffer();
}

BASEGRAPH_DATARENDER_API bool DataRender_Get2DBuf(IN int nBufferID, OUT StuSimpleBuffer & stuBuf)
{
	ObjectBuffer * pObjBuffer = DataBufferManager::Instance().Get2DBuf(nBufferID);
	if (pObjBuffer)
	{
		stuBuf.pBuffer = pObjBuffer->GetBuffer();
		stuBuf.nWidth = pObjBuffer->GetWidth();
		stuBuf.nHeight = pObjBuffer->GetHeight();
		return true;
	}
	return false;
}

BASEGRAPH_DATARENDER_API bool DataRender_Clear2DBuf(IN int nBufferID, IN COLORREF color)
{
	return DataBufferManager::Instance().Clear(nBufferID, color);
}

BASEGRAPH_DATARENDER_API bool DataRender_DebugOutPut2DBuffer(IN int nBufferID, IN CString strPath)
{
	ObjectBuffer * pObjBuffer = DataBufferManager::Instance().Get2DBuf(nBufferID);
	if (pObjBuffer)
	{
		pObjBuffer->DebugWriteImage(strPath);
		return true;
	}
	return false;
}

BASEGRAPH_DATARENDER_API bool DataRender_IsItemSelected(IN CString strID, OUT bool & bSel)
{
	return CalculatorManager::Instance().IsItemSelected(strID, bSel);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetSelectObj(OUT std::vector<CString> & vecSelObjID)
{
	return CalculatorManager::Instance().GetSelectObj(vecSelObjID);
}

BASEGRAPH_DATARENDER_API bool DataRender_ClearAllSelect()
{
	return CalculatorManager::Instance().ClearAllSelect();
}

BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, OPTIONAL bool & bSel)
{
	return CalculatorManager::Instance().SetItemSelected(strID, bSel);
}

BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, IN const bool &  bSel)
{
	return CalculatorManager::Instance().SetItemSelected(strID, bSel);
}

BASEGRAPH_DATARENDER_API int DataRender_GetCurFrame()
{
	//暂不实现
	return 0;
}

BASEGRAPH_DATARENDER_API bool DataRender_SetFrame(const int & nFrame)
{
	CalculatorManager::Instance().SetFrame(nFrame);
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_SetScale(const float & fScale)
{
	CalculatorManager::Instance().SetScale(fScale);
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_GetCtrlBound(IN CString strObjID, OUT rectf & rtBoundf)
{
	return CalculatorManager::Instance().GetCtrlBound(strObjID, rtBoundf);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam)
{
	//暂时还没有引入关键帧，先直接从database层调
	return DataBase_GetItemProperty(strID, emParam, stuParam);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetItemProperty(IN CString strID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	return DataBase_GetItemProperty(strID, mapParam);
}

BASEGRAPH_DATARENDER_API bool DataRender_SetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam)
{
	CalculatorManager::Instance().AddChgLevel(strID, emParam);
	return DataBase_SetItemProperty(strID, emParam, stuParam);
}

BASEGRAPH_DATARENDER_API bool DataRender_SetItemProperty(IN CString strID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	for (auto it : mapParam)
	{
		CalculatorManager::Instance().AddChgLevel(strID, it.first);
	}
	return DataBase_SetItemProperty(strID, mapParam);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetSceneBound(IN CString strObjID, OUT rectf & rtBoundf)
{
	return CalculatorManager::Instance().GetSceneBound(strObjID, rtBoundf);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetCtrlPoint(IN CString strObjID, OUT std::vector<pointf>& vecCtrlPoint)
{
	return CalculatorManager::Instance().GetCtrlPoint(strObjID, vecCtrlPoint);
}

//////////////////////////////////////////////////////////////////////////
2018年5月14日这里搞错了，这三个接口目的是通过点或者选择框来找到选中的物件，而不是判断指定物件是否被选中
这里边需要搞一套物件是否被选中或者被框中的接口，然后这三个接口仅仅遍历判定是否入选
BASEGRAPH_DATARENDER_API bool DataRender_HitTest(IN pointf ptHit, OPTIONAL CString & strID)
{
	//先拿到控制点矩形
	std::vector<pointf> vecCtrlPoint;
	if (CalculatorManager::Instance().GetCtrlPoint(strID, vecCtrlPoint))
	{
		return Common_IsPointInPolygon(ptHit, vecCtrlPoint);
	}
	return false;
}

//框选，入选的条件是碰着就算
BASEGRAPH_DATARENDER_API bool DataRender_BoxHitTest(IN rectf rtBox, OUT std::vector<CString>& vecStrObjID)
{
	return BASEGRAPH_DATARENDER_API bool();
}

//框选，入选的条件是框住了才算
BASEGRAPH_DATARENDER_API bool DataRender_BoxInterTest(IN rectf rtBox, OUT std::vector<CString>& vecStrObjID)
{
	//先拿到控制点矩形
	std::vector<pointf> vecCtrlPoint;
	if (CalculatorManager::Instance().GetCtrlPoint(strID, vecCtrlPoint))
	{
		for (size_t i = 0; i < vecCtrlPoint; i++)
		{
			//如果控制点都在矩形内，则就是框住了
			if (!SYRectf_PtInRect(&rtBox, vecCtrlPoint[i]))
			{
				//有一个不在里边就算是外边
				return false;
			}
		}
		return true;
	}
	return false;
}

