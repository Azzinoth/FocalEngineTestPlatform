#include "KeyboardKeyDownNode.h"
using namespace VisNodeSys;

bool KeyboardKeyDownNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("KeyboardKeyDownNode",
		[]() -> Node* {
			return new KeyboardKeyDownNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const KeyboardKeyDownNode& NodeToCopy = static_cast<const KeyboardKeyDownNode&>(CurrentNode);
			return new KeyboardKeyDownNode(NodeToCopy);
		}
	);

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

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

KeyboardKeyDownNode::KeyboardKeyDownNode(const KeyboardKeyDownNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
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