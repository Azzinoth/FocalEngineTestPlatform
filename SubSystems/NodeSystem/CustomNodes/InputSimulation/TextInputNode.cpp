#include "TextInputNode.h"
#include "../../FEPTActionSystem.h"
using namespace VisNodeSys;

bool TextInputNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new TextInputNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const TextInputNode& NodeToCopy = static_cast<const TextInputNode&>(CurrentNode);
		return new TextInputNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("TextInputNode", Creator, Copier);
	return true;
}();

TextInputNode::TextInputNode() : BaseExecutionFlowNode()
{
	Type = "TextInputNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 200));
	SetName("Text Input");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "STRING", "", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

TextInputNode::TextInputNode(const TextInputNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;
}

Json::Value TextInputNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["Text"] = Data;

	return Result;
}

bool TextInputNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Text") || !Json["Text"].isString())
		return false;

	Data = Json["Text"].asString();

	return true;
}

bool TextInputNode::SetText(std::string Text)
{
	Data = Text;

	return true;
}

void TextInputNode::Draw()
{
	Node::Draw();

	static char TemporaryTextBuffer[16384];
	strcpy_s(TemporaryTextBuffer, Data.c_str());

	bool bInputOverrides = Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0;

	float Zoom = ParentArea->GetZoomFactor();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 25.0f * Zoom, ImGui::GetCursorScreenPos().y + 40.0f * Zoom));
	if (bInputOverrides)
		ImGui::BeginDisabled();

	if (ImGui::InputTextMultiline("##TextInput", TemporaryTextBuffer, IM_ARRAYSIZE(TemporaryTextBuffer), ImVec2(200, 150) * Zoom, ImGuiInputTextFlags_AllowTabInput))
	{
		Data = TemporaryTextBuffer;
	}

	if (bInputOverrides)
		ImGui::EndDisabled();
}

void TextInputNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<std::string*>(TemporaryData);
		}

		INPUT_SYSTEM.SimulateTextInput(Data, 10);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool TextInputNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}