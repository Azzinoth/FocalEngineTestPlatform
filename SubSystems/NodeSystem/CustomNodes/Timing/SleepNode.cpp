#include "SleepNode.h"
using namespace VisNodeSys;

bool SleepNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new SleepNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const SleepNode& NodeToCopy = static_cast<const SleepNode&>(CurrentNode);
		return new SleepNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("SleepNode", Creator, Copier);
	return true;
}();

SleepNode::SleepNode() : BaseExecutionFlowNode()
{
	Type = "SleepNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(220, 90));
	SetName("Sleep");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "INT", "Duration", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

SleepNode::SleepNode(const SleepNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	SleepDurationMs = Other.SleepDurationMs;
}

Json::Value SleepNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["sleepNode_Data"] = SleepDurationMs;
	return Result;
}

bool SleepNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("sleepNode_Data"))
		return false;

	SleepDurationMs = Json["sleepNode_Data"].asInt();

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
	ImGui::BeginDisabled(Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0);
	ImGui::InputInt(("##SleepNodeDuration" + GetID()).c_str(), &SleepDurationMs);
	ImGui::EndDisabled();
}

void SleepNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				SleepDurationMs = *reinterpret_cast<int*>(TemporaryData);
		}

		Sleep(DWORD(SleepDurationMs /** currentlyRunning->getSpeedFactor()*/));

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

int SleepNode::GetSleepDuration() const
{
	return SleepDurationMs;
}

void SleepNode::SetSleepDuration(int NewValue)
{
	SleepDurationMs = NewValue;
}