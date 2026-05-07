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
	if (SeenNodeAreaID.find(NodeArea->GetID()) == SeenNodeAreaID.end())
	{
		SeenNodeAreaID[NodeArea->GetID()] = true;
		NewWindow->bShouldDockToCentralNode = true;
		NewWindow->bShouldCenterViewOnOpen = true;
	}
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

bool NodeAreaWindowManager::DeleteNodeAreaWindow(VisNodeSys::NodeArea* NodeArea)
{
	if (NodeArea == nullptr)
		return false;

	auto WindowIterator = NodeAreaWindows.find(NodeArea->GetID());
	if (WindowIterator != NodeAreaWindows.end())
	{
		delete WindowIterator->second;
		NodeAreaWindows.erase(WindowIterator);
		return true;
	}

	return false;
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

bool NodeAreaWindowManager::SetInFocusNodeAreaWindow(NodeAreaWindow* NodeAreaWindow)
{
	if (NodeAreaWindow == nullptr)
	{
		FocusedNodeAreaID = "";
		return true;
	}

	NodeArea* NodeArea = NodeAreaWindow->GetNodeArea();
	if (NodeArea == nullptr)
		return false;

	FocusedNodeAreaID = NodeArea->GetID();
	ImGui::SetWindowFocus(NodeAreaWindow->GetCaption().c_str());
	return true;
}

void NodeAreaWindowManager::Render()
{
	NODE_AREAS_GRAPH_WINDOW.Render();

	auto WindowIterator = NodeAreaWindows.begin();
	while (WindowIterator != NodeAreaWindows.end())
	{
		// Check if any of the node areas was deleted, if so, close the corresponding window and remove it from the map.
		if (WindowIterator->second->GetNodeArea() == nullptr)
		{
			delete WindowIterator->second;
			WindowIterator = NodeAreaWindows.erase(WindowIterator);
			continue;
		}

		if (WindowIterator->second->IsVisible())
			WindowIterator->second->Render();

		// Or user might have closed the window.
		if (WindowIterator->second->GetUserRequestedClose())
		{
			delete WindowIterator->second;
			WindowIterator = NodeAreaWindows.erase(WindowIterator);
			continue;
		}
		
		WindowIterator++;
	}
}