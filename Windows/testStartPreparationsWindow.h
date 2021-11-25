#pragma once

#include "previewWindow.h"

class testStartPreparationsWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(testStartPreparationsWindow)
	
	int selectedAction = -1;
	int hoveredAction = -1;

	char pathInput[1024];
public:
	SINGLETON_PUBLIC_PART(testStartPreparationsWindow)

	void show();
	void render() override;
};