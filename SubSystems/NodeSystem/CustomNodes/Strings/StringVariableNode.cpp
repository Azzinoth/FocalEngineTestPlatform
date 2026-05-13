#include "StringVariableNode.h"
using namespace VisNodeSys;

bool StringVariableNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringVariableNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringVariableNode& NodeToCopy = static_cast<const StringVariableNode&>(CurrentNode);
		return new StringVariableNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringVariableNode", Creator, Copier);
	return true;
}();

StringVariableNode::StringVariableNode() : BaseExecutionFlowNode()
{
	Type = "StringVariableNode";

	SetStyle(DEFAULT);
	SetName("String Variable");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(250, 200));
	Output[1]->SetFunctionToOutputData(StringDataGetter);
}

StringVariableNode::StringVariableNode(const StringVariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(StringDataGetter);
}

Json::Value StringVariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = Data;
	return Result;
}

bool StringVariableNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value") || !Json["Value"].isString())
		return false;

	Data = Json["Value"].asString();

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(StringDataGetter);
	return true;
}

void StringVariableNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	static char TemporaryTextBuffer[16384];
	strcpy_s(TemporaryTextBuffer, Data.c_str());

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 25.0f * Zoom, ImGui::GetCursorScreenPos().y + 40.0f * Zoom));
	if (ImGui::InputTextMultiline("##StringVariableValue", TemporaryTextBuffer, IM_ARRAYSIZE(TemporaryTextBuffer), ImVec2(200, 150) * Zoom, ImGuiInputTextFlags_AllowTabInput))
	{
		Data = TemporaryTextBuffer;
		if (Output[1]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnectedSockets()[0], UPDATE);
	}
}

void StringVariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<std::string*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool StringVariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

std::string StringVariableNode::GetData() const
{
	return Data;
}

void StringVariableNode::SetData(std::string NewValue)
{
	Data = NewValue;
}