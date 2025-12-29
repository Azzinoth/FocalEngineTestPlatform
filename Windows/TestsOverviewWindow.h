#pragma once

#include "TestEditorWindow.h"

class TestsOverviewWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(TestsOverviewWindow)

	void RenderMainMenu();
	void RenderContextMenu();
	bool bShouldOpenContextMenu = false;
	bool bContextMenuOpened = false;
	static int HoveredTestIndex;

	FETPImage* TestSuccess = nullptr;
	FETPImage* TestFailed = nullptr;
	FETPImage* TestUnknown = nullptr;

	static void TextInputCallback(std::string Text);
public:
	SINGLETON_PUBLIC_PART(TestsOverviewWindow)

	void Show();
	void Render() override;
};