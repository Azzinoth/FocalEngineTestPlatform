#include "NodeAreaWindow.h"
#include "../TextInputPopup.h"
#include "../ActionEditPopup.h"
#include "../LinkAreaSelectionPopup.h"
using namespace VisNodeSys;

NodeAreaWindow::NodeAreaWindow(VisNodeSys::NodeArea* NodeAreaToWorkWith)
{
	if (NodeAreaToWorkWith != nullptr)
	{
		NodeAreaID = NodeAreaToWorkWith->GetID();
		NodeArea* CurrentNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaID);
		CurrentNodeArea->SetMainContextMenuFunction(RenderMainContextMenu);
		CurrentNodeArea->SetSaveExecutedNodes(true);
	}

	bHaveCloseButton = true;
}

NodeAreaWindow::~NodeAreaWindow()
{
}

VisNodeSys::NodeArea* NodeAreaWindow::GetNodeArea() const
{
	NodeArea* CurrentNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaID);
	return CurrentNodeArea;
}

void NodeAreaWindow::Show()
{
	FEImGuiWindow::Show();
	FrameCountSinceOpen = 0;
}

void NodeAreaWindow::Render()
{
	if (!IsVisible())
		return;

	NodeArea* CurrentNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaID);
	if (CurrentNodeArea == nullptr)
		return;

	ImGuiID DockspaceID = FocalEngine::APPLICATION.GetMainWindow()->GetDefaultDockspaceID();
	if (DockspaceID != 0 && bShouldDockToCentralNode)
	{
		ImGuiDockNode* CentralNode = ImGui::DockBuilderGetCentralNode(DockspaceID);
		if (CentralNode != nullptr)
			ImGui::SetNextWindowDockID(CentralNode->ID, ImGuiCond_Appearing);
	}

	Flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	FEImGuiWindow::Render();

	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
	{
		NODE_AREA_WINDOW_MANAGER.SetFocusedNodeAreaID(CurrentNodeArea->GetID());

		// It might happen that this window is focused, but the node area is not marked as focused.
		// In this case, we need to set it as focused.
		if (!CurrentNodeArea->IsFocused())
			CurrentNodeArea->SetFocused(true);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	std::string Caption = CurrentNodeArea->GetName() + "##" + CurrentNodeArea->GetID();
	SetCaption(Caption);

	//CurrentNodeArea->SetMainContextMenuFunction(RenderMainContextMenu);
	CurrentNodeArea->SetPosition(ImVec2(0.0f, 0.0f));
	CurrentNodeArea->SetSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
	CurrentNodeArea->Update();

	if (bShouldCenterViewOnOpen && FrameCountSinceOpen > 1)
	{
		CurrentNodeArea->CenterViewOnAllElements();
		bShouldCenterViewOnOpen = false;
	}

	ImGui::PopStyleVar();
	FEImGuiWindow::OnRenderEnd();
	if (FrameCountSinceOpen < 10)
		FrameCountSinceOpen++;
}

