#include "StringLengthNode.h"
using namespace VisNodeSys;

bool StringLengthNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringLengthNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringLengthNode& NodeToCopy = static_cast<const StringLengthNode&>(CurrentNode);
		return new StringLengthNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringLengthNode", Creator, Copier);
	return true;
}();

StringLengthNode::StringLengthNode() : BaseExecutionFlowNode()
{
	Type = "StringLengthNode";

	SetStyle(DEFAULT);
	SetName("String Length");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "INT", "Length", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringLengthNode::StringLengthNode(const StringLengthNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringLengthNode::ToJson()
{
	return Node::ToJson();
}

bool StringLengthNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringLengthNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 40.0f * Zoom));
	ImGui::Text("%d", Result);
}

void StringLengthNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = static_cast<int>(InputValue.size());

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringLengthNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}