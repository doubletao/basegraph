#include "stdafx.h"
#include "ExportDataRender.h"
#include "CalculatorManager.h"

BASEGRAPH_DATARENDER_API bool DataRender_UpdateRender(IN CString strID)
{
	return CalculatorManager::Instance().UpdateRenderBuffer(strID);
}

BASEGRAPH_DATARENDER_API bool DataRender_GetBuffer(IN CString strID, OUT byte *& pBuffer, OUT int & nWidth, OUT int & nHeight)
{
	return CalculatorManager::Instance().GetBuffer(strID, pBuffer, nWidth, nHeight);
}

BASEGRAPH_DATARENDER_API bool DataRender_IsItemSelected(IN CString strID, OUT bool & bSel)
{
	return CalculatorManager::Instance().IsItemSelected(strID, bSel);
}

BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, OPTIONAL bool & bSel)
{
	return CalculatorManager::Instance().SetItemSelected(strID, bSel);
}

BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, IN const bool &  bSel)
{
	return CalculatorManager::Instance().SetItemSelected(strID, bSel);
}
