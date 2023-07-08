#include "sleepNode.h"

bool sleepNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("sleepNode",
		[]() -> VisualNode* {
			return new sleepNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const sleepNode& NodeToCopy = static_cast<const sleepNode&>(Node);
			return new sleepNode(NodeToCopy);
		}
	);

	return true;
}();

sleepNode::sleepNode() : basicLogicNode()
{
	Type = "sleepNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(220, 78));
	SetName("sleep node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "In", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "Out", true));
}

sleepNode::sleepNode(const sleepNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
}

void sleepNode::Draw()
{	
	VisualNode::Draw();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f, ImGui::GetCursorScreenPos().y + 45.0f));

	int xPosition = ImGui::GetCursorScreenPos().x + 20.0f;
	int yPosition = ImGui::GetCursorScreenPos().y + 0.0f;

	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("##value", &SleepFor);
}

void sleepNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		Sleep(DWORD(SleepFor /** currentlyRunning->getSpeedFactor()*/));

		if (Output[0]->GetConnections().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
	}
}

bool sleepNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* sleepNode::GetNextNode()
{
	return nullptr;
}