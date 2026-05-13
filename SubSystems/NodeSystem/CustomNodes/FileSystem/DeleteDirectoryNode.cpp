#include "DeleteDirectoryNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool DeleteDirectoryNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new DeleteDirectoryNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const DeleteDirectoryNode& NodeToCopy = static_cast<const DeleteDirectoryNode&>(CurrentNode);
		return new DeleteDirectoryNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("DeleteDirectoryNode", Creator, Copier);
	return true;
}();

DeleteDirectoryNode::DeleteDirectoryNode() : BaseExecutionFlowNode()
{
	Type = "DeleteDirectoryNode";

	SetStyle(DEFAULT);
	SetName("Delete Directory");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

DeleteDirectoryNode::DeleteDirectoryNode(const DeleteDirectoryNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value DeleteDirectoryNode::ToJson()
{
	return Node::ToJson();
}

bool DeleteDirectoryNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void DeleteDirectoryNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = FocalEngine::FILE_SYSTEM.DeleteDirectory(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool DeleteDirectoryNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}