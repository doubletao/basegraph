#pragma once
#ifdef BASEGRAPH_DATABASE_EXPORTS
#define BASEGRAPH_DATABASE_API __declspec(dllexport)
#else
#define BASEGRAPH_DATABASE_API __declspec(dllimport)
#include "DataBase\DataTypeDef.h"
#endif // BASEGRAPH_COMMON_EXPORTS

#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "DataBaseD.lib")
#else
#	pragma comment(lib, "DataBase.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
