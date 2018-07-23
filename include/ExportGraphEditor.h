#pragma once
#ifdef BASEGRAPH_GRAPHEDITOR_EXPORTS
#define BASEGRAPH_GRAPHEDITOR_API __declspec(dllexport)
#else
#define BASEGRAPH_GRAPHEDITOR_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "GraphEditorD.lib")
#else
#	pragma comment(lib, "GraphEditor.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_GRAPHEDITOR_EXPORTS

BASEGRAPH_GRAPHEDITOR_API bool GraphEditor_Initialize();
BASEGRAPH_GRAPHEDITOR_API bool GraphEditor_Release();
BASEGRAPH_GRAPHEDITOR_API CFrameWnd * GraphEditor_GetMainFrame();