#include "TestsOverviewWindow.h"
#include "TextInputPopup.h"
#include "ActionEditPopup.h"
int TestsOverviewWindow::HoveredTestIndex = -1;

TestsOverviewWindow::TestsOverviewWindow()
{
	std::string TemporaryCaption = "main area";
	strcpy_s(Caption, TemporaryCaption.size() + 1, TemporaryCaption.c_str());

	TestSuccess = new FETPImage("Resources//testSuccess.png");
	TestFailed = new FETPImage("Resources//testFailed.png");
	TestUnknown = new FETPImage("Resources//testUnKnown.png");
}

TestsOverviewWindow::~TestsOverviewWindow()
{
}

void TestsOverviewWindow::Show()
{

}

void TestsOverviewWindow::Render()
{
	ImGui::Begin("Tests overview", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	FETPImage* TestResultIndicator = TestUnknown;
	if (ImGui::BeginListBox("##Tests ListBox", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowContentRegionMax().y - 30.0f)))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

		ImVec2 PostionBeforeDraw = ImGui::GetCursorPos();
		size_t SelectedTest = TEST_MANAGER.GetSelectedTestIndex();
		for (size_t i = 0; i < TEST_MANAGER.Tests.size(); i++)
		{
			ImGui::SetCursorPos(PostionBeforeDraw);

			FETestResult* LastTestResult = nullptr;
			LastTestResult = TEST_MANAGER.Tests[i]->GetLastTestResult();
			if (LastTestResult != nullptr)
			{
				TestResultIndicator = LastTestResult->bIsSuccessful ? TestSuccess : TestFailed;
			}
			else
			{
				TestResultIndicator = TestUnknown;
			}

			ImGui::Image(TestResultIndicator->GetTextureID(), ImVec2(32.0f, 32.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
			ImGui::SameLine();
			ImGui::Text(TEST_MANAGER.Tests[i]->GetName().c_str());

			ImGui::SetCursorPos(PostionBeforeDraw);
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::Selectable("##item", SelectedTest == i ? true : false, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f)))
			{
				SelectedTest = i;
				TEST_MANAGER.SetSelectedTestIndex(SelectedTest);
			}
			ImGui::PopID();

			if (ImGui::IsItemHovered())
				HoveredTestIndex = static_cast<int>(i);

			PostionBeforeDraw.y += 34.0f;
		}

		if (ImGui::IsMouseDoubleClicked(0) && SelectedTest != -1)
		{
			//testList[selectedTest].window->show();
			//ACTION_SYSTEM.setNodeAreaToUse(TEST_MANAGER.testList[selectedTest].nodeArea);
			//ACTION_SYSTEM.load(TEST_MANAGER.testList[selectedTest].filePath.c_str());

			/*testEditorWindow::GetInstance().show();
			ACTION_SYSTEM.setNodeAreaToUse(testEditorWindow::GetInstance().currentNodeArea);
			ACTION_SYSTEM.load(testList[selectedTest].filePath.c_str());*/
		}

		ImGui::PopFont();

		renderContextMenu();
		ImGui::EndListBox();
	}

	renderMainMenu();

	ImGui::PopStyleVar();
	ImGui::End();
}

void TestsOverviewWindow::renderContextMenu()
{
	bool bIsListBoxHovered = false;
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
		bIsListBoxHovered = true;

	if (ImGui::IsMouseClicked(1))
	{
		if (bIsListBoxHovered || HoveredTestIndex != -1)
			bShouldOpenContextMenu = true;
	}

	if (bShouldOpenContextMenu)
	{
		bShouldOpenContextMenu = false;
		ImGui::OpenPopup("##tests_listBox_context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("##tests_listBox_context_menu"))
	{
		bContextMenuOpened = true;

		if (HoveredTestIndex != -1)
		{
			if (ImGui::MenuItem("Rename"))
			{
				TextInputPopup::GetInstance().Show(TextInputCallback, TEST_MANAGER.GetSelectedTest()->GetName());
			}

			if (ImGui::MenuItem("Remove"))
			{
				TEST_MANAGER.RemoveTest(HoveredTestIndex);
			}

			if (ImGui::MenuItem("Run"))
			{
				ACTION_SYSTEM.Run(TEST_MANAGER.Tests[HoveredTestIndex]);
			}
		}
		else
		{
			if (ImGui::MenuItem("Add new test"))
			{
				TEST_MANAGER.AddTest();
				TEST_MANAGER.SetSelectedTestIndex(TEST_MANAGER.Tests.size() - 1);
			}

			if (TEST_MANAGER.Tests.size() > 0)
			{
				if (ImGui::MenuItem("Run all"))
				{
					for (size_t i = 0; i < TEST_MANAGER.Tests.size(); i++)
					{
						for (size_t j = 0; j < TEST_MANAGER.Tests[i]->Results.size(); j++)
						{
							delete TEST_MANAGER.Tests[i]->Results[j];
						}
						TEST_MANAGER.Tests[i]->Results.clear();
					}

					for (size_t i = 0; i < TEST_MANAGER.Tests.size(); i++)
					{
						bool result = ACTION_SYSTEM.Run(TEST_MANAGER.Tests[i]);
						Sleep(250);
					}
				}
			}
		}

		ImGui::EndPopup();
	}

	if (!bContextMenuOpened && !TextInputPopup::GetInstance().IsOpened())
		HoveredTestIndex = -1;

	bContextMenuOpened = false;

	ImGui::PopStyleVar();
}

void TestsOverviewWindow::renderMainMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load test set..."))
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(Path, FETestsFileFilter, 1);

				if (!Path.empty())
					TEST_MANAGER.OpenTestSet(Path);
			}

			if (TEST_MANAGER.Tests.size() == 0)
				ImGui::BeginDisabled();

			if (ImGui::MenuItem("Save test set...") && TEST_MANAGER.Tests.size() > 0)
			{
				std::string Path;
				FocalEngine::FILE_SYSTEM.ShowFileSaveDialog(Path, FETestsFileFilter, 1);

				if (!Path.empty())
					TEST_MANAGER.SaveAsTestSet(Path);
			}

			if (TEST_MANAGER.Tests.size() == 0)
				ImGui::EndDisabled();

			ImGui::Separator();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleVar();
}

void TestsOverviewWindow::TextInputCallback(std::string Text)
{
	if (!Text.empty())
		TEST_MANAGER.RenameTest(HoveredTestIndex, Text);
}