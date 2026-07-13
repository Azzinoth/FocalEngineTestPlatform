#include "DoesFileExistNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool DoesFileExistNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new DoesFileExistNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const DoesFileExistNode& NodeToCopy = static_cast<const DoesFileExistNode&>(CurrentNode);
		return new DoesFileExistNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("DoesFileExistNode", Creator, Copier);
	return true;
}();

DoesFileExistNode::DoesFileExistNode() : BaseExecutionFlowNode()
{
	Type = "DoesFileExistNode";

	SetStyle(DEFAULT);
	SetName("Does File Exist");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Exists", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

DoesFileExistNode::DoesFileExistNode(const DoesFileExistNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value DoesFileExistNode::ToJson()
{
	return Node::ToJson();
}

bool DoesFileExistNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void DoesFileExistNode::Draw()
{
	Node::Draw();
}

void DoesFileExistNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = FocalEngine::FILE_SYSTEM.DoesFileExist(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool DoesFileExistNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}