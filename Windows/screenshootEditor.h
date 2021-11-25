#pragma once

#include "failedTestWindow.h"

class screenshootEditor : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(screenshootEditor)

	ImGuiButton* cutButton;
	void cutOutPartialImage();

	ImGuiButton* useCompleteButton;
	void useCompleteImage();

	ImGuiButton* loadButton;
	void loadPNGToCurrent();

	ImGuiButton* cancelButton;

	ImGuiButton* addImageVariantButton;
	void addNewImageVariant();

	ImGuiButton* deleteImageVariantButton;
	void deleteCurrentImageVariant();

	ScreenshootCompareAction* currentAction;
	size_t subImageIndex = 0;
	glm::vec2 imageSize;
	ImVec2 imagePosition;

	FEArrowScroller left;
	FEArrowScroller right;

	FEArrowScroller top;
	FEArrowScroller bottom;

	ImVec2 lastFrameWindowContentRegionMin;
	ImVec2 lastFrameWindowContentRegionMax;

	void changeCurrentImageVariant(size_t newValue);

	ImVec2 windowContentRegionMin;
	ImVec2 windowContentRegionMax;
	bool firstFrame = true;

	void replaceCurrentNode();

	void GUIComboboxImageVariant();
	void GUIInputIntThreshold();
	void GUISeveralAttemps();
public:
	SINGLETON_PUBLIC_PART(screenshootEditor)

	void show(ScreenshootCompareAction* action);
	void render() override;
};