#include "stdafx.h"
#include "ExportGraphEditor.h"
#include "GraphEditor.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CGraphEditorApp * g_pEditorApp = nullptr;

BASEGRAPH_GRAPHEDITOR_API bool GraphEditor_Initialize()
{
	bool bRet = false;
	if (nullptr == g_pEditorApp)
	{
		g_pEditorApp = new CGraphEditorApp;
		if (nullptr != g_pEditorApp)
		{
			g_pEditorApp->InitInstance();
		}
	}
	if (nullptr != g_pEditorApp)
	{
		bRet = true;
	}
	return bRet;
}

BASEGRAPH_GRAPHEDITOR_API bool GraphEditor_Release()
{
	bool bRet = false;
	if (nullptr != g_pEditorApp)
	{
		g_pEditorApp->ExitInstance();
		delete g_pEditorApp;
		g_pEditorApp = nullptr;
	}
	if (nullptr == g_pEditorApp)
	{
		bRet = true;
	}
	return bRet;
}

BASEGRAPH_GRAPHEDITOR_API CFrameWnd * GraphEditor_GetMainFrame()
{
	if (nullptr != g_pEditorApp)
	{
		return g_pEditorApp->GetBaseGraphFrameWnd();
	}
	return nullptr;
}
