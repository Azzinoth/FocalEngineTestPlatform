#include "FailedTestWindow.h"

FailedTestWindow::FailedTestWindow()
{
	std::string TempCaption = "Test failed!";
	strcpy_s(Caption, TempCaption.size() + 1, TempCaption.c_str());
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

	std::string tempCaption = std::string("Test ") + "\"" + Result->Parent->GetName() +"\"" + " failed information";
	strcpy_s(Caption, tempCaption.size() + 1, tempCaption.c_str());

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

	ImGui::Text(("Fail action ID: " + Result->FailedAction->GetID()).c_str());
	ImGui::Text(("Fail reason: " + FETestResult::FETestFailReasonToString(Result->FailReason)).c_str());

	if (Result->FailReason == FE_TEST_FAIL_SCREENSHOOT_COMPARE)
	{
		if (Result->getScreenshotCompareResult() == nullptr)
		{
			ImGui::Text("Screenshot compare result is nullptr !");
		}
		else
		{
			ImGui::Text(("Similarity: " + std::to_string(Result->getScreenshotCompareResult()->Similarity)).c_str());
			glm::vec2 imageSize = SCREEN_SYSTEM.ImageSizeInRegion(Result->getScreenshotCompareResult()->Difference->GetWidth(), Result->getScreenshotCompareResult()->Difference->GetHeight(),
				size_t((ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x) * 0.925f),
				size_t((ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y) - 90.0f));

			imageSize.x = floor(imageSize.x);
			imageSize.y = floor(imageSize.y);

			ImVec2 imagePosition = ImVec2(ImGui::GetWindowContentRegionMin().x + 20, ImGui::GetWindowContentRegionMin().y + 60);

			ImGui::SetCursorPos(imagePosition);
			ImGui::Image(Result->getScreenshotCompareResult()->Difference->GetTextureID(),
						 ImVec2(imageSize.x, imageSize.y),
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