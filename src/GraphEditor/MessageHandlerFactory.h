#pragma once
#include "MessageHandlerManager.h"

class MessageHandlerFactory
{
private:
	virtual ~MessageHandlerFactory() = 0;
public:
	static ISceneMessageHandler * CreateMessageHandler(EmMessageHandlerType emHandlerType);
};
