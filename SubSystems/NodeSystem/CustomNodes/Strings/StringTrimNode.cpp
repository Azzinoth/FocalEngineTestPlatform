#include "StringTrimNode.h"
using namespace VisNodeSys;

bool StringTrimNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringTrimNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringTrimNode& NodeToCopy = static_cast<const StringTrimNode&>(CurrentNode);
		return new StringTrimNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringTrimNode", Creator, Copier);
	return true;
}();

StringTrimNode::StringTrimNode() : BaseExecutionFlowNode()
{
	Type = "StringTrimNode";

	SetStyle(DEFAULT);
	SetName("String Trim");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 80));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

StringTrimNode::StringTrimNode(const StringTrimNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value StringTrimNode::ToJson()
{
	return Node::ToJson();
}

bool StringTrimNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void StringTrimNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		auto IsWhitespace = [](unsigned char Character) -> bool {
			return Character == ' ' || Character == '\t' || Character == '\n' || Character == '\r' || Character == '\f' || Character == '\v';
		};

		size_t Start = 0;
		while (Start < Value.size() && IsWhitespace(static_cast<unsigned char>(Value[Start])))
			Start++;

		size_t End = Value.size();
		while (End > Start && IsWhitespace(static_cast<unsigned char>(Value[End - 1])))
			End--;

		Result = Value.substr(Start, End - Start);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringTrimNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}