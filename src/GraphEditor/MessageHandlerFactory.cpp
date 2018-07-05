#include "stdafx.h"
#include "MessageHandlerFactory.h"
#include "MessageHandlerDefault.h"
#include "MessageHandlerTitlewordTool.h"
#include "MessageHandlerImageTool.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
ISceneMessageHandler * MessageHandlerFactory::CreateMessageHandler(EmMessageHandlerType emHandlerType)
{
	ISceneMessageHandler * pRet = nullptr;
	switch (emHandlerType)
	{
	case MessageHandlerType_Default:
		pRet = new MessageHandlerDefault;
		break;
	case MessageHandlerType_Scale:
		break;
	case MessageHandlerType_Rotate:
		break;
	case MessageHandlerType_CreateTitle:
		pRet = new MessageHanderTitlewordTool;
		break;
	case MessageHandlerType_CreateScroll:
		break;
	case MessageHandlerType_CreateImage:
		pRet = new MessageHandlerImageTool;
		break;
	case MessageHandlerType_EditTitle:
		break;
	case MessageHandlerType_EditScroll:
		break;
	case MessageHandlerType_EditImage:
		break;
	default:
		break;
	}
	return pRet;
}
