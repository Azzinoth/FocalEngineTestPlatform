#include "ExtractFileNameNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool ExtractFileNameNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ExtractFileNameNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ExtractFileNameNode& NodeToCopy = static_cast<const ExtractFileNameNode&>(CurrentNode);
		return new ExtractFileNameNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ExtractFileNameNode", Creator, Copier);
	return true;
}();

ExtractFileNameNode::ExtractFileNameNode() : BaseExecutionFlowNode()
{
	Type = "ExtractFileNameNode";

	SetStyle(DEFAULT);
	SetName("Extract File Name");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Name", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 80));
	Output[1]->SetFunctionToOutputData(NameDataGetter);
}

ExtractFileNameNode::ExtractFileNameNode(const ExtractFileNameNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	ExtractedName = Other.ExtractedName;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(NameDataGetter);
}

Json::Value ExtractFileNameNode::ToJson()
{
	return Node::ToJson();
}

bool ExtractFileNameNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(NameDataGetter);
	return true;
}

void ExtractFileNameNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		ExtractedName = FocalEngine::FILE_SYSTEM.GetFileName(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ExtractFileNameNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}