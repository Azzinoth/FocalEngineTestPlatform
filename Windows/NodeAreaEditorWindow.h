#pragma once

//#include "TestPropertiesWindow.h"
#include "../SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.h"
#include "../SubSystems/NodeSystem/CustomNodes/BeginNode.h"

class NodeAreaEditorWindow : public FEImGuiWindow
{
	~NodeAreaEditorWindow();
	//SINGLETON_PRIVATE_PART(TestEditorWindow)
	void RenderMainContextMenu();

	ImVec2 MousePositionWhenContextMenuWasOpened;

	//static void NodeCallback(VisNodeSys::Node* Node, VisNodeSys::NODE_EVENT CurrentNodeEvent);
	//static void OnFinishRecordingCallback(std::vector<FETPAction*>& RecordedActions);

	//void RenderMainMenu();
public:
	//SINGLETON_PUBLIC_PART(TestEditorWindow)
	NodeAreaEditorWindow();

	VisNodeSys::NodeArea* CurrentNodeArea = nullptr;

	void Show();
	void Render() override;
};