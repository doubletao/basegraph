// stdafx.cpp : 只包括标准包含文件的源文件
// GraphEditor.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#ifdef _DEBUG
LPCTSTR g_ModuleName = _T("GraphEditorD.lib");
#else
LPCTSTR g_ModuleName = _T("GraphEditor.lib");
#endif
