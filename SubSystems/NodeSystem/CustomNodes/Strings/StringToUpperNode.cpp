#include "StringToUpperNode.h"
using namespace VisNodeSys;

bool StringToUpperNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringToUpperNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringToUpperNode& NodeToCopy = static_cast<const StringToUpperNode&>(CurrentNode);
		return new StringToUpperNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringToUpperNode", Creator, Copier);
	return true;
}();

StringToUpperNode::StringToUpperNode() : BaseExecutionFlowNode()
{
	Type = "StringToUpperNode";

	SetStyle(DEFAULT);
	SetName("String To Upper");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 80));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringToUpperNode::StringToUpperNode(const StringToUpperNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringToUpperNode::ToJson()
{
	return Node::ToJson();
}

bool StringToUpperNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringToUpperNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = Value;
		for (size_t i = 0; i < Result.size(); i++)
			Result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(Result[i])));

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringToUpperNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}