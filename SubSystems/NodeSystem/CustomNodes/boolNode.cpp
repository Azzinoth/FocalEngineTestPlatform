#include "boolNode.h"

bool boolNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("boolNode",
		[]() -> VisualNode* {
			return new boolNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const boolNode& NodeToCopy = static_cast<const boolNode&>(Node);
			return new boolNode(NodeToCopy);
		}
	);

	return true;
}();

boolNode::boolNode() : basicLogicNode()
{
	Type = "boolNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(150, 78));
	SetName("bool node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new BoolSocket(this, "BOOL", "Out", true));
}

boolNode::boolNode(const boolNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
}

void boolNode::Draw()
{	
	VisualNode::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f, ImGui::GetCursorScreenPos().y + 45.0f));
	ImGui::Checkbox("True", &Data);
}

void boolNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool boolNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* boolNode::GetNextNode()
{
	return nullptr;
}