#include "StringReplaceNode.h"
using namespace VisNodeSys;

bool StringReplaceNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringReplaceNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringReplaceNode& NodeToCopy = static_cast<const StringReplaceNode&>(CurrentNode);
		return new StringReplaceNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringReplaceNode", Creator, Copier);
	return true;
}();

StringReplaceNode::StringReplaceNode() : BaseExecutionFlowNode()
{
	Type = "StringReplaceNode";

	SetStyle(DEFAULT);
	SetName("String Replace");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "Find", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "Replace With", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(260, 140));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringReplaceNode::StringReplaceNode(const StringReplaceNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringReplaceNode::ToJson()
{
	return Node::ToJson();
}

bool StringReplaceNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringReplaceNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		std::string Find = "";
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Find = *reinterpret_cast<std::string*>(TemporaryData);
		}

		std::string ReplaceWith = "";
		if (Input[3]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[3]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				ReplaceWith = *reinterpret_cast<std::string*>(TemporaryData);
		}

		if (Find.empty())
		{
			Result = Value;
		}
		else
		{
			Result = Value;
			size_t Position = 0;
			while ((Position = Result.find(Find, Position)) != std::string::npos)
			{
				Result.replace(Position, Find.size(), ReplaceWith);
				Position += ReplaceWith.size();
			}
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringReplaceNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}