#include "sleepNode.h"
using namespace VisNodeSys;

bool sleepNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("sleepNode",
		[]() -> Node* {
			return new sleepNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const sleepNode& NodeToCopy = static_cast<const sleepNode&>(CurrentNode);
			return new sleepNode(NodeToCopy);
		}
	);

	return true;
}();

sleepNode::sleepNode() : basicLogicNode()
{
	Type = "sleepNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(220, 78));
	SetName("sleep node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

sleepNode::sleepNode(const sleepNode& Src) : basicLogicNode(Src)
{
	SetStyle(DEFAULT);
	SleepFor = Src.SleepFor;
}

Json::Value sleepNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["sleepNode_Data"] = SleepFor;
	return Result;
}

void sleepNode::FromJson(Json::Value Json)
{
	Node::FromJson(Json);
	SleepFor = Json["sleepNode_Data"].asInt();
}

void sleepNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	int xPosition = ImGui::GetCursorScreenPos().x + 20.0f * Zoom;
	int yPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(xPosition, yPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	ImGui::InputInt("##value", &SleepFor);
}

void sleepNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		Sleep(DWORD(SleepFor /** currentlyRunning->getSpeedFactor()*/));

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool sleepNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* sleepNode::GetNextNode()
{
	return nullptr;
}