#include "globalActionNode.h"

VISUAL_NODE_CHILD_CPP(globalActionNode)
FETPImage* globalActionNode::sleepIcon = nullptr;

FETPImage* globalActionNode::keyDownIcon = nullptr;
FETPImage* globalActionNode::keyUpIcon = nullptr;

FETPImage* globalActionNode::moveMouseIcon = nullptr;
FETPImage* globalActionNode::leftMouseDownIcon = nullptr;
FETPImage* globalActionNode::leftMouseUpIcon = nullptr;
FETPImage* globalActionNode::rightMouseDownIcon = nullptr;
FETPImage* globalActionNode::rightMouseUpIcon = nullptr;
FETPImage* globalActionNode::scrollMouseIcon = nullptr;

FETPImage* globalActionNode::lunchApplicationIcon = nullptr;
FETPImage* globalActionNode::screenshotIcon = nullptr;

globalActionNode::globalActionNode() : FEVisualNode()
{
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

void globalActionNode::copyData(FETPAction* src)
{
	if (src->getType() == FETP_KEYBOARD_ACTION)
	{
		data = new KeyboardAction(*reinterpret_cast<KeyboardAction*>(src));
	}
	else if (src->getType() == FETP_MOUSE_ACTION)
	{
		data = new MouseAction(*reinterpret_cast<MouseAction*>(src));
	}
	else if (src->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		data = new ScreenshootCompareAction(*reinterpret_cast<ScreenshootCompareAction*>(src));
	}
	else if (src->getType() == FETP_SLEEP_ACTION)
	{
		data = new SleepAction(*reinterpret_cast<SleepAction*>(src));
	}
	else if (src->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		data = new LunchApplicationAction(*reinterpret_cast<LunchApplicationAction*>(src));
	}
	else
	{
		data = new FETPAction(*src);
	}
}

globalActionNode::globalActionNode(const globalActionNode& src) : FEVisualNode(src)
{
	copyData(src.data);
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

globalActionNode::~globalActionNode()
{
	if (data != nullptr)
		delete data;
}

void globalActionNode::initialize(FETPAction* data)
{
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
	type = "globalActionNode";
	this->data = data;

	titleBackgroundColor = ImColor(31, 117, 208);
	titleBackgroundColorHovered = ImColor(35, 145, 255);

	if (data->getType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(data);
		if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
		{
			setName("keyDown");
			titleBackgroundColor = ImColor(34, 177, 76);
			titleBackgroundColorHovered = ImColor(32, 166, 72);
		}
		else if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
		{
			setName("keyUp");
			titleBackgroundColor = ImColor(181, 230, 29);
			titleBackgroundColorHovered = ImColor(174, 224, 24);
		}
	}
	else if (data->getType() == FETP_MOUSE_ACTION)
	{
		MouseAction* action = reinterpret_cast<MouseAction*>(data);
		if (action->wParam == WM_LBUTTONUP)
		{
			setName("mouseLeftUp");
			titleBackgroundColor = ImColor(153, 217, 234);
			titleBackgroundColorHovered = ImColor(124, 207, 228);
		}
		else if (action->wParam == WM_RBUTTONUP)
		{
			setName("mouseRightUp");
			titleBackgroundColor = ImColor(200, 191, 231);
			titleBackgroundColorHovered = ImColor(171, 157, 219);
		}
		else if (action->wParam == WM_LBUTTONDOWN)
		{
			setName("mouseLeftDown");
			titleBackgroundColor = ImColor(0, 162, 232);
			titleBackgroundColorHovered = ImColor(0, 152, 217);
		}
		else if (action->wParam == WM_RBUTTONDOWN)
		{
			setName("mouseRightDown");
			titleBackgroundColor = ImColor(163, 73, 164);
			titleBackgroundColorHovered = ImColor(147, 66, 147);
		}
		else if (action->wParam == WM_MOUSEMOVE)
		{
			setName("mouseMove");
			titleBackgroundColor = ImColor(136, 0, 21);
			titleBackgroundColorHovered = ImColor(113, 0, 17);
		}
		else if (action->wParam == WM_MOUSEWHEEL)
		{
			setName("mouseWheel");
			titleBackgroundColor = ImColor(136, 0, 21);
			titleBackgroundColorHovered = ImColor(113, 0, 17);
		}
	}
	else if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		setName("screenshoot");
		titleBackgroundColor = ImColor(136, 0, 21);
		titleBackgroundColorHovered = ImColor(111, 0, 17);

		if (input.size() == 0 && output.size() == 0)
		{
			addInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, ""));

			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data);
			for (size_t i = 0; i < action->imagesInfo.size(); i++)
			{
				addOutputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, "next_" + std::to_string(i + 1)));
			}
		}
	}
	else if (data->getType() == FETP_SLEEP_ACTION)
	{
		setName("sleep");
	}
	else if (data->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		setName("lunch application");
	}

	setSize(ImVec2(330, 140));

	if (input.size() == 0 && output.size() == 0)
	{
		addInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, ""));
		addOutputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, ""));
	}
}

