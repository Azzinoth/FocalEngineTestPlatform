#include "GraphWindow.h"
#include "../../SubSystems/TestManager.h"
#include "../../SubSystems/FEPTActionSystem.h"
using namespace VisNodeSys;
using namespace SceneGraphUI;

NodeAreasGraphWindow::NodeAreasGraphWindow()
{
	GraphBackend = new NodeAreaGraphBackend();
	NodeAreaGraphUI = new TreeView(GraphBackend);
	NodeAreaGraphUI->SetFontSize(24.0f);
	//NodeAreaGraphUI->AddHiddenEntityTag(EDITOR_RESOURCE_TAG);

	NodeAreaGraphUI->AddOnNodeClickedCallback(NodeAreasGraphWindow::OnNodeClicked);
	NodeAreaGraphUI->AddOnNodeDoubleClickedCallback(NodeAreasGraphWindow::OnNodeDoubleClicked);
	NodeAreaGraphUI->SetNodeSelectionPredicate(NodeAreasGraphWindow::IsSelected);
	NodeAreaGraphUI->SetNodeDisplayNameProvider(NodeAreasGraphWindow::GetDisplayedName);
	//NodeAreaGraphUI->AddOnNodeHoveredCallback(NodeAreasGraphWindow::OnNodeHovered);
	//NodeAreaGraphUI->AddAfterNodeRenderCallback(NodeAreasGraphWindow::AfterNodeRender);
	NodeAreaGraphUI->SetContextMenuRenderingFunction(NodeAreasGraphWindow::ContextMenuRenderingFunction);
	NodeAreaGraphUI->SetRenameNodeFunction(NodeAreasGraphWindow::RenameNodeFunction);
}

void NodeAreasGraphWindow::InitializeResources()
{
	WindowIcon = new FETPImage("Resources/WindowIcon.png");
	BeginIcon = new FETPImage("Resources/beginNodeIcon.png");
	LinkNodeIcon = new FETPImage("Resources/linkNodeIcon.png");

	WindowIndicator.Icon = WindowIcon->GetTextureID();
	WindowIndicator.bIsInteractive = false;
	WindowIndicator.bIsVisibleByDefault = false;
	WindowIndicator.TooltipText = "Window is open for this node area";

	WindowIndicator.IsVisiblePredicate = [](SceneGraphUI::NodeHandle Node) -> bool {
		NodeArea* ClickedNodeArea = Node.As<NodeArea>();
		if (ClickedNodeArea == nullptr)
			return false;

		NodeAreaWindow* NodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetNodeAreaWindow(ClickedNodeArea);
		if (NodeAreaWindow == nullptr)
			return false;

		return true;
	};
	NodeAreaGraphUI->AddNodeWidget(WindowIndicator);

	BeginIndicator.Icon = BeginIcon->GetTextureID();
	BeginIndicator.bIsInteractive = false;
	BeginIndicator.bIsVisibleByDefault = false;
	BeginIndicator.TooltipText = "Execution will start from this node area";

	BeginIndicator.IsVisiblePredicate = [](SceneGraphUI::NodeHandle Node) -> bool {
		NodeArea* ClickedNodeArea = Node.As<NodeArea>();
		if (ClickedNodeArea == nullptr)
			return false;

		if (TEST_MANAGER.GetSelectedTest() == nullptr)
			return false;

		if (TEST_MANAGER.GetSelectedTest()->GetBeginNode()->GetParentArea() == ClickedNodeArea)
			return true;

		return false;
	};
	NodeAreaGraphUI->AddNodeWidget(BeginIndicator);

	FailedActionIndicator.Icon = TEST_PLATFORM.GetInfoIconRed()->GetTextureID();
	FailedActionIndicator.bIsInteractive = false;
	FailedActionIndicator.bIsVisibleByDefault = false;
	FailedActionIndicator.TooltipText = "Has failed action(s).";

	FailedActionIndicator.IsVisiblePredicate = [](SceneGraphUI::NodeHandle Node) -> bool {
		return ACTION_SYSTEM.DoesNodeAreaHaveProblematicAction(Node.As<NodeArea>()->GetID());
	};
	NodeAreaGraphUI->AddNodeWidget(FailedActionIndicator);

	LinkNodeIndicator.Icon = LinkNodeIcon->GetTextureID();
	LinkNodeIndicator.bIsInteractive = false;
	LinkNodeIndicator.bIsVisibleByDefault = false;
	LinkNodeIndicator.TooltipText = "Contains link node(s).";

	LinkNodeIndicator.IsVisiblePredicate = [](SceneGraphUI::NodeHandle Node) -> bool {
		NodeArea* CurrentNodeArea = Node.As<NodeArea>();
		if (CurrentNodeArea == nullptr)
			return false;

		return !CurrentNodeArea->GetNodesByStringType("LinkNode").empty();
	};
	NodeAreaGraphUI->AddNodeWidget(LinkNodeIndicator);
}

