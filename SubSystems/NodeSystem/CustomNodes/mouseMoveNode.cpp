#include "mouseMoveNode.h"

bool mouseMoveNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("mouseMoveNode",
		[]() -> VisualNode* {
			return new mouseMoveNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const mouseMoveNode& NodeToCopy = static_cast<const mouseMoveNode&>(Node);
			return new mouseMoveNode(NodeToCopy);
		}
	);

	return true;
}();

mouseMoveNode::mouseMoveNode() : basicLogicNode()
{
	Type = "mouseMoveNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(250, 78));
	SetName("Mouse move node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "In", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
}

mouseMoveNode::mouseMoveNode(const mouseMoveNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
}

void mouseMoveNode::Draw()
{	
	VisualNode::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 50.0f, ImGui::GetCursorScreenPos().y + 45.0f));

	ImGui::SetNextItemWidth(140);
	static int position[] = { 0 };
	position[0] = Data.x;
	position[1] = Data.y;

	if (ImGui::InputInt2("##Position", position))
	{
		Data = glm::vec2(position[0], position[1]);
		//ParentArea->PropagateUpdateToConnectedNodes(this);
	}
}

void mouseMoveNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		INPUT_SYSTEM.mouseMoveTo(Data.x, Data.y);
	}
}

bool mouseMoveNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* mouseMoveNode::GetNextNode()
{
	return nullptr;
}