globalActionNode::globalActionNode(FETPAction* data) : FEVisualNode()
{
	initialize(data);
}

void globalActionNode::draw()
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

		if (data->getType() == FETP_KEYBOARD_ACTION)
		{
			KeyboardAction* action = reinterpret_cast<KeyboardAction*>(data);
			ImGui::SetNextItemWidth(80);
			int keyCode = action->additionalInfo.vkCode;
			if (ImGui::InputInt("key code", &keyCode))
			{
				parentArea->propagateUpdateToConnectedNodes(this);
			}

			action->additionalInfo.vkCode = keyCode;
		}
		else if (data->getType() == FETP_MOUSE_ACTION)
		{
			MouseAction* action = reinterpret_cast<MouseAction*>(data);

			if (action->wParam == WM_MOUSEMOVE)
			{
				ImGui::SetNextItemWidth(140);
				static int position[] = { 0 };
				position[0] = action->additionalInfo.pt.x;
				position[1] = action->additionalInfo.pt.y;

				if (ImGui::InputInt2("position", position))
				{
					parentArea->propagateUpdateToConnectedNodes(this);
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
		else if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data);

			if (action->imagesInfo.size() > 0 && action->imagesInfo[0] != nullptr)
			{
				ImGui::SetCursorScreenPos(ImVec2(xPosition - 10.0f, yPosition - 10.0f));
				glm::vec2 imageSize = SCREEN_SYSTEM.imageSizeInRegion(action->imagesInfo[0]->image->getWidth(),
					action->imagesInfo[0]->image->getHeight(),
					size_t(this->getClientRegionSize().x * 0.95f),
					size_t(this->getClientRegionSize().y * 0.55f));

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
		else if (data->getType() == FETP_SLEEP_ACTION)
		{
			SleepAction* action = reinterpret_cast<SleepAction*>(data);
			ImGui::SetNextItemWidth(80);
			int sleepFor = action->sleepFor;
			if (ImGui::InputInt("sleep for", &sleepFor))
			{
				action->sleepFor = sleepFor;
				parentArea->propagateUpdateToConnectedNodes(this);
			}
		}
		else if (data->getType() == FETP_LUNCH_APPLICATION_ACTION)
		{
			LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(data);
			ImGui::Text(action->applicationPath.c_str());
		}
	}
	else if (getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		checkIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		renderIcon();

		if (parentArea->isMouseHovered() && isHovered() && !actionEditPopup::getInstance().isOpened() && !textInputPopup::getInstance().isOpened() /*&& !screenshootEditor::getInstance().isVisible()*/)
			showTooltip();
	}

	if (contextMenu)
	{
		contextMenu = false;
		ImGui::OpenPopup("##context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopup("##context_menu"))
	{
		if (ImGui::MenuItem(std::string("Edit...").c_str()))
		{
			if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
			{
				ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data);
				screenshootEditor::getInstance().show(action);
			}
			else if (data->getType() == FETP_LUNCH_APPLICATION_ACTION)
			{
				std::string path;
				FocalEngine::FILE_SYSTEM.showFileOpenDialog(path, applicationLoadFilter, 1);

				if (path != "")
				{
					LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(data);
					action->applicationPath = path;
				}
			}
			else
			{
				actionEditPopup::getInstance().show(data);
			}
		}

		if (ImGui::MenuItem(std::string("Remove").c_str()))
		{
			shouldBeDestroyed = true;
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

void globalActionNode::socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType)
{
	FEVisualNode::socketEvent(ownSocket,  connectedSocket, eventType);
}

FETPAction* globalActionNode::getData()
{
	return data;
}

bool globalActionNode::canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser)
{
	if (!FEVisualNode::canConnect(ownSocket, candidateSocket, nullptr))
		return false;

	if (candidateSocket->getType() == FE_NODE_SOCKET_FLOAT_CHANNEL_OUT && ownSocket->getType() == FE_NODE_SOCKET_FLOAT_CHANNEL_IN)
		return true;

	return false;
}

Json::Value globalActionNode::toJson()
{
	Json::Value result = FEVisualNode::toJson();

	if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data);
		action->saveImagesToDisk();
	}
	result["action"] = data->toJson();

	return result;
}

FEVisualNode* globalActionNode::getNextNode()
{
	if (output.size() > 0 && output[0]->getConnections().size() > 0)
	{
		if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
		{
			ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data);
			for (size_t i = 0; i < action->imagesInfo.size(); i++)
			{
				if (action->imagesInfo[i]->lastRunResult)
					return output[i]->getConnections()[0]->getParent();
			}

			return output[0]->getConnections()[0]->getParent();
		}
		else
		{
			return output[0]->getConnections()[0]->getParent();
		}
	}

	return nullptr;
}

