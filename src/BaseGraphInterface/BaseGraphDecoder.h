#pragma once
#include "BaseGraphDecoderAPI.h"
#include "ExportCommon.h"

class BaseGraphDecoder :
	public IBaseGraphDecoder
{
public:
	BaseGraphDecoder();
	virtual ~BaseGraphDecoder();
protected:
	int m_nWidth;
	int m_nHeight;
	ThreadPool m_ThreadPool;//物件自我渲染的线程池
protected:
	void RenderAllItemToTarget(std::vector<CString> vecObjID, int hDevice, int hTex, int hDepth, float fScale, RECT &viewPort);//将每个物件的贴片贴到场景上
	void UpdateAllItemToHal(std::vector<CString> vecObjID, float fScale);//生成物件贴片
	static DWORD UpdateItemToHalFunc(LPVOID pParam, HANDLE hEnd);//生成物件贴片线程
public:
	virtual bool OpenProject(CString strProjFile);//打开项目后就可以读取和修改项目基本信息及物件属性了
	virtual bool LoadProject();//加载项目才执行加载渲染环境等工作
public:
	virtual CString GetProjectInfo();//获取项目信息（XML形式的完整项目信息）
	virtual bool ChangeProjectCfg(CString strProInfo);//修改项目信息
	virtual int GetFrameLength();
public:
	virtual CString GetContent();//拿到内容信息（完整XML信息；从设计上来说，虽然XML可以完整的恢复一个字幕，但不对外暴露，只能先打开再通过XML加载）
	virtual bool ChangeContent(CString strContent);//修改内容信息
public:
	virtual bool GetDynamicFrame(int nFrame, DWORD * pBufferAddr);
	virtual bool GetStaticFrame(DWORD * pBufferAddr);
};

