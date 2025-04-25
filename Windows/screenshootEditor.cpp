#include "screenshootEditor.h"
using namespace VisNodeSys;

screenshootEditor::screenshootEditor()
{
	std::string tempCaption = "Screenshoot Editor";
	strcpy_s(caption, tempCaption.size() + 1, tempCaption.c_str());
}

screenshootEditor::~screenshootEditor()
{
	if (cutButton != nullptr)
		delete cutButton;

	if (useCompleteButton != nullptr)
		delete useCompleteButton;

	if (loadButton != nullptr)
		delete loadButton;

	if (cancelButton != nullptr)
		delete cancelButton;

	if (addImageVariantButton != nullptr)
		delete addImageVariantButton;

	if (deleteImageVariantButton != nullptr)
		delete deleteImageVariantButton;
}

void screenshootEditor::show(ScreenshootCompareAction* action)
{
	if (action == nullptr)
		return;

	currentAction = action;

	size = ImVec2(1024, 900);
	position = ImVec2(0.0, 0.0);

	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

	cutButton = new ImGuiButton("Use selected region");
	cutButton->setSize(ImVec2(170, 30));

	useCompleteButton = new ImGuiButton("Use complete screenshoot");
	useCompleteButton->setSize(ImVec2(220, 30));

	loadButton = new ImGuiButton("Load different png...");
	loadButton->setSize(ImVec2(190, 30));

	cancelButton = new ImGuiButton("Cancel");
	cancelButton->setSize(ImVec2(100, 30));
	cancelButton->setDefaultColor(ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
	cancelButton->setHoveredColor(ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
	cancelButton->setActiveColor(ImVec4(0.1f, 1.0f, 0.1f, 1.0f));

	addImageVariantButton = new ImGuiButton("Add variant...");
	addImageVariantButton->setSize(ImVec2(150, 30));
	addImageVariantButton->setPosition(ImVec2(20.0f, 29.0f));

	deleteImageVariantButton = new ImGuiButton("Delete");
	deleteImageVariantButton->setSize(ImVec2(120, 30));
	deleteImageVariantButton->setDefaultColor(ImVec4(0.7f, 0.5f, 0.5f, 1.0f));
	deleteImageVariantButton->setHoveredColor(ImVec4(0.95f, 0.5f, 0.0f, 1.0f));
	deleteImageVariantButton->setActiveColor(ImVec4(0.1f, 1.0f, 0.1f, 1.0f));
	deleteImageVariantButton->setPosition(ImVec2(addImageVariantButton->getPosition().x + addImageVariantButton->getSize().x + 10.0f, 29.0f));

	FEImGuiWindow::show();

	firstFrame = true;
	subImageIndex = 0;
}

void screenshootEditor::render()
{
	FEImGuiWindow::render();

	if (!isVisible())
		return;

	addImageVariantButton->render();
	if (addImageVariantButton->getWasClicked())
		addNewImageVariant();

	deleteImageVariantButton->render();
	if (deleteImageVariantButton->getWasClicked())
		deleteCurrentImageVariant();

	GUIComboboxImageVariant();
	GUIInputIntThreshold();
	GUISeveralAttemps();
	
	if (currentAction->imagesInfo[subImageIndex]->partialImage != nullptr)
	{
		ImGui::SetCursorPos(ImVec2(deleteImageVariantButton->getPosition().x + deleteImageVariantButton->getSize().x + 485.0f, 32.0f));
		bool currentSearchStatus = currentAction->imagesInfo[subImageIndex]->screenSearch != nullptr;
		bool oldStatus = currentSearchStatus;
		ImGui::Checkbox("search for partial image", &currentSearchStatus);
		if (oldStatus != currentSearchStatus)
		{
			if (!currentSearchStatus)
			{
				delete currentAction->imagesInfo[subImageIndex]->screenSearch;
				currentAction->imagesInfo[subImageIndex]->screenSearch = nullptr;
			}
			else
			{
				currentAction->imagesInfo[subImageIndex]->screenSearch = new screenSearchInfo();
				//currentAction->imagesInfo[subImageIndex]->screenSearchsetScreenRegion(ImVec2 ScreenRegionMin, ImVec2 ScreenRegionMax);
			}
		}

		if (currentAction->imagesInfo[subImageIndex]->screenSearch != nullptr)
		{
			int tempX = currentAction->imagesInfo[subImageIndex]->screenSearch->getXShiftFromFound();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::SetCursorPos(ImVec2(addImageVariantButton->getPosition().x + 380.0f, 70.0f));
			ImGui::InputInt("xShiftFromFound", &tempX);
			currentAction->imagesInfo[subImageIndex]->screenSearch->setXShiftFromFound(tempX);

			int tempY = currentAction->imagesInfo[subImageIndex]->screenSearch->getYShiftFromFound();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::SetCursorPos(ImVec2(addImageVariantButton->getPosition().x + 380.0f + 80.0f + 10.0f + 150.0f, 70.0f));
			ImGui::InputInt("yShiftFromFound", &tempY);
			currentAction->imagesInfo[subImageIndex]->screenSearch->setYShiftFromFound(tempY);

			bool bUseGPU = currentAction->bUseGPU;
			ImGui::Checkbox("Use GPU", &bUseGPU);
			currentAction->bUseGPU = bUseGPU;
		}
	}

	if (firstFrame)
	{
		firstFrame = false;
		windowContentRegionMin = ImGui::GetWindowContentRegionMin();
		windowContentRegionMax = ImGui::GetWindowContentRegionMax();
		changeCurrentImageVariant(subImageIndex);
	}

	if (subImageIndex < currentAction->imagesInfo.size() &&
		currentAction->imagesInfo[subImageIndex]->image->GetWidth() != 0 &&
		currentAction->imagesInfo[subImageIndex]->image->GetHeight() != 0)
	{
		compareImageInfo* currentImageInfo = currentAction->imagesInfo[subImageIndex];
		ImGui::SetCursorPos(imagePosition);
		ImGui::Image((void*)(intptr_t)currentImageInfo->image->GetTextureID(),
					 ImVec2(imageSize.x, imageSize.y),
					 ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

		float resizeRatio = imageSize.x / currentImageInfo->image->GetWidth();
		float twoPixelsResized = 2.0f * resizeRatio;
		// Left scroller.
		left.setPosition(ImVec2(left.getPosition().x, imagePosition.y));
		left.setAvailableRange(ImVec2(imagePosition.x, right.getPosition().x - twoPixelsResized));
		left.render();

		// Right scroller.
		right.setPosition(ImVec2(right.getPosition().x, imagePosition.y));
		right.setAvailableRange(ImVec2(left.getPosition().x + twoPixelsResized, imagePosition.x + imageSize.x));
		right.render();

		// Top scroller.
		top.setPosition(ImVec2(imagePosition.x, top.getPosition().y));
		top.setAvailableRange(ImVec2(imagePosition.y, bottom.getPosition().y - twoPixelsResized));
		top.render();

		// Bottom scroller.
		bottom.setPosition(ImVec2(imagePosition.x, bottom.getPosition().y));
		bottom.setAvailableRange(ImVec2(top.getPosition().y + twoPixelsResized, imagePosition.y + imageSize.y));
		bottom.render();

		// Show result image rect.
		ImVec2 imageRegionMin = ImVec2(ImGui::GetCurrentWindow()->Pos.x + left.getPosition().x, ImGui::GetCurrentWindow()->Pos.y + top.getPosition().y);
		ImVec2 imageRegionMax = ImVec2(ImGui::GetCurrentWindow()->Pos.x + right.getPosition().x, ImGui::GetCurrentWindow()->Pos.y + bottom.getPosition().y);
		ImGui::GetWindowDrawList()->AddRectFilled(imageRegionMin, imageRegionMax, IM_COL32(75, 75, 175, 125), 2.0f);
	}

	ImGui::SetCursorPos(ImVec2(size.x / 4.0f - size.x / 8.0f - cutButton->getSize().x / 2.0f, ImGui::GetWindowHeight() - 35.0f));
	cutButton->render();
	if (cutButton->getWasClicked())
		cutOutPartialImage();

	ImGui::SetCursorPos(ImVec2(size.x / 4.0f + size.x / 8.0f - useCompleteButton->getSize().x / 2.0f, ImGui::GetWindowHeight() - 35.0f));
	useCompleteButton->render();
	if (useCompleteButton->getWasClicked())
		useCompleteImage();

	ImGui::SetCursorPos(ImVec2(size.x / 2.0f + size.x / 8.0f - loadButton->getSize().x / 2.0f, ImGui::GetWindowHeight() - 35.0f));
	loadButton->render();
	if (loadButton->getWasClicked())
		loadPNGToCurrent();

	ImGui::SetCursorPos(ImVec2(size.x / 2.0f + size.x / 4.0f + size.x / 8.0f - cancelButton->getSize().x / 2.0f, ImGui::GetWindowHeight() - 35.0f));
	cancelButton->render();
	if (cancelButton->getWasClicked())
	{
		FEImGuiWindow::close();
		return;
	}

	FEImGuiWindow::onRenderEnd();
}

void screenshootEditor::addNewImageVariant()
{
	std::string path;
	FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(path, pngLoadFilter, 1);

	if (path != "")
	{
		std::vector<unsigned char> rawData;
		unsigned uWidth, uHeight;
		int error = lodepng::decode(rawData, uWidth, uHeight, path);

		if (error == 0)
		{
			compareImageInfo* newImageInfo = new compareImageInfo(new FETPImage(rawData.data(), uWidth, uHeight));
			currentAction->imagesInfo.push_back(newImageInfo);
			changeCurrentImageVariant(currentAction->imagesInfo.size() - 1);

			replaceCurrentNode();
		}
	}
}

void screenshootEditor::deleteCurrentImageVariant()
{
	if (subImageIndex == 0 || subImageIndex >= currentAction->imagesInfo.size())
		return;

	currentAction->imagesInfo.erase(currentAction->imagesInfo.begin() + subImageIndex);
	changeCurrentImageVariant(0);

	replaceCurrentNode();
}

void screenshootEditor::changeCurrentImageVariant(size_t newValue)
{
	if (newValue >= currentAction->imagesInfo.size())
		return;

	subImageIndex = newValue;

	compareImageInfo* currentImageInfo = currentAction->imagesInfo[subImageIndex];

	imageSize = SCREEN_SYSTEM.imageSizeInRegion(currentImageInfo->image->GetWidth(),
												currentImageInfo->image->GetHeight(),
												size_t((windowContentRegionMax.x - windowContentRegionMin.x) * 0.925f),
												size_t((windowContentRegionMax.y - windowContentRegionMin.y) - 80.0f));

	imageSize.x = floor(imageSize.x);
	imageSize.y = floor(imageSize.y);

	imagePosition = ImVec2(windowContentRegionMin.x + 20, windowContentRegionMin.y + 90);

	float leftInPercent = 0.0f;
	float rigthInPercent = 1.0f;

	float topInPercent = 0.0f;
	float bottomInPercent = 1.0f;

	if (currentImageInfo->partialImage != nullptr)
	{
		leftInPercent = float(currentImageInfo->partialImageLeft) / float(currentImageInfo->image->GetWidth());
		rigthInPercent = leftInPercent + float(currentImageInfo->partialImage->GetWidth()) / float(currentImageInfo->image->GetWidth());

		topInPercent = float(currentImageInfo->partialImageTop) / float(currentImageInfo->image->GetHeight());
		bottomInPercent = topInPercent + float(currentImageInfo->partialImage->GetHeight()) / float(currentImageInfo->image->GetHeight());
	}

	left.setPosition(ImVec2(imagePosition.x + imageSize.x * leftInPercent, imagePosition.y));
	right.setPosition(ImVec2(imagePosition.x + imageSize.x * rigthInPercent, imagePosition.y));

	top = FEArrowScroller(false);
	top.setPosition(ImVec2(imagePosition.x, imagePosition.y + imageSize.y * topInPercent));

	bottom = FEArrowScroller(false);
	bottom.setPosition(ImVec2(imagePosition.x, imagePosition.y + imageSize.y * bottomInPercent));
}

void screenshootEditor::loadPNGToCurrent()
{
	std::string path;
	FocalEngine::FILE_SYSTEM.ShowFileOpenDialog(path, pngLoadFilter, 1);

	if (path != "")
	{
		std::vector<unsigned char> rawData;
		unsigned uWidth, uHeight;
		int error = lodepng::decode(rawData, uWidth, uHeight, path);

		if (error == 0)
		{
			if (currentAction->imagesInfo[subImageIndex]->image != nullptr)
				delete currentAction->imagesInfo[subImageIndex]->image;

			if (currentAction->imagesInfo[subImageIndex]->partialImage != nullptr)
				delete currentAction->imagesInfo[subImageIndex]->partialImage;
			currentAction->imagesInfo[subImageIndex]->partialImage = nullptr;

			currentAction->imagesInfo[subImageIndex]->partialImageLeft = 0;
			currentAction->imagesInfo[subImageIndex]->partialImageTop = 0;

			unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
			memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
			currentAction->imagesInfo[subImageIndex]->image = new FETPImage(tempData, uWidth, uHeight);
			delete[] tempData;

			changeCurrentImageVariant(subImageIndex);
		}
	}
}

void screenshootEditor::cutOutPartialImage()
{
	compareImageInfo* currentImageInfo = currentAction->imagesInfo[subImageIndex];

	float percentage = (left.getPosition().x - imagePosition.x) / imageSize.x;
	int minX = int(floor(currentImageInfo->image->GetWidth() * percentage));

	percentage = (top.getPosition().y - imagePosition.y) / imageSize.y;
	int minY = int(floor(currentImageInfo->image->GetHeight() * percentage));

	percentage = (right.getPosition().x - imagePosition.x) / imageSize.x;
	int maxX = int(floor(currentImageInfo->image->GetWidth() * percentage));

	percentage = (bottom.getPosition().y - imagePosition.y) / imageSize.y;
	int maxY = int(floor(currentImageInfo->image->GetHeight() * percentage));

	int newImageW = maxX - minX;

	FETPImage* newImage = currentImageInfo->image->GetRegion(minX,
															 minY,
															 maxX - minX,
															 maxY - minY);

	if (currentImageInfo->partialImage != nullptr)
		delete currentImageInfo->partialImage;

	currentImageInfo->partialImage = newImage;
	currentImageInfo->partialImageLeft = minX;
	currentImageInfo->partialImageTop = minY;
}

void screenshootEditor::useCompleteImage()
{
	compareImageInfo* currentImageInfo = currentAction->imagesInfo[subImageIndex];

	if (currentImageInfo->partialImage != nullptr)
	{
		delete currentImageInfo->partialImage;
		currentImageInfo->partialImage = nullptr;
	}

	currentImageInfo->partialImageLeft = 0;
	currentImageInfo->partialImageTop = 0;

	changeCurrentImageVariant(subImageIndex);
}

void screenshootEditor::replaceCurrentNode()
{
	FETest* currentTest = TEST_MANAGER.getTestByAction(currentAction);
	VisNodeSys::Node* currentNode = TEST_MANAGER.getNodeByAction(currentAction);

	globalActionNode* newNode = new globalActionNode(currentAction);
	newNode->SetPosition(currentNode->GetPosition());

	currentTest->nodeArea->DeleteNode(currentNode);
	currentTest->nodeArea->AddNode(newNode);
}

void screenshootEditor::GUIComboboxImageVariant()
{
	ImGui::SetCursorPos(ImVec2(deleteImageVariantButton->getPosition().x + deleteImageVariantButton->getSize().x + 10.0f, 32.0f));
	ImGui::SetNextItemWidth(80.0f);
	if (ImGui::BeginCombo("Image variants", std::to_string(subImageIndex).c_str(), ImGuiWindowFlags_None))
	{
		for (size_t i = 0; i < currentAction->imagesInfo.size(); i++)
		{
			bool is_selected = (subImageIndex == i);
			if (ImGui::Selectable(std::to_string(i).c_str(), is_selected))
			{
				changeCurrentImageVariant(i);
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void screenshootEditor::GUIInputIntThreshold()
{
	int tempThreshold = currentAction->imagesInfo[subImageIndex]->correctnessThreshold;
	ImGui::SetNextItemWidth(30.0f);
	ImGui::SetCursorPos(ImVec2(deleteImageVariantButton->getPosition().x + deleteImageVariantButton->getSize().x + 220.0f, 32.0f));
	ImGui::InputInt("Threshold", &tempThreshold, 0);
	if (tempThreshold > 100)
		tempThreshold = 100;

	if (tempThreshold < 0)
		tempThreshold = 0;

	currentAction->imagesInfo[subImageIndex]->correctnessThreshold = tempThreshold;

	int tempMaxColorShift = currentAction->imagesInfo[subImageIndex]->maxColorShift;
	ImGui::SetNextItemWidth(30.0f);
	ImGui::SetCursorPos(ImVec2(deleteImageVariantButton->getPosition().x + deleteImageVariantButton->getSize().x + 335.0f, 32.0f));
	ImGui::InputInt("maxColorShift", &tempMaxColorShift, 0);
	if (tempMaxColorShift > 255)
		tempMaxColorShift = 255;

	if (tempMaxColorShift < 0)
		tempMaxColorShift = 0;

	currentAction->imagesInfo[subImageIndex]->maxColorShift = tempMaxColorShift;
}

void screenshootEditor::GUISeveralAttemps()
{
	ImGui::SetCursorPos(ImVec2(addImageVariantButton->getPosition().x, 70.0f));
	ImGui::Checkbox("Several attempts", &currentAction->imagesInfo[subImageIndex]->severalAttempts);

	if (!currentAction->imagesInfo[subImageIndex]->severalAttempts)
		ImGui::BeginDisabled();

	ImGui::SetCursorPos(ImVec2(addImageVariantButton->getPosition().x + 180.0f, 70.0f));
	ImGui::SetNextItemWidth(120);
	ImGui::InputInt("Timeout", &currentAction->imagesInfo[subImageIndex]->severalAttemptsTimeout, 10);
	if (currentAction->imagesInfo[subImageIndex]->severalAttemptsTimeout < 100)
		currentAction->imagesInfo[subImageIndex]->severalAttemptsTimeout = 100;

	if (!currentAction->imagesInfo[subImageIndex]->severalAttempts)
		ImGui::EndDisabled();
}