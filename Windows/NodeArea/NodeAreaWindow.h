#pragma once

#include "../SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.h"
#include "../SubSystems/NodeSystem/CustomNodes/BeginNode.h"

class NodeAreaWindow : public FEImGuiWindow
{
	friend class NodeAreaWindowManager;

	VisNodeSys::NodeArea* CurrentNodeArea = nullptr;

	NodeAreaWindow(VisNodeSys::NodeArea* NodeArea);
	~NodeAreaWindow();

	static void RenderMainContextMenu();
public:
	VisNodeSys::NodeArea* GetNodeArea() const;

	void Show() override;
	void Render() override;
};