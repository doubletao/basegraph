#pragma once

#ifdef BASEGRAPH_INTERFACE_EXPORTS
#define BASEGRAPH_INTERFACE_API __declspec(dllexport)
#else
#define BASEGRAPH_INTERFACE_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "BaseGraphInterfaceD.lib")
#else
#	pragma comment(lib, "BaseGraphInterface.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_INTERFACE_EXPORTS

class IBaseGraphDecoder
{
public:
	virtual bool OpenProject(CString strProjFile) = 0;//打开项目后就可以读取和修改项目基本信息及物件属性了
	virtual bool LoadProject() = 0;//加载项目才执行加载渲染环境等工作
	virtual CString GetProjectInfo() = 0;//获取项目信息
	virtual bool ChangeProjectCfg(CString strProInfo) = 0;//修改项目信息
	virtual CString GetContent() = 0;//拿到内容信息
	virtual bool ChangeContent(CString strContent) = 0;//修改内容信息
};

BASEGRAPH_INTERFACE_API bool Interface_InitialEnv();
BASEGRAPH_INTERFACE_API bool Interface_ReleaseEnv();
BASEGRAPH_INTERFACE_API bool Interface_CreateDecoder();
BASEGRAPH_INTERFACE_API bool Interface_ReleaseDecoder();
