#include "LinkAreaSelectionPopup.h"

LinkAreaSelectionPopup::LinkAreaSelectionPopup()
{
	PopupCaption = "Link to Node Area";
	strcpy_s(Filter, "");
}

LinkAreaSelectionPopup::~LinkAreaSelectionPopup()
{
}

void LinkAreaSelectionPopup::Show(const std::string& SourceAreaID, std::function<void(VisNodeSys::NodeArea* ChosenArea)> Function)
{
	if (Function == nullptr)
		return;

	CurrentAreaID = SourceAreaID;
	strcpy_s(Filter, "");
	bShouldOpen = true;
	Callback = Function;

	ImGuiModalPopup::Show();
}

static bool ContainsCaseInsensitive(const std::string& Source, const std::string& Needle)
{
	if (Needle.empty())
		return true;

	auto NeedleIterator = std::search(
		Source.begin(), Source.end(),
		Needle.begin(), Needle.end(),
		[](char Left, char Right) { return std::tolower(static_cast<unsigned char>(Left)) == std::tolower(static_cast<unsigned char>(Right)); });

	return NeedleIterator != Source.end();
}

void LinkAreaSelectionPopup::Render()
{
	ImGuiModalPopup::Render();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	ImGui::SetNextWindowSize(PopupSize);
	int Width, Height;
	FocalEngine::APPLICATION.GetMainWindow()->GetSize(&Width, &Height);
	ImGui::SetNextWindowPos(ImVec2(Width / 2 - PopupSize.x / 2.0f, Height / 2 - PopupSize.y / 2.0f));
	if (ImGui::BeginPopupModal(PopupCaption.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		if (Callback == nullptr)
		{
			ImGui::PopStyleVar();
			ImGui::EndPopup();
			return;
		}

		ImGui::Text("Filter:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputText("##LinkAreaSelectionPopup_Filter", Filter, IM_ARRAYSIZE(Filter));

		ImGui::Separator();

		const float BottomBarHeight = 40.0f;
		ImGui::BeginChild("##LinkAreaSelectionPopup_List", ImVec2(0, PopupSize.y - ImGui::GetCursorPosY() - BottomBarHeight), true);

		if (ImGui::Selectable("+ Create new area"))
		{
			Callback(nullptr);
			Exit();
			ImGui::EndChild();
			ImGui::PopStyleVar();
			ImGui::EndPopup();
			return;
		}

		ImGui::Separator();

		std::string FilterString = Filter;
		std::vector<std::string> AreaIDList = NODE_SYSTEM.GetNodeAreaIDList();
		bool bAnyShown = false;
		for (size_t i = 0; i < AreaIDList.size(); i++)
		{
			if (AreaIDList[i] == CurrentAreaID)
				continue;

			VisNodeSys::NodeArea* Area = NODE_SYSTEM.GetNodeAreaByID(AreaIDList[i]);
			if (Area == nullptr)
				continue;

			std::string DisplayName = Area->GetName();
			if (DisplayName.empty())
				DisplayName = "(unnamed)";

			std::string ShortID = AreaIDList[i].substr(0, 8);
			std::string Label = DisplayName + "  [" + ShortID + "]";

			if (!ContainsCaseInsensitive(Label, FilterString))
				continue;

			bAnyShown = true;
			if (ImGui::Selectable((Label + "##" + AreaIDList[i]).c_str()))
			{
				Callback(Area);
				Exit();
				ImGui::EndChild();
				ImGui::PopStyleVar();
				ImGui::EndPopup();
				return;
			}
		}

		if (!bAnyShown)
		{
			ImGui::TextDisabled("No other areas available.");
		}

		ImGui::EndChild();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - 120.0f / 2.0f);
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

void LinkAreaSelectionPopup::Exit()
{
	Callback = nullptr;
	CurrentAreaID.clear();
	strcpy_s(Filter, "");
	ImGuiModalPopup::Close();
}