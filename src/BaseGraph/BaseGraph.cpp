
// BaseGraph.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "BaseGraph.h"
#include "BaseGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBaseGraphApp

BEGIN_MESSAGE_MAP(CBaseGraphApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBaseGraphApp 构造

CBaseGraphApp::CBaseGraphApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBaseGraphApp 对象

CBaseGraphApp theApp;


// CBaseGraphApp 初始化

BOOL CBaseGraphApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	if (GraphEditor_Initialize())
	{
		CFrameWnd* pFrame = GraphEditor_GetMainFrame();
		if (!pFrame)
			return FALSE;
		m_pMainWnd = pFrame;

		// 唯一的一个窗口已初始化，因此显示它并对其进行更新
		pFrame->ShowWindow(SHOW_FULLSCREEN);
		pFrame->UpdateWindow();
		return TRUE;
	}
	else
	{
		return FALSE;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CBaseGraphApp::ExitInstance()
{
	int nRet = CWinApp::ExitInstance();
	if (GraphEditor_Release())
	{
		nRet = 0;
	}
	return nRet;
}

