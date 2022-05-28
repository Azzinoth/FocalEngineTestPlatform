#include "combinedActionNode.h"

VISUAL_NODE_CHILD_CPP(combinedActionNode)
combinedActionNode* combinedActionNode::nodeForCallback = nullptr;
FETPImage* combinedActionNode::moveMouseCombineIcon = nullptr;
FETPImage* combinedActionNode::leftMouseCombineIcon = nullptr;
FETPImage* combinedActionNode::rightMouseCombineIcon = nullptr;
FETPImage* combinedActionNode::wheelMouseCombineIcon = nullptr;

FETPImage* combinedActionNode::keyCombinedIcon = nullptr;
FETPImage* combinedActionNode::textCombinedIcon = nullptr;

combinedActionNode::combinedActionNode() : FEVisualNode()
{
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

combinedActionNode::combinedActionNode(const combinedActionNode& src) : FEVisualNode(src)
{
	data = src.data;
	actionType = src.actionType;
	beginPosition = src.beginPosition;
	endPosition = src.endPosition;
	text = src.text;

	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

void combinedActionNode::initialize(std::vector<FETPAction*> Data, FETP_COMBINED_ACTION_TYPE type)
{
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
	setSize(ImVec2(330, 140));

	if (input.size() == 0 && output.size() == 0)
	{
		addInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, ""));
		addOutputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, ""));
	}

	data = Data;

	titleBackgroundColor = ImColor(31, 117, 208);
	titleBackgroundColorHovered = ImColor(35, 145, 255);

	this->type = "combinedActionNode";
	actionType = type;

	if (actionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
	{
		setName("COMBINED_MOUSE_MOVE");

		MouseAction* action = reinterpret_cast<MouseAction*>(data[0]);
		beginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);

		endPosition = glm::vec2(0);
		for (int i = data.size() - 1; i >= 0; i--)
		{
			if (data[i]->getType() == FETP_MOUSE_ACTION)
			{
				action = reinterpret_cast<MouseAction*>(data[i]);
				endPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);

				break;
			}
		}
	}
	else if (actionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
	{
		setName("COMBINED_LEFT_MOUSE_CLICK");
		titleBackgroundColor = ImColor(0, 162, 232);
		titleBackgroundColorHovered = ImColor(0, 152, 217);

		MouseAction* action = reinterpret_cast<MouseAction*>(data[0]);
		beginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
	}
	else if (actionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
	{
		setName("COMBINED_RIGHT_MOUSE_CLICK");
		titleBackgroundColor = ImColor(163, 73, 164);
		titleBackgroundColorHovered = ImColor(147, 66, 147);

		MouseAction* action = reinterpret_cast<MouseAction*>(data[0]);
		beginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
	}
	else if (actionType == FETP_COMBINED_WHEEL_MOUSE_ACTION)
	{
		setName("COMBINED_WHEEL_MOUSE_ROTATION");
		titleBackgroundColor = ImColor(163, 73, 164);
		titleBackgroundColorHovered = ImColor(147, 66, 147);

		MouseAction* action = reinterpret_cast<MouseAction*>(data[0]);
		beginPosition = glm::vec2(action->additionalInfo.pt.x, action->additionalInfo.pt.y);
	}
	else if (actionType == FETP_COMBINED_KEY_PRESS_ACTION)
	{
		setName("COMBINED_KEY_PRESS");
		if (data[0]->getType() == FETP_KEYBOARD_ACTION)
		{
			text = "vkCode: " + std::to_string(int(reinterpret_cast<KeyboardAction*>(data[0])->additionalInfo.vkCode));
		}
	}
	else if (actionType == FETP_COMBINED_TEXT_INPUT_ACTION)
	{
		setName("COMBINED_TEXT_INPUT");
		titleBackgroundColor = ImColor(163, 73, 164);
		titleBackgroundColorHovered = ImColor(147, 66, 147);

		text = ACTION_SYSTEM.extractText(data);
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

combinedActionNode::combinedActionNode(std::vector<FETPAction*> data, FETP_COMBINED_ACTION_TYPE type) : FEVisualNode()
{
	initialize(data, type);
}

void combinedActionNode::draw()
{	
	FEVisualNode::draw();

	if (getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		// Show client rect.
		/*ImVec2 regionMin = ImVec2(ImGui::GetCursorScreenPos().x + this->getClientRegionPosition().x,
								  ImGui::GetCursorScreenPos().y + this->getClientRegionPosition().y);

		ImVec2 regionMax = ImVec2(regionMin.x + this->getClientRegionSize().x,
								  regionMin.y + this->getClientRegionSize().y);

		ImGui::GetWindowDrawList()->AddRectFilled(regionMin, regionMax, IM_COL32(175, 175, 175, 125), 8.0f);*/

		float xPosition = ImGui::GetCursorScreenPos().x;
		xPosition += 80.0f;
		float yPosition = ImGui::GetCursorScreenPos().y;
		yPosition += NODE_TITLE_HEIGHT + 13.0f;
		ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

		ImGui::SetNextItemWidth(80);
		int count = data.size();
		ImGui::BeginDisabled();
		ImGui::InputInt("actions count", &count);
		ImGui::EndDisabled();

		if (actionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int begin_Position[] = { 0 };
			begin_Position[0] = int(beginPosition.x);
			begin_Position[1] = int(beginPosition.y);

			if (ImGui::InputInt2("begin position", begin_Position))
			{
				parentArea->propagateUpdateToConnectedNodes(this);
			}

			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int end_Position[] = { 0 };
			end_Position[0] = int(endPosition.x);
			end_Position[1] = int(endPosition.y);

			if (ImGui::InputInt2("end position", end_Position))
			{
				parentArea->propagateUpdateToConnectedNodes(this);
			}
		}
		else if (actionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int position[] = { 0 };
			position[0] = int(beginPosition.x);
			position[1] = int(beginPosition.y);

			if (ImGui::InputInt2("position", position))
			{
				parentArea->propagateUpdateToConnectedNodes(this);
			}
		}
		else if (actionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::SetNextItemWidth(140);
			static int position[] = { 0 };
			position[0] = int(beginPosition.x);
			position[1] = int(beginPosition.y);

			if (ImGui::InputInt2("position", position))
			{
				parentArea->propagateUpdateToConnectedNodes(this);
			}
		}
		else if (actionType == FETP_COMBINED_KEY_PRESS_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::Text(("vkCode: " + std::to_string(int(reinterpret_cast<KeyboardAction*>(data[0])->additionalInfo.vkCode))).c_str());
		}
		else if (actionType == FETP_COMBINED_TEXT_INPUT_ACTION)
		{
			yPosition += 20.0f;
			ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));

			ImGui::Text(text.c_str());
		}
	}
	else if (getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		checkIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		renderIcon();

		if (parentArea->isMouseHovered() && isHovered() && !actionEditPopup::getInstance().isOpened() && !textInputPopup::getInstance().isOpened())
			showTooltip();
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
				textInputPopup::getInstance().show(changeTextCallback, text);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			shouldBeDestroyed = true;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();*/
}

void combinedActionNode::socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType)
{
	FEVisualNode::socketEvent(ownSocket,  connectedSocket, eventType);
}

std::vector<FETPAction*> combinedActionNode::getData()
{
	return data;
}

bool combinedActionNode::canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser)
{
	if (!FEVisualNode::canConnect(ownSocket, candidateSocket, nullptr))
		return false;

	if (candidateSocket->getType() == FE_NODE_SOCKET_FLOAT_CHANNEL_OUT && ownSocket->getType() == FE_NODE_SOCKET_FLOAT_CHANNEL_IN)
		return true;

	return false;
}

