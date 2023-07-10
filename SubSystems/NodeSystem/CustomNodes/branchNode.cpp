#include "branchNode.h"

bool branchNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("branchNode",
		[]() -> VisualNode* {
			return new branchNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const branchNode& NodeToCopy = static_cast<const branchNode&>(Node);
			return new branchNode(NodeToCopy);
		}
	);

	return true;
}();

branchNode::branchNode() : basicLogicNode()
{
	Type = "branchNode";

	SetStyle(VISUAL_NODE_STYLE_DEFAULT);

	SetSize(ImVec2(220, 78));
	SetName("branch node");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "EXECUTE", "", false));
	AddSocket(new NodeSocket(this, "BOOL", "Condition", false));
	
	AddSocket(new NodeSocket(this, "EXECUTE", "True", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "False", true));
}

branchNode::branchNode(const branchNode& Src) : basicLogicNode(Src)
{
	SetStyle(VISUAL_NODE_STYLE_DEFAULT);
}

void branchNode::Draw()
{	
	VisualNode::Draw();
}

void branchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	VisualNode::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == VISUAL_NODE_SOCKET_EXECUTE)
	{
		bool Condition = false;

		if (Input[1]->GetConnections().size() > 0)
		{
			void* TempData = Input[1]->GetConnections()[0]->GetData();
			if (TempData != nullptr)
				Condition = *reinterpret_cast<bool*>(TempData);
		}

		if (Condition)
		{
			if (Output[0]->GetConnections().size() > 0)
				ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
		}
		else
		{
			if (Output[1]->GetConnections().size() > 0)
				ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnections()[0], VISUAL_NODE_SOCKET_EXECUTE);
		}
	}
}

bool branchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!VisualNode::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* branchNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnections().size() > 0)
		return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnections()[0]->GetParent());
	
	return nullptr;
}