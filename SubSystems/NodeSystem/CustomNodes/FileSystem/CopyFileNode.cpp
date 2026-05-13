#include "CopyFileNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool CopyFileNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new CopyFileNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const CopyFileNode& NodeToCopy = static_cast<const CopyFileNode&>(CurrentNode);
		return new CopyFileNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("CopyFileNode", Creator, Copier);
	return true;
}();

CopyFileNode::CopyFileNode() : BaseExecutionFlowNode()
{
	Type = "CopyFileNode";

	SetStyle(DEFAULT);
	SetName("Copy File");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Source", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "Destination", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

CopyFileNode::CopyFileNode(const CopyFileNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value CopyFileNode::ToJson()
{
	return Node::ToJson();
}

bool CopyFileNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void CopyFileNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string SourceInput = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				SourceInput = *reinterpret_cast<std::string*>(TemporaryData);
		}

		std::string DestinationInput = "";
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				DestinationInput = *reinterpret_cast<std::string*>(TemporaryData);
		}

		Result = FocalEngine::FILE_SYSTEM.CopyFile(SourceInput, DestinationInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool CopyFileNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}