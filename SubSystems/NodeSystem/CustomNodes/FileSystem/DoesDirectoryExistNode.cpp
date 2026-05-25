#include "DoesDirectoryExistNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool DoesDirectoryExistNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new DoesDirectoryExistNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const DoesDirectoryExistNode& NodeToCopy = static_cast<const DoesDirectoryExistNode&>(CurrentNode);
		return new DoesDirectoryExistNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("DoesDirectoryExistNode", Creator, Copier);
	return true;
}();

DoesDirectoryExistNode::DoesDirectoryExistNode() : BaseExecutionFlowNode()
{
	Type = "DoesDirectoryExistNode";

	SetStyle(DEFAULT);
	SetName("Does Directory Exist");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Exists", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

DoesDirectoryExistNode::DoesDirectoryExistNode(const DoesDirectoryExistNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value DoesDirectoryExistNode::ToJson()
{
	return Node::ToJson();
}

bool DoesDirectoryExistNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void DoesDirectoryExistNode::Draw()
{
	Node::Draw();
}

void DoesDirectoryExistNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = FocalEngine::FILE_SYSTEM.DoesDirectoryExist(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool DoesDirectoryExistNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}