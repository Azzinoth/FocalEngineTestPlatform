#include "TestPropertiesWindow.h"

TestPropertiesWindow::TestPropertiesWindow()
{
	std::string TemporaryCaption = "Test properties";
	strcpy_s(Caption, TemporaryCaption.size() + 1, TemporaryCaption.c_str());
}

TestPropertiesWindow::~TestPropertiesWindow()
{
}

void TestPropertiesWindow::Show()
{
}

void TestPropertiesWindow::Render()
{
	ImGui::Begin("Test Properties", nullptr, ImGuiWindowFlags_None);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	float DataXShift = ImGui::GetWindowContentRegionMax().x - 200.0f;
	if (TEST_MANAGER.GetSelectedTest() != nullptr)
	{
		if (ImGui::BeginTable("properties", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Name");

			ImGui::TableNextColumn();
			ImGui::Text(TEST_MANAGER.GetSelectedTest()->GetName().c_str());

			ImGui::TableNextColumn();
			ImGui::Text("File path");

			ImGui::TableNextColumn();
			ImGui::Text(TEST_MANAGER.GetSelectedTest()->FilePath.c_str());

			ImGui::TableNextColumn();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5.0f));
			ImGui::Text("Speed factor");

			ImGui::TableNextColumn();
			float SpeedFactor = TEST_MANAGER.GetSelectedTest()->GetSpeedFactor();
			ImGui::DragFloat("##speedFactor", &SpeedFactor, 0.005f);
			TEST_MANAGER.GetSelectedTest()->SetSpeedFactor(SpeedFactor);

			ImGui::TableNextColumn();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5.0f));
			ImGui::Text("Loop count");

			int LoopCount = TEST_MANAGER.GetSelectedTest()->GetLoopCount();
			ImGui::DragInt("##loopCount", &LoopCount);
			TEST_MANAGER.GetSelectedTest()->SetLoopCount(LoopCount);

			ImGui::TableNextColumn();
			ImGui::Text("Last run time");
			FETestResult* LastTestResult = TEST_MANAGER.GetSelectedTest()->GetLastTestResult();
			if (LastTestResult == nullptr)
			{
				ImGui::TableNextColumn();
				ImGui::Text("0 sec");
			}
			else
			{
				ImGui::TableNextColumn();
				std::string Time = std::to_string((LastTestResult->EndTime - LastTestResult->StartTime) / 1000.0f);
				ImGui::Text((Time + " sec").c_str());
			}

			ImGui::TableNextColumn();
			ImGui::Text("Last result");
			if (LastTestResult == nullptr)
			{
				ImGui::TableNextColumn();
				ImGui::Text("NONE");
			}
			else
			{
				ImGui::TableNextColumn();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 3.0f));
				LastTestResult->bIsSuccessful ? ImGui::Text("Success") : ImGui::Text("Failed");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 4.0f));
				if (ImGui::Button("...", ImVec2(35.0f, 20.0f)))
				{
					FailedTestWindow::GetInstance().Show(LastTestResult);
				}
			}

			ImGui::TableNextColumn();
			ImGui::Text("Actions before test");

			ImGui::TableNextColumn();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 3.0f));
			ImGui::Text((std::to_string(TEST_MANAGER.GetSelectedTest()->BeforeStart.size()) + " Actions").c_str());
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 4.0f));
			ImGui::PushID("secondButton");
			if (ImGui::Button("...", ImVec2(35.0f, 20.0f)))
			{
				TestStartPreparationsWindow::GetInstance().Show();
			}
			ImGui::PopID();

			ImGui::EndTable();
		}

		static int MacrosCount = 0;
		static char* MacrosText = new char[2048];
		static char* MacrosTextSecond = new char[2048];

		ImGui::Text("Macros:");
		if (ImGui::BeginTable("macros", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders,
			ImVec2(ImGui::GetWindowContentRegionMax().x - 40.0f, 25.0f * MacrosCount)))
		{
			MacrosCount = 0;
			int BeginY = int(ImGui::GetCursorPos().y);

			auto CurrentMacro = TEST_MANAGER.GetSelectedTest()->MacrosToReplace.begin();
			while (CurrentMacro != TEST_MANAGER.GetSelectedTest()->MacrosToReplace.end())
			{
				ImGui::TableNextColumn();

				strcpy_s(MacrosText, 2048, CurrentMacro->first.c_str());
				ImGui::InputText("##macrosText", MacrosText, 2048);
				if (MacrosText != CurrentMacro->first && strlen(MacrosText) >= 3 && MacrosText[0] == '$' && MacrosText[1] == '(' && MacrosText[strlen(MacrosText) - 1] == ')')
				{
					std::string Second = CurrentMacro->second;
					TEST_MANAGER.GetSelectedTest()->MacrosToReplace.erase(CurrentMacro->first);
					TEST_MANAGER.GetSelectedTest()->AddMacro(MacrosText, Second);
					break;
				}

				ImGui::TableNextColumn();
				strcpy_s(MacrosTextSecond, 2048, CurrentMacro->second.c_str());
				ImGui::InputText("##macrosTextSecond", MacrosTextSecond, 2048);
				if (MacrosTextSecond != CurrentMacro->second && strlen(MacrosTextSecond) > 1)
				{
					std::string First = CurrentMacro->first;
					TEST_MANAGER.GetSelectedTest()->MacrosToReplace.erase(CurrentMacro->first);
					TEST_MANAGER.GetSelectedTest()->AddMacro(First, MacrosTextSecond);
					break;
				}

				CurrentMacro++;
				MacrosCount++;
			}

			ImGui::EndTable();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 1.0f, 0.1f, 1.0f));

			for (size_t i = 0; i < size_t(MacrosCount); i++)
			{
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 30.0f, BeginY + i * 30.0f));
				ImGui::PushID(("button" + std::to_string(i)).c_str());
				if (ImGui::Button("-", ImVec2(25.0f, 25.0f)))
				{
					auto CurrentMacro = TEST_MANAGER.GetSelectedTest()->MacrosToReplace.begin();
					for (size_t j = 0; j < i; j++)
						CurrentMacro++;

					TEST_MANAGER.GetSelectedTest()->MacrosToReplace.erase(CurrentMacro->first);
				}
				ImGui::PopID();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, BeginY + MacrosCount * 28.0f + 10.0f));
			
			if (ImGui::Button("Add macro", ImVec2(125.0f, 25.0f)))
			{
				std::string Macros = "$(MACRO_" + std::to_string(TEST_MANAGER.GetSelectedTest()->MacrosToReplace.size())  + ")";
				TEST_MANAGER.GetSelectedTest()->AddMacro(Macros, "YOUR TEXT");
			}
		}
	}

	ImGui::PopStyleVar();
	ImGui::End();
}