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

#include "ExportCommon.h"
#include "ExportSceneRender.h"
#include "ExportDataBase.h"

#include "DataRender\3DGeometry.h"
#include "DataRender\3DPieModeling.h"
#include "DataRender\AlgorithmRip.h"
#include "DataRender\Arc2BezierAPI.h"
#include "DataRender\BevelPoly.h"
#include "DataRender\DataRenderDataDef.h"
#include "DataRender\DataRenderUtityFunc.h"
#include "DataRender\DrawTextDef.h"
#include "DataRender\LineAPI.h"
#include "DataRender\MaterialDef.h"
#include "DataRender\OffsetCurve.h"
#include "DataRender\RipRender.h"
#include "DataRender\ShapeForm.h"
#include "DataRender\TextDrawer.h"
#include "DataRender\TriangulateApp.h"

struct StuSimpleBuffer
{
	byte * pBuffer;
	int nWidth;
	int nHeight;
	StuSimpleBuffer()
		: pBuffer(nullptr)
		, nWidth(0)
		, nHeight(0)
	{
	}
};

BASEGRAPH_DATARENDER_API bool DataRender_UpdateRender(IN CString strID);
//BufferManager相关操作，注意，拿到的是内部指针，不要释放
BASEGRAPH_DATARENDER_API bool DataRender_GetBuffer(IN CString strID, OUT StuSimpleBuffer & stuBuf);
BASEGRAPH_DATARENDER_API bool DataRender_GetTexture(IN CString strID, OUT int & nTexture);
BASEGRAPH_DATARENDER_API bool DataRender_GetDesHalDef(IN CString strID, OUT DXHalDef32 *& pHalDef);
BASEGRAPH_DATARENDER_API bool DataRender_Get2DRenderMatrix(IN CString strID, OUT matrix4f & matWorld);
BASEGRAPH_DATARENDER_API bool DataRender_Query2DBuffer(IN int nWidth, IN int nHeight, OUT int & nBufID);//取一个2Dbuf
BASEGRAPH_DATARENDER_API bool DataRender_Query2DBuffer(OPTIONAL int & nBufferID, IN int nWidth, IN int nHeight, OUT StuSimpleBuffer & stuBuf);//取一个2Dbuf
BASEGRAPH_DATARENDER_API bool DataRender_UnQuery2DBuffer(OPTIONAL int & nBufferID);//释放一个2Dbuf的使用权（不释放）
BASEGRAPH_DATARENDER_API bool DataRender_Compress2DBuffer();//压缩2Dbuf（释放掉所有未使用的bufer）
BASEGRAPH_DATARENDER_API bool DataRender_Get2DBuf(IN int nBufferID, OUT StuSimpleBuffer & stuBuf);
BASEGRAPH_DATARENDER_API bool DataRender_Clear2DBuf(IN int nBufferID, IN COLORREF color);
BASEGRAPH_DATARENDER_API bool DataRender_DebugOutPut2DBuffer(IN int nBufferID, IN CString strPath);
BASEGRAPH_DATARENDER_API bool DataRender_IsItemSelected(IN CString strID, OUT bool & bSel);
BASEGRAPH_DATARENDER_API bool DataRender_GetSelectObj(OUT std::vector<CString> & vecSelObjID);
BASEGRAPH_DATARENDER_API bool DataRender_ClearAllSelect();
BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, OPTIONAL bool & bSel);//bSel会返回之前的状态
BASEGRAPH_DATARENDER_API bool DataRender_SetItemSelected(IN CString strID, IN const bool & bSel);
BASEGRAPH_DATARENDER_API int DataRender_GetCurFrame();//注意，单位是场
BASEGRAPH_DATARENDER_API bool DataRender_SetFrame(const int & nFrame);//注意，单位是场
BASEGRAPH_DATARENDER_API bool DataRender_SetScale(const float & fScale);
BASEGRAPH_DATARENDER_API bool DataRender_GetCtrlBound(IN CString strObjID, OUT rectf & rtBoundf);//获取物件原始bound
//这里的scene bound相对场景坐标系(场景中心为原点，上为Y轴正向，右为X轴正向)
BASEGRAPH_DATARENDER_API bool DataRender_GetSceneBound(IN CString strObjID, OUT rectf & rtBoundf);//获取物件原始bound
BASEGRAPH_DATARENDER_API bool DataRender_GetCtrlPoint(IN CString strObjID, OUT std::vector<point3f> & vecCtrlPoint);//获取物件原始bound
//到这个层面的hittest，参数进来的点都是场景点(场景中心为原点，上为Y轴正向，右为X轴正向)
BASEGRAPH_DATARENDER_API bool DataRender_HitTest(IN CPoint ptHit, OPTIONAL CString & strID);//根据一次单击查找选中块(),strID为空则选中最上层，否则会查找指定点的重叠物件中指定块的下一个物件
BASEGRAPH_DATARENDER_API bool DataRender_BoxHitTest(IN CRect rtBox, OUT std::vector<CString> & vecStrObjID);//框选，入选的条件是碰着就算
BASEGRAPH_DATARENDER_API bool DataRender_BoxSelectTest(IN CRect rtBox, OUT std::vector<CString> & vecStrObjID);//框选，入选的条件是框住了才算
