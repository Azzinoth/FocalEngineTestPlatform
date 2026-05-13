#include "GetTestWorkingPathNode.h"
#include "../../../FEFileSystem.h"
#include "../../../FEPTActionSystem.h"
using namespace VisNodeSys;

bool GetTestWorkingPathNode::bIsRegistered = []()
{
	auto Creator = []() -> Node* {
		return new GetTestWorkingPathNode();
	};

	auto Copier = [](const Node& CurrentNode) -> Node* {
		const GetTestWorkingPathNode& NodeToCopy = static_cast<const GetTestWorkingPathNode&>(CurrentNode);
		return new GetTestWorkingPathNode(NodeToCopy);
	};

	NODE_FACTORY.RegisterNodeType("GetTestWorkingPathNode", Creator, Copier);
	return true;
}();

GetTestWorkingPathNode::GetTestWorkingPathNode() : BaseExecutionFlowNode()
{
	Type = "GetTestWorkingPathNode";

	SetStyle(DEFAULT);
	SetName("Get Test Working Path");

	TitleBackgroundColor = ImColor(217, 119, 6);
	TitleBackgroundColorHovered = ImColor(245, 145, 35);

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "STRING", "Path", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(280, 80));
	Output[1]->SetFunctionToOutputData(PathDataGetter);
}

GetTestWorkingPathNode::GetTestWorkingPathNode(const GetTestWorkingPathNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	TestWorkingPath = Other.TestWorkingPath;
	Output[1]->SetFunctionToOutputData(PathDataGetter);
}

Json::Value GetTestWorkingPathNode::ToJson()
{
	return Node::ToJson();
}

bool GetTestWorkingPathNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(PathDataGetter);
	return true;
}

FETest* GetTestWorkingPathNode::FindOwningTest()
{
	for (size_t i = 0; i < TEST_MANAGER.Tests.size(); i++)
	{
		FETest* CurrentTest = TEST_MANAGER.Tests[i];
		if (NODE_SYSTEM.GetNodeAreaByID(CurrentTest->EntryPointNodeArea->GetID()) != nullptr)
			return CurrentTest;
	}

	return nullptr;
}

std::string GetTestWorkingPathNode::ResolveOwningTestPath()
{
	FETest* Test = FindOwningTest();
	if (Test == nullptr)
		return "";

	return FocalEngine::FILE_SYSTEM.GetDirectoryPath(Test->FilePath);
}

void GetTestWorkingPathNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		TestWorkingPath = ResolveOwningTestPath();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool GetTestWorkingPathNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}