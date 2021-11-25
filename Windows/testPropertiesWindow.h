#pragma once

#include "previewWindow.h"
#include "testStartPreparationsWindow.h"

class testPropertiesWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(testPropertiesWindow)

public:
	SINGLETON_PUBLIC_PART(testPropertiesWindow)

	void show();
	void render() override;
};