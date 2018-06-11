#include "stdafx.h"
#include "MessageHandlerFactory.h"
#include "MessageHandlerDefault.h"

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
	default:
		break;
	}
	return pRet;
}
