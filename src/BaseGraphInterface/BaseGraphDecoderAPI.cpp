#include "stdafx.h"
#include "BaseGraphDecoder.h"
#include "BaseGraphDecoderAPI.h"
#include "ExportDataRender.h"
#include "ExportSceneRender.h"

BASEGRAPH_INTERFACE_API bool Interface_InitialEnv()
{
	//初始化3D环境
	CString strBinPath = CommonFunction::GetBinPath();
	DX3DEngine_Init(DX_DX10_Device, strBinPath + _T("\\FX\\"));
	//初始化材质
	BaseGraphMaterialModuleInit(strBinPath + _T("\\FX\\"));
	//初始化主device
	DX3DEngine_Initialize3DDevice(1920, 1080);
	return true;
}

BASEGRAPH_INTERFACE_API bool Interface_ReleaseEnv()
{
	//释放材质
	BaseGraphMaterialModuleRelease();
	//释放主device
	DX3DEngine_ReleaseMainDevice();
	//释放3D环境
	DX3DEngine_Release();
	return true;
}

BASEGRAPH_INTERFACE_API bool Interface_CreateDecoder(IBaseGraphDecoder *& pDecoder)
{
	pDecoder = new BaseGraphDecoder;
	return true;
}

BASEGRAPH_INTERFACE_API bool Interface_ReleaseDecoder(IBaseGraphDecoder *& pDecoder)
{
	delete pDecoder;
	pDecoder = nullptr;
	return true;
}
