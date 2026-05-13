#include "StringEqualsNode.h"
using namespace VisNodeSys;

bool StringEqualsNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringEqualsNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringEqualsNode& NodeToCopy = static_cast<const StringEqualsNode&>(CurrentNode);
		return new StringEqualsNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringEqualsNode", Creator, Copier);
	return true;
}();

StringEqualsNode::StringEqualsNode() : BaseExecutionFlowNode()
{
	Type = "StringEqualsNode";

	SetStyle(DEFAULT);
	SetName("String Equals");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "A", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "B", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Equals", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 120));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringEqualsNode::StringEqualsNode(const StringEqualsNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringEqualsNode::ToJson()
{
	return Node::ToJson();
}

bool StringEqualsNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringEqualsNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 50.0f * Zoom));
	ImGui::Text("%s", Result ? "true" : "false");
}

void StringEqualsNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string A = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				A = *reinterpret_cast<std::string*>(TemporaryData);
		}

		std::string B = "";
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				B = *reinterpret_cast<std::string*>(TemporaryData);
		}

		Result = (A == B);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringEqualsNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}