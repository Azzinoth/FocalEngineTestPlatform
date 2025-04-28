#pragma once

#include "../SubSystems/FEPTActionSystem.h"
#include "PreviewWindow.h"

class NodeRegionWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(NodeRegionWindow)

	static RegionNode* CurrentRegion;
	ImGuiButton* CancelButton = nullptr;

	static ImVec2 MousePositionWhenContextMenuWasOpened;

	static void RenderMainContextMenu();
	static void TextInputCallback(std::string Text);
public:
	SINGLETON_PUBLIC_PART(NodeRegionWindow)

	void Show(RegionNode* Region);
	void Render() override;
};