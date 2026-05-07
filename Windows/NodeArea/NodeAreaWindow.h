#pragma once

#include "../SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.h"
#include "../SubSystems/NodeSystem/CustomNodes/BeginNode.h"

class NodeAreaWindow : public FEImGuiWindow
{
	friend class NodeAreaWindowManager;

	std::string NodeAreaID;
	int FrameCountSinceOpen = 0;
	bool bShouldDockToCentralNode = false;
	bool bShouldCenterViewOnOpen = false;

	NodeAreaWindow(VisNodeSys::NodeArea* NodeAreaToWorkWith);
	~NodeAreaWindow();

	static void RenderMainContextMenu();
public:
	VisNodeSys::NodeArea* GetNodeArea() const;

	void Show() override;
	void Render() override;
};