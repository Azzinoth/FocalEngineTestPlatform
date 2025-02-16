#include "combinedActionNode.h"
using namespace VisNodeSys;

combinedActionNode* combinedActionNode::NodeForCallback = nullptr;
FETPImage* combinedActionNode::MoveMouseCombineIcon = nullptr;
FETPImage* combinedActionNode::LeftMouseCombineIcon = nullptr;
FETPImage* combinedActionNode::RightMouseCombineIcon = nullptr;
FETPImage* combinedActionNode::WheelMouseCombineIcon = nullptr;

FETPImage* combinedActionNode::KeyCombinedIcon = nullptr;
FETPImage* combinedActionNode::TextCombinedIcon = nullptr;

bool combinedActionNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("combinedActionNode",
		[]() -> Node* {
			return new combinedActionNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const combinedActionNode& NodeToCopy = static_cast<const combinedActionNode&>(CurrentNode);
			return new combinedActionNode(NodeToCopy);
		}
	);

	return true;
}();


combinedActionNode::combinedActionNode() : basicLogicNode()
{
	SetStyle(CIRCLE);
}

combinedActionNode::combinedActionNode(const combinedActionNode& Src) : basicLogicNode(Src)
{
	Data = Src.Data;
	ActionType = Src.ActionType;
	BeginPosition = Src.BeginPosition;
	EndPosition = Src.EndPosition;
	Text = Src.Text;

	SetStyle(CIRCLE);
}

