#include "stdafx.h"
#include "MessageHandlerFactory.h"
#include "MessageHandlerDefault.h"
#include "MessageHandlerTitlewordTool.h"

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
		pRet = new MessageHanderTitleword;
		break;
	case MessageHandlerType_CreateScroll:
		break;
	case MessageHandlerType_CreateImage:
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
