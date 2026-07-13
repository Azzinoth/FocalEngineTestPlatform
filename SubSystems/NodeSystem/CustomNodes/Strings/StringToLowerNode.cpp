#include "StringToLowerNode.h"
using namespace VisNodeSys;

bool StringToLowerNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringToLowerNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringToLowerNode& NodeToCopy = static_cast<const StringToLowerNode&>(CurrentNode);
		return new StringToLowerNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringToLowerNode", Creator, Copier);
	return true;
}();

StringToLowerNode::StringToLowerNode() : BaseExecutionFlowNode()
{
	Type = "StringToLowerNode";

	SetStyle(DEFAULT);
	SetName("String To Lower");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 80));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringToLowerNode::StringToLowerNode(const StringToLowerNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringToLowerNode::ToJson()
{
	return Node::ToJson();
}

bool StringToLowerNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringToLowerNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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
			Result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(Result[i])));

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringToLowerNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}