void combinedActionNode::Initialize(std::vector<FETPAction*> Data, FETP_COMBINED_ACTION_TYPE Type)
{
	SetStyle(CIRCLE);
	SetSize(ImVec2(330, 140));

	if (Input.size() == 0 && Output.size() == 0)
	{
		AddSocket(new NodeSocket(this, "FLOAT", "", false));
		AddSocket(new NodeSocket(this, "FLOAT", "", true));
	}

	Data = Data;

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	this->Type = "combinedActionNode";
	ActionType = Type;

	if (ActionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
	{
		SetName("COMBINED_MOUSE_MOVE");

		MouseAction* action = reinterpret_cast<MouseAction*>(Data[0]);
		BeginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);

		EndPosition = glm::vec2(0);
		for (int i = static_cast<int>(Data.size() - 1); i >= 0; i--)
		{
			if (Data[i]->getType() == FETP_MOUSE_ACTION)
			{
				action = reinterpret_cast<MouseAction*>(Data[i]);
				EndPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);

				break;
			}
		}
	}
	else if (ActionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
	{
		SetName("COMBINED_LEFT_MOUSE_CLICK");
		TitleBackgroundColor = ImColor(0, 162, 232);
		TitleBackgroundColorHovered = ImColor(0, 152, 217);

		MouseAction* action = reinterpret_cast<MouseAction*>(Data[0]);
		BeginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
	}
	else if (ActionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
	{
		SetName("COMBINED_RIGHT_MOUSE_CLICK");
		TitleBackgroundColor = ImColor(163, 73, 164);
		TitleBackgroundColorHovered = ImColor(147, 66, 147);

		MouseAction* action = reinterpret_cast<MouseAction*>(Data[0]);
		BeginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
	}
	else if (ActionType == FETP_COMBINED_WHEEL_MOUSE_ACTION)
	{
		SetName("COMBINED_WHEEL_MOUSE_ROTATION");
		TitleBackgroundColor = ImColor(163, 73, 164);
		TitleBackgroundColorHovered = ImColor(147, 66, 147);

		MouseAction* action = reinterpret_cast<MouseAction*>(Data[0]);
		BeginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
	}
	else if (ActionType == FETP_COMBINED_KEY_PRESS_ACTION)
	{
		SetName("COMBINED_KEY_PRESS");
		if (Data[0]->getType() == FETP_KEYBOARD_ACTION)
		{
			Text = "vkCode: " + std::to_string(int(reinterpret_cast<KeyboardAction*>(Data[0])->additionalInfo.vkCode));
		}
	}
	else if (ActionType == FETP_COMBINED_TEXT_INPUT_ACTION)
	{
		SetName("COMBINED_TEXT_INPUT");
		TitleBackgroundColor = ImColor(163, 73, 164);
		TitleBackgroundColorHovered = ImColor(147, 66, 147);

		Text = ACTION_SYSTEM.extractText(Data);
		/*text = "";
		for (size_t i = 0; i < data.size(); i++)
		{
			if (data[i]->getType() != FETP_KEYBOARD_ACTION)
				continue;

			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(data[i]);
			if (action->wParam == WM_KEYUP)
			{
				char tempChar = INPUT_SYSTEM.getChar(action);
				if (tempChar != 0)
					text += tempChar;
			}
		}*/
	}
}

combinedActionNode::combinedActionNode(std::vector<FETPAction*> Data, FETP_COMBINED_ACTION_TYPE Type) : basicLogicNode()
{
	Initialize(Data, Type);
}

void combinedActionNode::Draw()
{	
	Node::Draw();

	if (GetStyle() == DEFAULT)
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

		ImGui::SetNextItemWidth(80);
		int count = static_cast<int>(Data.size());
		ImGui::BeginDisabled();
		ImGui::InputInt("actions count", &count);
		ImGui::EndDisabled();

		if (ActionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int begin_Position[] = { 0, 0 };
			begin_Position[0] = int(BeginPosition.x);
			begin_Position[1] = int(BeginPosition.y);

			if (ImGui::InputInt2("begin position", begin_Position))
			{
				ParentArea->PropagateUpdateToConnectedNodes(this);
			}

			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int end_Position[] = { 0, 0 };
			end_Position[0] = int(EndPosition.x);
			end_Position[1] = int(EndPosition.y);

			if (ImGui::InputInt2("end position", end_Position))
			{
				ParentArea->PropagateUpdateToConnectedNodes(this);
			}
		}
		else if (ActionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int position[] = { 0, 0 };
			position[0] = int(BeginPosition.x);
			position[1] = int(BeginPosition.y);

			if (ImGui::InputInt2("position", position))
			{
				ParentArea->PropagateUpdateToConnectedNodes(this);
			}
		}
		else if (ActionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int position[] = { 0, 0 };
			position[0] = int(BeginPosition.x);
			position[1] = int(BeginPosition.y);

			if (ImGui::InputInt2("position", position))
			{
				ParentArea->PropagateUpdateToConnectedNodes(this);
			}
		}
		else if (ActionType == FETP_COMBINED_KEY_PRESS_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::Text(("vkCode: " + std::to_string(int(reinterpret_cast<KeyboardAction*>(Data[0])->additionalInfo.vkCode))).c_str());
		}
		else if (ActionType == FETP_COMBINED_TEXT_INPUT_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::Text(Text.c_str());
		}
	}
	else if (GetStyle() == CIRCLE)
	{
		CheckIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		RenderIcon();

		if (ParentArea->IsMouseHovered() && IsHovered() && !actionEditPopup::GetInstance().isOpened() && !textInputPopup::GetInstance().isOpened())
			ShowTooltip();
	}

	/*if (contextMenu)
	{
		contextMenu = false;
		ImGui::OpenPopup("##context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopup("##context_menu"))
	{
		if (actionType == FETP_COMBINED_TEXT_INPUT_ACTION)
		{
			if (ImGui::MenuItem(std::string("Change input text").c_str()))
			{
				combinedActionNode::nodeForCallback = this;
				textInputPopup::GetInstance().show(changeTextCallback, text);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			bShouldBeDestroyed = true;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();*/
}

void combinedActionNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

std::vector<FETPAction*> combinedActionNode::GetData()
{
	return Data;
}

bool combinedActionNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	if (CandidateSocket->GetType() == "FLOAT" && OwnSocket->GetType() == "FLOAT")
		return true;

	return false;
}

