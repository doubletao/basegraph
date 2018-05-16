#pragma once
class GraphEditorStatusManager
{
private:
	GraphEditorStatusManager();
	virtual ~GraphEditorStatusManager();
public:
	static GraphEditorStatusManager & Instance();

	int GetCurrentUIElementTool();
};

