#include "SleepNode.h"
using namespace VisNodeSys;

bool SleepNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("SleepNode",
		[]() -> Node* {
			return new SleepNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const SleepNode& NodeToCopy = static_cast<const SleepNode&>(CurrentNode);
			return new SleepNode(NodeToCopy);
		}
	);

	return true;
}();

SleepNode::SleepNode() : BaseExecutionFlowNode()
{
	Type = "SleepNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(220, 78));
	SetName("Sleep");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

SleepNode::SleepNode(const SleepNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	SleepFor = Other.SleepFor;
}

Json::Value SleepNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["sleepNode_Data"] = SleepFor;
	return Result;
}

bool SleepNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("sleepNode_Data"))
		return false;
	
	SleepFor = Json["sleepNode_Data"].asInt();

	return true;
}

void SleepNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	float XPosition = ImGui::GetCursorScreenPos().x + 20.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	ImGui::InputInt("##value", &SleepFor);
}

void SleepNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		Sleep(DWORD(SleepFor /** currentlyRunning->getSpeedFactor()*/));

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool SleepNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}