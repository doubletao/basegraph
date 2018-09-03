#pragma once
#include <vector>

#ifdef BASEGRAPH_COMMON_EXPORTS
#define BASEGRAPH_COMMON_API __declspec(dllexport)
#else
#define BASEGRAPH_COMMON_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "CommonD.lib")
#else
#	pragma comment(lib, "Common.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_COMMON_EXPORTS

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)                            { if(p) p->Release(); p = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)								{ if(p) delete p; p = NULL;}
#endif

#ifndef SAFEDELETE_ARRAY
#define SAFEDELETE_ARRAY(p)						{ if(p) delete []p; p = NULL;}
#endif

//开启tinyxml的stl支持
#define TIXML_USE_STL
#include <string>
#include "Common\DataDef.h"
#include "Common\CommonDataContainer.h"
#include "Common\CommonFunction.h"
#include "Common\BufferCommonDef.h"
#include "Common\BufferFunction.h"
#include "Common\ResourceLock.h"
#include "Common\tinyxml.h"
#include "Common\MathDef.h"
#include "Common\MatrixDef.h"
#include "Common\PlaneDef.h"
#include "Common\QuatnDef.h"
#include "Common\VectorDef.h"
#include "Common\2DBuffer.h"
#include "Common\2DBufferApi.h"
#include "Common\ColorDef.h"
#include "Common\ColorAPI.h"
#include "Common\BezierDef.h"
#include "Common\CommonUtility.h"
#include "Common\vectorgraph.h"
#include "Common\2DBufferDC.h"
#include "Common\ThreadPool.h"
#include "Common\ColorDef.h"
#include "Common\ColorAPI.h"

BASEGRAPH_COMMON_API bool Common_IsPointInPolygon(IN pointf ptHit, IN const std::vector<pointf> & vecPolygon);
BASEGRAPH_COMMON_API bool Common_IsPointInPolygon(IN pointf ptHit, IN rectf rtRect);
BASEGRAPH_COMMON_API bool Common_IsPointInPolygon(IN float fX, IN float fY, IN const std::vector<pointf> & vecPolygon);
BASEGRAPH_COMMON_API bool Common_IsCrossLine(IN pointf ptLine11, IN pointf ptLine12, IN pointf ptLine21, IN pointf ptLine22);
