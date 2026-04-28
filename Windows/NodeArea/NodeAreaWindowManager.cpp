#include "NodeAreaWindowManager.h"
using namespace VisNodeSys;

NodeAreaWindowManager::NodeAreaWindowManager() {}

NodeAreaWindow* NodeAreaWindowManager::CreateNodeAreaWindow(NodeArea* NodeArea, ImVec2 Position, ImVec2 Size)
{
	if (NodeArea == nullptr)
		return nullptr;

	if (NodeAreaWindows.find(NodeArea->GetID()) != NodeAreaWindows.end())
		return NodeAreaWindows[NodeArea->GetID()];

	NodeAreaWindow* NewWindow = new NodeAreaWindow(NodeArea);
	NewWindow->SetPosition(Position);
	NewWindow->SetSize(Size);
	NewWindow->SetCaption("Node Area: " + NodeArea->GetName());
	NewWindow->Show();

	NodeAreaWindows[NodeArea->GetID()] = NewWindow;
	return NewWindow;
}

bool NodeAreaWindowManager::OpenNodeAreaWindow(VisNodeSys::NodeArea* NodeArea)
{
	if (NodeArea == nullptr)
		return false;

	if (NodeAreaWindows.find(NodeArea->GetID()) != NodeAreaWindows.end())
	{
		NodeAreaWindows[NodeArea->GetID()]->Show();
		return true;
	}

	CreateNodeAreaWindow(NodeArea);
	return true;
}

NodeAreaWindow* NodeAreaWindowManager::GetNodeAreaWindow(VisNodeSys::NodeArea* NodeArea) const
{
	if (NodeArea == nullptr)
		return nullptr;

	auto WindowIterator = NodeAreaWindows.find(NodeArea->GetID());
	if (WindowIterator != NodeAreaWindows.end())
		return WindowIterator->second;

	return nullptr;
}

void NodeAreaWindowManager::SetFocusedNodeAreaID(std::string NewFocusedNodeAreaID)
{
	FocusedNodeAreaID = NewFocusedNodeAreaID;
}

NodeAreaWindow* NodeAreaWindowManager::GetInFocusNodeAreaWindow() const
{
	NodeArea* FocusedNodeArea = NODE_SYSTEM.GetNodeAreaByID(FocusedNodeAreaID);
	return GetNodeAreaWindow(FocusedNodeArea);
}

void NodeAreaWindowManager::Render()
{
	NODE_AREAS_GRAPH_WINDOW.Render();

	auto WindowIterator = NodeAreaWindows.begin();
	while (WindowIterator != NodeAreaWindows.end())
	{
		if (WindowIterator->second->IsVisible())
			WindowIterator->second->Render();
		
		WindowIterator++;
	}
}