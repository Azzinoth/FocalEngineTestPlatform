#include "IsStringNumberNode.h"
#include <cstdlib>
using namespace VisNodeSys;

bool IsStringNumberNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new IsStringNumberNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const IsStringNumberNode& NodeToCopy = static_cast<const IsStringNumberNode&>(CurrentNode);
		return new IsStringNumberNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("IsStringNumberNode", Creator, Copier);
	return true;
}();

IsStringNumberNode::IsStringNumberNode() : BaseExecutionFlowNode()
{
	Type = "IsStringNumberNode";

	SetStyle(DEFAULT);
	SetName("Is String Number");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "Value", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "BOOL", "Is Number", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(220, 95));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

IsStringNumberNode::IsStringNumberNode(const IsStringNumberNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Result = Other.Result;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value IsStringNumberNode::ToJson()
{
	return Node::ToJson();
}

bool IsStringNumberNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	return true;
}

void IsStringNumberNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 OriginPosition = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(ImVec2(OriginPosition.x + 25.0f * Zoom, OriginPosition.y + 40.0f * Zoom));
	ImGui::Text("%s", Result ? "true" : "false");
}

void IsStringNumberNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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
			std::strtod(InputValue.c_str(), &EndPointer);
			Result = (EndPointer == InputValue.c_str() + InputValue.size());
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool IsStringNumberNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}