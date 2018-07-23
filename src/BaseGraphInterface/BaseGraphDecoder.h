#pragma once
#include "BaseGraphDecoderAPI.h"

class BaseGraphDecoder :
	public IBaseGraphDecoder
{
public:
	BaseGraphDecoder();
	virtual ~BaseGraphDecoder();
public:
	virtual bool OpenProject(CString strProjFile) = 0;//打开项目后就可以读取和修改项目基本信息及物件属性了
	virtual bool LoadProject() = 0;//加载项目才执行加载渲染环境等工作
	virtual CString GetProjectInfo() = 0;//获取项目信息
	virtual bool ChangeProjectCfg(CString strProInfo) = 0;//修改项目信息
	virtual CString GetContent() = 0;//拿到内容信息
	virtual bool ChangeContent(CString strContent) = 0;//修改内容信息
};

