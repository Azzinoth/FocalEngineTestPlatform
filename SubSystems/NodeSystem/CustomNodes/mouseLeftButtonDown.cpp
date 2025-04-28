#include "MouseLeftButtonDown.h"
using namespace VisNodeSys;

bool MouseLeftButtonDown::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("MouseLeftButtonDown",
		[]() -> Node* {
			return new MouseLeftButtonDown();
		},

		[](const Node& CurrentNode) -> Node* {
			const MouseLeftButtonDown& NodeToCopy = static_cast<const MouseLeftButtonDown&>(CurrentNode);
			return new MouseLeftButtonDown(NodeToCopy);
		}
	);

	return true;
}();

MouseLeftButtonDown::MouseLeftButtonDown() : BasicLogicNode()
{
	Type = "MouseLeftButtonDown";

	SetStyle(DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse left button down");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

MouseLeftButtonDown::MouseLeftButtonDown(const MouseLeftButtonDown& Src) : BasicLogicNode(Src)
{
	SetStyle(DEFAULT);
}

void MouseLeftButtonDown::Draw()
{	
	Node::Draw();
}

void MouseLeftButtonDown::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		INPUT_SYSTEM.SimulateMouseDown();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool MouseLeftButtonDown::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* MouseLeftButtonDown::GetNextNode()
{
	return nullptr;
}