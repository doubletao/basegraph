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
	return CalculatorManager::Instance().GetFrame();
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

BASEGRAPH_DATARENDER_API bool DataRender_GetOriBound(IN CString strObjID, OUT rectf & rtBoundf)
{
	return CalculatorManager::Instance().GetOriBound(strObjID, rtBoundf);
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

BASEGRAPH_DATARENDER_API bool DataRender_OffsetObject(IN const std::vector<CString>& vecStrObjID, IN float fXOffset, IN float fYOffset, IN float fZOffset)
{
	bool bRet = false;
	for (auto it : vecStrObjID)
	{
		//对每个物件取坐标，然后做相应的offset，再设置回去
		Value stuParamX;
		Value stuParamY;
		Value stuParamZ;
		if (DataRender_GetItemProperty(it, BaseGraph_DataBase::SpacePosition_X_Float, stuParamX)
			&& DataRender_GetItemProperty(it, BaseGraph_DataBase::SpacePosition_Y_Float, stuParamY)
			&& DataRender_GetItemProperty(it, BaseGraph_DataBase::SpacePosition_Z_Float, stuParamZ))
		{
			float fX = stuParamX.ToFloat();
			float fY = stuParamY.ToFloat();
			float fZ = stuParamZ.ToFloat();
			stuParamX.SetValue(fX + fXOffset);
			stuParamY.SetValue(fY + fYOffset);
			stuParamZ.SetValue(fZ + fZOffset);
			if (DataRender_SetItemProperty(it, BaseGraph_DataBase::SpacePosition_X_Float, stuParamX)
				&& DataRender_SetItemProperty(it, BaseGraph_DataBase::SpacePosition_Y_Float, stuParamY)
				&& DataRender_SetItemProperty(it, BaseGraph_DataBase::SpacePosition_Z_Float, stuParamZ))
			{
				bRet = true;
			}
		}
	}
	return bRet;
}

BASEGRAPH_DATARENDER_API bool DataRender_OffsetSelObject(IN float fXOffset, IN float fYOffset, IN float fZOffset)
{
	std::vector<CString> vecSelObjID;
	if (DataRender_GetSelectObj(vecSelObjID))
	{
		return DataRender_OffsetObject(vecSelObjID, fXOffset, fYOffset, fZOffset);
	}
	return false;
}

BASEGRAPH_DATARENDER_API bool DataRender_GetSceneBound(IN CString strObjID, OUT rectf & rtBoundf)
{
	return CalculatorManager::Instance().GetSceneBound(strObjID, rtBoundf);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetCtrlPoint(IN CString strObjID, OUT std::vector<pointf>& vecCtrlPoint)
{
	return CalculatorManager::Instance().GetCtrlPoint(strObjID, vecCtrlPoint);
}

BASEGRAPH_DATARENDER_API EmHittestRetType DataRender_HitTest(IN pointf ptHit, OPTIONAL CString & strID, OPTIONAL EmCtrlPointIndex & emCtrlPoint)
{
	return CalculatorManager::Instance().HitTest(ptHit, strID, emCtrlPoint);
}

//框选，入选的条件是碰着就算
BASEGRAPH_DATARENDER_API bool DataRender_BoxHitTest(IN rectf rtBox, OUT std::vector<CString>& vecStrObjID)
{
	return CalculatorManager::Instance().BoxHitTest(rtBox, vecStrObjID);
}

//框选，入选的条件是框住了才算
BASEGRAPH_DATARENDER_API bool DataRender_BoxInterTest(IN rectf rtBox, OUT std::vector<CString>& vecStrObjID)
{
	return CalculatorManager::Instance().BoxInterTest(rtBox, vecStrObjID);
}

