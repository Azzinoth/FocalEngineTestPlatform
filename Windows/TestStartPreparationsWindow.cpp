#include "TestStartPreparationsWindow.h"

TestStartPreparationsWindow::TestStartPreparationsWindow()
{
}

TestStartPreparationsWindow::~TestStartPreparationsWindow()
{
}

void TestStartPreparationsWindow::Show()
{
	if (TEST_MANAGER.GetSelectedTest() != nullptr)
		FEImGuiWindow::Show();
}

void TestStartPreparationsWindow::Render()
{
	if (!IsVisible() || TEST_MANAGER.GetSelectedTest() == nullptr)
		return;

	FEImGuiWindow::Render();

	ImGui::SetNextWindowSize(ImVec2(800, 800));
	ImGui::Begin("Actions taken before start of test.", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	//ImGui::ImageButton((void*)(intptr_t)texture->getTextureID(), size, uv0, uv1, framePadding, backgroundColor, tintColor);
	if (ImGui::Button("Add delete file action"))
	{
		FETestBeforeAction* NewAction = new FETestBeforeAction();
		NewAction->Type = FE_BEFORE_TEST_ACTION_DELETE_FILE;
		TEST_MANAGER.GetSelectedTest()->AddBeforeStartAction(NewAction);
	}

	if (ImGui::Button("Add delete folder action"))
	{
		FETestBeforeAction* NewAction = new FETestBeforeAction();
		NewAction->Type = FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY;
		TEST_MANAGER.GetSelectedTest()->AddBeforeStartAction(NewAction);
	}

	if (ImGui::BeginListBox("##Actions ListBox", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowContentRegionMax().y - 250.0f)))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

		ImVec2 PostionBeforeDraw = ImGui::GetCursorPos();
		for (size_t i = 0; i < TEST_MANAGER.GetSelectedTest()->BeforeStart.size(); i++)
		{
			ImGui::SetCursorPos(PostionBeforeDraw);
			ImGui::Text(FETest::FEBeforeTestActionTypeToString(TEST_MANAGER.GetSelectedTest()->BeforeStart[i]->Type).c_str());

			ImGui::SetCursorPos(PostionBeforeDraw);
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::Selectable("##item", SelectedAction == i ? true : false, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f)))
			{
				SelectedAction = static_cast<int>(i);
				strcpy_s(PathInputBuffer, TEST_MANAGER.GetSelectedTest()->BeforeStart[SelectedAction]->Path.size() + 1, TEST_MANAGER.GetSelectedTest()->BeforeStart[SelectedAction]->Path.c_str());
				TEST_MANAGER.GetSelectedTest()->BeforeStart[SelectedAction]->Path = PathInputBuffer;
			}
			ImGui::PopID();

			if (ImGui::IsItemHovered())
				HoveredAction = static_cast<int>(i);

			PostionBeforeDraw.y += 34.0f;
		}

		if (ImGui::IsMouseDoubleClicked(0) && SelectedAction != -1)
		{
		}

		ImGui::PopFont();
		ImGui::EndListBox();
	}

	ImVec2 PostionBeforeDraw = ImGui::GetCursorPos();
	ImGui::Text("Path: ");
	ImGui::SetNextItemWidth(600.0f);
	ImGui::InputText("##PathInput", PathInputBuffer, IM_ARRAYSIZE(PathInputBuffer));

	PostionBeforeDraw.x += 610.0f;
	PostionBeforeDraw.y += 19.0f;
	ImGui::SetCursorPos(PostionBeforeDraw);
	if (ImGui::Button("Update"))
	{
		if (SelectedAction >= 0 && SelectedAction < int(TEST_MANAGER.GetSelectedTest()->BeforeStart.size()))
		{
			TEST_MANAGER.GetSelectedTest()->BeforeStart[SelectedAction]->Path = PathInputBuffer;
		}
	}

	if (ImGui::Button("Close"))
	{
		FEImGuiWindow::Close();
	}

	ImGui::PopStyleVar();
	ImGui::End();
}