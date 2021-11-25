#include "beginNode.h"

EDITOR_NODE_CHILD_CPP(beginNode)

beginNode::beginNode() : FEEditorNode()
{
	type = "beginNode";
	couldBeDestroyed = false;

	setStyle(FE_EDITOR_NODE_VISUAL_STYLE_CIRCLE);

	setSize(ImVec2(220, 78));
	setName("beginNode");

	titleBackgroundColor = ImColor(31, 117, 208);
	titleBackgroundColorHovered = ImColor(35, 145, 255);
	
	addOutputSocket(new FEEditorNodeSocket(this, FE_NODE_SOCKET_FLOAT_CHANNEL_OUT, "out"));

	if (icon == nullptr)
		icon = new FETPImage("Resources//beginNodeIcon.png");
}

beginNode::beginNode(const beginNode& src) : FEEditorNode(src)
{
	data = src.data;
	couldBeDestroyed = false;

	setStyle(FE_EDITOR_NODE_VISUAL_STYLE_CIRCLE);

	if (icon == nullptr)
		icon = new FETPImage("Resources//beginNodeIcon.png");
}

void beginNode::draw()
{	
	FEEditorNode::draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
	ImGui::Image((void*)(intptr_t)icon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void beginNode::socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType)
{
	FEEditorNode::socketEvent(ownSocket,  connectedSocket, eventType);
}

float beginNode::getData()
{
	return data;
}

bool beginNode::canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser)
{
	if (!FEEditorNode::canConnect(ownSocket, candidateSocket, nullptr))
		return false;

	return false;
}

FEEditorNode* beginNode::getNextNode()
{
	if (output.size() > 0 && output[0]->getConnections().size() > 0)
		return output[0]->getConnections()[0]->getParent();
	
	return nullptr;
}