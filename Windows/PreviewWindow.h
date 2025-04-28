#pragma once

#include "../SubSystems/TestManager.h"
#include "FailedTestWindow.h"

class PreviewWindow : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(PreviewWindow)

	ImVec2 PopupSize = ImVec2(1200, 800);
	ImGuiButton* CancelButton;
	ImGuiButton* NewTestButton;
	ImGuiButton* AddButton;
	static bool bIsReadOnly;

	// ************** Node area **************
	static void RenderMainContextMenu();

	static ImVec2 WindowPosition;
	static ImVec2 NodeGridRelativePosition;
	static ImVec2 MousePositionWhenContextMenuWasOpened;

	static void NodeCallback(VisNodeSys::Node* Node, VisNodeSys::NODE_EVENT CurrentNodeEvent);
	bool bIsFirstFrame = false;
	static ImVec2 NeededShift;
	void PositionNodesInCenter();
	void PositionNodesInTargetCenter();
	// ************** Node area END **************

	void Close();
public:
	SINGLETON_PUBLIC_PART(PreviewWindow)

	static VisNodeSys::NodeArea* CurrentNodeArea;

	void Show(bool bIsReadOnlyIn = false);
	void Render() override;
};