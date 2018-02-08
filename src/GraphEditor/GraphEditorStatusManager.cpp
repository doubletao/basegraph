#include "stdafx.h"
#include "GraphEditorStatusManager.h"


GraphEditorStatusManager::GraphEditorStatusManager()
	: m_nCurrentObjectTool(0)
	, m_nCurrentObjectEditor(0)
	, m_nCurrentActionTool(0)
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

void GraphEditorStatusManager::SetCurrentObjectTool(int nCurrentObjectTool)
{
	m_nCurrentObjectTool = nCurrentObjectTool;
}

int GraphEditorStatusManager::GetCurrentObjectTool()
{
	return m_nCurrentObjectTool;
}

void GraphEditorStatusManager::SetCurrentObjectEditor(int nCurrentObjectEditor)
{
	m_nCurrentObjectEditor = nCurrentObjectEditor;
}

int GraphEditorStatusManager::GetCurrentObjectEditor()
{
	return m_nCurrentObjectEditor;
}

void GraphEditorStatusManager::SetCurrentActionTool(int nCurrentActionTool)
{
	m_nCurrentActionTool = nCurrentActionTool;
}

int GraphEditorStatusManager::GetCurrentActionTool()
{
	return m_nCurrentActionTool;
}

