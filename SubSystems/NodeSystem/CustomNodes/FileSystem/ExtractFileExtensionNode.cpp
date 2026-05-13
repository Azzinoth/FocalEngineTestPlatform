#include "ExtractFileExtensionNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool ExtractFileExtensionNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new ExtractFileExtensionNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const ExtractFileExtensionNode& NodeToCopy = static_cast<const ExtractFileExtensionNode&>(CurrentNode);
		return new ExtractFileExtensionNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("ExtractFileExtensionNode", Creator, Copier);
	return true;
}();

ExtractFileExtensionNode::ExtractFileExtensionNode() : BaseExecutionFlowNode()
{
	Type = "ExtractFileExtensionNode";

	SetStyle(DEFAULT);
	SetName("Extract File Extension");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Extension", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(240, 80));
	Output[1]->SetFunctionToOutputData(ExtensionDataGetter);
}

ExtractFileExtensionNode::ExtractFileExtensionNode(const ExtractFileExtensionNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	ExtractedExtension = Other.ExtractedExtension;
	Output[1]->SetFunctionToOutputData(ExtensionDataGetter);
}

Json::Value ExtractFileExtensionNode::ToJson()
{
	return Node::ToJson();
}

bool ExtractFileExtensionNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ExtensionDataGetter);
	return true;
}

void ExtractFileExtensionNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		ExtractedExtension = FocalEngine::FILE_SYSTEM.GetFileExtension(PathInput);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool ExtractFileExtensionNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}