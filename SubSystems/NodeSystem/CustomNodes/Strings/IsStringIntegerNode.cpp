#include "IsStringIntegerNode.h"
#include <cstdlib>
using namespace VisNodeSys;

bool IsStringIntegerNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new IsStringIntegerNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const IsStringIntegerNode& NodeToCopy = static_cast<const IsStringIntegerNode&>(CurrentNode);
		return new IsStringIntegerNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("IsStringIntegerNode", Creator, Copier);
	return true;
}();

IsStringIntegerNode::IsStringIntegerNode() : BaseExecutionFlowNode()
{
	Type = "IsStringIntegerNode";

	SetStyle(DEFAULT);
	SetName("Is String Integer");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Is Integer", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

IsStringIntegerNode::IsStringIntegerNode(const IsStringIntegerNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value IsStringIntegerNode::ToJson()
{
	return Node::ToJson();
}

bool IsStringIntegerNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void IsStringIntegerNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 40.0f * Zoom));
	ImGui::Text("%s", Result ? "true" : "false");
}

void IsStringIntegerNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		std::string InputValue = "";
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				InputValue = *reinterpret_cast<std::string*>(TemporaryData);
		}

		if (InputValue.empty())
		{
			Result = false;
		}
		else
		{
			char* EndPointer = nullptr;
			std::strtol(InputValue.c_str(), &EndPointer, 10);
			Result = (EndPointer == InputValue.c_str() + InputValue.size());
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool IsStringIntegerNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}