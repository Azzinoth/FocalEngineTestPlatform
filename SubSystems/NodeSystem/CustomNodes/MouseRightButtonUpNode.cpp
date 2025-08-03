#include "MouseRightButtonUpNode.h"
using namespace VisNodeSys;

bool MouseRightButtonUpNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseRightButtonUp",
		[]() -> Node* {
			return new MouseRightButtonUpNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseRightButtonUpNode& NodeToCopy = static_cast<const MouseRightButtonUpNode&>(CurrentNode);
			return new MouseRightButtonUpNode(NodeToCopy);
		}
	);

	return true;
}();

MouseRightButtonUpNode::MouseRightButtonUpNode() : BaseExecutionFlowNode()
{
	Type = "MouseRightButtonUpNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse Right Button Up");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

MouseRightButtonUpNode::MouseRightButtonUpNode(const MouseRightButtonUpNode& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
}

void MouseRightButtonUpNode::Draw()
{
	Node::Draw();
}

void MouseRightButtonUpNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseUp(false);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseRightButtonUpNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}