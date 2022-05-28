#pragma once

#include "../SubSystems/TestManager.h"
#include "failedTestWindow.h"

class previewWindow : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(previewWindow)

	ImVec2 popupSize = ImVec2(1200, 800);
	ImGuiButton* cancelButton;
	ImGuiButton* newTestButton;
	ImGuiButton* addButton;
	static bool readOnly;

	// ************** Node area **************
	static void mainContextMenu();

	static ImVec2 windowPosition;
	static ImVec2 nodeGridRelativePosition;
	static ImVec2 mousePositionWhenContextMenuWasOpened;

	static void nodeCallback(FEVisualNode* node, FE_VISUAL_NODE_EVENT eventWithNode);
	bool firstFrame = false;
	static ImVec2 neededShift;
	void positionNodesInCenter();
	void positionNodesInTargetCenter();
	// ************** Node area END **************

	void close();
public:
	SINGLETON_PUBLIC_PART(previewWindow)

	static FEVisualNodeArea* currentNodeArea;

	void show(bool isReadOnly = false);
	void render() override;
};