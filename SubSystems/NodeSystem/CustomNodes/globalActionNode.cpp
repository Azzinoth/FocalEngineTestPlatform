#include "globalActionNode.h"

VISUAL_NODE_CHILD_CPP(globalActionNode)
FETPImage* globalActionNode::SleepIcon = nullptr;

FETPImage* globalActionNode::KeyDownIcon = nullptr;
FETPImage* globalActionNode::KeyUpIcon = nullptr;

FETPImage* globalActionNode::MoveMouseIcon = nullptr;
FETPImage* globalActionNode::LeftMouseDownIcon = nullptr;
FETPImage* globalActionNode::LeftMouseUpIcon = nullptr;
FETPImage* globalActionNode::RightMouseDownIcon = nullptr;
FETPImage* globalActionNode::RightMouseUpIcon = nullptr;
FETPImage* globalActionNode::ScrollMouseIcon = nullptr;

FETPImage* globalActionNode::LunchApplicationIcon = nullptr;
FETPImage* globalActionNode::ScreenshotIcon = nullptr;

globalActionNode::globalActionNode() : FEVisualNode()
{
	SetStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

void globalActionNode::CopyData(FETPAction* Src)
{
	if (Src->getType() == FETP_KEYBOARD_ACTION)
	{
		Data = new KeyboardAction(*reinterpret_cast<KeyboardAction*>(Src));
	}
	else if (Src->getType() == FETP_MOUSE_ACTION)
	{
		Data = new MouseAction(*reinterpret_cast<MouseAction*>(Src));
	}
	else if (Src->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		Data = new ScreenshootCompareAction(*reinterpret_cast<ScreenshootCompareAction*>(Src));
	}
	else if (Src->getType() == FETP_SLEEP_ACTION)
	{
		Data = new SleepAction(*reinterpret_cast<SleepAction*>(Src));
	}
	else if (Src->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		Data = new LunchApplicationAction(*reinterpret_cast<LunchApplicationAction*>(Src));
	}
	else
	{
		Data = new FETPAction(*Src);
	}
}

globalActionNode::globalActionNode(const globalActionNode& Src) : FEVisualNode(Src)
{
	CopyData(Src.Data);
	SetStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

globalActionNode::~globalActionNode()
{
	if (Data != nullptr)
		delete Data;
}

void globalActionNode::Initialize(FETPAction* Data)
{
	SetStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
	Type = "globalActionNode";
	this->Data = Data;

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	if (Data->getType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(Data);
		if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
		{
			SetName("keyDown");
			TitleBackgroundColor = ImColor(34, 177, 76);
			TitleBackgroundColorHovered = ImColor(32, 166, 72);
		}
		else if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
		{
			SetName("keyUp");
			TitleBackgroundColor = ImColor(181, 230, 29);
			TitleBackgroundColorHovered = ImColor(174, 224, 24);
		}
	}
	else if (Data->getType() == FETP_MOUSE_ACTION)
	{
		MouseAction* action = reinterpret_cast<MouseAction*>(Data);
		if (action->wParam == WM_LBUTTONUP)
		{
			SetName("mouseLeftUp");
			TitleBackgroundColor = ImColor(153, 217, 234);
			TitleBackgroundColorHovered = ImColor(124, 207, 228);
		}
		else if (action->wParam == WM_RBUTTONUP)
		{
			SetName("mouseRightUp");
			TitleBackgroundColor = ImColor(200, 191, 231);
			TitleBackgroundColorHovered = ImColor(171, 157, 219);
		}
		else if (action->wParam == WM_LBUTTONDOWN)
		{
			SetName("mouseLeftDown");
			TitleBackgroundColor = ImColor(0, 162, 232);
			TitleBackgroundColorHovered = ImColor(0, 152, 217);
		}
		else if (action->wParam == WM_RBUTTONDOWN)
		{
			SetName("mouseRightDown");
			TitleBackgroundColor = ImColor(163, 73, 164);
			TitleBackgroundColorHovered = ImColor(147, 66, 147);
		}
		else if (action->wParam == WM_MOUSEMOVE)
		{
			SetName("mouseMove");
			TitleBackgroundColor = ImColor(136, 0, 21);
			TitleBackgroundColorHovered = ImColor(113, 0, 17);
		}
		else if (action->wParam == WM_MOUSEWHEEL)
		{
			SetName("mouseWheel");
			TitleBackgroundColor = ImColor(136, 0, 21);
			TitleBackgroundColorHovered = ImColor(113, 0, 17);
		}
	}
	else if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		SetName("screenshoot");
		TitleBackgroundColor = ImColor(136, 0, 21);
		TitleBackgroundColorHovered = ImColor(111, 0, 17);

		if (Input.size() == 0 && Output.size() == 0)
		{
			AddInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, ""));

			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data);
			for (size_t i = 0; i < action->imagesInfo.size(); i++)
			{
				AddOutputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, "next_" + std::to_string(i + 1)));
			}
		}
	}
	else if (Data->getType() == FETP_SLEEP_ACTION)
	{
		SetName("sleep");
	}
	else if (Data->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		SetName("lunch application");
	}

	SetSize(ImVec2(330, 140));

	if (Input.size() == 0 && Output.size() == 0)
	{
		AddInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, ""));
		AddOutputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, ""));
	}
}

