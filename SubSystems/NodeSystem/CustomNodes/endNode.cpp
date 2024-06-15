#include "endNode.h"
using namespace VisNodeSys;

bool endNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("endNode",
		[]() -> Node* {
			return new endNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const endNode& NodeToCopy = static_cast<const endNode&>(CurrentNode);
			return new endNode(NodeToCopy);
		}
	);

	return true;
}();

endNode::endNode() : basicLogicNode()
{
	Type = "endNode";
	bCouldBeDestroyed = false;

	SetStyle(CIRCLE);

	SetSize(ImVec2(220, 78));
	SetName("endNode");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
	
	AddSocket(new NodeSocket(this, "FLOAT", "in", false));

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

endNode::endNode(const endNode& Src) : basicLogicNode(Src)
{
	Data = Src.Data;
	bCouldBeDestroyed = false;

	SetStyle(CIRCLE);

	if (Icon == nullptr)
		Icon = new FETPImage("Resources//beginNodeIcon.png");
}

void endNode::Draw()
{	
	Node::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 4.0f, ImGui::GetCursorScreenPos().y - 4.0f));
	ImGui::Image((void*)(intptr_t)Icon->GetTextureID(), ImVec2(116.0f, 116.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
}

void endNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float endNode::GetData()
{
	return Data;
}

bool endNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* endNode::GetNextNode()
{
	return NextNode;
}