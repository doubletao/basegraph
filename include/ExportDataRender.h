#pragma once
#ifdef BASEGRAPH_DATARENDER_EXPORTS
#define BASEGRAPH_DATARENDER_API __declspec(dllexport)
#else
#define BASEGRAPH_DATARENDER_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "DataRenderD.lib")
#else
#	pragma comment(lib, "DataRender.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_DATARENDER_EXPORTS

BASEGRAPH_DATARENDER_API bool DataRender_UpdateRender(IN CString strID);
//注意，拿到的是内部指针，不要释放
BASEGRAPH_DATARENDER_API bool DataRender_GetBuffer(IN CString strID, OUT byte *& pBuffer, OUT int & nWidth, OUT int & nHeight);
BASEGRAPH_DATARENDER_API bool DataRender_IsItemSelected(IN CString strID, OUT bool & bSel);
BASEGRAPH_DATARENDER_API bool DataRender_ClearAllSelect();
BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, OPTIONAL bool & bSel);//bSel会返回之前的状态
BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, IN const bool & bSel);
