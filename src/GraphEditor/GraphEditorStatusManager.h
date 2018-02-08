#pragma once
class GraphEditorStatusManager
{
private:
	int m_nCurrentObjectTool;
	int m_nCurrentObjectEditor;
	int m_nCurrentActionTool;
private:
	GraphEditorStatusManager();
	virtual ~GraphEditorStatusManager();
public:
	static GraphEditorStatusManager & Instance();

	void SetCurrentObjectTool(int nCurrentObjectTool);
	int GetCurrentObjectTool();
	void SetCurrentObjectEditor(int nCurrentObjectEditor);
	int GetCurrentObjectEditor();
	void SetCurrentActionTool(int nCurrentActionTool);
	int GetCurrentActionTool();
};

