#include "endNode.h"

VISUAL_NODE_CHILD_CPP(endNode)

endNode::endNode() : FEVisualNode()
{
	type = "endNode";
	couldBeDestroyed = false;

	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);

	setSize(ImVec2(220, 78));
	setName("endNode");

	titleBackgroundColor = ImColor(31, 117, 208);
	titleBackgroundColorHovered = ImColor(35, 145, 255);
	
	addInputSocket(new FEVisualNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, "in"));

	if (icon == nullptr)
		icon = new FETPImage("Resources//beginNodeIcon.png");
}

endNode::endNode(const endNode& src) : FEVisualNode(src)
{
	data = src.data;
	couldBeDestroyed = false;

	setStyle(FE_VISUAL_NODE_STYLE_CIRCLE);

	if (icon == nullptr)
		icon = new FETPImage("Resources//beginNodeIcon.png");
}

void endNode::draw()
{	
	FEVisualNode::draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
	ImGui::Image((void*)(intptr_t)icon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void endNode::socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType)
{
	FEVisualNode::socketEvent(ownSocket,  connectedSocket, eventType);
}

float endNode::getData()
{
	return data;
}

bool endNode::canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser)
{
	if (!FEVisualNode::canConnect(ownSocket, candidateSocket, nullptr))
		return false;

	return true;
}

FEVisualNode* endNode::getNextNode()
{
	return getLogicallyNextNode();
}

FEVisualNode* endNode::getLogicallyNextNode()
{
	return nextNode;
}