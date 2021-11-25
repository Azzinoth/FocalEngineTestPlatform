#pragma once

#include "../SubSystems/FEPTActionSystem.h"

class textInputPopup : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(textInputPopup)

	std::function<void(std::string inputFromUser)> callback;
	char text[512];
	ImVec2 popupSize = ImVec2(300, 150);

	void exit();
public:
	SINGLETON_PUBLIC_PART(textInputPopup)

	void show(std::function<void(std::string inputFromUser)> func, std::string startText = "");
	void render() override;
};