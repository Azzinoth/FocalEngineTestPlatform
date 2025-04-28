#include "ActionEditPopup.h"

ActionEditPopup::ActionEditPopup()
{
	PopupCaption = "Edit action";
}

ActionEditPopup::~ActionEditPopup()
{
}

void ActionEditPopup::Show(FETPAction* action)
{
	if (action == nullptr)
		return;

	if (action->GetType() == FETP_BASE_ACTION ||
		action->GetType() == FETP_SCREENSHOT_COMPARE_ACTION || 
		action->GetType() == FETP_LAUNCH_APPLICATION_ACTION)
		return;

	CurrentAction = action;
	bShouldOpen = true;
	ImGuiModalPopup::Show();
}

void ActionEditPopup::Render()
{
	ImGuiModalPopup::Render();

	if (CurrentAction == nullptr)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::SetNextWindowSize(PopupSize);
	int Width, Height;
	FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);
	ImGui::SetNextWindowPos(ImVec2(Width / 2 - PopupSize.x / 2.0f, Height / 2 - PopupSize.y / 2.0f));
	if (ImGui::BeginPopupModal(PopupCaption.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		if (CurrentAction->GetType() == FETP_SLEEP_ACTION)
		{
			SleepAction* action = reinterpret_cast<SleepAction*>(CurrentAction);
			ImGui::SetNextItemWidth(150);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - (150.0f + 80.0f) / 2.0f);
			int SleepDurationMS = action->SleepDurationMS;
			if (ImGui::InputInt("sleep for", &SleepDurationMS))
				action->SleepDurationMS = SleepDurationMS;
		}
		else if (CurrentAction->GetType() == FETP_MOUSE_ACTION)
		{
			MouseAction* action = reinterpret_cast<MouseAction*>(CurrentAction);

			if (action->EventType == WM_MOUSEMOVE)
			{
				ImGui::SetNextItemWidth(140);
				static int position[] = { 0, 0 };
				position[0] = action->HookInfo.pt.x;
				position[1] = action->HookInfo.pt.y;

				ImGui::InputInt2("position", position);

				action->HookInfo.pt.x = position[0];
				action->HookInfo.pt.y = position[1];
			}

			if (action->EventType == WM_MOUSEWHEEL)
			{
				static int value = 0;

				float yPosition = ImGui::GetCursorScreenPos().y;
				yPosition += 20.0f;
				ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, yPosition));

				ImGui::SetNextItemWidth(140);
				value = (short)HIWORD(action->HookInfo.mouseData);
				ImGui::InputInt("wheel movement", &value);
			}
		}
		else if (CurrentAction->GetType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(CurrentAction);
			ImGui::SetNextItemWidth(80);
			int keyCode = action->HookInfo.vkCode;
			ImGui::InputInt("key code", &keyCode);
			action->HookInfo.vkCode = keyCode;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - 120.0f / 2.0f);
		ImGui::SetCursorPosY(PopupSize.y - 35.0f);
		if (ImGui::Button("Ok", ImVec2(120, 0)))
			Exit();

		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}

void ActionEditPopup::Exit()
{
	CurrentAction = nullptr;
	ImGuiModalPopup::Close();
}