void globalActionNode::fromJson(Json::Value json)
{
	FEVisualNode::fromJson(json);

	if (FETP_ACTION_TYPE(json["action"]["internalType"].asInt()) == FETP_KEYBOARD_ACTION)
	{
		data = new KeyboardAction();
	}
	else if (FETP_ACTION_TYPE(json["action"]["internalType"].asInt()) == FETP_MOUSE_ACTION)
	{
		data = new MouseAction();
	}
	else if (FETP_ACTION_TYPE(json["action"]["internalType"].asInt()) == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		data = new ScreenshootCompareAction();
	}
	else if (FETP_ACTION_TYPE(json["action"]["internalType"].asInt()) == FETP_LUNCH_APPLICATION_ACTION)
	{
		data = new LunchApplicationAction();
	}
	else if (FETP_ACTION_TYPE(json["action"]["internalType"].asInt()) == FETP_SLEEP_ACTION)
	{
		data = new SleepAction();
	}

	if (data != nullptr)
	{
		data->fromJson(json["action"]);
		initialize(data);
	}
}

FEVisualNode* globalActionNode::getLogicallyNextNode()
{
	if (data->getType() != FETP_SCREENSHOOT_COMPARE_ACTION)
		return FEVisualNode::getLogicallyNextNode();

	ScreenshootCompareAction* action = reinterpret_cast<ScreenshootCompareAction*>(data);
	for (size_t i = 0; i < action->imagesInfo.size(); i++)
	{
		if (action->imagesInfo[i]->lastRunResult)
		{
			if (output[i]->getConnections().size() > 0)
				return output[i]->getConnections()[0]->getParent();
		}
	}

	return nullptr;
}

bool globalActionNode::openContextMenu()
{
	contextMenu = true;
	return true;
}

