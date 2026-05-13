#include "KeyboardKeyUpNode.h"
using namespace VisNodeSys;

bool KeyboardKeyUpNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new KeyboardKeyUpNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const KeyboardKeyUpNode& NodeToCopy = static_cast<const KeyboardKeyUpNode&>(CurrentNode);
		return new KeyboardKeyUpNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("KeyboardKeyUpNode", Creator, Copier);
	return true;
}();

KeyboardKeyUpNode::KeyboardKeyUpNode() : BaseExecutionFlowNode()
{
	Type = "KeyboardKeyUpNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Keyboard Key Up");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "INT", "Key Code", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

KeyboardKeyUpNode::KeyboardKeyUpNode(const KeyboardKeyUpNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
	VirtualKeyCode = Src.VirtualKeyCode;
}

Json::Value KeyboardKeyUpNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["VirtualKeyCode"] = static_cast<int>(VirtualKeyCode);
	return Result;
}

bool KeyboardKeyUpNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Json.isMember("VirtualKeyCode") && Json["VirtualKeyCode"].isNumeric())
		VirtualKeyCode = static_cast<DWORD>(Json["VirtualKeyCode"].asInt());

	return true;
}

void KeyboardKeyUpNode::Draw()
{
	Node::Draw();
}

void KeyboardKeyUpNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
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

		INPUT_SYSTEM.SimulateKeyEvent(WM_KEYUP, VirtualKeyCode);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool KeyboardKeyUpNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

void KeyboardKeyUpNode::SetVirtualKeyCode(DWORD NewValue)
{
	VirtualKeyCode = NewValue;
}