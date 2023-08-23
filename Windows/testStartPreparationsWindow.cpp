#include "testStartPreparationsWindow.h"

testStartPreparationsWindow* testStartPreparationsWindow::Instance = nullptr;

testStartPreparationsWindow::testStartPreparationsWindow()
{
}

testStartPreparationsWindow::~testStartPreparationsWindow()
{
}

void testStartPreparationsWindow::show()
{
	if (TEST_MANAGER.getSelectedTest() != nullptr)
		FEImGuiWindow::show();
}

void testStartPreparationsWindow::render()
{
	if (!isVisible() || TEST_MANAGER.getSelectedTest() == nullptr)
		return;

	ImGui::SetNextWindowSize(ImVec2(800, 800));
	ImGui::Begin("Actions taken before start of test.", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	//ImGui::ImageButton((void*)(intptr_t)texture->getTextureID(), size, uv0, uv1, framePadding, backgroundColor, tintColor);
	if (ImGui::Button("Add delete file action"))
	{
		FETestBeforeAction* newAction = new FETestBeforeAction();
		newAction->type = FE_BEFORE_TEST_ACTION_DELETE_FILE;
		TEST_MANAGER.getSelectedTest()->addBeforeStartAction(newAction);
	}

	if (ImGui::Button("Add delete folder action"))
	{
		FETestBeforeAction* newAction = new FETestBeforeAction();
		newAction->type = FE_BEFORE_TEST_ACTION_DELETE_DIRECTORY;
		TEST_MANAGER.getSelectedTest()->addBeforeStartAction(newAction);
	}

	if (ImGui::BeginListBox("##Actions ListBox", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y - 250.0f)))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

		ImVec2 postionBeforeDraw = ImGui::GetCursorPos();
		for (size_t i = 0; i < TEST_MANAGER.getSelectedTest()->beforeStart.size(); i++)
		{
			ImGui::SetCursorPos(postionBeforeDraw);
			ImGui::Text(FETest::FEBeforeTestActionTypeToString(TEST_MANAGER.getSelectedTest()->beforeStart[i]->type).c_str());

			ImGui::SetCursorPos(postionBeforeDraw);
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::Selectable("##item", selectedAction == i ? true : false, ImGuiSelectableFlags_None, ImVec2(ImGui::GetWindowContentRegionWidth(), 32.0f)))
			{
				selectedAction = static_cast<int>(i);
				strcpy_s(pathInput, TEST_MANAGER.getSelectedTest()->beforeStart[selectedAction]->path.size() + 1, TEST_MANAGER.getSelectedTest()->beforeStart[selectedAction]->path.c_str());
				TEST_MANAGER.getSelectedTest()->beforeStart[selectedAction]->path = pathInput;
			}
			ImGui::PopID();

			if (ImGui::IsItemHovered())
				hoveredAction = static_cast<int>(i);

			postionBeforeDraw.y += 34.0f;
		}

		if (ImGui::IsMouseDoubleClicked(0) && selectedAction != -1)
		{
		}

		ImGui::PopFont();
		ImGui::EndListBox();
	}

	ImVec2 postionBeforeDraw = ImGui::GetCursorPos();
	ImGui::Text("Path: ");
	ImGui::SetNextItemWidth(600.0f);
	ImGui::InputText("##PathInput", pathInput, IM_ARRAYSIZE(pathInput));

	postionBeforeDraw.x += 610.0f;
	postionBeforeDraw.y += 19.0f;
	ImGui::SetCursorPos(postionBeforeDraw);
	if (ImGui::Button("Update"))
	{
		if (selectedAction >= 0 && selectedAction < int(TEST_MANAGER.getSelectedTest()->beforeStart.size()))
		{
			TEST_MANAGER.getSelectedTest()->beforeStart[selectedAction]->path = pathInput;
		}
	}

	if (ImGui::Button("Close"))
	{
		visible = false;
	}

	ImGui::PopStyleVar();
	ImGui::End();
}