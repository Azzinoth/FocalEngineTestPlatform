#include "testsOverviewWindow.h"

testsOverviewWindow* testsOverviewWindow::Instance = nullptr;
int testsOverviewWindow::hoveredTestIndex = -1;

testsOverviewWindow::testsOverviewWindow()
{
	std::string tempCaption = "main area";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());

	testSuccess = new FETPImage("Resources//testSuccess.png");
	testFailed = new FETPImage("Resources//testFailed.png");
	testUnKnown = new FETPImage("Resources//testUnKnown.png");
}

testsOverviewWindow::~testsOverviewWindow()
{
}

void testsOverviewWindow::show()
{

}

void testsOverviewWindow::render()
{
	ImGui::Begin("Tests overview", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	FETPImage* testResultIndicator = testUnKnown;
	if (ImGui::BeginListBox("##Tests ListBox", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowContentRegionMax().y - 30.0f)))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

		ImVec2 postionBeforeDraw = ImGui::GetCursorPos();
		size_t selectedTest = TEST_MANAGER.getSelectedTestIndex();
		for (size_t i = 0; i < TEST_MANAGER.list.size(); i++)
		{
			ImGui::SetCursorPos(postionBeforeDraw);

			FETestResult* lastTestResult = nullptr;
			lastTestResult = TEST_MANAGER.list[i]->getLastTestResult();
			if (lastTestResult != nullptr)
			{
				testResultIndicator = lastTestResult->success ? testSuccess : testFailed;
			}
			else
			{
				testResultIndicator = testUnKnown;
			}

			ImGui::Image((void*)(intptr_t)testResultIndicator->GetTextureID(), ImVec2(32.0f, 32.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
			ImGui::SameLine();
			ImGui::Text(TEST_MANAGER.list[i]->getName().c_str());

			ImGui::SetCursorPos(postionBeforeDraw);
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::Selectable("##item", selectedTest == i ? true : false, ImGuiSelectableFlags_None, ImVec2(ImGui::GetContentRegionAvail().x, 32.0f)))
			{
				selectedTest = i;
				TEST_MANAGER.setSelelectedTestIndex(selectedTest);
			}
			ImGui::PopID();

			if (ImGui::IsItemHovered())
				hoveredTestIndex = static_cast<int>(i);

			postionBeforeDraw.y += 34.0f;
		}

		if (ImGui::IsMouseDoubleClicked(0) && selectedTest != -1)
		{
			//testList[selectedTest].window->show();
			//ACTION_SYSTEM.setNodeAreaToUse(TEST_MANAGER.testList[selectedTest].nodeArea);
			//ACTION_SYSTEM.load(TEST_MANAGER.testList[selectedTest].filePath.c_str());

			/*testEditorWinow::getInstance().show();
			ACTION_SYSTEM.setNodeAreaToUse(testEditorWinow::getInstance().currentNodeArea);
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

void testsOverviewWindow::renderContextMenu()
{
	bool isListBoxHovered = false;
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
		isListBoxHovered = true;

	if (ImGui::IsMouseClicked(1))
	{
		if (isListBoxHovered || hoveredTestIndex != -1)
			openContextMenu = true;
	}

	if (openContextMenu)
	{
		openContextMenu = false;
		ImGui::OpenPopup("##tests_listBox_context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("##tests_listBox_context_menu"))
	{
		contextMenuOpened = true;

		if (hoveredTestIndex != -1)
		{
			if (ImGui::MenuItem("Rename"))
			{
				textInputPopup::getInstance().show(textInputCallback, TEST_MANAGER.getSelectedTest()->getName());
			}

			if (ImGui::MenuItem("Remove"))
			{
				TEST_MANAGER.removeTest(hoveredTestIndex);
			}

			if (ImGui::MenuItem("Run"))
			{
				ACTION_SYSTEM.run(TEST_MANAGER.list[hoveredTestIndex]);
			}
		}
		else
		{
			if (ImGui::MenuItem("Add new test"))
			{
				TEST_MANAGER.addTest();
				TEST_MANAGER.setSelelectedTestIndex(TEST_MANAGER.list.size() - 1);
			}

			if (TEST_MANAGER.list.size() > 0)
			{
				if (ImGui::MenuItem("Run all"))
				{
					for (size_t i = 0; i < TEST_MANAGER.list.size(); i++)
					{
						for (size_t j = 0; j < TEST_MANAGER.list[i]->results.size(); j++)
						{
							delete TEST_MANAGER.list[i]->results[j];
						}
						TEST_MANAGER.list[i]->results.clear();
					}

					for (size_t i = 0; i < TEST_MANAGER.list.size(); i++)
					{
						bool result = ACTION_SYSTEM.run(TEST_MANAGER.list[i]);
						Sleep(250);
					}
				}
			}
		}

		ImGui::EndPopup();
	}

	if (!contextMenuOpened && !textInputPopup::getInstance().isOpened())
		hoveredTestIndex = -1;

	contextMenuOpened = false;

	ImGui::PopStyleVar();
}

void testsOverviewWindow::renderMainMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load test set..."))
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, FETestsFileFilter, 1);

				if (path != "")
				{
					TEST_MANAGER.openTestSet(path);
				}
			}

			if (TEST_MANAGER.list.size() == 0)
				ImGui::BeginDisabled();

			if (ImGui::MenuItem("Save test set...") && TEST_MANAGER.list.size() > 0)
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileSaveDialog(path, FETestsFileFilter, 1);

				if (path != "")
				{
					TEST_MANAGER.saveAsTestSet(path);
				}
			}

			if (TEST_MANAGER.list.size() == 0)
				ImGui::EndDisabled();

			ImGui::Separator();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleVar();
}

void testsOverviewWindow::textInputCallback(std::string text)
{
	if (text != "")
		TEST_MANAGER.renameTest(hoveredTestIndex, text);
}