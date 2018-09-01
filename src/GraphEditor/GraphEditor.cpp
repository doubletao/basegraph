// GraphEditor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "GraphEditor.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CGraphEditorApp


// CGraphEditorApp 构造

BaseGraphMainFrame * CGraphEditorApp::m_pMainFrame = nullptr;

CGraphEditorApp::CGraphEditorApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_pMainFrame = new BaseGraphMainFrame;
}

CGraphEditorApp::~CGraphEditorApp()
{
}


// CGraphEditorApp 初始化

BOOL CGraphEditorApp::InitInstance()
{
	//初始化3D环境
	CString strBinPath = CommonFunction::GetBinPath();
	DX3DEngine_Init(DX_DX10_Device, strBinPath + _T("\\FX\\"));
	//初始化材质
	BaseGraphMaterialModuleInit(strBinPath + _T("\\FX\\"));
	//初始化主device
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	//创建的宽高也不能太小
	if (nScreenWidth < 1440)
	{
		nScreenWidth = 1440;
	}
	if (nScreenHeight < 900)
	{
		nScreenHeight = 900;
	}
	DX3DEngine_Initialize3DDevice(nScreenWidth, nScreenHeight);

	if (nullptr != m_pMainFrame)
	{
		BOOL bRet = m_pMainFrame->Create(nullptr, _T("BaseGraphEditor"));
		if (!bRet)
		{
			return FALSE;
		}
	}

	return TRUE;
}

int CGraphEditorApp::ExitInstance()
{
	int nRet = TRUE;
	//释放材质
	BaseGraphMaterialModuleRelease();
	//释放主device
	DX3DEngine_ReleaseMainDevice();
	//释放3D环境
	DX3DEngine_Release();
	return nRet;
}

BaseGraphMainFrame * CGraphEditorApp::GetBaseGraphFrameWnd()
{
	return m_pMainFrame;
}
