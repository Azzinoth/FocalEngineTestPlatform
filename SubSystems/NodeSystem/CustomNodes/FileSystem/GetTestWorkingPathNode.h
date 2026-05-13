#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class GetTestWorkingPathNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	class FETest* FindOwningTest();
	std::string ResolveOwningTestPath();

	std::string TestWorkingPath = "";

	std::function<void* ()> PathDataGetter = [this]() -> void* {
		return &TestWorkingPath;
	};

public:
	GetTestWorkingPathNode();
	GetTestWorkingPathNode(const GetTestWorkingPathNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};