#include "RenameDirectoryNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool RenameDirectoryNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new RenameDirectoryNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const RenameDirectoryNode& NodeToCopy = static_cast<const RenameDirectoryNode&>(CurrentNode);
		return new RenameDirectoryNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("RenameDirectoryNode", Creator, Copier);
	return true;
}();

RenameDirectoryNode::RenameDirectoryNode() : BaseExecutionFlowNode()
{
	Type = "RenameDirectoryNode";

	SetStyle(DEFAULT);
	SetName("Rename Directory");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Old Path", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "New Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

RenameDirectoryNode::RenameDirectoryNode(const RenameDirectoryNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value RenameDirectoryNode::ToJson()
{
	return Node::ToJson();
}

bool RenameDirectoryNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void RenameDirectoryNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string OldPathInput = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				OldPathInput = *reinterpret_cast<std::string*>(TemporaryData);
		}

		std::string NewPathInput = "";
		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				NewPathInput = *reinterpret_cast<std::string*>(TemporaryData);
		}

		Result = FocalEngine::FILE_SYSTEM.RenameDirectory(OldPathInput, NewPathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool RenameDirectoryNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}