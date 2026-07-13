#include "StringEndsWithNode.h"
using namespace VisNodeSys;

bool StringEndsWithNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringEndsWithNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringEndsWithNode& NodeToCopy = static_cast<const StringEndsWithNode&>(CurrentNode);
		return new StringEndsWithNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringEndsWithNode", Creator, Copier);
	return true;
}();

StringEndsWithNode::StringEndsWithNode() : BaseExecutionFlowNode()
{
	Type = "StringEndsWithNode";

	SetStyle(DEFAULT);
	SetName("String Ends With");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "Suffix", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Ends With", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 120));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringEndsWithNode::StringEndsWithNode(const StringEndsWithNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringEndsWithNode::ToJson()
{
	return Node::ToJson();
}

bool StringEndsWithNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringEndsWithNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 50.0f * Zoom));
	ImGui::Text("%s", Result ? "true" : "false");
}

void StringEndsWithNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		std::string Suffix = "";
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Suffix = *reinterpret_cast<std::string*>(TemporaryData);
		}

		Result = !Suffix.empty() && Value.size() >= Suffix.size() && Value.compare(Value.size() - Suffix.size(), Suffix.size(), Suffix) == 0;

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringEndsWithNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}