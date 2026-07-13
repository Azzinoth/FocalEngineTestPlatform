#include "RenameFileNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool RenameFileNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new RenameFileNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const RenameFileNode& NodeToCopy = static_cast<const RenameFileNode&>(CurrentNode);
		return new RenameFileNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("RenameFileNode", Creator, Copier);
	return true;
}();

RenameFileNode::RenameFileNode() : BaseExecutionFlowNode()
{
	Type = "RenameFileNode";

	SetStyle(DEFAULT);
	SetName("Rename File");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Old Path", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "STRING", "New Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Result", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

RenameFileNode::RenameFileNode(const RenameFileNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value RenameFileNode::ToJson()
{
	return Node::ToJson();
}

bool RenameFileNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void RenameFileNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		Result = FocalEngine::FILE_SYSTEM.RenameFile(OldPathInput, NewPathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool RenameFileNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}