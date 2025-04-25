#include "textInputPopup.h"

textInputPopup::textInputPopup()
{
	popupCaption = "Input text";
	strcpy_s(text, "");
}

textInputPopup::~textInputPopup()
{
}

void textInputPopup::show(std::function<void(std::string inputFromUser)> func, std::string startText)
{
	if (func == nullptr)
		return;

	strcpy_s(text, startText.c_str());
	shouldOpen = true;
	callback = func;

	ImGuiModalPopup::show();
}

void textInputPopup::render()
{
	ImGuiModalPopup::render();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::SetNextWindowSize(popupSize);
	int Width, Height;
	FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);
	ImGui::SetNextWindowPos(ImVec2(Width / 2 - popupSize.x / 2.0f, Height / 2 - popupSize.y / 2.0f));
	if (ImGui::BeginPopupModal(popupCaption.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (callback == nullptr)
			return;

		ImGui::Text("Text :");
		ImGui::InputText("", text, IM_ARRAYSIZE(text));

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 4.0f - 120.0f / 2.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
		if (ImGui::Button("Apply", ImVec2(120, 0)))
		{
			if (strlen(text) > 0)
				callback(text);

			exit();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f + ImGui::GetWindowWidth() / 4.0f - 120.0f / 2.0f);
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			exit();
		}

		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}

void textInputPopup::exit()
{
	callback = nullptr;
	strcpy_s(text, "");
	ImGuiModalPopup::close();
}