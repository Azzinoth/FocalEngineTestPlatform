#include "PreviewWindow.h"
using namespace VisNodeSys;

ImVec2 PreviewWindow::NodeGridRelativePosition = ImVec2(0, 0);
ImVec2 PreviewWindow::WindowPosition = ImVec2(0, 0);
ImVec2 PreviewWindow::MousePositionWhenContextMenuWasOpened = ImVec2(0, 0);
NodeArea* PreviewWindow::CurrentNodeArea = nullptr;
ImVec2 PreviewWindow::NeededShift = ImVec2(0, 0);
bool PreviewWindow::bIsReadOnly = false;

PreviewWindow::PreviewWindow()
{
	PopupCaption = "Preview of nodes to add";
}

PreviewWindow::~PreviewWindow()
{
	if (CancelButton != nullptr)
		delete CancelButton;

	if (AddButton != nullptr)
		delete AddButton;

	if (NewTestButton != nullptr)
		delete NewTestButton;
}

void PreviewWindow::Show(bool bIsReadOnlyIn)
{
	bIsReadOnly = bIsReadOnlyIn;
	bShouldOpen = true;
	bIsFirstFrame = true;

	if (CurrentNodeArea == nullptr)
	{
		CurrentNodeArea = NODE_SYSTEM.CreateNodeArea();
		CurrentNodeArea->SetMainContextMenuFunc(RenderMainContextMenu);
		CurrentNodeArea->AddNodeEventCallback(NodeCallback);

		CancelButton = new ImGuiButton("Cancel");
		CancelButton->SetPosition(ImVec2(PopupSize.x - PopupSize.x / 6.0f - CancelButton->GetSize().x / 2.0f, PopupSize.y - 30.0f));
		CancelButton->SetSize(ImVec2(80.0f, 25.0f));
		CancelButton->SetDefaultColor(ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
		CancelButton->SetHoveredColor(ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
		CancelButton->SetActiveColor(ImVec4(0.1f, 1.0f, 0.1f, 1.0f));

		AddButton = new ImGuiButton("Add to current test");
		AddButton->SetPosition(ImVec2(PopupSize.x * 0.33f - PopupSize.x / 6.0f - AddButton->GetSize().x / 2.0f, PopupSize.y - 30.0f));
		AddButton->SetSize(ImVec2(180.0f, 25.0f));

		NewTestButton = new ImGuiButton("Add as new test");
		NewTestButton->SetPosition(ImVec2(PopupSize.x * 0.66f - PopupSize.x / 6.0f - NewTestButton->GetSize().x / 2.0f, PopupSize.y - 30.0f));
		NewTestButton->SetSize(ImVec2(150.0f, 25.0f));
	}

	if (bIsReadOnly)
		CancelButton->SetPosition(ImVec2(PopupSize.x / 2.0f - CancelButton->GetSize().x / 2.0f, PopupSize.y - 30.0f));
}

void PreviewWindow::Close()
{
	ImGuiModalPopup::Close();
	NODE_SYSTEM.DeleteNodeArea(CurrentNodeArea);
	CurrentNodeArea = nullptr;
}

void PreviewWindow::Render()
{
	if (CurrentNodeArea == nullptr)
		return;

	ImGuiModalPopup::Render();

	ImGui::SetNextWindowSize(PopupSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginPopupModal(PopupCaption.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
	{
		int Width, Height;
		FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);

		ImGui::SetWindowPos(ImVec2(Width / 2 - PopupSize.x / 2.0f, Height / 2 - PopupSize.y / 2.0f));

		CurrentNodeArea->SetPosition(NodeGridRelativePosition);
		CurrentNodeArea->SetSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f));

		if (bIsFirstFrame)
		{
			bIsFirstFrame = false;
			PositionNodesInCenter();
		}

		CurrentNodeArea->Update();

		if (ImGui::GetIO().MouseReleased[1])
			MousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y) - CurrentNodeArea->GetRenderOffset();

		if (!bIsReadOnly)
		{
			if (TEST_MANAGER.GetSelectedTest() == nullptr)
				ImGui::BeginDisabled();

			AddButton->Render();
			if (AddButton->IsClicked() && TEST_MANAGER.GetSelectedTest() != nullptr)
			{
				// Shift all nodes in order to place them in view center of selected test node area.
				PositionNodesInTargetCenter();

				NODE_SYSTEM.MoveNodesTo(CurrentNodeArea, TEST_MANAGER.GetSelectedTest()->NodeArea, true);
				Close();
			}

			if (TEST_MANAGER.GetSelectedTest() == nullptr)
				ImGui::EndDisabled();

			NewTestButton->Render();
			if (NewTestButton->IsClicked())
			{
				TEST_MANAGER.AddTest();

				NODE_SYSTEM.MoveNodesTo(CurrentNodeArea, TEST_MANAGER.Tests.back()->NodeArea);
				Close();
			}
		}

		ImGui::SetItemDefaultFocus();
		CancelButton->Render();
		if (CancelButton->IsClicked())
		{
			CurrentNodeArea->Clear();
			Close();
		}

		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}

void PreviewWindow::RenderMainContextMenu()
{
	if (bIsReadOnly)
		return;

	if (ImGui::BeginMenu("Add"))
	{
		ImGui::EndMenu();
	}
}

void PreviewWindow::NodeCallback(VisNodeSys::Node* Node, VisNodeSys::NODE_EVENT CurrentNodeEvent)
{

}

void PreviewWindow::PositionNodesInCenter()
{
	ImVec2 ViewCenter = CurrentNodeArea->GetRenderedViewCenter();
	ImVec2 NodesAABBCenter = CurrentNodeArea->GetAllElementsAABBCenter();

	NeededShift = ViewCenter - NodesAABBCenter;

	CurrentNodeArea->RunOnEachNode([](VisNodeSys::Node* Node) {
		size_t OutSocketCount = Node->GetOutputSocketCount();
		Node->SetPosition(Node->GetPosition() + NeededShift);
	});

	NeededShift = ImVec2(0, 0);
}

void PreviewWindow::PositionNodesInTargetCenter()
{
	if (TEST_MANAGER.GetSelectedTest() == nullptr)
		return;

	ImVec2 ViewCenter = TEST_MANAGER.GetSelectedTest()->NodeArea->GetRenderedViewCenter();
	ImVec2 NodesAABBCenter = CurrentNodeArea->GetAllElementsAABBCenter();
	NodesAABBCenter -= CurrentNodeArea->GetRenderOffset();

	NeededShift = ViewCenter - NodesAABBCenter;

	CurrentNodeArea->RunOnEachNode([](VisNodeSys::Node* Node) {
		size_t OutSocketCount = Node->GetOutputSocketCount();
		Node->SetPosition(Node->GetPosition() + NeededShift);
	});

	NeededShift = ImVec2(0, 0);
}