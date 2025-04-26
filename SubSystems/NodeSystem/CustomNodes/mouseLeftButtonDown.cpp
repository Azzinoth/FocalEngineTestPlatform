#include "mouseLeftButtonDown.h"
using namespace VisNodeSys;

bool mouseLeftButtonDown::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("mouseLeftButtonDown",
		[]() -> Node* {
			return new mouseLeftButtonDown();
		},

		[](const Node& CurrentNode) -> Node* {
			const mouseLeftButtonDown& NodeToCopy = static_cast<const mouseLeftButtonDown&>(CurrentNode);
			return new mouseLeftButtonDown(NodeToCopy);
		}
	);

	return true;
}();

mouseLeftButtonDown::mouseLeftButtonDown() : basicLogicNode()
{
	Type = "mouseLeftButtonDown";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse left button down");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

mouseLeftButtonDown::mouseLeftButtonDown(const mouseLeftButtonDown& Src) : basicLogicNode(Src)
{
	SetStyle(DEFAULT);
}

void mouseLeftButtonDown::Draw()
{	
	Node::Draw();
}

void mouseLeftButtonDown::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseDown();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool mouseLeftButtonDown::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* mouseLeftButtonDown::GetNextNode()
{
	return nullptr;
}