void NodeAreasGraphWindow::Clear()
{
	bLastFrameWasInvisible = true;
}

void NodeAreasGraphWindow::Render()
{
	if (!bVisible)
	{
		bLastFrameWasInvisible = true;
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::Begin("Scene Graph", nullptr, ImGuiWindowFlags_None))
	{
		if (TEST_MANAGER.GetSelectedTest() != nullptr && TEST_MANAGER.GetSelectedTest()->GetBeginNode() != nullptr)
		{
			NodeArea* RootNodeArea = TEST_MANAGER.GetSelectedTest()->GetDummyRootNodeArea();
			GraphBackend->SetRootNodeAreaID(RootNodeArea->GetID());
			GraphBackend->SetCurrentTest(TEST_MANAGER.GetSelectedTest());
			NodeAreaGraphUI->Render(SceneGraphUI::NodeHandle(RootNodeArea, GraphBackend));
			if (LastFrameRootNodeID != RootNodeArea->GetID())
			{
				LastFrameRootNodeID = RootNodeArea->GetID();
				NodeAreaGraphUI->SetNodeExpanded(SceneGraphUI::NodeHandle(RootNodeArea, GraphBackend), true);
			}
		}
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

void NodeAreasGraphWindow::OnNodeClicked(SceneGraphUI::NodeHandle Node, ImGuiMouseButton_ MouseButton)
{
	if (MouseButton != ImGuiMouseButton_Left)
		return;

	NodeArea* ClickedNodeArea = Node.As<NodeArea>();
	if (ClickedNodeArea == nullptr)
		return;

	NodeAreaWindow* NodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetNodeAreaWindow(ClickedNodeArea);
	if (NodeAreaWindow == nullptr)
		return;

	NODE_AREA_WINDOW_MANAGER.SetInFocusNodeAreaWindow(NodeAreaWindow);
}

void NodeAreasGraphWindow::OnNodeDoubleClicked(SceneGraphUI::NodeHandle Node, ImGuiMouseButton_ MouseButton)
{
	if (MouseButton != ImGuiMouseButton_Left)
		return;

	NodeArea* ClickedNodeArea = Node.As<NodeArea>();
	if (ClickedNodeArea == nullptr)
		return;

	NodeAreaWindow* NodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetNodeAreaWindow(ClickedNodeArea);
	if (NodeAreaWindow == nullptr)
		NodeAreaWindow = NODE_AREA_WINDOW_MANAGER.CreateNodeAreaWindow(ClickedNodeArea);

	NODE_AREA_WINDOW_MANAGER.SetInFocusNodeAreaWindow(NodeAreaWindow);
}

bool NodeAreasGraphWindow::IsSelected(SceneGraphUI::NodeHandle Node)
{
	NodeAreaWindow* FocusedNodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetInFocusNodeAreaWindow();
	if (FocusedNodeAreaWindow == nullptr)
		return false;

	NodeArea* CurrentNodeArea = FocusedNodeAreaWindow->GetNodeArea();
	if (CurrentNodeArea == nullptr)
		return false;

	if (CurrentNodeArea->GetID() == Node.As<NodeArea>()->GetID())
		return true;

	return false;
}

std::string NodeAreasGraphWindow::GetDisplayedName(SceneGraphUI::NodeHandle Node)
{
	NodeArea* CurrentNodeArea = Node.As<NodeArea>();
	std::string DisplayedName = CurrentNodeArea->GetName();
	FETest* CurrentTest = TEST_MANAGER.GetSelectedTest();

	// Special case for the dummy root node area.
	if (CurrentTest->GetDummyRootNodeArea() == CurrentNodeArea)
		DisplayedName = CurrentTest->GetName();
	
	return DisplayedName;
}

//void FEEditorSceneGraphWindow::OnNodeHovered(SceneGraphUI::NodeHandle Node)
//{
//	FENaiveSceneGraphNode* NaiveNode = Node.As<FENaiveSceneGraphNode>();
//	if (!DRAG_AND_DROP_MANAGER.ObjectIsDraged())
//	{
//		if (ImGui::IsMouseDragging(0))
//			DRAG_AND_DROP_MANAGER.SetObjectToDrag(NaiveNode, nullptr, ImVec2(), ImVec2());
//	}
//}
//
//void FEEditorSceneGraphWindow::AfterNodeRender(SceneGraphUI::NodeHandle Node)
//{
//	FENaiveSceneGraphNode* NaiveNode = Node.As<FENaiveSceneGraphNode>();
//	SCENE_GRAPH_WINDOW.GetSceneNodeDragAndDropTarget(NaiveNode)->StickToItem();
//}

void NodeAreasGraphWindow::ContextMenuRenderingFunction(SceneGraphUI::NodeHandle Node)
{
	NodeArea* CurrentNodeArea = Node.As<NodeArea>();
	if (CurrentNodeArea == nullptr)
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("New Node Area"))
			{
				NODE_SYSTEM.CreateNodeArea();
			}

			ImGui::EndMenu();
		}
	}
	else
	{
		NodeAreaWindow* NodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetNodeAreaWindow(CurrentNodeArea);
		if (NodeAreaWindow != nullptr)
		{
			if (ImGui::MenuItem("Center view"))
			{
				NodeAreaWindow->GetNodeArea()->CenterViewOnAllElements();
			}
		}
		else
		{
			if (ImGui::MenuItem("Open in new window"))
			{
				NodeAreaWindow = NODE_AREA_WINDOW_MANAGER.CreateNodeAreaWindow(CurrentNodeArea);
				NODE_AREA_WINDOW_MANAGER.SetInFocusNodeAreaWindow(NodeAreaWindow);
			}
		}

		if (ImGui::MenuItem("Rename"))
		{
			NODE_AREAS_GRAPH_WINDOW.NodeAreaGraphUI->SetNodeForRenaming(Node);
		}

		ImGui::Separator();

		if (NodeAreaWindow != nullptr)
		{
			if (ImGui::MenuItem("Close window"))
			{
				NODE_AREA_WINDOW_MANAGER.DeleteNodeAreaWindow(CurrentNodeArea);
			}
		}

		if (ImGui::MenuItem("Delete"))
		{
			NODE_SYSTEM.DeleteNodeArea(CurrentNodeArea);
		}
	}
}

SceneGraphUI::TreeView* NodeAreasGraphWindow::GetNodeAreaGraphUI() const
{
	return NodeAreaGraphUI;
}

void NodeAreasGraphWindow::RenameNodeFunction(SceneGraphUI::NodeHandle Node, std::string NewName)
{
	NodeArea* CurrentNodeArea = Node.As<NodeArea>();
	if (CurrentNodeArea == nullptr)
		return;

	CurrentNodeArea->SetName(NewName);
}