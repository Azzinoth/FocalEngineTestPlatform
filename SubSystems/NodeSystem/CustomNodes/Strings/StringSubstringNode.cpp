#include "StringSubstringNode.h"
using namespace VisNodeSys;

bool StringSubstringNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringSubstringNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringSubstringNode& NodeToCopy = static_cast<const StringSubstringNode&>(CurrentNode);
		return new StringSubstringNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringSubstringNode", Creator, Copier);
	return true;
}();

StringSubstringNode::StringSubstringNode() : BaseExecutionFlowNode()
{
	Type = "StringSubstringNode";

	SetStyle(DEFAULT);
	SetName("String Substring");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "INT", "Start", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "INT", "Length", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 140));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringSubstringNode::StringSubstringNode(const StringSubstringNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringSubstringNode::ToJson()
{
	return Node::ToJson();
}

bool StringSubstringNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringSubstringNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		int Start = 0;
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Start = *reinterpret_cast<int*>(TemporaryData);
		}

		int Length = 0;
		if (Input[3]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[3]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Length = *reinterpret_cast<int*>(TemporaryData);
		}

		if (Start < 0 || Length <= 0 || static_cast<size_t>(Start) >= Value.size())
		{
			Result = "";
		}
		else
		{
			size_t SafeLength = std::min(static_cast<size_t>(Length), Value.size() - static_cast<size_t>(Start));
			Result = Value.substr(static_cast<size_t>(Start), SafeLength);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringSubstringNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}