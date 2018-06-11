#pragma once
#include "ISceneMessageHandler.h"

//默认工具担当点选，框选，移动，控制点缩放，控制点旋转等一系列功能
class MessageHanderTitleword :
	public ISceneMessageHandler
{
public:
	MessageHanderTitleword();
	virtual ~MessageHanderTitleword();
};
