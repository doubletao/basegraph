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
#include "DataRender\SimpleSpaceConvert.h"

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
BASEGRAPH_DATARENDER_API int DataRender_GetCurField();//注意，单位是场
BASEGRAPH_DATARENDER_API bool DataRender_SetField(const int & nField);//注意，单位是场
BASEGRAPH_DATARENDER_API bool DataRender_SetScale(const float & fScale);
BASEGRAPH_DATARENDER_API bool DataRender_GetOriBound(IN CString strObjID, OUT rectf & rtBoundf);//获取物件原始bound（不考虑旋转缩放等因素）
BASEGRAPH_DATARENDER_API bool DataRender_IsItemNeedRender(IN CString strObjID);
BASEGRAPH_DATARENDER_API void DataRender_SetStaticMode(bool bStatic);
BASEGRAPH_DATARENDER_API bool DataRender_IsStaticMode();
BASEGRAPH_DATARENDER_API CString DataRender_GetCurParentObjID();
BASEGRAPH_DATARENDER_API void DataRender_SetCurParentObjID(CString strParentObjID);
BASEGRAPH_DATARENDER_API bool DataRender_DeleteSelObj();//删除当前选中物件
//datarender层面的属性获取与设置是考虑了关键帧之后，注意与database层区分
//属性获取
BASEGRAPH_DATARENDER_API bool DataRender_GetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam);
BASEGRAPH_DATARENDER_API bool DataRender_GetItemProperty(IN CString strID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam);
//属性设置
BASEGRAPH_DATARENDER_API bool DataRender_SetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam);
BASEGRAPH_DATARENDER_API bool DataRender_SetItemProperty(IN CString strID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam);
//物件移动动作
BASEGRAPH_DATARENDER_API bool DataRender_OffsetObject(IN const std::vector<CString> & vecStrObjID, IN float fXOffset, IN float fYOffset, IN float fZOffset);
BASEGRAPH_DATARENDER_API bool DataRender_OffsetSelObject(IN float fXOffset, IN float fYOffset, IN float fZOffset);
//这里的scene bound相对场景坐标系(场景中心为原点，上为Y轴正向，右为X轴正向)
BASEGRAPH_DATARENDER_API bool DataRender_GetSceneBound(IN CString strObjID, OUT rectf & rtBoundf);//获取物件场景bound
BASEGRAPH_DATARENDER_API bool DataRender_GetCtrlPoint(IN CString strObjID, OUT std::vector<pointf> & vecCtrlPoint);//获取物件场景控制点
//到这个层面的hittest，参数进来的点都是场景点(场景中心为原点，上为Y轴正向，右为X轴正向)
BASEGRAPH_DATARENDER_API EmHittestRetType DataRender_HitTest(IN pointf ptHit, OPTIONAL CString & strID, OPTIONAL EmCtrlPointIndex & emCtrlPoint);//根据一次单击查找选中块(),strID为空则选中最上层，否则会查找指定点的重叠物件中指定块的下一个物件
BASEGRAPH_DATARENDER_API bool DataRender_BoxHitTest(IN rectf rtBox, OUT std::vector<CString> & vecStrObjID);//框选，入选的条件是碰着就算
BASEGRAPH_DATARENDER_API bool DataRender_BoxInterTest(IN rectf rtBox, OUT std::vector<CString> & vecStrObjID);//框选，入选的条件是框住了才算
