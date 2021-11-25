#pragma once

#include "../SubSystems/FEPTActionSystem.h"

class actionEditPopup : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(actionEditPopup)

	FETPAction* currentAction = nullptr;
	ImVec2 popupSize = ImVec2(300, 150);

	void exit();
public:
	SINGLETON_PUBLIC_PART(actionEditPopup)

	void show(FETPAction* action);
	void render() override;
};