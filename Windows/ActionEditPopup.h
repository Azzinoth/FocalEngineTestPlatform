#pragma once

#include "../SubSystems/FEPTActionSystem.h"

class ActionEditPopup : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(ActionEditPopup)

	FETPAction* CurrentAction = nullptr;
	ImVec2 PopupSize = ImVec2(300, 150);

	void Exit();
public:
	SINGLETON_PUBLIC_PART(ActionEditPopup)

	void Show(FETPAction* Action);
	void Render() override;
};