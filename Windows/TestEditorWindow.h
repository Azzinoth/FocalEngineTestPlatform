#pragma once

#include "TestPropertiesWindow.h"
#include "NodeRegionWindow.h"

class TestEditorWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(TestEditorWindow)
	static void RenderMainContextMenu();

	static ImVec2 MousePositionWhenContextMenuWasOpened;

	static void NodeCallback(VisNodeSys::Node* Node, VisNodeSys::NODE_EVENT CurrentNodeEvent);
	static void OnFinishRecordingCallback(std::vector<FETPAction*>& RecordedActions);

	void RenderMainMenu();
public:
	SINGLETON_PUBLIC_PART(TestEditorWindow)

	void Show();
	void Render() override;
};