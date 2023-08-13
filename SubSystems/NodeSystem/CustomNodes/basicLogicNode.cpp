#include "basicLogicNode.h"
using namespace VisNodeSys;

bool basicLogicNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("basicLogicNode",
		[]() -> Node* {
			return new basicLogicNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const basicLogicNode& NodeToCopy = static_cast<const basicLogicNode&>(CurrentNode);
			return new basicLogicNode(NodeToCopy);
		}
	);

	return true;
}();

basicLogicNode::basicLogicNode() : Node()
{
	Type = "basicLogicNode";
}

basicLogicNode* basicLogicNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());

	return nullptr;
}