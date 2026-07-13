#include "GetFileSizeNode.h"
#include "../../../FEFileSystem.h"
using namespace VisNodeSys;

bool GetFileSizeNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new GetFileSizeNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const GetFileSizeNode& NodeToCopy = static_cast<const GetFileSizeNode&>(CurrentNode);
		return new GetFileSizeNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("GetFileSizeNode", Creator, Copier);
	return true;
}();

GetFileSizeNode::GetFileSizeNode() : BaseExecutionFlowNode()
{
	Type = "GetFileSizeNode";

	SetStyle(DEFAULT);
	SetName("Get File Size");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "INT", "Bytes", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(SizeDataGetter);
}

GetFileSizeNode::GetFileSizeNode(const GetFileSizeNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	SizeInBytes = Other.SizeInBytes;
	Output[1]->SetFunctionToOutputData(SizeDataGetter);
}

Json::Value GetFileSizeNode::ToJson()
{
	return Node::ToJson();
}

bool GetFileSizeNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(SizeDataGetter);
	return true;
}

void GetFileSizeNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 40.0f * Zoom));
	ImGui::Text("%d bytes", SizeInBytes);
}

void GetFileSizeNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		SizeInBytes = static_cast<int>(FocalEngine::FILE_SYSTEM.GetFileSize(PathInput));

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool GetFileSizeNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}