#include "MouseLeftButtonUpNode.h"
using namespace VisNodeSys;

bool MouseLeftButtonUpNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseLeftButtonUpNode",
		[]() -> Node* {
			return new MouseLeftButtonUpNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseLeftButtonUpNode& NodeToCopy = static_cast<const MouseLeftButtonUpNode&>(CurrentNode);
			return new MouseLeftButtonUpNode(NodeToCopy);
		}
	);

	return true;
}();

MouseLeftButtonUpNode::MouseLeftButtonUpNode() : BaseExecutionFlowNode()
{
	Type = "MouseLeftButtonUpNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse Left Button Up");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
}

MouseLeftButtonUpNode::MouseLeftButtonUpNode(const MouseLeftButtonUpNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
}

void MouseLeftButtonUpNode::Draw()
{	
	Node::Draw();
}

void MouseLeftButtonUpNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseUp();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseLeftButtonUpNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}