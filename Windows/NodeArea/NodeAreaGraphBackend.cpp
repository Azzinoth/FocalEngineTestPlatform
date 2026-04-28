#pragma once
#include "NodeAreaGraphBackend.h"
using namespace VisNodeSys;

NodeAreaGraphBackend::NodeAreaGraphBackend() {}

bool NodeAreaGraphBackend::IsReady() const
{
	NodeArea* RootNodeArea = NODE_SYSTEM.GetNodeAreaByID(RootNodeAreaID);
	return RootNodeArea != nullptr;
}

void NodeAreaGraphBackend::SetRootNodeAreaID(std::string NewRootNodeAreaID)
{
    RootNodeAreaID = NewRootNodeAreaID;
}

SceneGraphUI::NodeHandle NodeAreaGraphBackend::GetRoot()
{
    NodeArea* RootNodeArea = NODE_SYSTEM.GetNodeAreaByID(RootNodeAreaID);
    if (RootNodeArea == nullptr)
        return { nullptr, this };

    return { RootNodeArea, this };
}

std::vector<SceneGraphUI::NodeHandle> NodeAreaGraphBackend::GetChildren(SceneGraphUI::NodeHandle Node)
{
	std::vector<SceneGraphUI::NodeHandle> Result;
    if (!Node)
        return Result;

	std::vector<NodeArea*> ChildNodeAreas;
    NodeArea* CurrentNodeArea = Node.As<NodeArea>();
    for (NodeArea* Child : CurrentNodeArea->GetImediateChildren())
		Result.push_back({ Child, this });
    
    return Result;
}

SceneGraphUI::NodeHandle NodeAreaGraphBackend::GetParent(SceneGraphUI::NodeHandle Node)
{
    if (!Node)
		return { nullptr, this };

    NodeArea* CurrentNodeArea = Node.As<NodeArea>();
	return { CurrentNodeArea->GetParent(), this };
}

SceneGraphUI::NodeHandle NodeAreaGraphBackend::GetNodeByID(const std::string& ID)
{
    NodeArea* FoundNodeArea = NODE_SYSTEM.GetNodeAreaByID(ID);
    if (FoundNodeArea == nullptr)
        return { nullptr, this };

	return { FoundNodeArea, this };
}

std::string NodeAreaGraphBackend::GetNodeID(SceneGraphUI::NodeHandle Node)
{
	return Node.As<NodeArea>()->GetID();
}

std::string NodeAreaGraphBackend::GetNodeName(SceneGraphUI::NodeHandle Node)
{
	return Node.As<NodeArea>()->GetName();
}

std::string NodeAreaGraphBackend::GetTag(SceneGraphUI::NodeHandle Node)
{
    return "";
}

bool NodeAreaGraphBackend::MoveNode(SceneGraphUI::NodeHandle Node, SceneGraphUI::NodeHandle NewParent)
{
    //return Graph->MoveNode(Node.GetID(), NewParent.GetID());
	return false;
}

bool NodeAreaGraphBackend::IsAlive(SceneGraphUI::NodeHandle Node)
{
    if (!Node)
        return false;

    NodeArea* RootNodeArea = NODE_SYSTEM.GetNodeAreaByID(RootNodeAreaID);
    if (RootNodeArea == nullptr)
        return false;

    NodeArea* FoundNodeArea = NODE_SYSTEM.GetNodeAreaByID(Node.GetID());
    if (FoundNodeArea == nullptr)
		return false;

    return true;
}