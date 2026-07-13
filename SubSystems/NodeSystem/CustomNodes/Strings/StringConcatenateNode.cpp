#include "StringConcatenateNode.h"
using namespace VisNodeSys;

bool StringConcatenateNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringConcatenateNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringConcatenateNode& NodeToCopy = static_cast<const StringConcatenateNode&>(CurrentNode);
		return new StringConcatenateNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringConcatenateNode", Creator, Copier);
	return true;
}();

StringConcatenateNode::StringConcatenateNode() : BaseExecutionFlowNode()
{
	Type = "StringConcatenateNode";

	SetStyle(DEFAULT);
	SetName("String Concatenate");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "A", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "B", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 110));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringConcatenateNode::StringConcatenateNode(const StringConcatenateNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringConcatenateNode::ToJson()
{
	return Node::ToJson();
}

bool StringConcatenateNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringConcatenateNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = A + B;

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringConcatenateNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}