void NodeAreaWindow::RenderMainContextMenu()
{
	NodeAreaWindow* CurrentlyActiveNodeAreaWindow = NODE_AREA_WINDOW_MANAGER.GetInFocusNodeAreaWindow();
	if (CurrentlyActiveNodeAreaWindow == nullptr)
		return;

	VisNodeSys::NodeArea* CurrentlyActiveNodeArea = CurrentlyActiveNodeAreaWindow->GetNodeArea();
	if (CurrentlyActiveNodeArea == nullptr)
		return;

	NodeAreaContextMenuOpenState ContextMenuState = CurrentlyActiveNodeArea->GetContextMenuOpenState();
	VisNodeSys::Node* HoveredNode = ContextMenuState.GetNode();
	VisNodeSys::GroupComment* HoveredGroupComment = ContextMenuState.GetGroupComment();
	ImVec2 MousePositionWhenContextMenuWasOpened = ContextMenuState.MousePositionRecorded;

	if (HoveredNode == nullptr && CurrentlyActiveNodeArea->GetSelected().size() == 0)
	{
		if (ImGui::BeginMenu("Add Node"))
		{
			VisNodeSys::Node* NewNode = nullptr;

			if (ImGui::BeginMenu("Test Control"))
			{
				if (ImGui::MenuItem("Fail Test"))
				{
					NewNode = new FailTestNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Timing"))
			{
				if (ImGui::MenuItem("Sleep"))
				{
					NewNode = new SleepNode();
				}

				if (ImGui::MenuItem("Timer"))
				{
					NewNode = new TimerNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Images"))
			{
				if (ImGui::MenuItem("Image Literal"))
				{
					NewNode = new ImageLiteralNode();
				}

				if (ImGui::MenuItem("Image Variable"))
				{
					NewNode = new ImageVariableNode();
				}

				if (ImGui::MenuItem("Image Load"))
				{
					NewNode = new ImageLoadNode();
				}

				if (ImGui::MenuItem("Image Save"))
				{
					NewNode = new ImageSaveNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Image Search"))
				{
					NewNode = new ImageSearchNode();
				}

				if (ImGui::MenuItem("Screenshot"))
				{
					NewNode = new ScreenshotNode();
				}

				if (ImGui::MenuItem("Recognize Text"))
				{
					NewNode = new RecognizeTextNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Strings"))
			{
				if (ImGui::MenuItem("String Literal"))
				{
					NewNode = new StringLiteralNode();
				}

				if (ImGui::MenuItem("String Variable"))
				{
					NewNode = new StringVariableNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Is String Empty"))
				{
					NewNode = new IsStringEmptyNode();
				}

				if (ImGui::MenuItem("Is String Number"))
				{
					NewNode = new IsStringNumberNode();
				}

				if (ImGui::MenuItem("Is String Integer"))
				{
					NewNode = new IsStringIntegerNode();
				}

				if (ImGui::MenuItem("String Equals"))
				{
					NewNode = new StringEqualsNode();
				}

				if (ImGui::MenuItem("String Contains"))
				{
					NewNode = new StringContainsNode();
				}

				if (ImGui::MenuItem("String Starts With"))
				{
					NewNode = new StringStartsWithNode();
				}

				if (ImGui::MenuItem("String Ends With"))
				{
					NewNode = new StringEndsWithNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("String Length"))
				{
					NewNode = new StringLengthNode();
				}

				if (ImGui::MenuItem("String To Int"))
				{
					NewNode = new StringToIntNode();
				}

				if (ImGui::MenuItem("String To Float"))
				{
					NewNode = new StringToFloatNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("String Concatenate"))
				{
					NewNode = new StringConcatenateNode();
				}

				if (ImGui::MenuItem("String Substring"))
				{
					NewNode = new StringSubstringNode();
				}

				if (ImGui::MenuItem("String Replace"))
				{
					NewNode = new StringReplaceNode();
				}

				if (ImGui::MenuItem("String To Upper"))
				{
					NewNode = new StringToUpperNode();
				}

				if (ImGui::MenuItem("String To Lower"))
				{
					NewNode = new StringToLowerNode();
				}

				if (ImGui::MenuItem("String Trim"))
				{
					NewNode = new StringTrimNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("System"))
			{
				if (ImGui::MenuItem("Launch Application"))
				{
					NewNode = new LaunchApplicationNode();
				}

				if (ImGui::MenuItem("Launch Application..."))
				{
					LaunchApplicationNode* TemporaryNewNode = new LaunchApplicationNode();
					std::string Path;
					FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, ApplicationLoadFilter, 1);
					if (!Path.empty())
						TemporaryNewNode->SetPath(Path);
					NewNode = TemporaryNewNode;
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Create Link Node..."))
			{
				std::string SourceAreaID = CurrentlyActiveNodeArea->GetID();
				ImVec2 DropPosition = MousePositionWhenContextMenuWasOpened;
				LinkAreaSelectionPopup::GetInstance().Show(SourceAreaID, [SourceAreaID, DropPosition](VisNodeSys::NodeArea* ChosenArea)
				{
					VisNodeSys::NodeArea* SourceArea = NODE_SYSTEM.GetNodeAreaByID(SourceAreaID);
					if (SourceArea == nullptr)
						return;

					VisNodeSys::NodeArea* TargetArea = ChosenArea;
					if (TargetArea == nullptr)
						TargetArea = NODE_SYSTEM.CreateNodeArea();

					std::pair<std::string, std::string> LinkIDs;
					if (!NODE_SYSTEM.LinkNodeAreas(SourceArea->GetID(), TargetArea->GetID(), &LinkIDs))
						return;

					VisNodeSys::Node* NewLinkNode = NODE_SYSTEM.GetNodeByID(LinkIDs.first);
					if (NewLinkNode != nullptr)
						NewLinkNode->SetPosition(DropPosition);
				});
			}

			if (ImGui::MenuItem("Create New SubArea Node..."))
			{
				VisNodeSys::Node* NewSubAreaNode = NODE_SYSTEM.CreateSubAreaNode(CurrentlyActiveNodeArea->GetID());
				NewSubAreaNode->SetPosition(MousePositionWhenContextMenuWasOpened);
			}

			if (ImGui::BeginMenu("File System"))
			{
				if (ImGui::MenuItem("Extract File Name"))
				{
					NewNode = new ExtractFileNameNode();
				}

				if (ImGui::MenuItem("Extract File Extension"))
				{
					NewNode = new ExtractFileExtensionNode();
				}

				if (ImGui::MenuItem("Extract Directory Path"))
				{
					NewNode = new ExtractDirectoryPathNode();
				}

				if (ImGui::MenuItem("Get Absolute Path"))
				{
					NewNode = new GetAbsolutePathNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Does File Exist"))
				{
					NewNode = new DoesFileExistNode();
				}

				if (ImGui::MenuItem("Does Directory Exist"))
				{
					NewNode = new DoesDirectoryExistNode();
				}

				if (ImGui::MenuItem("Get File Size"))
				{
					NewNode = new GetFileSizeNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Get Current Working Path"))
				{
					NewNode = new GetCurrentWorkingPathNode();
				}

				if (ImGui::MenuItem("Get Test Working Path"))
				{
					NewNode = new GetTestWorkingPathNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Copy File"))
				{
					NewNode = new CopyFileNode();
				}

				if (ImGui::MenuItem("Rename File"))
				{
					NewNode = new RenameFileNode();
				}

				if (ImGui::MenuItem("Delete File"))
				{
					NewNode = new DeleteFileNode();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Create Directory"))
				{
					NewNode = new CreateDirectoryNode();
				}

				if (ImGui::MenuItem("Copy Directory"))
				{
					NewNode = new CopyDirectoryNode();
				}

				if (ImGui::MenuItem("Rename Directory"))
				{
					NewNode = new RenameDirectoryNode();
				}

				if (ImGui::MenuItem("Delete Directory"))
				{
					NewNode = new DeleteDirectoryNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Input Simulation"))
			{
				if (ImGui::BeginMenu("Mouse"))
				{
					if (ImGui::MenuItem("Move"))
					{
						MouseMoveNode* NewNode = new MouseMoveNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("LeftButtonDown"))
					{
						MouseLeftButtonDownNode* NewNode = new MouseLeftButtonDownNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("LeftButtonUp"))
					{
						MouseLeftButtonUpNode* NewNode = new MouseLeftButtonUpNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("RightButtonDown"))
					{
						MouseRightButtonDownNode* NewNode = new MouseRightButtonDownNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("RightButtonUp"))
					{
						MouseRightButtonUpNode* NewNode = new MouseRightButtonUpNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("WheelRotation"))
					{

					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Keyboard"))
				{
					if (ImGui::MenuItem("KeyDown"))
					{
						KeyboardKeyDownNode* NewNode = new KeyboardKeyDownNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("KeyUp"))
					{

						KeyboardKeyUpNode* NewNode = new KeyboardKeyUpNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					if (ImGui::MenuItem("KeyPress"))
					{
					}

					if (ImGui::MenuItem("Text Input"))
					{
						TextInputNode* NewNode = new TextInputNode();
						NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
						NewNode->SetText("Text Input");
						CurrentlyActiveNodeArea->AddNode(NewNode);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (NewNode != nullptr)
			{
				NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
				CurrentlyActiveNodeArea->AddNode(NewNode);
			}

			ImGui::EndMenu();
		}
	}
	else if (HoveredNode != nullptr)
	{
		if (ImGui::MenuItem("Run From Here"))
		{
			FETest* OwningTest = TEST_MANAGER.GetSelectedTest();
			if (OwningTest != nullptr)
				ACTION_SYSTEM.Run(OwningTest, HoveredNode);
		}

		if (ImGui::MenuItem("Copy NodeID to clipboard"))
		{
			ImGui::SetClipboardText(HoveredNode->GetID().c_str());
		}

		if (HoveredNode->GetType() == "LinkNode")
		{
			LinkNode* CurrentLinkNode = static_cast<LinkNode*>(HoveredNode);
			if (CurrentLinkNode->GetLinkedArea() != nullptr)
			{
				if (ImGui::MenuItem("Open Linked Area"))
				{
					NODE_AREA_WINDOW_MANAGER.OpenNodeAreaWindow(CurrentLinkNode->GetLinkedArea());
				}
			}

			if (CurrentLinkNode->IsDangling())
			{
				if (ImGui::MenuItem("Try to fix dangling link"))
				{
					NODE_SYSTEM.TryToFixDanglingLinkNode(CurrentLinkNode, true);
				}
			}
		}

		if (HoveredNode->GetType() == "SubAreaNode")
		{
			SubAreaNode* CurrentSubAreaNode = static_cast<SubAreaNode*>(HoveredNode);
			if (CurrentSubAreaNode->GetOwnedArea() != nullptr)
			{
				if (ImGui::MenuItem("Open Area"))
				{
					ImVec2 Min, Max;
					CurrentSubAreaNode->GetOwnedArea()->GetAllElementsAABB(Min, Max);
					// We want user to see center of the area.
					// FE_TO_DO: It is not placing it to the exact center, later it should be fixed.
					CurrentSubAreaNode->GetOwnedArea()->SetRenderOffset((Min + Max) * 0.5f);
					NODE_AREA_WINDOW_MANAGER.OpenNodeAreaWindow(CurrentSubAreaNode->GetOwnedArea());
				}
			}
		}
	}
	else if (CurrentlyActiveNodeArea->GetSelected().size() > 1)
	{
		auto SelectedList = CurrentlyActiveNodeArea->GetSelected();
		if (ImGui::MenuItem("Delete Selected Nodes"))
		{
			for (size_t i = 0; i < SelectedList.size(); i++)
			{
				CurrentlyActiveNodeArea->Delete(SelectedList[i]);
			}

			CurrentlyActiveNodeArea->UnSelectAll();
		}

		if (ImGui::MenuItem("Save Selected Nodes..."))
		{
			std::string Path;
			FocalEngine::FILE_SYSTEM.ShowFileSaveDialog(Path, NULL, 0);

			if (Path != "")
			{
				CurrentlyActiveNodeArea->SaveNodesToFile(Path.c_str(), SelectedList);
			}
		}

		/*if (ImGui::MenuItem("Place in a grid"))
		{
			int nodesPerW = 4;
			int nodesPerH = 4;
			int disBetweenOnW = 40;
			int disBetweenOnH = 150;

			auto selectedList = CurrentlyActiveNodeArea->GetSelected();
			for (size_t i = 0; i < selectedList.size(); i++)
			{
				int XPosition = selectedList[0]->getPosition().x;
				XPosition += (i % nodesPerW * int(selectedList[i]->getSize().x + disBetweenOnW));
				XPosition %= TEST_PLATFORM.getWindowWidth();

				int YPosition = i / nodesPerH;
				YPosition *= disBetweenOnH;

				selectedList[i]->SetPosition(ImVec2(float(XPosition), float(YPosition)));
			}
		}*/
	}
}