#pragma once
#include "../../../NodeSystem/VisualNodeSystem/VisualNodeSystem.h"

class StringToIntNode : public BaseExecutionFlowNode
{
	friend class NodeFactory;
	static bool bIsRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int Result = 0;

	std::function<void* ()> ResultDataGetter = [this]() -> void* {
		return &Result;
	};

public:
	StringToIntNode();
	StringToIntNode(const StringToIntNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};