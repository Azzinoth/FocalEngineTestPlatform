#pragma once
#include "NodeAreaGraphBackend.h"
#include "../../SubSystems/FETest.h"
using namespace VisNodeSys;

NodeAreaGraphBackend::NodeAreaGraphBackend() {}

bool NodeAreaGraphBackend::IsReady() const
{
	NodeArea* RootNodeArea = NODE_SYSTEM.GetNodeAreaByID(RootNodeAreaID);
	return RootNodeArea != nullptr && CurrentTest != nullptr;
}

void NodeAreaGraphBackend::SetCurrentTest(FETest* NewTest)
{
	CurrentTest = NewTest;
}

FETest* NodeAreaGraphBackend::GetCurrentTest() const
{
	return CurrentTest;
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

    NodeArea* CurrentNodeArea = Node.As<NodeArea>();
    // Special case for the dummy root node area.
    // It is not actually part of the node area hierarchy, but should be displayed as the root of the hierarchy.
    if (CurrentTest->GetDummyRootNodeArea() == CurrentNodeArea)
    {
        NodeArea* EntryPointNodeArea = CurrentTest->EntryPointNodeArea;
        if (EntryPointNodeArea != nullptr)
            Result.push_back({ EntryPointNodeArea, this });

		std::vector<std::string> NodeAreaIDList = NODE_SYSTEM.GetNodeAreaIDList();
        for (const std::string& NodeAreaID : NodeAreaIDList)
        {
            NodeArea* NodeAreaWithoutParent = NODE_SYSTEM.GetNodeAreaByID(NodeAreaID);
            if (NodeAreaWithoutParent == nullptr)
                continue;

			if (NodeAreaWithoutParent == EntryPointNodeArea || NodeAreaWithoutParent == CurrentTest->GetDummyRootNodeArea())
                continue;

            if (NodeAreaWithoutParent->GetParent() == nullptr)
                Result.push_back({ NodeAreaWithoutParent, this });
		}

		return Result;
    }

	std::vector<NodeArea*> ChildNodeAreas;
    for (NodeArea* Child : CurrentNodeArea->GetImediateChildren())
		Result.push_back({ Child, this });
    
    return Result;
}

SceneGraphUI::NodeHandle NodeAreaGraphBackend::GetParent(SceneGraphUI::NodeHandle Node)
{
    if (!Node)
		return { nullptr, this };

    NodeArea* CurrentNodeArea = Node.As<NodeArea>();
    if (CurrentNodeArea->GetParent() == nullptr && CurrentTest->GetDummyRootNodeArea() != CurrentNodeArea)
        return { CurrentTest->GetDummyRootNodeArea(), this };
    
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