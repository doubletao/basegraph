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
public:
	virtual CString GetProjectInfo() = 0;//获取项目信息（XML形式的完整项目信息）
	virtual bool ChangeProjectCfg(CString strProInfo) = 0;//修改项目信息
	virtual int GetFrameLength() = 0;
public:
	virtual CString GetContent() = 0;//拿到内容信息（完整XML信息；从设计上来说，虽然XML可以完整的恢复一个字幕，但不对外暴露，只能先打开再通过XML加载）
	virtual bool ChangeContent(CString strContent) = 0;//修改内容信息
public:
	virtual bool GetDynamicFrame(int nFrame, DWORD * pBufferAddr) = 0;
	virtual bool GetStaticFrame(DWORD * pBufferAddr) = 0;
};

BASEGRAPH_INTERFACE_API bool Interface_InitialEnv();
BASEGRAPH_INTERFACE_API bool Interface_ReleaseEnv();
BASEGRAPH_INTERFACE_API bool Interface_CreateDecoder(IBaseGraphDecoder *& pDecoder);
BASEGRAPH_INTERFACE_API bool Interface_ReleaseDecoder(IBaseGraphDecoder *& pDecoder);
