#include "EndNode.h"
using namespace VisNodeSys;

bool EndNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("EndNode",
		[]() -> Node* {
			return new EndNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const EndNode& NodeToCopy = static_cast<const EndNode&>(CurrentNode);
			return new EndNode(NodeToCopy);
		}
	);

	return true;
}();

EndNode::EndNode() : BasicLogicNode()
{
	Type = "EndNode";
	bCouldBeDestroyed = false;

	SetStyle(CIRCLE);

	SetSize(ImVec2(220, 78));
	SetName("EndNode");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
	
	AddSocket(new NodeSocket(this, "FLOAT", "in", false));

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

EndNode::EndNode(const EndNode& Other) : BasicLogicNode(Other)
{
	Data = Other.Data;
	bCouldBeDestroyed = false;

	SetStyle(CIRCLE);

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

void EndNode::Draw()
{	
	Node::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
	ImGui::Image((void*)(intptr_t)Icon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void EndNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float EndNode::GetData()
{
	return Data;
}

bool EndNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* EndNode::GetNextNode()
{
	return NextNode;
}