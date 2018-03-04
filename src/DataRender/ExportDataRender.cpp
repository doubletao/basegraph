#include "stdafx.h"
#include "ExportDataRender.h"
#include "CalculatorManager.h"
#include "DataBufferManager.h"

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
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_GetDesHalDef(IN CString strID, OUT DXHalDef32 *& pHalDef)
{
	return true;
}

BASEGRAPH_DATARENDER_API bool DataRender_Get2DRenderMatrix(IN CString strID, IN float fScale, OUT matrix4f & matWorld)
{
	return true;
}

BASEGRAPH_DATARENDER_API bool Query2DBuffer(IN int nWidth, IN int nHeight, OUT int nBufID)
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

BASEGRAPH_DATARENDER_API bool DataRender_IsItemSelected(IN CString strID, OUT bool & bSel)
{
	return CalculatorManager::Instance().IsItemSelected(strID, bSel);
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
	//暂不实现
	return false;
}
