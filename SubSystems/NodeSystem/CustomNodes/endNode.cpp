#include "endNode.h"

EDITOR_NODE_CHILD_CPP(endNode)

endNode::endNode() : FEEditorNode()
{
	type = "endNode";
	couldBeDestroyed = false;

	setStyle(FE_EDITOR_NODE_VISUAL_STYLE_CIRCLE);

	setSize(ImVec2(220, 78));
	setName("endNode");

	titleBackgroundColor = ImColor(31, 117, 208);
	titleBackgroundColorHovered = ImColor(35, 145, 255);
	
	addInputSocket(new FEEditorNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_IN, "in"));

	if (icon == nullptr)
		icon = new FETPImage("Resources//beginNodeIcon.png");
}

endNode::endNode(const endNode& src) : FEEditorNode(src)
{
	data = src.data;
	couldBeDestroyed = false;

	setStyle(FE_EDITOR_NODE_VISUAL_STYLE_CIRCLE);

	if (icon == nullptr)
		icon = new FETPImage("Resources//beginNodeIcon.png");
}

void endNode::draw()
{	
	FEEditorNode::draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
	ImGui::Image((void*)(intptr_t)icon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void endNode::socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType)
{
	FEEditorNode::socketEvent(ownSocket,  connectedSocket, eventType);
}

float endNode::getData()
{
	return data;
}

bool endNode::canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser)
{
	if (!FEEditorNode::canConnect(ownSocket, candidateSocket, nullptr))
		return false;

	return true;
}

FEEditorNode* endNode::getNextNode()
{
	return getLogicallyNextNode();
}

FEEditorNode* endNode::getLogicallyNextNode()
{
	return nextNode;
}