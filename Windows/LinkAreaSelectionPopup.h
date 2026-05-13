#pragma once

#include "../SubSystems/FEPTActionSystem.h"

class LinkAreaSelectionPopup : public ImGuiModalPopup
{
	SINGLETON_PRIVATE_PART(LinkAreaSelectionPopup)

	std::function<void(VisNodeSys::NodeArea* ChosenArea)> Callback;
	std::string CurrentAreaID;
	char Filter[256];
	ImVec2 PopupSize = ImVec2(400, 380);

	void Exit();
public:
	SINGLETON_PUBLIC_PART(LinkAreaSelectionPopup)

	void Show(const std::string& SourceAreaID, std::function<void(VisNodeSys::NodeArea* ChosenArea)> Function);
	void Render() override;
};