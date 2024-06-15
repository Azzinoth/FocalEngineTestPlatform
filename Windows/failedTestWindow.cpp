#include "failedTestWindow.h"

failedTestWindow* failedTestWindow::Instance = nullptr;

failedTestWindow::failedTestWindow()
{
	std::string tempCaption = "Test failed!";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());
}

failedTestWindow::~failedTestWindow()
{
	if (okButton != nullptr)
		delete okButton;
}

void failedTestWindow::show(FETestResult* result)
{
	if (result == nullptr || result->success)
		return;

	this->result = result;

	size = ImVec2(1024, 900);
	position = ImVec2(0.0, 0.0);

	std::string tempCaption = std::string("Test ") + "\"" + result->parent->getName() +"\"" + " failed information";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());

	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse;

	okButton = new ImGuiButton("OK");
	okButton->setSize(ImVec2(100, 0));

	FEImGuiWindow::show();
}

void failedTestWindow::render()
{
	FEImGuiWindow::render();

	if (!isVisible())
		return;

	ImGui::Text(("Fail action ID: " + result->failedAction->getID()).c_str());
	ImGui::Text(("Fail reason: " + FETestResult::FETestFailReasonToString(result->failReason)).c_str());

	if (result->failReason == FE_TEST_FAIL_SCREENSHOOT_COMPARE)
	{
		if (result->getScreenshootCompareResult() == nullptr)
		{
			ImGui::Text("Screenshoot compare result is nullptr !");
		}
		else
		{
			ImGui::Text(("simularity: " + std::to_string(result->getScreenshootCompareResult()->similarity)).c_str());
			glm::vec2 imageSize = SCREEN_SYSTEM.imageSizeInRegion(result->getScreenshootCompareResult()->difference->GetWidth(), result->getScreenshootCompareResult()->difference->GetHeight(),
				size_t((ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x) * 0.925f),
				size_t((ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y) - 90.0f));

			imageSize.x = floor(imageSize.x);
			imageSize.y = floor(imageSize.y);

			ImVec2 imagePosition = ImVec2(ImGui::GetWindowContentRegionMin().x + 20, ImGui::GetWindowContentRegionMin().y + 60);

			ImGui::SetCursorPos(imagePosition);
			ImGui::Image((void*)(intptr_t)result->getScreenshootCompareResult()->difference->GetTextureID(),
				ImVec2(imageSize.x, imageSize.y),
				ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
		}
	}

	okButton->render();
	if (okButton->getWasClicked())
	{
		FEImGuiWindow::close();
		return;
	}

	FEImGuiWindow::onRenderEnd();
}