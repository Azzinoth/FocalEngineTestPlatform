#include "actionEditPopup.h"

actionEditPopup* actionEditPopup::_instance = nullptr;

actionEditPopup::actionEditPopup()
{
	popupCaption = "Edit action";
}

actionEditPopup::~actionEditPopup()
{
}

void actionEditPopup::show(FETPAction* action)
{
	if (action == nullptr)
		return;

	if (action->getType() == FETP_BASE_ACTION ||
		action->getType() == FETP_SCREENSHOOT_COMPARE_ACTION || 
		action->getType() == FETP_LUNCH_APPLICATION_ACTION)
		return;

	currentAction = action;
	shouldOpen = true;
	ImGuiModalPopup::show();
}

void actionEditPopup::render()
{
	ImGuiModalPopup::render();

	if (currentAction == nullptr)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::SetNextWindowSize(popupSize);
	ImGui::SetNextWindowPos(ImVec2(TEST_PLATFORM.getWindowWidth() / 2 - popupSize.x / 2.0f, TEST_PLATFORM.getWindowHeight() / 2 - popupSize.y / 2.0f));
	if (ImGui::BeginPopupModal(popupCaption.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		if (currentAction->getType() == FETP_SLEEP_ACTION)
		{
			SleepAction* action = reinterpret_cast<SleepAction*>(currentAction);
			ImGui::SetNextItemWidth(150);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - (150.0f + 80.0f) / 2.0f);
			int sleepFor = action->sleepFor;
			if (ImGui::InputInt("sleep for", &sleepFor))
				action->sleepFor = sleepFor;
		}
		else if (currentAction->getType() == FETP_MOUSE_ACTION)
		{
			MouseAction* action = reinterpret_cast<MouseAction*>(currentAction);

			if (action->wParam == WM_MOUSEMOVE)
			{
				ImGui::SetNextItemWidth(140);
				static int position[] = { 0 };
				position[0] = action->additionalInfo.pt.x;
				position[1] = action->additionalInfo.pt.y;

				ImGui::InputInt2("position", position);

				action->additionalInfo.pt.x = position[0];
				action->additionalInfo.pt.y = position[1];
			}

			if (action->wParam == WM_MOUSEWHEEL)
			{
				static int value = 0;

				float yPosition = ImGui::GetCursorScreenPos().y;
				yPosition += 20.0f;
				ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, yPosition));

				ImGui::SetNextItemWidth(140);
				value = (short)HIWORD(action->additionalInfo.mouseData);
				ImGui::InputInt("wheel movement", &value);
			}
		}
		else if (currentAction->getType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(currentAction);
			ImGui::SetNextItemWidth(80);
			int keyCode = action->additionalInfo.vkCode;
			ImGui::InputInt("key code", &keyCode);
			action->additionalInfo.vkCode = keyCode;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - 120.0f / 2.0f);
		ImGui::SetCursorPosY(popupSize.y - 35.0f);
		if (ImGui::Button("Ok", ImVec2(120, 0)))
			exit();

		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}

void actionEditPopup::exit()
{
	currentAction = nullptr;
	ImGuiModalPopup::close();
}