globalActionNode::globalActionNode(FETPAction* Data) : FEVisualNode()
{
	Initialize(Data);
}

void globalActionNode::Draw()
{	
	FEVisualNode::Draw();

	if (GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		// Show client rect.
		/*ImVec2 regionMin = ImVec2(ImGui::GetCursorScreenPos().x + this->getClientRegionPosition().x,
								  ImGui::GetCursorScreenPos().y + this->getClientRegionPosition().y);

		ImVec2 regionMax = ImVec2(regionMin.x + this->GetClientRegionSize().x,
								  regionMin.y + this->GetClientRegionSize().y);

		ImGui::GetWindowDrawList()->AddRectFilled(regionMin, regionMax, IM_COL32(175, 175, 175, 125), 8.0f);*/

		float xPosition = ImGui::GetCursorScreenPos().x;
		xPosition += 80.0f;
		float yPosition = ImGui::GetCursorScreenPos().y;
		yPosition += NODE_TITLE_HEIGHT + 13.0f;
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

		if (Data->getType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(Data);
			ImGui::SetNextItemWidth(80);
			int keyCode = action->additionalInfo.vkCode;
			if (ImGui::InputInt("key code", &keyCode))
			{
				ParentArea->PropagateUpdateToConnectedNodes(this);
			}

			action->additionalInfo.vkCode = keyCode;
		}
		else if (Data->getType() == FETP_MOUSE_ACTION)
		{
			MouseAction* action = reinterpret_cast<MouseAction*>(Data);

			if (action->wParam == WM_MOUSEMOVE)
			{
				ImGui::SetNextItemWidth(140);
				static int position[] = { 0 };
				position[0] = action->additionalInfo.pt.x;
				position[1] = action->additionalInfo.pt.y;

				if (ImGui::InputInt2("position", position))
				{
					ParentArea->PropagateUpdateToConnectedNodes(this);
				}

				action->additionalInfo.pt.x = position[0];
				action->additionalInfo.pt.y = position[1];
			}

			if (action->wParam == WM_MOUSEWHEEL)
			{
				static int value = 0;

				float yPosition = ImGui::GetCursorScreenPos().y;
				yPosition += 20.0f;
				ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

				ImGui::SetNextItemWidth(140);
				value = (short)HIWORD(action->additionalInfo.mouseData);
				ImGui::InputInt("wheel movement", &value);
			}
		}
		else if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data);

			if (action->imagesInfo.size() > 0 && action->imagesInfo[0] != nullptr)
			{
				ImGui::SetCursorScreenPos(ImVec2(xPosition - 10.0f, yPosition - 10.0f));
				glm::vec2 imageSize = SCREEN_SYSTEM.imageSizeInRegion(action->imagesInfo[0]->image->getWidth(),
					action->imagesInfo[0]->image->getHeight(),
					size_t(this->GetClientRegionSize().x * 0.95f),
					size_t(this->GetClientRegionSize().y * 0.55f));

				ImGui::Image((void*)(intptr_t)action->imagesInfo[0]->image->getTextureID(),
					ImVec2(imageSize.x, imageSize.y),
					ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

				ImGui::SetCursorScreenPos(ImVec2(xPosition - 10.0f, yPosition + imageSize.y));
				ImGui::Checkbox("Several attempts", &action->imagesInfo[0]->severalAttempts);

				if (!action->imagesInfo[0]->severalAttempts)
					ImGui::BeginDisabled();

				ImGui::SetCursorScreenPos(ImVec2(xPosition - 10.0f, yPosition + imageSize.y + 20.0f));
				ImGui::SetNextItemWidth(100);
				ImGui::InputInt("Timeout", &action->imagesInfo[0]->severalAttemptsTimeout, 10);
				if (action->imagesInfo[0]->severalAttemptsTimeout < 100)
					action->imagesInfo[0]->severalAttemptsTimeout = 100;

				if (!action->imagesInfo[0]->severalAttempts)
					ImGui::EndDisabled();
			}
		}
		else if (Data->getType() == FETP_SLEEP_ACTION)
		{
			SleepAction* action = reinterpret_cast<SleepAction*>(Data);
			ImGui::SetNextItemWidth(80);
			int sleepFor = action->sleepFor;
			if (ImGui::InputInt("sleep for", &sleepFor))
			{
				action->sleepFor = sleepFor;
				ParentArea->PropagateUpdateToConnectedNodes(this);
			}
		}
		else if (Data->getType() == FETP_LUNCH_APPLICATION_ACTION)
		{
			LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(Data);
			ImGui::Text(action->applicationPath.c_str());
		}
	}
	else if (GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		CheckIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		RenderIcon();

		if (ParentArea->IsMouseHovered() && IsHovered() && !actionEditPopup::getInstance().isOpened() && !textInputPopup::getInstance().isOpened() /*&& !screenshootEditor::getInstance().isVisible()*/)
			ShowTooltip();
	}

	if (ContextMenu)
	{
		ContextMenu = false;
		ImGui::OpenPopup("##context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopup("##context_menu"))
	{
		if (ImGui::MenuItem(std::string("Edit...").c_str()))
		{
			if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
			{
				ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data);
				screenshootEditor::getInstance().show(action);
			}
			else if (Data->getType() == FETP_LUNCH_APPLICATION_ACTION)
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, applicationLoadFilter, 1);

				if (path != "")
				{
					LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(Data);
					action->applicationPath = path;
				}
			}
			else
			{
				actionEditPopup::getInstance().show(Data);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			bShouldBeDestroyed = true;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

void globalActionNode::SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType)
{
	FEVisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

FETPAction* globalActionNode::GetData()
{
	return Data;
}

bool globalActionNode::CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!FEVisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	if (CandidateSocket->GetType() == FE_NODE_SOCKET_FLOAT_CHANNEL_OUT && OwnSocket->GetType() == FE_NODE_SOCKET_FLOAT_CHANNEL_IN)
		return true;

	return false;
}

Json::Value globalActionNode::ToJson()
{
	Json::Value result = FEVisualNode::ToJson();

	if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data);
		action->saveImagesToDisk();
	}
	result["action"] = Data->toJson();

	return result;
}

