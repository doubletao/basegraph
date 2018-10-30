#include "stdafx.h"
#include "BaseGraphDecoder.h"
#include <vector>
#include "ExportDataBase.h"
#include "ExportDataRender.h"
#include "ExportSceneRender.h"

struct DataBaseLocker
{
	DataBaseLocker()
	{
		DataBase_LockData();
	}
	~DataBaseLocker()
	{
		DataBase_UnlockData();
	}
};

struct DXEngineLocker
{
	int m_nDevice;
	DXEngineLocker(int nDevice)
		: m_nDevice(nDevice)
	{
		DX3DEngine_BeginRender(nDevice);
	}
	~DXEngineLocker()
	{
		DX3DEngine_EndRender(m_nDevice);
	}
};

BaseGraphDecoder::BaseGraphDecoder()
	: m_nWidth(1920)
	, m_nHeight(1080)
{
}


BaseGraphDecoder::~BaseGraphDecoder()
{
}

void BaseGraphDecoder::RenderAllItemToTarget(std::vector<CString> vecObjID, int hDevice, int hTex, int hDepth, float fScale, RECT & viewPort)
{
	//如果没有物件，则清屏
	if (vecObjID.size() < 1)
	{
		DX3DEngine_ClearTexture(hDevice, hTex, RGBA(255, 125, 125, 255));
		return;
	}

	//遍历物件，取出其矩阵，然后贴上去
	for (size_t i = 0; i < vecObjID.size(); i++)
	{
		CString strObjID = vecObjID[i];
		DXHalDef32 * pHalDef = nullptr;
		matrix4f worldMat;
		if (DataRender_IsItemNeedRender(strObjID))
		{
			if (DataRender_Get2DRenderMatrix(strObjID, worldMat))
			{
				if (DataRender_GetDesHalDef(strObjID, pHalDef))
				{
					if (pHalDef)
					{
						//渲染bufer到3D
						DX3DEngine_RenderTexture(hDevice, pHalDef, &worldMat, &viewPort, FALSE, 1.0f);
						if (false)
						{
							DX3DEngine_DumpTexture(hDevice, pHalDef->m_hTex, _T("e:\\test.bmp"));
							DX3DEngine_DumpRenderTarget(hDevice);
						}
					}
				}
			}
		}
	}
}

void BaseGraphDecoder::UpdateAllItemToHal(std::vector<CString> vecObjID, float fScale)
{
	//设置缩放
	DataRender_SetScale(fScale);
	//设置帧号SetFrame
	int nCurFrame = DataRender_GetCurField();
	DataRender_SetField(nCurFrame);
	//遍历并生成物件贴片
	for (size_t i = 0; i < vecObjID.size(); i++)
	{
		CString * pStrObjID = new CString(vecObjID[i]);
		//是否启动多线程，目前单线程仅用于调试
		if (true)
		//if (false)
		{
			m_ThreadPool.StartNewTask(UpdateItemToHalFunc, pStrObjID);
		}
		else
		{
			DataRender_UpdateRender(*pStrObjID);
		}
		if (false)
		{
			StuSimpleBuffer stuSimpleBuf;
			if (DataRender_GetBuffer(*pStrObjID, stuSimpleBuf))
			{
				BufferFunction::WriteBitmap(_T("e:\\test.bmp"), stuSimpleBuf.nWidth, stuSimpleBuf.nHeight, 32, stuSimpleBuf.pBuffer);
			}
		}
	}
	int nTm = clock();
	m_ThreadPool.WaitAll();
	//TRACE(_T("wait time = %d\n"), clock() - nTm);
}

DWORD BaseGraphDecoder::UpdateItemToHalFunc(LPVOID pParam, HANDLE hEnd)
{
	CString * pStrObjID = (CString *)pParam;
	if (pStrObjID)
	{
		DataRender_UpdateRender(*pStrObjID);
		delete pStrObjID;
		pStrObjID = nullptr;
	}
	return 0;
}

bool BaseGraphDecoder::OpenProject(CString strProjFile)
{
	std::vector<CString> vecObjID;
	DataBase_LoadObjFromXMLFile(strProjFile, vecObjID);
	return false;
}

bool BaseGraphDecoder::LoadProject()
{
	std::vector<CString> vecObjID;
	if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
	{
		//拿到视图缩放
		float fScale = 1.0f;
		//load时形成第一次贴片
		UpdateAllItemToHal(vecObjID, fScale);
	}
	return false;
}

CString BaseGraphDecoder::GetProjectInfo()
{
	return CString();
}

bool BaseGraphDecoder::ChangeProjectCfg(CString strProInfo)
{
	return false;
}

int BaseGraphDecoder::GetFrameLength()
{
	return 0;
}

CString BaseGraphDecoder::GetContent()
{
	return CString();
}

bool BaseGraphDecoder::ChangeContent(CString strContent)
{
	return false;
}

bool BaseGraphDecoder::GetDynamicFrame(int nFrame, DWORD * pBufferAddr)
{
	std::vector<CString> vecObjID;
	if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
	{
		//拿到视图缩放
		float fScale = 1.0f;
		//load时形成第一次贴片
		UpdateAllItemToHal(vecObjID, fScale);

		//拿到默认设备
		int hDevice = DX3DEngine_GetMainDevice();
		if (!hDevice)
			return false;

		//创建纹理及深度buf
		int hDepth = DXTextureManager_QueryTexture(DXTexture_Depth, 1);
		int hTex = DXTextureManager_QueryTexture(DXTexture_Normal2DTex, 1);
		DX3DEngine_ClearTexture(hDevice, hTex, RGBA(0, 0, 0, 0));
		DX3DEngine_ClearDepth(hDevice, hDepth, 1);

		//设定渲染目标
		DX3DEngine_SetRenderTarget(hDevice, hTex, hDepth);

		//设置好视窗
		RECT viewPort = { 0, 0, m_nWidth, m_nHeight };
		DX3DEngine_SetViewPort(hDevice, &viewPort);
		DX3DEngine_SetScissor(hDevice, &viewPort);

		//定位渲染中心点
		OffsetRect(&viewPort, -(m_nWidth / 2), -(m_nHeight / 2));

		//上锁，要绘制了
		DataBaseLocker DataLocker;
		DXEngineLocker EngineLocker(hDevice);

		//绘制物件
		RenderAllItemToTarget(vecObjID, hDevice, hTex, hDepth, fScale, viewPort);

		if (false)
			DX3DEngine_DumpTexture(hDevice, hTex);

		//善后
		DXTextureManager_UnQueryTexture(hTex);
		DXTextureManager_UnQueryTexture(hDepth);
	}
	return false;
}

bool BaseGraphDecoder::GetStaticFrame(DWORD * pBufferAddr)
{
	return false;
}
