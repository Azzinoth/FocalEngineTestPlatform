#pragma once

#include "../SubSystems/FEPTActionSystem.h"

class TextInputPopup : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(TextInputPopup)

	std::function<void(std::string InputFromUser)> Callback;
	char Text[512];
	ImVec2 PopupSize = ImVec2(300, 150);

	void Exit();
public:
	SINGLETON_PUBLIC_PART(TextInputPopup)

	void Show(std::function<void(std::string InputFromUser)> Function, std::string StartText = "");
	void Render() override;
};