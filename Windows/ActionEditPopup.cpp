#include "ActionEditPopup.h"

ActionEditPopup::ActionEditPopup()
{
	PopupCaption = "Edit Action";
}

ActionEditPopup::~ActionEditPopup()
{
}

void ActionEditPopup::Show(FETPAction* Action)
{
	if (Action == nullptr)
		return;

	if (Action->GetType() == FETP_BASE_ACTION ||
		Action->GetType() == FETP_SCREENSHOT_COMPARE_ACTION || 
		Action->GetType() == FETP_LAUNCH_APPLICATION_ACTION)
		return;

	CurrentAction = Action;
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
		/*if (CurrentAction->GetType() == FETP_SLEEP_ACTION)
		{
			SleepAction* Action = reinterpret_cast<SleepAction*>(CurrentAction);
			ImGui::SetNextItemWidth(150);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - (150.0f + 80.0f) / 2.0f);
			int SleepDurationMS = Action->SleepDurationMS;
			if (ImGui::InputInt("sleep for", &SleepDurationMS))
				Action->SleepDurationMS = SleepDurationMS;
		}
		else*/ if (CurrentAction->GetType() == FETP_MOUSE_ACTION)
		{
			MouseAction* Action = reinterpret_cast<MouseAction*>(CurrentAction);

			if (Action->EventType == WM_MOUSEMOVE)
			{
				ImGui::SetNextItemWidth(140);
				static int position[] = { 0, 0 };
				position[0] = Action->HookInfo.pt.x;
				position[1] = Action->HookInfo.pt.y;

				ImGui::InputInt2("position", position);

				Action->HookInfo.pt.x = position[0];
				Action->HookInfo.pt.y = position[1];
			}

			if (Action->EventType == WM_MOUSEWHEEL)
			{
				static int Value = 0;

				float YPosition = ImGui::GetCursorScreenPos().y;
				YPosition += 20.0f;
				ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, YPosition));

				ImGui::SetNextItemWidth(140);
				Value = (short)HIWORD(Action->HookInfo.mouseData);
				ImGui::InputInt("wheel movement", &Value);
			}
		}
		else if (CurrentAction->GetType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* Action = reinterpret_cast<KeyboardAction*>(CurrentAction);
			ImGui::SetNextItemWidth(80);
			int KeyCode = Action->HookInfo.vkCode;
			ImGui::InputInt("key code", &KeyCode);
			Action->HookInfo.vkCode = KeyCode;
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