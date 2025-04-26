#pragma once

#include "testPropertiesWindow.h"
#include "nodeRegionWindow.h"

class testEditorWinow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(testEditorWinow)
	static void mainContextMenu();

	static ImVec2 mousePositionWhenContextMenuWasOpened;

	static void nodeCallback(VisNodeSys::Node* node, VisNodeSys::NODE_EVENT eventWithNode);
	static void OnFinishRecordingCallback(std::vector<FETPAction*>& RecordedActions);

	void renderMainMenu();

	static void textInputChangeNameCallback(std::string text);
	static void textInputCallback(std::string text);
public:
	SINGLETON_PUBLIC_PART(testEditorWinow)

	void show();
	void render() override;
};