#pragma once

#include "NodeAreaWindow.h"

class NodeAreaWindowManager
{
	friend class NodeAreaWindow;
	SINGLETON_PRIVATE_PART(NodeAreaWindowManager)

	std::unordered_map<std::string, NodeAreaWindow*> NodeAreaWindows;

	std::string FocusedNodeAreaID;
	void SetFocusedNodeAreaID(std::string NewFocusedNodeAreaID);
public:
	SINGLETON_PUBLIC_PART(NodeAreaWindowManager)

	NodeAreaWindow* CreateNodeAreaWindow(VisNodeSys::NodeArea* NodeArea, ImVec2 Position = ImVec2(100.0f, 100.0f), ImVec2 Size = ImVec2(800.0f, 600.0f));
	bool OpenNodeAreaWindow(VisNodeSys::NodeArea* NodeArea);
	NodeAreaWindow* GetNodeAreaWindow(VisNodeSys::NodeArea* NodeArea) const;

	NodeAreaWindow* GetInFocusNodeAreaWindow() const;

	void Render();
};

#define NODE_AREA_WINDOW_MANAGER NodeAreaWindowManager::GetInstance()
