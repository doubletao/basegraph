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

#include "Common\DataDef.h"
#include "Common\CommonMatrix.h"
#include "Common\CommonFunction.h"
#include "Common\BufferCommonDef.h"
#include "Common\BufferFunction.h"