FEVisualNode* globalActionNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnections().size() > 0)
	{
		if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data);
			for (size_t i = 0; i < action->imagesInfo.size(); i++)
			{
				if (action->imagesInfo[i]->lastRunResult)
					return Output[i]->GetConnections()[0]->GetParent();
			}

			return Output[0]->GetConnections()[0]->GetParent();
		}
		else
		{
			return Output[0]->GetConnections()[0]->GetParent();
		}
	}

	return nullptr;
}

void globalActionNode::FromJson(Json::Value Json)
{
	FEVisualNode::FromJson(Json);

	if (FETP_ACTION_TYPE(Json["action"]["internalType"].asInt()) == FETP_KEYBOARD_ACTION)
	{
		Data = new KeyboardAction();
	}
	else if (FETP_ACTION_TYPE(Json["action"]["internalType"].asInt()) == FETP_MOUSE_ACTION)
	{
		Data = new MouseAction();
	}
	else if (FETP_ACTION_TYPE(Json["action"]["internalType"].asInt()) == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		Data = new ScreenshootCompareAction();
	}
	else if (FETP_ACTION_TYPE(Json["action"]["internalType"].asInt()) == FETP_LUNCH_APPLICATION_ACTION)
	{
		Data = new LunchApplicationAction();
	}
	else if (FETP_ACTION_TYPE(Json["action"]["internalType"].asInt()) == FETP_SLEEP_ACTION)
	{
		Data = new SleepAction();
	}

	if (Data != nullptr)
	{
		Data->fromJson(Json["action"]);
		Initialize(Data);
	}
}

FEVisualNode* globalActionNode::GetLogicallyNextNode()
{
	if (Data->getType() != FETP_SCREENSHOOT_COMPARE_ACTION)
		return FEVisualNode::GetLogicallyNextNode();

	ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data);
	for (size_t i = 0; i < action->imagesInfo.size(); i++)
	{
		if (action->imagesInfo[i]->lastRunResult)
		{
			if (Output[i]->GetConnections().size() > 0)
				return Output[i]->GetConnections()[0]->GetParent();
		}
	}

	return nullptr;
}

bool globalActionNode::OpenContextMenu()
{
	ContextMenu = true;
	return true;
}

