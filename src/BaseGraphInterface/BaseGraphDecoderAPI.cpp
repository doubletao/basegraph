#include "stdafx.h"
#include "BaseGraphDecoder.h"
#include "BaseGraphDecoderAPI.h"

BASEGRAPH_INTERFACE_API bool Interface_InitialEnv()
{
	return true;
}

BASEGRAPH_INTERFACE_API bool Interface_ReleaseEnv()
{
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
