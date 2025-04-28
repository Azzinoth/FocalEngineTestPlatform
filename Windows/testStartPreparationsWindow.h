#pragma once

#include "PreviewWindow.h"

class TestStartPreparationsWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(TestStartPreparationsWindow)
	
	int SelectedAction = -1;
	int HoveredAction = -1;

	char PathInputBuffer[1024];
public:
	SINGLETON_PUBLIC_PART(TestStartPreparationsWindow)

	void Show();
	void Render() override;
};