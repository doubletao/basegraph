// DataRender.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "DataRender.h"
#include "Typeset\TextTypesetDataAPI.h"

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

// CDataRenderApp

BEGIN_MESSAGE_MAP(CDataRenderApp, CWinApp)
END_MESSAGE_MAP()


// CDataRenderApp 构造

CDataRenderApp::CDataRenderApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDataRenderApp 对象

CDataRenderApp theApp;


// CDataRenderApp 初始化

BOOL CDataRenderApp::InitInstance()
{
	CWinApp::InitInstance();
	//初始化typeset
	SYTypeset_InitInterfaceEnv();
	//初始化栅格化表
	rip_init_table();

	return TRUE;
}

int CDataRenderApp::ExitInstance()
{
	//释放栅格化表
	rip_release_table();
	int nRet = CWinApp::ExitInstance();
	return nRet;
}
