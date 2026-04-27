#include "MouseRightButtonDownNode.h"
using namespace VisNodeSys;

bool MouseRightButtonDownNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseRightButtonDown",
		[]() -> Node* {
			return new MouseRightButtonDownNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseRightButtonDownNode& NodeToCopy = static_cast<const MouseRightButtonDownNode&>(CurrentNode);
			return new MouseRightButtonDownNode(NodeToCopy);
		}
	);

	return true;
}();

MouseRightButtonDownNode::MouseRightButtonDownNode() : BaseExecutionFlowNode()
{
	Type = "MouseRightButtonDownNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse Right Button Down");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

MouseRightButtonDownNode::MouseRightButtonDownNode(const MouseRightButtonDownNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
}

void MouseRightButtonDownNode::Draw()
{	
	Node::Draw();
}

void MouseRightButtonDownNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseDown(false);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseRightButtonDownNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}