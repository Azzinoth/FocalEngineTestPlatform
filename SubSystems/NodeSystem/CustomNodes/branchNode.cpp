#include "branchNode.h"
using namespace VisNodeSys;

bool branchNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("branchNode",
		[]() -> Node* {
			return new branchNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const branchNode& NodeToCopy = static_cast<const branchNode&>(CurrentNode);
			return new branchNode(NodeToCopy);
		}
	);

	return true;
}();

branchNode::branchNode() : basicLogicNode()
{
	Type = "branchNode";

	SetStyle(DEFAULT);

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
	SetStyle(DEFAULT);
}

void branchNode::Draw()
{	
	Node::Draw();
}

void branchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		bool Condition = false;

		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				Condition = *reinterpret_cast<bool*>(TempData);
		}

		if (Condition)
		{
			if (Output[0]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
		}
		else
		{
			if (Output[1]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnectedSockets()[0], EXECUTE);
		}
	}
}

bool branchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* branchNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());
	
	return nullptr;
}