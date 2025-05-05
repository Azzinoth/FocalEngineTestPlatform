#include "MouseLeftButtonUp.h"
using namespace VisNodeSys;

bool MouseLeftButtonUp::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseLeftButtonUp",
		[]() -> Node* {
			return new MouseLeftButtonUp();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseLeftButtonUp& NodeToCopy = static_cast<const MouseLeftButtonUp&>(CurrentNode);
			return new MouseLeftButtonUp(NodeToCopy);
		}
	);

	return true;
}();

MouseLeftButtonUp::MouseLeftButtonUp() : BaseExecutionFlowNode()
{
	Type = "MouseLeftButtonUp";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse left button up");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

MouseLeftButtonUp::MouseLeftButtonUp(const MouseLeftButtonUp& Src) : BaseExecutionFlowNode(Src)
{
	SetStyle(DEFAULT);
}

void MouseLeftButtonUp::Draw()
{	
	Node::Draw();
}

void MouseLeftButtonUp::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseUp();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseLeftButtonUp::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}