Json::Value combinedActionNode::ToJson()
{
	Json::Value result = Node::ToJson();
	result["actionType"] = ActionType;

	for (size_t i = 0; i < Data.size(); i++)
	{
		result["actions"][std::to_string(i)] = Data[i]->toJson();
		if (Data[i]->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(Data[i]);
			action->saveImagesToDisk();
		}
	}

	return result;
}

basicLogicNode* combinedActionNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());

	return nullptr;
}

void combinedActionNode::FromJson(Json::Value Json)
{
	Node::FromJson(Json);

	ActionType = FETP_COMBINED_ACTION_TYPE(Json["actionType"].asInt());

	std::vector<Json::String> actionsList = Json["actions"].getMemberNames();
	Data.resize(actionsList.size());
	for (size_t i = 0; i < Data.size(); i++)
	{
		size_t index = atoi(actionsList[i].c_str());
		if (FETP_ACTION_TYPE(Json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_KEYBOARD_ACTION)
		{
			Data[index] = new KeyboardAction();
		}
		else if (FETP_ACTION_TYPE(Json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_MOUSE_ACTION)
		{
			Data[index] = new MouseAction();
		}
		else if (FETP_ACTION_TYPE(Json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			Data[index] = new ScreenshootCompareAction();
		}
		else if (FETP_ACTION_TYPE(Json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_LUNCH_APPLICATION_ACTION)
		{
			Data[index] = new LunchApplicationAction();
		}
		else if (FETP_ACTION_TYPE(Json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_SLEEP_ACTION)
		{
			Data[index] = new SleepAction();
		}
		
		Data[index]->fromJson(Json["actions"][actionsList[i]]);
	}

	Initialize(Data, ActionType);
}

void combinedActionNode::ChangeText(std::string NewText, int AvarageDelay)
{
	if (ActionType != FETP_COMBINED_TEXT_INPUT_ACTION)
		return;

	if (AvarageDelay < 0)
	{
		int totalSleepTime = 0;
		int totalSleepNodes = 0;
		for (size_t i = 0; i < Data.size(); i++)
		{
			if (Data[i]->getType() == FETP_SLEEP_ACTION)
			{
				totalSleepTime += reinterpret_cast<SleepAction*>(Data[i])->sleepFor;
				totalSleepNodes++;
			}
		}

		AvarageDelay = 0;
		if (totalSleepNodes != 0)
			AvarageDelay = totalSleepTime / totalSleepNodes;
	}

	// Clear all previous data.
	for (size_t i = 0; i < Data.size(); i++)
		delete Data[i];
	Data.clear();

	Data = ACTION_SYSTEM.generateInputTextActions(NewText, 20);

	//for (size_t i = 0; i < newText.size(); i++)
	//{
	//	int convertedKey = VkKeyScanExA(char(newText[i]), GetKeyboardLayout(0));
	//	int vkCode = convertedKey & 0xff;
	//	int keysState = (convertedKey & 0xff00) >> 8;

	//	// Can't find appropriate key for that char.
	//	if (vkCode == -1)
	//		continue;

	//	if (keysState & 1)
	//	{
	//		KeyboardAction* newAction = new KeyboardAction();
	//		newAction->additionalInfo.vkCode = 0x10;
	//		newAction->wParam = WM_KEYDOWN;
	//		newAction->shiftPressed = false;
	//		data.push_back(newAction);
	//	}

	//	KeyboardAction* newAction = new KeyboardAction();
	//	newAction->additionalInfo.vkCode = vkCode;
	//	newAction->wParam = WM_KEYDOWN;
	//	newAction->shiftPressed = keysState & 1;
	//	data.push_back(newAction);

	//	SleepAction* newSleepAction = new SleepAction(avarageDelay);
	//	data.push_back(newSleepAction);

	//	newAction = new KeyboardAction();
	//	newAction->additionalInfo.vkCode = vkCode;
	//	newAction->wParam = WM_KEYUP;
	//	newAction->shiftPressed = keysState & 1;
	//	data.push_back(newAction);

	//	if (keysState & 1)
	//	{
	//		KeyboardAction* newAction = new KeyboardAction();
	//		newAction->additionalInfo.vkCode = 0x10;
	//		newAction->wParam = WM_KEYUP;
	//		newAction->shiftPressed = false;
	//		data.push_back(newAction);
	//	}
	//}

	Text = NewText;
}

void combinedActionNode::ChangeTextCallback(std::string NewText)
{
	if (NodeForCallback != nullptr)
		NodeForCallback->ChangeText(NewText);
}

//bool combinedActionNode::openContextMenu()
//{
//	contextMenu = true;
//	return true;
//}

void combinedActionNode::CheckIcons()
{
	if (MoveMouseCombineIcon == nullptr)
		MoveMouseCombineIcon = new FETPImage("Resources//moveMouseCombineIcon.png");

	if (LeftMouseCombineIcon == nullptr)
		LeftMouseCombineIcon = new FETPImage("Resources//leftMouseCombineIcon.png");

	if (RightMouseCombineIcon == nullptr)
		RightMouseCombineIcon = new FETPImage("Resources//rightMouseCombineIcon.png");

	if (WheelMouseCombineIcon == nullptr)
		WheelMouseCombineIcon = new FETPImage("Resources//scrollMouseCombinedIcon.png");

	if (KeyCombinedIcon == nullptr)
		KeyCombinedIcon = new FETPImage("Resources//keyCombinedIcon.png");

	if (TextCombinedIcon == nullptr)
		TextCombinedIcon = new FETPImage("Resources//textCombinedIcon.png");
}

void combinedActionNode::RenderIcon()
{
	if (ActionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
	{
		ImGui::Image((void*)(intptr_t)MoveMouseCombineIcon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (ActionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 6.0f));
		ImGui::Image((void*)(intptr_t)LeftMouseCombineIcon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (ActionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 6.0f));
		ImGui::Image((void*)(intptr_t)RightMouseCombineIcon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (ActionType == FETP_COMBINED_WHEEL_MOUSE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 6.0f));
		ImGui::Image((void*)(intptr_t)WheelMouseCombineIcon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (ActionType == FETP_COMBINED_KEY_PRESS_ACTION)
	{
		ImGui::Image((void*)(intptr_t)KeyCombinedIcon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (ActionType == FETP_COMBINED_TEXT_INPUT_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 14.0f, ImGui::GetCursorScreenPos().y + 14.0f));
		ImGui::Image((void*)(intptr_t)TextCombinedIcon->GetTextureID(), ImVec2(88.0f, 88.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
}

void combinedActionNode::ShowTooltip()
{
	std::string textToShow = "Name: " + GetName();

	if (ActionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
	{
		textToShow += "\nType: COMBINED_MOUSE_MOVE";

		textToShow += "\nstart: x = " + std::to_string(int(BeginPosition.x)) + " y = " + std::to_string(int(BeginPosition.y));
		textToShow += "\nend: x = " + std::to_string(int(EndPosition.x)) + " y = " + std::to_string(int(EndPosition.y));
	}
	else if (ActionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
	{
		textToShow += "\nType: COMBINED_LEFT_MOUSE";
	}
	else if (ActionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
	{
		textToShow += "\nType: COMBINED_RIGHT_MOUSE";
	}
	else if (ActionType == FETP_COMBINED_WHEEL_MOUSE_ACTION)
	{
		textToShow += "\nType: COMBINED_WHEEL_MOUSE";
	}
	else if (ActionType == FETP_COMBINED_KEY_PRESS_ACTION)
	{
		textToShow += "\nType: COMBINED_KEY_PRESS";
	}
	else if (ActionType == FETP_COMBINED_TEXT_INPUT_ACTION)
	{
		textToShow += "\nType: COMBINED_TEXT_INPUT";
		textToShow += "\nText: " + Text;
	}

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::BeginTooltip();
	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	ImGui::TextUnformatted(textToShow.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndTooltip();
	ImGui::PopFont();
}

FETP_COMBINED_ACTION_TYPE combinedActionNode::GetCombinedActionType()
{
	return ActionType;
}

void combinedActionNode::Remove()
{
	bShouldBeDestroyed = true;
}