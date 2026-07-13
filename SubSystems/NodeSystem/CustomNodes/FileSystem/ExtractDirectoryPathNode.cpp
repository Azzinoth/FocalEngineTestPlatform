#include "ExtractDirectoryPathNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool ExtractDirectoryPathNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ExtractDirectoryPathNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ExtractDirectoryPathNode& NodeToCopy = static_cast<const ExtractDirectoryPathNode&>(CurrentNode);
		return new ExtractDirectoryPathNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ExtractDirectoryPathNode", Creator, Copier);
	return true;
}();

ExtractDirectoryPathNode::ExtractDirectoryPathNode() : BaseExecutionFlowNode()
{
	Type = "ExtractDirectoryPathNode";

	SetStyle(DEFAULT);
	SetName("Extract Directory Path");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Full Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Directory", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 80));
	Output[1]->SetFunctionToOutputData(DirectoryDataGetter);
}

ExtractDirectoryPathNode::ExtractDirectoryPathNode(const ExtractDirectoryPathNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	ExtractedDirectory = Other.ExtractedDirectory;
	Output[1]->SetFunctionToOutputData(DirectoryDataGetter);
}

Json::Value ExtractDirectoryPathNode::ToJson()
{
	return Node::ToJson();
}

bool ExtractDirectoryPathNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DirectoryDataGetter);
	return true;
}

void ExtractDirectoryPathNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		ExtractedDirectory = FocalEngine::FILE_SYSTEM.GetDirectoryPath(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ExtractDirectoryPathNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}