#include "IsStringEmptyNode.h"
using namespace VisNodeSys;

bool IsStringEmptyNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new IsStringEmptyNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const IsStringEmptyNode& NodeToCopy = static_cast<const IsStringEmptyNode&>(CurrentNode);
		return new IsStringEmptyNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("IsStringEmptyNode", Creator, Copier);
	return true;
}();

IsStringEmptyNode::IsStringEmptyNode() : BaseExecutionFlowNode()
{
	Type = "IsStringEmptyNode";

	SetStyle(DEFAULT);
	SetName("Is String Empty");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Is Empty", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

IsStringEmptyNode::IsStringEmptyNode(const IsStringEmptyNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value IsStringEmptyNode::ToJson()
{
	return Node::ToJson();
}

bool IsStringEmptyNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void IsStringEmptyNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 40.0f * Zoom));
	ImGui::Text("%s", Result ? "true" : "false");
}

void IsStringEmptyNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string InputValue = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				InputValue = *reinterpret_cast<std::string*>(TemporaryData);
		}

		Result = InputValue.empty();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool IsStringEmptyNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}