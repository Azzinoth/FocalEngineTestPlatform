#include "NodeRegionWindow.h"

ImVec2 NodeRegionWindow::MousePositionWhenContextMenuWasOpened = ImVec2(0, 0);
RegionNode* NodeRegionWindow::CurrentRegion = nullptr;

NodeRegionWindow::NodeRegionWindow()
{
	std::string TemporaryCaption = "node region area";
	strcpy_s(Caption, TemporaryCaption.size() + 1, TemporaryCaption.c_str());

	Size = ImVec2(1200, 800);
}

NodeRegionWindow::~NodeRegionWindow()
{
	if (CancelButton != nullptr)
		delete CancelButton;
}

void NodeRegionWindow::Show(RegionNode* Region)
{
	if (Region == nullptr)
		return;

	CurrentRegion = Region;
	FEImGuiWindow::Show();

	Flags |= ImGuiWindowFlags_NoScrollbar;

	if (CancelButton == nullptr)
	{
		CancelButton = new ImGuiButton("Cancel");
		CancelButton->SetPosition(ImVec2(Size.x / 2.0f - CancelButton->GetSize().x / 2, Size.y - 30));
		CancelButton->SetSize(ImVec2(80.0f, 25.0f));
		CancelButton->SetDefaultColor(ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
		CancelButton->SetHoveredColor(ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
		CancelButton->SetActiveColor(ImVec4(0.1f, 1.0f, 0.1f, 1.0f));
	}
}

void NodeRegionWindow::Render()
{
	FEImGuiWindow::Render();

	if (!IsVisible())
		return;

	if (CurrentRegion->GetData() != nullptr)
	{
		CurrentRegion->GetData()->SetMainContextMenuFunc(RenderMainContextMenu);
		CurrentRegion->GetData()->SetSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f));
		CurrentRegion->GetData()->Update();
	}

	ImGui::SetItemDefaultFocus();
	CancelButton->SetPosition(ImVec2(Size.x / 2.0f - CancelButton->GetSize().x / 2, Size.y - 30));
	CancelButton->Render();
	if (CancelButton->IsClicked())
		FEImGuiWindow::Close();

	if (ImGui::GetIO().MouseReleased[1])
		MousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y) - CurrentRegion->GetData()->GetRenderOffset();

	FEImGuiWindow::OnRenderEnd();
}

void NodeRegionWindow::RenderMainContextMenu()
{
	if (CurrentRegion == nullptr)
		return;

	if (CurrentRegion->GetData()->GetHovered() == nullptr && CurrentRegion->GetData()->GetSelected().size() == 0)
	{
		if (ImGui::BeginMenu("Add"))
		{
			ImGui::EndMenu();
		}
	}
}

void NodeRegionWindow::TextInputCallback(std::string Text)
{
	if (Text != "")
	{
	}
}