Json::Value combinedActionNode::toJson()
{
	Json::Value result = FEVisualNode::toJson();
	result["actionType"] = actionType;

	for (size_t i = 0; i < data.size(); i++)
	{
		result["actions"][std::to_string(i)] = data[i]->toJson();
		if (data[i]->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data[i]);
			action->saveImagesToDisk();
		}
	}

	return result;
}

FEVisualNode* combinedActionNode::getNextNode()
{
	if (output.size() > 0 && output[0]->getConnections().size() > 0)
		return output[0]->getConnections()[0]->getParent();

	return nullptr;
}

void combinedActionNode::fromJson(Json::Value json)
{
	FEVisualNode::fromJson(json);

	actionType = FETP_COMBINED_ACTION_TYPE(json["actionType"].asInt());

	std::vector<Json::String> actionsList = json["actions"].getMemberNames();
	data.resize(actionsList.size());
	for (size_t i = 0; i < data.size(); i++)
	{
		size_t index = atoi(actionsList[i].c_str());
		if (FETP_ACTION_TYPE(json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_KEYBOARD_ACTION)
		{
			data[index] = new KeyboardAction();
		}
		else if (FETP_ACTION_TYPE(json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_MOUSE_ACTION)
		{
			data[index] = new MouseAction();
		}
		else if (FETP_ACTION_TYPE(json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			data[index] = new ScreenshootCompareAction();
		}
		else if (FETP_ACTION_TYPE(json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_LUNCH_APPLICATION_ACTION)
		{
			data[index] = new LunchApplicationAction();
		}
		else if (FETP_ACTION_TYPE(json["actions"][actionsList[i]]["internalType"].asInt()) == FETP_SLEEP_ACTION)
		{
			data[index] = new SleepAction();
		}
		
		data[index]->fromJson(json["actions"][actionsList[i]]);
	}

	initialize(data, actionType);
}

void combinedActionNode::changeText(std::string newText, int avarageDelay)
{
	if (actionType != FETP_COMBINED_TEXT_INPUT_ACTION)
		return;

	if (avarageDelay < 0)
	{
		int totalSleepTime = 0;
		int totalSleepNodes = 0;
		for (size_t i = 0; i < data.size(); i++)
		{
			if (data[i]->getType() == FETP_SLEEP_ACTION)
			{
				totalSleepTime += reinterpret_cast<SleepAction*>(data[i])->sleepFor;
				totalSleepNodes++;
			}
		}

		avarageDelay = 0;
		if (totalSleepNodes != 0)
			avarageDelay = totalSleepTime / totalSleepNodes;
	}

	// Clear all previous data.
	for (size_t i = 0; i < data.size(); i++)
		delete data[i];
	data.clear();

	data = ACTION_SYSTEM.generateInputTextActions(newText, 20);

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

	text = newText;
}

void combinedActionNode::changeTextCallback(std::string newText)
{
	if (nodeForCallback != nullptr)
		nodeForCallback->changeText(newText);
}

//bool combinedActionNode::openContextMenu()
//{
//	contextMenu = true;
//	return true;
//}

void combinedActionNode::checkIcons()
{
	if (moveMouseCombineIcon == nullptr)
		moveMouseCombineIcon = new FETPImage("Resources//moveMouseCombineIcon.png");

	if (leftMouseCombineIcon == nullptr)
		leftMouseCombineIcon = new FETPImage("Resources//leftMouseCombineIcon.png");

	if (rightMouseCombineIcon == nullptr)
		rightMouseCombineIcon = new FETPImage("Resources//rightMouseCombineIcon.png");

	if (wheelMouseCombineIcon == nullptr)
		wheelMouseCombineIcon = new FETPImage("Resources//scrollMouseCombinedIcon.png");

	if (keyCombinedIcon == nullptr)
		keyCombinedIcon = new FETPImage("Resources//keyCombinedIcon.png");

	if (textCombinedIcon == nullptr)
		textCombinedIcon = new FETPImage("Resources//textCombinedIcon.png");
}

void combinedActionNode::renderIcon()
{
	if (actionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
	{
		ImGui::Image((void*)(intptr_t)moveMouseCombineIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (actionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 6.0f));
		ImGui::Image((void*)(intptr_t)leftMouseCombineIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (actionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 6.0f));
		ImGui::Image((void*)(intptr_t)rightMouseCombineIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (actionType == FETP_COMBINED_WHEEL_MOUSE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 6.0f));
		ImGui::Image((void*)(intptr_t)wheelMouseCombineIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (actionType == FETP_COMBINED_KEY_PRESS_ACTION)
	{
		ImGui::Image((void*)(intptr_t)keyCombinedIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (actionType == FETP_COMBINED_TEXT_INPUT_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 14.0f, ImGui::GetCursorScreenPos().y + 14.0f));
		ImGui::Image((void*)(intptr_t)textCombinedIcon->getTextureID(), ImVec2(88.0f, 88.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
}

void combinedActionNode::showTooltip()
{
	std::string textToShow = "Name: " + getName();

	if (actionType == FETP_COMBINED_MOUSE_MOVE_ACTION)
	{
		textToShow += "\nType: COMBINED_MOUSE_MOVE";

		textToShow += "\nstart: x = " + std::to_string(int(beginPosition.x)) + " y = " + std::to_string(int(beginPosition.y));
		textToShow += "\nend: x = " + std::to_string(int(endPosition.x)) + " y = " + std::to_string(int(endPosition.y));
	}
	else if (actionType == FETP_COMBINED_LEFT_MOUSE_ACTION)
	{
		textToShow += "\nType: COMBINED_LEFT_MOUSE";
	}
	else if (actionType == FETP_COMBINED_RIGHT_MOUSE_ACTION)
	{
		textToShow += "\nType: COMBINED_RIGHT_MOUSE";
	}
	else if (actionType == FETP_COMBINED_WHEEL_MOUSE_ACTION)
	{
		textToShow += "\nType: COMBINED_WHEEL_MOUSE";
	}
	else if (actionType == FETP_COMBINED_KEY_PRESS_ACTION)
	{
		textToShow += "\nType: COMBINED_KEY_PRESS";
	}
	else if (actionType == FETP_COMBINED_TEXT_INPUT_ACTION)
	{
		textToShow += "\nType: COMBINED_TEXT_INPUT";
		textToShow += "\nText: " + text;
	}

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::BeginTooltip();
	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	ImGui::TextUnformatted(textToShow.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndTooltip();
	ImGui::PopFont();
}

FETP_COMBINED_ACTION_TYPE combinedActionNode::getCombinedActionType()
{
	return actionType;
}

void combinedActionNode::remove()
{
	shouldBeDestroyed = true;
}