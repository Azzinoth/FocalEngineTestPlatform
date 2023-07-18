#include "mouseLeftButtonUp.h"

bool mouseLeftButtonUp::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("mouseLeftButtonUp",
		[]() -> VisualNode* {
			return new mouseLeftButtonUp();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const mouseLeftButtonUp& NodeToCopy = static_cast<const mouseLeftButtonUp&>(Node);
			return new mouseLeftButtonUp(NodeToCopy);
		}
	);

	return true;
}();

mouseLeftButtonUp::mouseLeftButtonUp() : basicLogicNode()
{
	Type = "mouseLeftButtonUp";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(250, 90));
	SetName("Mouse left button up");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
}

mouseLeftButtonUp::mouseLeftButtonUp(const mouseLeftButtonUp& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
}

void mouseLeftButtonUp::Draw()
{	
	VisualNode::Draw();
}

void mouseLeftButtonUp::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		INPUT_SYSTEM.mouseUp();

		if (Output[0]->GetConnections().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
	}
}

bool mouseLeftButtonUp::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* mouseLeftButtonUp::GetNextNode()
{
	return nullptr;
}