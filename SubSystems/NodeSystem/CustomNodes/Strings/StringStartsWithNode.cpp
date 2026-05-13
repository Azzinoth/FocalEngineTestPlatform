#include "StringStartsWithNode.h"
using namespace VisNodeSys;

bool StringStartsWithNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringStartsWithNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringStartsWithNode& NodeToCopy = static_cast<const StringStartsWithNode&>(CurrentNode);
		return new StringStartsWithNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringStartsWithNode", Creator, Copier);
	return true;
}();

StringStartsWithNode::StringStartsWithNode() : BaseExecutionFlowNode()
{
	Type = "StringStartsWithNode";

	SetStyle(DEFAULT);
	SetName("String Starts With");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "Prefix", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Starts With", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 120));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringStartsWithNode::StringStartsWithNode(const StringStartsWithNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringStartsWithNode::ToJson()
{
	return Node::ToJson();
}

bool StringStartsWithNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringStartsWithNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 50.0f * Zoom));
	ImGui::Text("%s", Result ? "true" : "false");
}

void StringStartsWithNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string Value = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Value = *reinterpret_cast<std::string*>(TemporaryData);
		}

		std::string Prefix = "";
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Prefix = *reinterpret_cast<std::string*>(TemporaryData);
		}

		Result = !Prefix.empty() && Value.size() >= Prefix.size() && Value.compare(0, Prefix.size(), Prefix) == 0;

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringStartsWithNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}