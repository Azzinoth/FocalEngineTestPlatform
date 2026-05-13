#include "StringLiteralNode.h"
using namespace VisNodeSys;

bool StringLiteralNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new StringLiteralNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const StringLiteralNode& NodeToCopy = static_cast<const StringLiteralNode&>(CurrentNode);
		return new StringLiteralNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("StringLiteralNode", Creator, Copier);
	return true;
}();

StringLiteralNode::StringLiteralNode() : VisNodeSys::Node()
{
	Type = "StringLiteralNode";

	SetStyle(DEFAULT);
	SetName("String Literal");

	TitleBackgroundColor = ImColor(241, 0, 165);
	TitleBackgroundColorHovered = ImColor(255, 80, 195);

	AddSocket(new NodeSocket(this, "STRING", "", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(250, 200));
	Output[0]->SetFunctionToOutputData(StringDataGetter);
}

StringLiteralNode::StringLiteralNode(const StringLiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(StringDataGetter);
}

Json::Value StringLiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = Data;
	return Result;
}

bool StringLiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value") || !Json["Value"].isString())
		return false;

	Data = Json["Value"].asString();

	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(StringDataGetter);
	return true;
}

void StringLiteralNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	static char TemporaryTextBuffer[16384];
	strcpy_s(TemporaryTextBuffer, Data.c_str());

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 25.0f * Zoom, ImGui::GetCursorScreenPos().y + 40.0f * Zoom));
	if (ImGui::InputTextMultiline("##StringLiteralValue", TemporaryTextBuffer, IM_ARRAYSIZE(TemporaryTextBuffer), ImVec2(200, 150) * Zoom, ImGuiInputTextFlags_AllowTabInput))
	{
		Data = TemporaryTextBuffer;
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void StringLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);
}

bool StringLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

std::string StringLiteralNode::GetData() const
{
	return Data;
}

void StringLiteralNode::SetData(std::string NewValue)
{
	Data = NewValue;
}