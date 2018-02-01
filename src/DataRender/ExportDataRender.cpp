#include "stdafx.h"
#include "ExportDataRender.h"
#include "CalculatorManager.h"

BASEGRAPH_DATARENDER_API bool DataRender_UpdateRender(IN CString strID)
{
	return CalculatorManager::Instance().UpdateRenderBuffer(strID);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetBuffer(IN CString strID, OUT byte *& pBuffer)
{
	return CalculatorManager::Instance().GetBuffer(strID, pBuffer);
}
