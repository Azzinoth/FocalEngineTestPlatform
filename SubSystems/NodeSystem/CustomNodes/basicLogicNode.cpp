#include "BasicLogicNode.h"
using namespace VisNodeSys;

bool BasicLogicNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("BasicLogicNode",
		[]() -> Node* {
			return new BasicLogicNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const BasicLogicNode& NodeToCopy = static_cast<const BasicLogicNode&>(CurrentNode);
			return new BasicLogicNode(NodeToCopy);
		}
	);

	return true;
}();

BasicLogicNode::BasicLogicNode() : Node()
{
	Type = "BasicLogicNode";
}

BasicLogicNode* BasicLogicNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		return reinterpret_cast<BasicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());

	return nullptr;
}