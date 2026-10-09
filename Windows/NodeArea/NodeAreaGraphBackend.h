#pragma once
#include "../../SubSystems/FESceneGraphUI/BackendInterface.h"
#include "NodeAreaWindow.h"

class NodeAreaGraphBackend : public SceneGraphUI::BackendInterface
{
	FETest* CurrentTest = nullptr;
	FocalEngine::FEUUID RootNodeAreaID;
public:
    NodeAreaGraphBackend();

    bool IsReady() const override;
    bool IsAlive(SceneGraphUI::NodeHandle Node) override;

	void SetCurrentTest(FETest* NewTest);
	FETest* GetCurrentTest() const;

	void SetRootNodeAreaID(const FocalEngine::FEUUID& NewRootNodeAreaID);
    SceneGraphUI::NodeHandle GetRoot() override;

    std::vector<SceneGraphUI::NodeHandle> GetChildren(SceneGraphUI::NodeHandle Node) override;
    SceneGraphUI::NodeHandle GetParent(SceneGraphUI::NodeHandle Node) override;

    SceneGraphUI::NodeHandle GetNodeByID(const FocalEngine::FEUUID& ID) override;
    FocalEngine::FEUUID GetNodeID(SceneGraphUI::NodeHandle Node)   override;

    std::string GetNodeName(SceneGraphUI::NodeHandle Node) override;
    std::string GetTag(SceneGraphUI::NodeHandle Node) override;

    bool MoveNode(SceneGraphUI::NodeHandle Node, SceneGraphUI::NodeHandle NewParent) override;
};