void globalActionNode::ShowTooltip()
{
	std::string textToShow = "Name: " + GetName() + "\nAction type: " + FETPAction::FETPActionTypeToString(Data->getType());
	textToShow += "\nID: " + Data->getID();

	if (Data->getType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(Data);
		if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
			textToShow += "\nSub type: Key down";
		if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
			textToShow += "\nSub type: Key up";

		textToShow += "\nKey code: " + std::to_string(int(action->additionalInfo.vkCode));
	}
	else if (Data->getType() == FETP_MOUSE_ACTION)
	{
		MouseAction* action = reinterpret_cast<MouseAction*>(Data);
		if (action->wParam == WM_MOUSEMOVE)
		{
			textToShow += "\nSub type: Move";
			textToShow += "\nTo position: x = " + std::to_string(int(action->additionalInfo.pt.x)) + " y = " + std::to_string(int(action->additionalInfo.pt.y));
		}
		else if (action->wParam == WM_LBUTTONDOWN)
		{
			textToShow += "\nSub type: Left Down";
		}
		else if (action->wParam == WM_LBUTTONUP)
		{
			textToShow += "\nSub type: Left Up";
		}
		else if (action->wParam == WM_RBUTTONDOWN)
		{
			textToShow += "\nSub type: Right Down";
		}
		else if (action->wParam == WM_RBUTTONUP)
		{
			textToShow += "\nSub type: Right Up";
		}
		else if (action->wParam == WM_MOUSEWHEEL)
		{
			textToShow += "\nSub type: Wheel";
			textToShow += "\nScroll amount: " + std::to_string((short)HIWORD(action->additionalInfo.mouseData));
		}
	}
	else if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		textToShow += "\nSub type: Screenshot";
	}
	else if (Data->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(Data);
		textToShow += "\nSub type: Lunch Application";
		textToShow += "\nPath: " + action->applicationPath;
	}
	else if (Data->getType() == FETP_SLEEP_ACTION)
	{
		SleepAction* action = reinterpret_cast<SleepAction*>(Data);
		textToShow += "\nDuration: " + std::to_string(action->sleepFor);
	}

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::BeginTooltip();
	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	ImGui::TextUnformatted(textToShow.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndTooltip();
	ImGui::PopFont();
}

void globalActionNode::CheckIcons()
{
	if (SleepIcon == nullptr)
		SleepIcon = new FETPImage("Resources//sleepIcon.png");

	if (KeyDownIcon == nullptr)
		KeyDownIcon = new FETPImage("Resources//keyDownIcon.png");

	if (KeyUpIcon == nullptr)
		KeyUpIcon = new FETPImage("Resources//keyUpIcon.png");

	if (MoveMouseIcon == nullptr)
		MoveMouseIcon = new FETPImage("Resources//moveMouseIcon.png");

	if (LeftMouseDownIcon == nullptr)
		LeftMouseDownIcon = new FETPImage("Resources//leftMouseDownIcon.png");

	if (LeftMouseUpIcon == nullptr)
		LeftMouseUpIcon = new FETPImage("Resources//leftMouseUpIcon.png");

	if (RightMouseDownIcon == nullptr)
		RightMouseDownIcon = new FETPImage("Resources//rightMouseDownIcon.png");

	if (RightMouseUpIcon == nullptr)
		RightMouseUpIcon = new FETPImage("Resources//rightMouseUpIcon.png");

	if (ScrollMouseIcon == nullptr)
		ScrollMouseIcon = new FETPImage("Resources//scrollMouseIcon.png");

	if (LunchApplicationIcon == nullptr)
		LunchApplicationIcon = new FETPImage("Resources//lunchApplicationIcon.png");

	if (ScreenshotIcon == nullptr)
		ScreenshotIcon = new FETPImage("Resources//screenshotIcon.png");
}

void globalActionNode::RenderIcon()
{
	if (Data->getType() == FETP_KEYBOARD_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 7.0f, ImGui::GetCursorScreenPos().y + 7.0f));
		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(Data);
		if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
			ImGui::Image((void*)(intptr_t)KeyDownIcon->getTextureID(), ImVec2(102.0f, 102.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
			ImGui::Image((void*)(intptr_t)KeyUpIcon->getTextureID(), ImVec2(102.0f, 102.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (Data->getType() == FETP_MOUSE_ACTION)
	{
		MouseAction* action = reinterpret_cast<MouseAction*>(Data);
		if (action->wParam == WM_MOUSEMOVE)
		{
			ImGui::Image((void*)(intptr_t)MoveMouseIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_LBUTTONDOWN)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)LeftMouseDownIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_LBUTTONUP)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)LeftMouseUpIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_RBUTTONDOWN)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)RightMouseDownIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_RBUTTONUP)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)RightMouseUpIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_MOUSEWHEEL)
		{
			ImGui::Image((void*)(intptr_t)ScrollMouseIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
	}
	else if (Data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 14.0f, ImGui::GetCursorScreenPos().y + 14.0f));
		ImGui::Image((void*)(intptr_t)ScreenshotIcon->getTextureID(), ImVec2(86.0f, 86.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (Data->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f, ImGui::GetCursorScreenPos().y + 10.0f));
		ImGui::Image((void*)(intptr_t)LunchApplicationIcon->getTextureID(), ImVec2(100.0f, 100.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (Data->getType() == FETP_SLEEP_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 4.0f, ImGui::GetCursorScreenPos().y + 4.0f));
		ImGui::Image((void*)(intptr_t)SleepIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
}