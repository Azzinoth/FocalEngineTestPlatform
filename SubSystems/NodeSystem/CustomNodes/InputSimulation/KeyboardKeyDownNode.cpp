#include "KeyboardKeyDownNode.h"
using namespace VisNodeSys;

bool KeyboardKeyDownNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new KeyboardKeyDownNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const KeyboardKeyDownNode& NodeToCopy = static_cast<const KeyboardKeyDownNode&>(CurrentNode);
		return new KeyboardKeyDownNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("KeyboardKeyDownNode", Creator, Copier);
	return true;
}();

KeyboardKeyDownNode::KeyboardKeyDownNode() : BaseExecutionFlowNode()
{
	Type = "KeyboardKeyDownNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Keyboard Key Down");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "INT", "Key Code", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

KeyboardKeyDownNode::KeyboardKeyDownNode(const KeyboardKeyDownNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
	VirtualKeyCode = Src.VirtualKeyCode;
}

Json::Value KeyboardKeyDownNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["VirtualKeyCode"] = static_cast<int>(VirtualKeyCode);
	return Result;
}

bool KeyboardKeyDownNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Json.isMember("VirtualKeyCode") && Json["VirtualKeyCode"].isNumeric())
		VirtualKeyCode = static_cast<DWORD>(Json["VirtualKeyCode"].asInt());

	return true;
}

void KeyboardKeyDownNode::Draw()
{
	Node::Draw();
}

void KeyboardKeyDownNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				VirtualKeyCode = static_cast<DWORD>(*reinterpret_cast<int*>(TemporaryData));
		}

		INPUT_SYSTEM.SimulateKeyEvent(WM_KEYDOWN, VirtualKeyCode);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool KeyboardKeyDownNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

void KeyboardKeyDownNode::SetVirtualKeyCode(DWORD NewValue)
{
	VirtualKeyCode = NewValue;
}