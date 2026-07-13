#include "TextInputPopup.h"

TextInputPopup::TextInputPopup()
{
	PopupCaption = "Input text";
	strcpy_s(Text, "");
}

TextInputPopup::~TextInputPopup()
{
}

void TextInputPopup::Show(std::function<void(std::string InputFromUser)> Function, std::string StartText)
{
	if (Function == nullptr)
		return;

	strcpy_s(Text, StartText.c_str());
	bShouldOpen = true;
	Callback = Function;

	ImGuiModalPopup::Show();
}

void TextInputPopup::Render()
{
	ImGuiModalPopup::Render();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::SetNextWindowSize(PopupSize);
	int Width, Height;
	FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);
	ImGui::SetNextWindowPos(ImVec2(Width / 2 - PopupSize.x / 2.0f, Height / 2 - PopupSize.y / 2.0f));
	if (ImGui::BeginPopupModal(PopupCaption.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (Callback == nullptr)
			return;

		ImGui::Text("Text :");
		ImGui::InputText("##TextInputPopup_InputText", Text, IM_ARRAYSIZE(Text));

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 4.0f - 120.0f / 2.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
		if (ImGui::Button("Apply", ImVec2(120, 0)))
		{
			if (strlen(Text) > 0)
				Callback(Text);

			Exit();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f + ImGui::GetWindowWidth() / 4.0f - 120.0f / 2.0f);
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			Exit();
		}

		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}

void TextInputPopup::Exit()
{
	Callback = nullptr;
	strcpy_s(Text, "");
	ImGuiModalPopup::Close();
}