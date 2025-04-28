#include "Vec2AddNode.h"
using namespace VisNodeSys;

bool Vec2AddNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("Vec2AddNode",
		[]() -> Node* {
			return new Vec2AddNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const Vec2AddNode& NodeToCopy = static_cast<const Vec2AddNode&>(CurrentNode);
			return new Vec2AddNode(NodeToCopy);
		}
	);

	return true;
}();

Vec2AddNode::Vec2AddNode() : BasicLogicNode()
{
	Type = "Vec2AddNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(90, 78));
	SetName("Add vec2");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "VEC2", "", false));
	AddSocket(new NodeSocket(this, "VEC2", "", false));

	AddSocket(new NodeSocket(this, "VEC2", "", true));
	Output[0]->SetFunctionToOutputData(Vec2AddDataGetter);
}

Vec2AddNode::Vec2AddNode(const Vec2AddNode& Src) : BasicLogicNode(Src)
{
	SetStyle(DEFAULT);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec2AddDataGetter);
}

void Vec2AddNode::Draw()
{	
	Node::Draw();
}

void Vec2AddNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool Vec2AddNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

BasicLogicNode* Vec2AddNode::GetNextNode()
{
	return nullptr;
}