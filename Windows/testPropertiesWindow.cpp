#include "testPropertiesWindow.h"

testPropertiesWindow::testPropertiesWindow()
{
	std::string tempCaption = "Test properties";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());
}

testPropertiesWindow::~testPropertiesWindow()
{
}

void testPropertiesWindow::show()
{
}

void testPropertiesWindow::render()
{
	ImGui::Begin("Test Properties", nullptr, ImGuiWindowFlags_None);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	float dataXShift = ImGui::GetWindowContentRegionMax().x - 200.0f;
	if (TEST_MANAGER.getSelectedTest() != nullptr)
	{
		if (ImGui::BeginTable("properties", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Name");

			ImGui::TableNextColumn();
			ImGui::Text(TEST_MANAGER.getSelectedTest()->getName().c_str());

			ImGui::TableNextColumn();
			ImGui::Text("File path");

			ImGui::TableNextColumn();
			ImGui::Text(TEST_MANAGER.getSelectedTest()->filePath.c_str());

			ImGui::TableNextColumn();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5.0f));
			ImGui::Text("Speed factor");

			ImGui::TableNextColumn();
			float speedFactor = TEST_MANAGER.getSelectedTest()->getSpeedFactor();
			ImGui::DragFloat("##speedFactor", &speedFactor, 0.005f);
			TEST_MANAGER.getSelectedTest()->setSpeedFactor(speedFactor);

			ImGui::TableNextColumn();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5.0f));
			ImGui::Text("Loop count");

			int loopCount = TEST_MANAGER.getSelectedTest()->getLoopCount();
			ImGui::DragInt("##loopCount", &loopCount);
			TEST_MANAGER.getSelectedTest()->setLoopCount(loopCount);

			ImGui::TableNextColumn();
			ImGui::Text("Last run time");
			FETestResult* lastTestResult = TEST_MANAGER.getSelectedTest()->getLastTestResult();
			if (lastTestResult == nullptr)
			{
				ImGui::TableNextColumn();
				ImGui::Text("0 sec");
			}
			else
			{
				ImGui::TableNextColumn();
				std::string time = std::to_string((lastTestResult->endTime - lastTestResult->startTime) / 1000.0f);
				ImGui::Text((time + " sec").c_str());
			}

			ImGui::TableNextColumn();
			ImGui::Text("Last result");
			if (lastTestResult == nullptr)
			{
				ImGui::TableNextColumn();
				ImGui::Text("NONE");
			}
			else
			{
				ImGui::TableNextColumn();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 3.0f));
				lastTestResult->success ? ImGui::Text("Success") : ImGui::Text("Failed");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 4.0f));
				if (ImGui::Button("...", ImVec2(35.0f, 20.0f)))
				{
					failedTestWindow::GetInstance().show(lastTestResult);
				}
			}

			ImGui::TableNextColumn();
			ImGui::Text("Actions before test");

			ImGui::TableNextColumn();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 3.0f));
			ImGui::Text((std::to_string(TEST_MANAGER.getSelectedTest()->beforeStart.size()) + " Actions").c_str());
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 4.0f));
			ImGui::PushID("secondButton");
			if (ImGui::Button("...", ImVec2(35.0f, 20.0f)))
			{
				testStartPreparationsWindow::GetInstance().show();
			}
			ImGui::PopID();

			ImGui::EndTable();
		}

		static int macrosCount = 0;
		static char* macrosText = new char[2048];
		static char* macrosTextSecond = new char[2048];

		ImGui::Text("Macros:");
		if (ImGui::BeginTable("macros", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders,
			ImVec2(ImGui::GetWindowContentRegionMax().x - 40.0f, 25.0f * macrosCount)))
		{
			macrosCount = 0;
			int beginY = int(ImGui::GetCursorPos().y);

			auto currentMacro = TEST_MANAGER.getSelectedTest()->macrosToReplace.begin();
			while (currentMacro != TEST_MANAGER.getSelectedTest()->macrosToReplace.end())
			{
				ImGui::TableNextColumn();

				strcpy_s(macrosText, 2048, currentMacro->first.c_str());
				ImGui::InputText("##macrosText", macrosText, 2048);
				if (macrosText != currentMacro->first && strlen(macrosText) >= 3 && macrosText[0] == '$' && macrosText[1] == '(' && macrosText[strlen(macrosText) - 1] == ')')
				{
					std::string second = currentMacro->second;
					TEST_MANAGER.getSelectedTest()->macrosToReplace.erase(currentMacro->first);
					TEST_MANAGER.getSelectedTest()->addMacro(macrosText, second);
					break;
				}

				ImGui::TableNextColumn();
				strcpy_s(macrosTextSecond, 2048, currentMacro->second.c_str());
				ImGui::InputText("##macrosTextSecond", macrosTextSecond, 2048);
				if (macrosTextSecond != currentMacro->second && strlen(macrosTextSecond) > 1)
				{
					std::string first = currentMacro->first;
					TEST_MANAGER.getSelectedTest()->macrosToReplace.erase(currentMacro->first);
					TEST_MANAGER.getSelectedTest()->addMacro(first, macrosTextSecond);
					break;
				}

				currentMacro++;
				macrosCount++;
			}

			ImGui::EndTable();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 1.0f, 0.1f, 1.0f));

			for (size_t i = 0; i < size_t(macrosCount); i++)
			{
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 30.0f, beginY + i * 30.0f));
				ImGui::PushID(("button" + std::to_string(i)).c_str());
				if (ImGui::Button("-", ImVec2(25.0f, 25.0f)))
				{
					auto currentMacro = TEST_MANAGER.getSelectedTest()->macrosToReplace.begin();
					for (size_t j = 0; j < i; j++)
						currentMacro++;

					TEST_MANAGER.getSelectedTest()->macrosToReplace.erase(currentMacro->first);
				}
				ImGui::PopID();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, beginY + macrosCount * 28.0f + 10.0f));
			
			if (ImGui::Button("Add macro", ImVec2(125.0f, 25.0f)))
			{
				std::string macro = "$(MACRO_" + std::to_string(TEST_MANAGER.getSelectedTest()->macrosToReplace.size())  + ")";
				TEST_MANAGER.getSelectedTest()->addMacro(macro, "YOUR TEXT");
			}
		}
	}

	/*renderMainMenu();

	if (TEST_MANAGER.list.size() == 0)
		currentNodeArea = nullptr;

	currentTest = nullptr;

	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 8.0f));
	if (ImGui::BeginTabBar("##Test", ImGuiTabBarFlags_None))
	{
		for (size_t i = 0; i < TEST_MANAGER.list.size(); i++)
		{
			if (ImGui::BeginTabItem(TEST_MANAGER.list[i]->name.c_str()))
			{
				currentTest = TEST_MANAGER.list[i];
				currentNodeArea = TEST_MANAGER.list[i]->nodeArea;

				currentNodeArea->SetMainContextMenuFunc(mainContextMenu);
				currentNodeArea->SetNodeEventCallback(nodeCallback);

				currentNodeArea->SetAreaPosition(ImVec2(0, 50.0f));
				currentNodeArea->SetAreaSize(ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 50.0f));
				currentNodeArea->Update();

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	if (ImGui::GetIO().MouseReleased[1])
		mousePositionWhenContextMenuWasOpened = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y);*/

	ImGui::PopStyleVar();
	ImGui::End();
}