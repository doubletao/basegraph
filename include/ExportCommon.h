#pragma once

#ifdef BASEGRAPH_COMMON_EXPORTS
#define BASEGRAPH_COMMON_API __declspec(dllexport)
#else
#define BASEGRAPH_COMMON_API __declspec(dllimport)
#include "DataDef.h"
#endif // BASEGRAPH_COMMON_EXPORTS

#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "CommonD.lib")
#else
#	pragma comment(lib, "Common.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
