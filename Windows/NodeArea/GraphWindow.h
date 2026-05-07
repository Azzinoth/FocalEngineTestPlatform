#pragma once

#include "NodeAreaWindow.h"
#include "NodeAreaGraphBackend.h"
#include "../SubSystems/FESceneGraphUI/TreeView.h"

class NodeAreasGraphWindow
{
	friend class NodeAreaWindowManager;
    friend class FETestPlatform;
    SINGLETON_PRIVATE_PART(NodeAreasGraphWindow)

    bool bVisible = true;

    FETPImage* WindowIcon = nullptr;
    FETPImage* BeginIcon = nullptr;

	bool bLastFrameWasInvisible = true;

    // Rendering and initialization.
    void Render();
    void Clear();
    void InitializeResources();

    SceneGraphUI::TreeView* NodeAreaGraphUI = nullptr;
    NodeAreaGraphBackend* GraphBackend = nullptr;
	std::string LastFrameRootNodeID = "";
    static void OnNodeClicked(SceneGraphUI::NodeHandle Node, ImGuiMouseButton_ MouseButton);
	static void OnNodeDoubleClicked(SceneGraphUI::NodeHandle Node, ImGuiMouseButton_ MouseButton);
    static bool IsSelected(SceneGraphUI::NodeHandle Node);
    static std::string GetDisplayedName(SceneGraphUI::NodeHandle Node);
	//static void OnNodeHovered(SceneGraphUI::NodeHandle Node);
	//static void AfterNodeRender(SceneGraphUI::NodeHandle Node);
    static void ContextMenuRenderingFunction(SceneGraphUI::NodeHandle Node);
	static void RenameNodeFunction(SceneGraphUI::NodeHandle Node, std::string NewName);

    SceneGraphUI::NodeWidget WindowIndicator;
    SceneGraphUI::NodeWidget BeginIndicator;
    SceneGraphUI::NodeWidget FailedActionIndicator;
public:
    SINGLETON_PUBLIC_PART(NodeAreasGraphWindow)

    SceneGraphUI::TreeView* GetNodeAreaGraphUI() const;
};

#define NODE_AREAS_GRAPH_WINDOW NodeAreasGraphWindow::GetInstance()