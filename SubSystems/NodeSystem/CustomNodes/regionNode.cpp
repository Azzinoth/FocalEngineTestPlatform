#include "regionNode.h"

VISUAL_NODE_CHILD_CPP(regionNode)
FETPImage* regionNode::regionIcon = nullptr;

regionNode::regionNode() : FEVisualNode()
{
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
	type = "regionNode";
	data = NODE_SYSTEM.createNodeArea();

	begin = new beginNode();
	begin->setPosition(ImVec2(300.0f, 350.0f));
	data->addNode(begin);

	end = new endNode();
	end->setPosition(begin->getPosition() + ImVec2(600.0f, 0.0f));
	data->addNode(end);

	addInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, ""));
	addOutputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, ""));
}

regionNode::regionNode(const regionNode& src) : FEVisualNode(src)
{
	data = src.data;
	begin = reinterpret_cast<beginNode*>(data->getNodesByType("beginNode")[0]);
	end = reinterpret_cast<endNode*>(data->getNodesByType("endNode")[0]);
	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);
}

void regionNode::draw()
{	
	FEVisualNode::draw();

	if (getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f, ImGui::GetCursorScreenPos().y + NODE_TITLE_HEIGHT + 13.0f));
		ImGui::SetNextItemWidth(140);

		ImGui::Text("REGION NODE");
	}
	else if (getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		checkIcons();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
		renderIcon();

		if (parentArea->isMouseHovered() && isHovered() && !actionEditPopup::getInstance().isOpened() && !textInputPopup::getInstance().isOpened())
			showTooltip();
	}
}

void regionNode::socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType)
{
	FEVisualNode::socketEvent(ownSocket,  connectedSocket, eventType);
}

FEVisualNodeArea* regionNode::getData()
{
	return data;
}

bool regionNode::canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser)
{
	if (!FEVisualNode::canConnect(ownSocket, candidateSocket, nullptr))
		return true;

	return true;
}

FEVisualNode* regionNode::getNextNode()
{
	end->nextNode = nullptr;
	if (output.size() > 0 && output[0]->getConnections().size() > 0)
		end->nextNode = output[0]->getConnections()[0]->getParent();
	
	return begin;
}

FEVisualNode* regionNode::getLogicallyNextNode()
{
	return getNextNode();
}

void regionNode::checkIcons()
{
	if (regionIcon == nullptr)
		regionIcon = new FETPImage("Resources//regionIcon.png");
}

void regionNode::renderIcon()
{
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 8.0f, ImGui::GetCursorScreenPos().y + 8.0f));
	ImGui::Image((void*)(intptr_t)regionIcon->getTextureID(), ImVec2(100.0f, 100.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void regionNode::showTooltip()
{
	std::string textToShow = "Region : " + getName();
	textToShow += "\nNode count : " + std::to_string(data->getNodeCount());

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::BeginTooltip();
	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	ImGui::TextUnformatted(textToShow.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndTooltip();
	ImGui::PopFont();
}

Json::Value regionNode::toJson()
{
	Json::Value result = FEVisualNode::toJson();
	result["nodeArea"] = data->toJson();

	return result;
}

void regionNode::fromJson(Json::Value json)
{
	FEVisualNode::fromJson(json);
	data = FEVisualNodeArea::fromJson(json["nodeArea"].asCString());
}