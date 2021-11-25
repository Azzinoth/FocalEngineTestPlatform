#pragma once

#include "../SubSystems/FEPTActionSystem.h"
#include "previewWindow.h"

class nodeRegionWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(nodeRegionWindow)

	static regionNode* currentRegion;
	ImGuiButton* cancelButton;

	static ImVec2 mousePositionWhenContextMenuWasOpened;

	static void mainContextMenu();
	static void textInputCallback(std::string text);
public:
	SINGLETON_PUBLIC_PART(nodeRegionWindow)

	void show(regionNode* region);
	void render() override;
};