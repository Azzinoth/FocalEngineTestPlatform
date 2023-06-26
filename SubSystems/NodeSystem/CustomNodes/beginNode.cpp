#include "beginNode.h"

VISUAL_NODE_CHILD_CPP(beginNode)

beginNode::beginNode() : VisualNode()
{
	Type = "beginNode";
	bCouldBeDestroyed = false;

	SetStyle(VISUAL_NODE_STYLE_CIRCLE);

	SetSize(ImVec2(220, 78));
	SetName("beginNode");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
	
	AddSocket(new NodeSocket(this, "FLOAT", "out", true));

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

beginNode::beginNode(const beginNode& Src) : VisualNode(Src)
{
	Data = Src.Data;
	bCouldBeDestroyed = false;

	SetStyle(VISUAL_NODE_STYLE_CIRCLE);

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

void beginNode::Draw()
{	
	VisualNode::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
	ImGui::Image((void*)(intptr_t)Icon->getTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void beginNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float beginNode::GetData()
{
	return Data;
}

bool beginNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return false;
}

VisualNode* beginNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnections().size() > 0)
		return Output[0]->GetConnections()[0]->GetParent();
	
	return nullptr;
}