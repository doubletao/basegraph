#pragma once

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

#ifndef SAFERELEASE
#define SAFERELEASE(p)                            { if(p) p->Release(); p = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)								{ if(p) delete p; p = NULL;}
#endif

#ifndef SAFEDELETE_ARRAY
#define SAFEDELETE_ARRAY(p)						{ if(p) delete []p; p = NULL;}
#endif

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
#include "Common\CommonSimpleBuffer.h"