void globalActionNode::showTooltip()
{
	std::string textToShow = "Name: " + getName() + "\nAction type: " + FETPAction::FETPActionTypeToString(data->getType());
	textToShow += "\nID: " + data->getID();

	if (data->getType() == FETP_KEYBOARD_ACTION)
	{
		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(data);
		if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
			textToShow += "\nSub type: Key down";
		if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
			textToShow += "\nSub type: Key up";

		textToShow += "\nKey code: " + std::to_string(int(action->additionalInfo.vkCode));
	}
	else if (data->getType() == FETP_MOUSE_ACTION)
	{
		MouseAction* action = reinterpret_cast<MouseAction*>(data);
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
	else if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		textToShow += "\nSub type: Screenshot";
	}
	else if (data->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		LunchApplicationAction* action = reinterpret_cast<LunchApplicationAction*>(data);
		textToShow += "\nSub type: Lunch Application";
		textToShow += "\nPath: " + action->applicationPath;
	}
	else if (data->getType() == FETP_SLEEP_ACTION)
	{
		SleepAction* action = reinterpret_cast<SleepAction*>(data);
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

void globalActionNode::checkIcons()
{
	if (sleepIcon == nullptr)
		sleepIcon = new FETPImage("Resources//sleepIcon.png");

	if (keyDownIcon == nullptr)
		keyDownIcon = new FETPImage("Resources//keyDownIcon.png");

	if (keyUpIcon == nullptr)
		keyUpIcon = new FETPImage("Resources//keyUpIcon.png");

	if (moveMouseIcon == nullptr)
		moveMouseIcon = new FETPImage("Resources//moveMouseIcon.png");

	if (leftMouseDownIcon == nullptr)
		leftMouseDownIcon = new FETPImage("Resources//leftMouseDownIcon.png");

	if (leftMouseUpIcon == nullptr)
		leftMouseUpIcon = new FETPImage("Resources//leftMouseUpIcon.png");

	if (rightMouseDownIcon == nullptr)
		rightMouseDownIcon = new FETPImage("Resources//rightMouseDownIcon.png");

	if (rightMouseUpIcon == nullptr)
		rightMouseUpIcon = new FETPImage("Resources//rightMouseUpIcon.png");

	if (scrollMouseIcon == nullptr)
		scrollMouseIcon = new FETPImage("Resources//scrollMouseIcon.png");

	if (lunchApplicationIcon == nullptr)
		lunchApplicationIcon = new FETPImage("Resources//lunchApplicationIcon.png");

	if (screenshotIcon == nullptr)
		screenshotIcon = new FETPImage("Resources//screenshotIcon.png");
}

void globalActionNode::renderIcon()
{
	if (data->getType() == FETP_KEYBOARD_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 7.0f, ImGui::GetCursorScreenPos().y + 7.0f));
		KeyboardAction* action = reinterpret_cast<KeyboardAction*>(data);
		if (action->wParam == WM_KEYDOWN || action->wParam == WM_SYSKEYDOWN)
			ImGui::Image((void*)(intptr_t)keyDownIcon->getTextureID(), ImVec2(102.0f, 102.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		if (action->wParam == WM_KEYUP || action->wParam == WM_SYSKEYUP)
			ImGui::Image((void*)(intptr_t)keyUpIcon->getTextureID(), ImVec2(102.0f, 102.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (data->getType() == FETP_MOUSE_ACTION)
	{
		MouseAction* action = reinterpret_cast<MouseAction*>(data);
		if (action->wParam == WM_MOUSEMOVE)
		{
			ImGui::Image((void*)(intptr_t)moveMouseIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_LBUTTONDOWN)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)leftMouseDownIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_LBUTTONUP)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)leftMouseUpIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_RBUTTONDOWN)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)rightMouseDownIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_RBUTTONUP)
		{
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 2.0f, ImGui::GetCursorScreenPos().y));
			ImGui::Image((void*)(intptr_t)rightMouseUpIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
		else if (action->wParam == WM_MOUSEWHEEL)
		{
			ImGui::Image((void*)(intptr_t)scrollMouseIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
		}
	}
	else if (data->getType() == FETP_SCREENSHOOT_COMPARE_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 14.0f, ImGui::GetCursorScreenPos().y + 14.0f));
		ImGui::Image((void*)(intptr_t)screenshotIcon->getTextureID(), ImVec2(86.0f, 86.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (data->getType() == FETP_LUNCH_APPLICATION_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f, ImGui::GetCursorScreenPos().y + 10.0f));
		ImGui::Image((void*)(intptr_t)lunchApplicationIcon->getTextureID(), ImVec2(100.0f, 100.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
	else if (data->getType() == FETP_SLEEP_ACTION)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 4.0f, ImGui::GetCursorScreenPos().y + 4.0f));
		ImGui::Image((void*)(intptr_t)sleepIcon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	}
}