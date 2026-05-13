#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class GetAbsolutePathNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	std::string AbsolutePath = "";

	std::function<void* ()> AbsolutePathDataGetter = [this]() -> void* {
		return &AbsolutePath;
	};

public:
	GetAbsolutePathNode();
	GetAbsolutePathNode(const GetAbsolutePathNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);
};