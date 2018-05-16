#include "stdafx.h"
#include "GraphEditorStatusManager.h"
#include "MessageHandlerManager.h"
#include "GraphEditorCommonDef.h"

GraphEditorStatusManager::GraphEditorStatusManager()
{
}


GraphEditorStatusManager::~GraphEditorStatusManager()
{
}

GraphEditorStatusManager & GraphEditorStatusManager::Instance()
{
	static GraphEditorStatusManager inst;
	return inst;
}

int GraphEditorStatusManager::GetCurrentUIElementTool()
{
	EmMessageHandlerType emHandlerType = MessageHandlerManager::Instance().GetTopHandlerType();
	switch (emHandlerType)
	{
	case MessageHandlerType_NULL:
		return -1;
	case MessageHandlerType_Default:
		return UIElementMsg_DefaultAction;
	case MessageHandlerType_Scale:
		return UIElementMsg_ZoomAction;
	case MessageHandlerType_Rotate:
		return UIElementMsg_RotateAction;
	case MessageHandlerType_CreateTitle:
		return UIElementMsg_CreateTitle;
	case MessageHandlerType_CreateScroll:
		return UIElementMsg_CreateScroll;
	case MessageHandlerType_CreateImage:
		return UIElementMsg_CreateImage;
	case MessageHandlerType_EditTitle:
		break;
	case MessageHandlerType_EditScroll:
		break;
	case MessageHandlerType_EditImage:
		break;
	default:
		break;
	}
	return -1;
}
