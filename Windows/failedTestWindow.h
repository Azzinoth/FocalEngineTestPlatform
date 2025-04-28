#pragma once

#include "../SubSystems/FEPTActionSystem.h"

struct FETestResult;
class FailedTestWindow : public FEImGuiWindow
{
	SINGLETON_PRIVATE_PART(FailedTestWindow)

	ImGuiButton* OKButton;
	FETestResult* Result = nullptr;
public:
	SINGLETON_PUBLIC_PART(FailedTestWindow)

	void Show(FETestResult* Result);
	void Render() override;
};