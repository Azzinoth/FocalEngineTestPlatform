#pragma once

#include "../SubSystems/FEPTActionSystem.h"

struct FETestResult;
class failedTestWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(failedTestWindow)

	ImGuiButton* okButton;
	FETestResult* result = nullptr;
public:
	SINGLETON_PUBLIC_PART(failedTestWindow)

	void show(FETestResult* result);
	void render() override;
};