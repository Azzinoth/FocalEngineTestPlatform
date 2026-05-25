#include "FailedTestWindow.h"

FailedTestWindow::FailedTestWindow()
{
	std::string TemporaryCaption = "Test failed!";
	strcpy_s(Caption, TemporaryCaption.size() + 1, TemporaryCaption.c_str());
}

FailedTestWindow::~FailedTestWindow()
{
	if (OKButton != nullptr)
		delete OKButton;
}

void FailedTestWindow::Show(FETestResult* Result)
{
	if (Result == nullptr || Result->bIsSuccessful)
		return;

	this->Result = Result;

	Size = ImVec2(1024, 900);
	Position = ImVec2(0.0, 0.0);

	std::string TemporaryCaption = std::string("Test ") + "\"" + Result->Parent->GetName() +"\"" + " failed information";
	strcpy_s(Caption, TemporaryCaption.size() + 1, TemporaryCaption.c_str());

	Flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse;

	OKButton = new ImGuiButton("OK");
	OKButton->SetSize(ImVec2(100, 0));

	FEImGuiWindow::Show();
}

void FailedTestWindow::Render()
{
	FEImGuiWindow::Render();

	if (!IsVisible())
		return;

	if (Result->FailedAction != nullptr)
		ImGui::Text(("Fail action ID: " + Result->FailedAction->GetID()).c_str());

	ImGui::Text(("Fail reason: " + FETestResult::FETestFailReasonToString(Result->FailReason)).c_str());

	if (!Result->FailMessage.empty())
		ImGui::Text(("Fail message: " + Result->FailMessage).c_str());

	if (Result->FailReason == FE_TEST_FAIL_SCREENSHOOT_COMPARE)
	{
		if (Result->GetScreenshotCompareResult() == nullptr)
		{
			ImGui::Text("Screenshot compare result is nullptr !");
		}
		else
		{
			ImGui::Text(("Similarity: " + std::to_string(Result->GetScreenshotCompareResult()->Similarity)).c_str());
			glm::vec2 ImageSize = SCREEN_SYSTEM.ImageSizeInRegion(Result->GetScreenshotCompareResult()->Difference->GetWidth(), Result->GetScreenshotCompareResult()->Difference->GetHeight(),
				size_t((ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x) * 0.925f),
				size_t((ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y) - 90.0f));

			ImageSize.x = floor(ImageSize.x);
			ImageSize.y = floor(ImageSize.y);

			ImVec2 ImagePosition = ImVec2(ImGui::GetWindowContentRegionMin().x + 20, ImGui::GetWindowContentRegionMin().y + 60);

			ImGui::SetCursorPos(ImagePosition);
			ImGui::Image(Result->GetScreenshotCompareResult()->Difference->GetTextureID(),
						 ImVec2(ImageSize.x, ImageSize.y),
						 ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
		}
	}

	OKButton->Render();
	if (OKButton->IsClicked())
	{
		FEImGuiWindow::Close();
		return;
	}

	FEImGuiWindow::OnRenderEnd();
}