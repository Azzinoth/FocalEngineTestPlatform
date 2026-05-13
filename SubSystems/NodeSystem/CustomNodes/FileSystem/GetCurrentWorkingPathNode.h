#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class GetCurrentWorkingPathNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string WorkingPath = "";

	std::function<void* ()> PathDataGetter = [this]() -> void* {
		return &WorkingPath;
	};

public:
	GetCurrentWorkingPathNode();
	GetCurrentWorkingPathNode(const GetCurrentWorkingPathNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};