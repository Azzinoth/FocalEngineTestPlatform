#include "GetAbsolutePathNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool GetAbsolutePathNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new GetAbsolutePathNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const GetAbsolutePathNode& NodeToCopy = static_cast<const GetAbsolutePathNode&>(CurrentNode);
		return new GetAbsolutePathNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("GetAbsolutePathNode", Creator, Copier);
	return true;
}();

GetAbsolutePathNode::GetAbsolutePathNode() : BaseExecutionFlowNode()
{
	Type = "GetAbsolutePathNode";

	SetStyle(DEFAULT);
	SetName("Get Absolute Path");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Absolute", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 80));
	Output[1]->SetFunctionToOutputData(AbsolutePathDataGetter);
}

GetAbsolutePathNode::GetAbsolutePathNode(const GetAbsolutePathNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	AbsolutePath = Other.AbsolutePath;
	Output[1]->SetFunctionToOutputData(AbsolutePathDataGetter);
}

Json::Value GetAbsolutePathNode::ToJson()
{
	return Node::ToJson();
}

bool GetAbsolutePathNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(AbsolutePathDataGetter);
	return true;
}

void GetAbsolutePathNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string PathInput = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				PathInput = *reinterpret_cast<std::string*>(TemporaryData);
		}

		AbsolutePath = FocalEngine::FILE_SYSTEM.GetAbsolutePath(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool GetAbsolutePathNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}