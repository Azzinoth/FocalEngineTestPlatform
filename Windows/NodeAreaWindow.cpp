#include "NodeAreaWindow.h"
#include "TextInputPopup.h"
#include "ActionEditPopup.h"
using namespace VisNodeSys;

NodeAreaWindow::NodeAreaWindow(VisNodeSys::NodeArea* NodeArea)
{
	CurrentNodeArea = NodeArea;
	CurrentNodeArea->SetMainContextMenuFunction(RenderMainContextMenu);
}

NodeAreaWindow::~NodeAreaWindow()
{
}

VisNodeSys::NodeArea* NodeAreaWindow::GetNodeArea() const
{ 
	return CurrentNodeArea;
}

void NodeAreaWindow::Show()
{
	FEImGuiWindow::Show();
}

void NodeAreaWindow::Render()
{
	Flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	FEImGuiWindow::Render();

	if (!IsVisible())
		return;

	if (CurrentNodeArea == nullptr)
		return;

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

	ImGui::PopStyleVar();
	FEImGuiWindow::OnRenderEnd();
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

			if (ImGui::MenuItem("Sleep"))
			{
				NewNode = new SleepNode();
			}

			if (ImGui::MenuItem("Timer"))
			{
				NewNode = new TimerNode();
			}

			if (ImGui::MenuItem("Image"))
			{
				NewNode = new ImageNode();
			}

			if (ImGui::MenuItem("Image Search"))
			{
				NewNode = new ImageSearchNode();
			}

			if (ImGui::MenuItem("Application Launch..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, ApplicationLoadFilter, 1);

				if (!Path.empty())
				{
					LunchApplicationNode* TemporaryNewNode = new LunchApplicationNode();
					TemporaryNewNode->SetPath(Path);
					NewNode = TemporaryNewNode;
				}
			}

			if (ImGui::MenuItem("Reference Area..."))
			{
				VisualReferenceNode* NewNode = new VisualReferenceNode(true);
				NewNode->SetPosition(MousePositionWhenContextMenuWasOpened);
				CurrentlyActiveNodeArea->AddNode(NewNode);
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
		if (ImGui::MenuItem("Copy NodeID to clipboard"))
		{
			ImGui::SetClipboardText(HoveredNode->GetID().c_str());
		}

		if (HoveredNode->GetType() == "VisualReferenceNode")
		{
			VisualReferenceNode* ReferenceNode = static_cast<VisualReferenceNode*>(HoveredNode);
			if (ReferenceNode->GetReferencedArea() != nullptr)
			{
				if (ImGui::MenuItem("Open Reference Area"))
				{
					NODE_AREA_WINDOW_MANAGER.OpenNodeAreaWindow(ReferenceNode->GetReferencedArea());
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
				CurrentlyActiveNodeArea->DeleteNode(SelectedList[i]);
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