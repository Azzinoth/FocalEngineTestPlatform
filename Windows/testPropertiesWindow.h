#pragma once

#include "PreviewWindow.h"
#include "TestStartPreparationsWindow.h"

class TestPropertiesWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(TestPropertiesWindow)

public:
	SINGLETON_PUBLIC_PART(TestPropertiesWindow)

	void Show();
	void Render() override;
};