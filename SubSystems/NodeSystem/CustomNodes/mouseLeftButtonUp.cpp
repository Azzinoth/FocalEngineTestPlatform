#include "mouseLeftButtonUp.h"
using namespace VisNodeSys;

bool mouseLeftButtonUp::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("mouseLeftButtonUp",
		[]() -> Node* {
			return new mouseLeftButtonUp();
		},

		[](const Node& CurrentNode) -> Node* {
			const mouseLeftButtonUp& NodeToCopy = static_cast<const mouseLeftButtonUp&>(CurrentNode);
			return new mouseLeftButtonUp(NodeToCopy);
		}
	);

	return true;
}();

mouseLeftButtonUp::mouseLeftButtonUp() : basicLogicNode()
{
	Type = "mouseLeftButtonUp";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse left button up");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

mouseLeftButtonUp::mouseLeftButtonUp(const mouseLeftButtonUp& Src) : basicLogicNode(Src)
{
	SetStyle(DEFAULT);
}

void mouseLeftButtonUp::Draw()
{	
	Node::Draw();
}

void mouseLeftButtonUp::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseUp();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool mouseLeftButtonUp::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* mouseLeftButtonUp::GetNextNode()
{
	return nullptr;
}