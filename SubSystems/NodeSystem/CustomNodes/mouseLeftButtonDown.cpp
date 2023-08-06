#include "mouseLeftButtonDown.h"

bool mouseLeftButtonDown::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("mouseLeftButtonDown",
		[]() -> VisualNode* {
			return new mouseLeftButtonDown();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const mouseLeftButtonDown& NodeToCopy = static_cast<const mouseLeftButtonDown&>(Node);
			return new mouseLeftButtonDown(NodeToCopy);
		}
	);

	return true;
}();

mouseLeftButtonDown::mouseLeftButtonDown() : basicLogicNode()
{
	Type = "mouseLeftButtonDown";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse left button down");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

mouseLeftButtonDown::mouseLeftButtonDown(const mouseLeftButtonDown& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
}

void mouseLeftButtonDown::Draw()
{	
	VisualNode::Draw();
}

void mouseLeftButtonDown::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		INPUT_SYSTEM.mouseDown();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], VISUAL_NODE_SOCKET_EXECUTE);
	}
}

bool mouseLeftButtonDown::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* mouseLeftButtonDown::GetNextNode()
{
	return nullptr;
}