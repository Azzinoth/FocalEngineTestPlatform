#include "CreateDirectoryNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool CreateDirectoryNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new CreateDirectoryNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const CreateDirectoryNode& NodeToCopy = static_cast<const CreateDirectoryNode&>(CurrentNode);
		return new CreateDirectoryNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("CreateDirectoryNode", Creator, Copier);
	return true;
}();

CreateDirectoryNode::CreateDirectoryNode() : BaseExecutionFlowNode()
{
	Type = "CreateDirectoryNode";

	SetStyle(DEFAULT);
	SetName("Create Directory");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

CreateDirectoryNode::CreateDirectoryNode(const CreateDirectoryNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value CreateDirectoryNode::ToJson()
{
	return Node::ToJson();
}

bool CreateDirectoryNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void CreateDirectoryNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = FocalEngine::FILE_SYSTEM.CreateDirectory(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool CreateDirectoryNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}