#include "vec2AddNode.h"
using namespace VisNodeSys;

bool vec2AddNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("vec2AddNode",
		[]() -> Node* {
			return new vec2AddNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const vec2AddNode& NodeToCopy = static_cast<const vec2AddNode&>(CurrentNode);
			return new vec2AddNode(NodeToCopy);
		}
	);

	return true;
}();

vec2AddNode::vec2AddNode() : basicLogicNode()
{
	Type = "vec2AddNode";

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

vec2AddNode::vec2AddNode(const vec2AddNode& Src) : basicLogicNode(Src)
{
	SetStyle(DEFAULT);

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec2AddDataGetter);
}

void vec2AddNode::Draw()
{	
	Node::Draw();
}

void vec2AddNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool vec2AddNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

basicLogicNode* vec2AddNode::GetNextNode()
{
	return nullptr;
}