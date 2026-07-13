#include "GetCurrentWorkingPathNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool GetCurrentWorkingPathNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new GetCurrentWorkingPathNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const GetCurrentWorkingPathNode& NodeToCopy = static_cast<const GetCurrentWorkingPathNode&>(CurrentNode);
		return new GetCurrentWorkingPathNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("GetCurrentWorkingPathNode", Creator, Copier);
	return true;
}();

GetCurrentWorkingPathNode::GetCurrentWorkingPathNode() : BaseExecutionFlowNode()
{
	Type = "GetCurrentWorkingPathNode";

	SetStyle(DEFAULT);
	SetName("Get Current Working Path");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(280, 80));
	Output[1]->SetFunctionToOutputData(PathDataGetter);
}

GetCurrentWorkingPathNode::GetCurrentWorkingPathNode(const GetCurrentWorkingPathNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	WorkingPath = Other.WorkingPath;
	Output[1]->SetFunctionToOutputData(PathDataGetter);
}

Json::Value GetCurrentWorkingPathNode::ToJson()
{
	return Node::ToJson();
}

bool GetCurrentWorkingPathNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;
	if (Output.size() < 2 || Output[1] == nullptr)
		return false;
	Output[1]->SetFunctionToOutputData(PathDataGetter);
	return true;
}

void GetCurrentWorkingPathNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		WorkingPath = FocalEngine::FILE_SYSTEM.GetCurrentWorkingPath();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool GetCurrentWorkingPathNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;
	return true;
}