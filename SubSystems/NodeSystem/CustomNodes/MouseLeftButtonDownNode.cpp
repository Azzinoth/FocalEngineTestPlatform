#include "MouseLeftButtonDownNode.h"
using namespace VisNodeSys;

bool MouseLeftButtonDownNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseLeftButtonDown",
		[]() -> Node* {
			return new MouseLeftButtonDownNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseLeftButtonDownNode& NodeToCopy = static_cast<const MouseLeftButtonDownNode&>(CurrentNode);
			return new MouseLeftButtonDownNode(NodeToCopy);
		}
	);

	return true;
}();

MouseLeftButtonDownNode::MouseLeftButtonDownNode() : BaseExecutionFlowNode()
{
	Type = "MouseLeftButtonDownNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse Left Button Down");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

MouseLeftButtonDownNode::MouseLeftButtonDownNode(const MouseLeftButtonDownNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
}

void MouseLeftButtonDownNode::Draw()
{	
	Node::Draw();
}

void MouseLeftButtonDownNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseDown();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseLeftButtonDownNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}