#include "basicLogicNode.h"

bool basicLogicNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("basicLogicNode",
		[]() -> VisualNode* {
			return new basicLogicNode();
		},

		[](const VisualNode& Node) -> VisualNode* {
			const basicLogicNode& NodeToCopy = static_cast<const basicLogicNode&>(Node);
			return new basicLogicNode(NodeToCopy);
		}
	);

	return true;
}();

basicLogicNode::basicLogicNode() : VisualNode()
{
	Type = "basicLogicNode";
}

basicLogicNode* basicLogicNode::GetNextNode()
{
	if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
		return reinterpret_cast<basicLogicNode*>(Output[0]->GetConnectedSockets()[0]->GetParent());

	return nullptr;
}