#include "KeyboardKeyUpNode.h"
using namespace VisNodeSys;

bool KeyboardKeyUpNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("KeyboardKeyUpNode",
		[]() -> Node* {
			return new KeyboardKeyUpNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const KeyboardKeyUpNode& NodeToCopy = static_cast<const KeyboardKeyUpNode&>(CurrentNode);
			return new KeyboardKeyUpNode(NodeToCopy);
		}
	);

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

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

KeyboardKeyUpNode::KeyboardKeyUpNode(const KeyboardKeyUpNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
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