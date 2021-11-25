#pragma once

#include "testEditorWindow.h"

class testsOverviewWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(testsOverviewWindow)

	void renderMainMenu();
	void renderContextMenu();
	bool openContextMenu = false;
	bool contextMenuOpened = false;
	static int hoveredTestIndex;

	FETPImage* testSuccess = nullptr;
	FETPImage* testFailed = nullptr;
	FETPImage* testUnKnown = nullptr;

	static void textInputCallback(std::string text);
public:
	SINGLETON_PUBLIC_PART(testsOverviewWindow)

	void show();
	void render() override;
};